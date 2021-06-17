const std = @import("std");
const Allocator = std.mem.Allocator;
const mem = std.mem;
const log = std.log;
const fs = std.fs;
const path = fs.path;
const assert = std.debug.assert;
const Version = std.builtin.Version;

const target_util = @import("target.zig");
const Compilation = @import("Compilation.zig");
const build_options = @import("build_options");
const trace = @import("tracy.zig").trace;
const Cache = std.Build.Cache;
const Package = @import("Package.zig");

pub const Lib = struct {
    name: []const u8,
    sover: u8,
};

pub const ABI = struct {
    all_versions: []const Version,
    all_targets: []const target_util.ArchOsAbi,
    /// The bytes from the file verbatim, starting from the u16 number
    /// of function inclusions.
    inclusions: []const u8,
    arena_state: std.heap.ArenaAllocator.State,

    pub fn destroy(abi: *ABI, gpa: Allocator) void {
        abi.arena_state.promote(gpa).deinit();
    }
};

// The order of the elements in this array defines the linking order.
pub const libs = [_]Lib{
    .{ .name = "m", .sover = 6 },
    .{ .name = "pthread", .sover = 0 },
    .{ .name = "c", .sover = 6 },
    .{ .name = "dl", .sover = 2 },
    .{ .name = "rt", .sover = 1 },
    .{ .name = "ld", .sover = 2 },
    .{ .name = "util", .sover = 1 },
    .{ .name = "resolv", .sover = 2 },
};

pub const LoadMetaDataError = error{
    /// The files that ship with the Zig compiler were unable to be read, or otherwise had malformed data.
    ZigInstallationCorrupt,
    OutOfMemory,
};

pub const abilists_path = "libc" ++ path.sep_str ++ "glibc" ++ path.sep_str ++ "abilists";
pub const abilists_max_size = 800 * 1024; // Bigger than this and something is definitely borked.

/// This function will emit a log error when there is a problem with the zig
/// installation and then return `error.ZigInstallationCorrupt`.
pub fn loadMetaData(gpa: Allocator, contents: []const u8) LoadMetaDataError!*ABI {
    const tracy = trace(@src());
    defer tracy.end();

    var arena_allocator = std.heap.ArenaAllocator.init(gpa);
    errdefer arena_allocator.deinit();
    const arena = arena_allocator.allocator();

    var index: usize = 0;

    {
        const libs_len = contents[index];
        index += 1;

        var i: u8 = 0;
        while (i < libs_len) : (i += 1) {
            const lib_name = mem.sliceTo(contents[index..], 0);
            index += lib_name.len + 1;

            if (i >= libs.len or !mem.eql(u8, libs[i].name, lib_name)) {
                log.err("libc" ++ path.sep_str ++ "glibc" ++ path.sep_str ++
                    "abilists: invalid library name or index ({d}): '{s}'", .{ i, lib_name });
                return error.ZigInstallationCorrupt;
            }
        }
    }

    const versions = b: {
        const versions_len = contents[index];
        index += 1;

        const versions = try arena.alloc(Version, versions_len);
        var i: u8 = 0;
        while (i < versions.len) : (i += 1) {
            versions[i] = .{
                .major = contents[index + 0],
                .minor = contents[index + 1],
                .patch = contents[index + 2],
            };
            index += 3;
        }
        break :b versions;
    };

    const targets = b: {
        const targets_len = contents[index];
        index += 1;

        const targets = try arena.alloc(target_util.ArchOsAbi, targets_len);
        var i: u8 = 0;
        while (i < targets.len) : (i += 1) {
            const target_name = mem.sliceTo(contents[index..], 0);
            index += target_name.len + 1;

            var component_it = mem.tokenize(u8, target_name, "-");
            const arch_name = component_it.next() orelse {
                log.err("abilists: expected arch name", .{});
                return error.ZigInstallationCorrupt;
            };
            const os_name = component_it.next() orelse {
                log.err("abilists: expected OS name", .{});
                return error.ZigInstallationCorrupt;
            };
            const abi_name = component_it.next() orelse {
                log.err("abilists: expected ABI name", .{});
                return error.ZigInstallationCorrupt;
            };
            const arch_tag = std.meta.stringToEnum(std.Target.Cpu.Arch, arch_name) orelse {
                log.err("abilists: unrecognized arch: '{s}'", .{arch_name});
                return error.ZigInstallationCorrupt;
            };
            if (!mem.eql(u8, os_name, "linux")) {
                log.err("abilists: expected OS 'linux', found '{s}'", .{os_name});
                return error.ZigInstallationCorrupt;
            }
            const abi_tag = std.meta.stringToEnum(std.Target.Abi, abi_name) orelse {
                log.err("abilists: unrecognized ABI: '{s}'", .{abi_name});
                return error.ZigInstallationCorrupt;
            };

            targets[i] = .{
                .arch = arch_tag,
                .os = .linux,
                .abi = abi_tag,
            };
        }
        break :b targets;
    };

    const abi = try arena.create(ABI);
    abi.* = .{
        .all_versions = versions,
        .all_targets = targets,
        .inclusions = contents[index..],
        .arena_state = arena_allocator.state,
    };
    return abi;
}

pub const CRTFile = enum {
    crti_o,
    crtn_o,
    scrt1_o,
    libc_nonshared_a,
};

pub fn buildCRTFile(comp: *Compilation, crt_file: CRTFile) !void {
    if (!build_options.have_llvm) {
        return error.ZigCompilerNotBuiltWithLLVMExtensions;
    }
    const gpa = comp.gpa;
    var arena_allocator = std.heap.ArenaAllocator.init(gpa);
    defer arena_allocator.deinit();
    const arena = arena_allocator.allocator();

    const target = comp.getTarget();
    const target_ver = target.os.version_range.linux.glibc;
    const start_old_init_fini = target_ver.order(.{ .major = 2, .minor = 33 }) != .gt;

    // In all cases in this function, we add the C compiler flags to
    // cache_exempt_flags rather than extra_flags, because these arguments
    // depend on only properties that are already covered by the cache
    // manifest. Including these arguments in the cache could only possibly
    // waste computation and create false negatives.

    switch (crt_file) {
        .crti_o => {
            var args = std.ArrayList([]const u8).init(arena);
            try add_include_dirs(comp, arena, &args);
            try args.appendSlice(&[_][]const u8{
                "-D_LIBC_REENTRANT",
                "-include",
                try lib_path(comp, arena, lib_libc_glibc ++ "include" ++ path.sep_str ++ "libc-modules.h"),
                "-DMODULE_NAME=libc",
                "-Wno-nonportable-include-path",
                "-include",
                try lib_path(comp, arena, lib_libc_glibc ++ "include" ++ path.sep_str ++ "libc-symbols.h"),
                "-DTOP_NAMESPACE=glibc",
                "-DASSEMBLER",
                "-Wa,--noexecstack",
            });
            return comp.build_crt_file("crti", .Obj, 