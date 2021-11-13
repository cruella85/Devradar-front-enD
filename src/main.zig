const std = @import("std");
const builtin = @import("builtin");
const assert = std.debug.assert;
const io = std.io;
const fs = std.fs;
const mem = std.mem;
const process = std.process;
const Allocator = mem.Allocator;
const ArrayList = std.ArrayList;
const Ast = std.zig.Ast;
const warn = std.log.warn;

const tracy = @import("tracy.zig");
const Compilation = @import("Compilation.zig");
const link = @import("link.zig");
const Package = @import("Package.zig");
const build_options = @import("build_options");
const introspect = @import("introspect.zig");
const LibCInstallation = @import("libc_installation.zig").LibCInstallation;
const wasi_libc = @import("wasi_libc.zig");
const translate_c = @import("translate_c.zig");
const clang = @import("clang.zig");
const Cache = std.Build.Cache;
const target_util = @import("target.zig");
const ThreadPool = @import("ThreadPool.zig");
const crash_report = @import("crash_report.zig");

pub const std_options = struct {
    pub const wasiCwd = wasi_cwd;
    pub const logFn = log;
    pub const enable_segfault_handler = false;

    pub const log_level: std.log.Level = switch (builtin.mode) {
        .Debug => .debug,
        .ReleaseSafe, .ReleaseFast => .info,
        .ReleaseSmall => .err,
    };
};

// Crash report needs to override the panic handler
pub const panic = crash_report.panic;

var wasi_preopens: fs.wasi.Preopens = undefined;
pub fn wasi_cwd() fs.Dir {
    // Expect the first preopen to be current working directory.
    const cwd_fd: std.os.fd_t = 3;
    assert(mem.eql(u8, wasi_preopens.names[cwd_fd], "."));
    return .{ .fd = cwd_fd };
}

pub fn getWasiPreopen(name: []const u8) Compilation.Directory {
    return .{
        .path = name,
        .handle = .{
            .fd = wasi_preopens.find(name) orelse fatal("WASI preopen not found: '{s}'", .{name}),
        },
    };
}

pub fn fatal(comptime format: []const u8, args: anytype) noreturn {
    std.log.err(format, args);
    process.exit(1);
}

/// There are many assumptions in the entire codebase that Zig source files can
/// be byte-indexed with a u32 integer.
pub const max_src_size = std.math.maxInt(u32);

pub const debug_extensions_enabled = builtin.mode == .Debug;

pub const Color = enum {
    auto,
    off,
    on,
};

const normal_usage =
    \\Usage: zig [command] [options]
    \\
    \\Commands:
    \\
    \\  build            Build project from build.zig
    \\  init-exe         Initialize a `zig build` application in the cwd
    \\  init-lib         Initialize a `zig build` library in the cwd
    \\
    \\  ast-check        Look for simple compile errors in any set of files
    \\  build-exe        Create executable from source or object files
    \\  build-lib        Create library from source or object files
    \\  build-obj        Create object from source or object files
    \\  fmt              Reformat Zig source into canonical form
    \\  run              Create executable and run immediately
    \\  test             Create and run a test build
    \\  translate-c      Convert C code to Zig code
    \\
    \\  ar               Use Zig as a drop-in archiver
    \\  cc               Use Zig as a drop-in C compiler
    \\  c++              Use Zig as a drop-in C++ compiler
    \\  dlltool          Use Zig as a drop-in dlltool.exe
    \\  lib              Use Zig as a drop-in lib.exe
    \\  ranlib           Use Zig as a drop-in ranlib
    \\  objcopy          Use Zig as a drop-in objcopy
    \\
    \\  env              Print lib path, std path, cache directory, and version
    \\  help             Print this help and exit
    \\  libc             Display native libc paths file or validate one
    \\  targets          List available compilation targets
    \\  version          Print version number and exit
    \\  zen              Print Zen of Zig and exit
    \\
    \\General Options:
    \\
    \\  -h, --help       Print command-specific usage
    \\
;

const debug_usage = normal_usage ++
    \\
    \\Debug Commands:
    \\
    \\  changelist       Compute mappings from old ZIR to new ZIR
    \\
;

const usage = if (debug_extensions_enabled) debug_usage else normal_usage;

var log_scopes: std.ArrayListUnmanaged([]const u8) = .{};

pub fn log(
    comptime level: std.log.Level,
    comptime scope: @TypeOf(.EnumLiteral),
    comptime format: []const u8,
    args: anytype,
) void {
    // Hide debug messages unless:
    // * logging enabled with `-Dlog`.
    // * the --debug-log arg for the scope has been provided
    if (@enumToInt(level) > @enumToInt(std.options.log_level) or
        @enumToInt(level) > @enumToInt(std.log.Level.info))
    {
        if (!build_options.enable_logging) return;

        const scope_name = @tagName(scope);
        for (log_scopes.items) |log_scope| {
            if (mem.eql(u8, log_scope, scope_name))
                break;
        } else return;
    }

    const prefix1 = comptime level.asText();
    const prefix2 = if (scope == .default) ": " else "(" ++ @tagName(scope) ++ "): ";

    // Print the message to stderr, silently ignoring any errors
    std.debug.print(prefix1 ++ prefix2 ++ format ++ "\n", args);
}

var general_purpose_allocator = std.heap.GeneralPurposeAllocator(.{
    .stack_trace_frames = build_options.mem_leak_frames,
}){};

pub fn main() anyerror!void {
    crash_report.initialize();

    const use_gpa = (build_options.force_gpa or !builtin.link_libc) and builtin.os.tag != .wasi;
    const gpa = gpa: {
        if (builtin.os.tag == .wasi) {
            break :gpa std.heap.wasm_allocator;
        }
        if (use_gpa) {
            break :gpa general_purpose_allocator.allocator();
        }
        // We would prefer to use raw libc allocator here, but cannot
        // use it if it won't support the alignment we need.
        if (@alignOf(std.c.max_align_t) < @alignOf(i128)) {
            break :gpa std.heap.c_allocator;
        }
        break :gpa std.heap.raw_c_allocator;
    };
    defer if (use_gpa) {
        _ = general_purpose_allocator.deinit();
    };
    var arena_instance = std.heap.ArenaAllocator.init(gpa);
    defer arena_instance.deinit();
    const arena = arena_instance.allocator();

    const args = try process.argsAlloc(arena);

    if (tracy.enable_allocation) {
        var gpa_tracy = tracy.tracyAllocator(gpa);
        return mainArgs(gpa_tracy.allocator(), arena, args);
    }

    if (builtin.os.tag == .wasi) {
        wasi_preopens = try fs.wasi.preopensAlloc(arena);
    }

    // Short circuit some of the other logic for bootstrapping.
    if (build_options.only_c) {
        if (mem.eql(u8, args[1], "build-exe")) {
            return buildOutputType(gpa, arena, args, .{ .build = .Exe });
        } else if (mem.eql(u8, args[1], "build-obj")) {
            return buildOutputType(gpa, arena, args, .{ .build = .Obj });
        } else {
            @panic("only build-exe or build-obj is supported in a -Donly-c build");
        }
    }

    return mainArgs(gpa, arena, args);
}

/// Check that LLVM and Clang have been linked properly so that they are using the same
/// libc++ and can safely share objects with pointers to static variables in libc++
fn verifyLibcxxCorrectlyLinked() void {
    if (build_options.have_llvm and ZigClangIsLLVMUsingSeparateLibcxx()) {
        fatal(
            \\Zig was built/linked incorrectly: LLVM and Clang have separate copies of libc++
            \\       If you are dynamically linking LLVM, make sure you dynamically link libc++ too
        , .{});
    }
}

pub fn mainArgs(gpa: Allocator, arena: Allocator, args: []const []const u8) !void {
    if (args.len <= 1) {
        std.log.info("{s}", .{usage});
        fatal("expected command argument", .{});
    }

    if (std.process.can_execv and std.os.getenvZ("ZIG_IS_DETECTING_LIBC_PATHS") != null) {
        // In this case we have accidentally invoked ourselves as "the system C compiler"
        // to figure out where libc is installed. This is essentially infinite recursion
        // via child process execution due to the CC environment variable pointing to Zig.
        // Here we ignore the CC environment variable and exec `cc` as a child process.
        // However it's possible Zig is installed as *that* C compiler as well, which is
        // why we have this additional environment variable here to check.
        var env_map = try std.process.getEnvMap(arena);

        const inf_loop_env_key = "ZIG_IS_TRYING_TO_NOT_CALL_ITSELF";
        if (env_map.get(inf_loop_env_key) != null) {
            fatal("The compilation links against libc, but Zig is unable to provide a libc " ++
                "for this operating system, and no --libc " ++
                "parameter was provided, so Zig attempted to invoke the system C compiler " ++
                "in order to determine where libc is installed. However the system C " ++
                "compiler is `zig cc`, so no libc installation was found.", .{});
        }
        try env_map.put(inf_loop_env_key, "1");

        // Some programs such as CMake will strip the `cc` and subsequent args from the
        // CC environment variable. We detect and support this scenario here because of
        // the ZIG_IS_DETECTING_LIBC_PATHS environment variable.
        if (mem.eql(u8, args[1], "cc")) {
            return std.process.execve(arena, args[1..], &env_map);
        } else {
            const modified_args = try arena.dupe([]const u8, args);
            modified_args[0] = "cc";
            return std.process.execve(arena, modified_args, &env_map);
        }
    }

    defer log_scopes.deinit(gpa);

    const cmd = args[1];
    const cmd_args = args[2..];
    if (mem.eql(u8, cmd, "build-exe")) {
        return buildOutputType(gpa, arena, args, .{ .build = .Exe });
    } else if (mem.eql(u8, cmd, "build-lib")) {
        return buildOutputType(gpa, arena, args, .{ .build = .Lib });
    } else if (mem.eql(u8, cmd, "build-obj")) {
        return buildOutputType(gpa, arena, args, .{ .build = .Obj });
    } else if (mem.eql(u8, cmd, "test")) {
        return buildOutputType(gpa, arena, args, .zig_test);
    } else if (mem.eql(u8, cmd, "run")) {
        return buildOutputType(gpa, arena, args, .run);
    } else if (mem.eql(u8, cmd, "dlltool") or
        mem.eql(u8, cmd, "ranlib") or
        mem.eql(u8, cmd, "lib") or
        mem.eql(u8, cmd, "ar"))
    {
        return process.exit(try llvmArMain(arena, args));
    } else if (mem.eql(u8, cmd, "cc")) {
        return buildOutputType(gpa, arena, args, .cc);
    } else if (mem.eql(u8, cmd, "c++")) {
        return buildOutputType(gpa, arena, args, .cpp);
    } else if (mem.eql(u8, cmd, "translate-c")) {
        return buildOutputType(gpa, arena, args, .translate_c);
    } else if (mem.eql(u8, cmd, "clang") or
        mem.eql(u8, cmd, "-cc1") or mem.eql(u8, cmd, "-cc1as"))
    {
        return process.exit(try clangMain(arena, args));
    } else if (mem.eql(u8, cmd, "ld.lld") or
        mem.eql(u8, cmd, "lld-link") or
        mem.eql(u8, cmd, "wasm-ld"))
    {
        return process.exit(try lldMain(arena, args, true));
    } else if (mem.eql(u8, cmd, "build")) {
        return cmdBuild(gpa, arena, cmd_args);
    } else if (mem.eql(u8, cmd, "fmt")) {
        return cmdFmt(gpa, arena, cmd_args);
    } else if (mem.eql(u8, cmd, "objcopy")) {
        return @import("objcopy.zig").cmdObjCopy(gpa, arena, cmd_args);
    } else if (mem.eql(u8, cmd, "libc")) {
        return cmdLibC(gpa, cmd_args);
    } else if (mem.eql(u8, cmd, "init-exe")) {
        return cmdInit(gpa, arena, cmd_args, .Exe);
    } else if (mem.eql(u8, cmd, "init-lib")) {
        return cmdInit(gpa, arena, cmd_args, .Lib);
    } else if (mem.eql(u8, cmd, "targets")) {
        const info = try detectNativeTargetInfo(.{});
        const stdout = io.getStdOut().writer();
        return @import("print_targets.zig").cmdTargets(arena, cmd_args, stdout, info.target);
    } else if (mem.eql(u8, cmd, "version")) {
        try std.io.getStdOut().writeAll(build_options.version ++ "\n");
        // Check libc++ linkage to make sure Zig was built correctly, but only for "env" and "version"
        // to avoid affecting the startup time for build-critical commands (check takes about ~10 μs)
        return verifyLibcxxCorrectlyLinked();
    } else if (mem.eql(u8, cmd, "env")) {
        verifyLibcxxCorrectlyLinked();
        return @import("print_env.zig").cmdEnv(arena, cmd_args, io.getStdOut().writer());
    } else if (mem.eql(u8, cmd, "zen")) {
        return io.getStdOut().writeAll(info_zen);
    } else if (mem.eql(u8, cmd, "help") or mem.eql(u8, cmd, "-h") or mem.eql(u8, cmd, "--help")) {
        return io.getStdOut().writeAll(usage);
    } else if (mem.eql(u8, cmd, "ast-check")) {
        return cmdAstCheck(gpa, arena, cmd_args);
    } else if (debug_extensions_enabled and mem.eql(u8, cmd, "changelist")) {
        return cmdChangelist(gpa, arena, cmd_args);
    } else {
        std.log.info("{s}", .{usage});
        fatal("unknown command: {s}", .{args[1]});
    }
}

const usage_build_generic =
    \\Usage: zig build-exe   [options] [files]
    \\       zig build-lib   [options] [files]
    \\       zig build-obj   [options] [files]
    \\       zig test        [options] [files]
    \\       zig run         [options] [files] [-- [args]]
    \\       zig translate-c [options] [file]
    \\
    \\Supported file types:
    \\                    .zig    Zig source code
    \\                      .o    ELF object file
    \\                      .o    Mach-O (macOS) object file
    \\                      .o    WebAssembly object file
    \\                    .obj    COFF (Windows) object file
    \\                    .lib    COFF (Windows) static library
    \\                      .a    ELF static library
    \\                      .a    Mach-O (macOS) static library
    \\                      .a    WebAssembly static library
    \\                     .so    ELF shared object (dynamic link)
    \\                    .dll    Windows Dynamic Link Library
    \\                  .dylib    Mach-O (macOS) dynamic library
    \\                    .tbd    (macOS) text-based dylib definition
    \\                      .s    Target-specific assembly source code
    \\                      .S    Assembly with C preprocessor (requires LLVM extensions)
    \\                      .c    C source code (requires LLVM extensions)
    \\  .cxx .cc .C .cpp .stub    C++ source code (requires LLVM extensions)
    \\                      .m    Objective-C source code (requires LLVM extensions)
    \\                     .mm    Objective-C++ source code (requires LLVM extensions)
    \\                     .bc    LLVM IR Module (requires LLVM extensions)
    \\                     .cu    Cuda source code (requires LLVM extensions)
    \\
    \\General Options:
    \\  -h, --help                Print this help and exit
    \\  --watch                   Enable compiler REPL
    \\  --color [auto|off|on]     Enable or disable colored error messages
    \\  -femit-bin[=path]         (default) Output machine code
    \\  -fno-emit-bin             Do not output machine code
    \\  -femit-asm[=path]         Output .s (assembly code)
    \\  -fno-emit-asm             (default) Do not output .s (assembly code)
    \\  -femit-llvm-ir[=path]     Produce a .ll file with LLVM IR (requires LLVM extensions)
    \\  -fno-emit-llvm-ir         (default) Do not produce a .ll file with LLVM IR
    \\  -femit-llvm-bc[=path]     Produce a LLVM module as a .bc file (requires LLVM extensions)
    \\  -fno-emit-llvm-bc         (default) Do not produce a LLVM module as a .bc file
    \\  -femit-h[=path]           Generate a C header file (.h)
    \\  -fno-emit-h               (default) Do not generate a C header file (.h)
    \\  -femit-docs[=path]        Create a docs/ dir with html documentation
    \\  -fno-emit-docs            (default) Do not produce docs/ dir with html documentation
    \\  -femit-analysis[=path]    Write analysis JSON file with type information
    \\  -fno-emit-analysis        (default) Do not write analysis JSON file with type information
    \\  -femit-implib[=path]      (default) Produce an import .lib when building a Windows DLL
    \\  -fno-emit-implib          Do not produce an import .lib when building a Windows DLL
    \\  --show-builtin            Output the source of @import("builtin") then exit
    \\  --cache-dir [path]        Override the local cache directory
    \\  --global-cache-dir [path] Override the global cache directory
    \\  --zig-lib-dir [path]      Override path to Zig installation lib directory
    \\  --enable-cache            Output to cache directory; print path to stdout
    \\
    \\Compile Options:
    \\  -target [name]            <arch><sub>-<os>-<abi> see the targets command
    \\  -mcpu [cpu]               Specify target CPU and feature set
    \\  -mcmodel=[default|tiny|   Limit range of code and data virtual addresses
    \\            small|kernel|
    \\            medium|large]
    \\  -x language               Treat subsequent input files as having type <language>
    \\  -mred-zone                Force-enable the "red-zone"
    \\  -mno-red-zone             Force-disable the "red-zone"
    \\  -fomit-frame-pointer      Omit the stack frame pointer
    \\  -fno-omit-frame-pointer   Store the stack frame pointer
    \\  -mexec-model=[value]      (WASI) Execution model
    \\  --name [name]             Override root name (not a file path)
    \\  -O [mode]                 Choose what to optimize for
    \\    Debug                   (default) Optimizations off, safety on
    \\    ReleaseFast             Optimizations on, safety off
    \\    ReleaseSafe             Optimizations on, safety on
    \\    ReleaseSmall            Optimize for small binary, safety off
    \\  --mod [name]:[deps]:[src] Make a module available for dependency under the given name
    \\      deps: [dep],[dep],...
    \\      dep:  [[import=]name]
    \\  --deps [dep],[dep],...    Set dependency names for the root package
    \\      dep:  [[import=]name]
    \\  --main-pkg-path           Set the director