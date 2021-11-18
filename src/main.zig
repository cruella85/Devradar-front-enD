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
    \\  --main-pkg-path           Set the directory of the root package
    \\  -fPIC                     Force-enable Position Independent Code
    \\  -fno-PIC                  Force-disable Position Independent Code
    \\  -fPIE                     Force-enable Position Independent Executable
    \\  -fno-PIE                  Force-disable Position Independent Executable
    \\  -flto                     Force-enable Link Time Optimization (requires LLVM extensions)
    \\  -fno-lto                  Force-disable Link Time Optimization
    \\  -fstack-check             Enable stack probing in unsafe builds
    \\  -fno-stack-check          Disable stack probing in safe builds
    \\  -fstack-protector         Enable stack protection in unsafe builds
    \\  -fno-stack-protector      Disable stack protection in safe builds
    \\  -fsanitize-c              Enable C undefined behavior detection in unsafe builds
    \\  -fno-sanitize-c           Disable C undefined behavior detection in safe builds
    \\  -fvalgrind                Include valgrind client requests in release builds
    \\  -fno-valgrind             Omit valgrind client requests in debug builds
    \\  -fsanitize-thread         Enable Thread Sanitizer
    \\  -fno-sanitize-thread      Disable Thread Sanitizer
    \\  -fdll-export-fns          Mark exported functions as DLL exports (Windows)
    \\  -fno-dll-export-fns       Force-disable marking exported functions as DLL exports
    \\  -funwind-tables           Always produce unwind table entries for all functions
    \\  -fno-unwind-tables        Never produce unwind table entries
    \\  -fLLVM                    Force using LLVM as the codegen backend
    \\  -fno-LLVM                 Prevent using LLVM as the codegen backend
    \\  -fClang                   Force using Clang as the C/C++ compilation backend
    \\  -fno-Clang                Prevent using Clang as the C/C++ compilation backend
    \\  -freference-trace[=num]   How many lines of reference trace should be shown per compile error
    \\  -fno-reference-trace      Disable reference trace
    \\  -ferror-tracing           Enable error tracing in ReleaseFast mode
    \\  -fno-error-tracing        Disable error tracing in Debug and ReleaseSafe mode
    \\  -fsingle-threaded         Code assumes there is only one thread
    \\  -fno-single-threaded      Code may not assume there is only one thread
    \\  -fbuiltin                 Enable implicit builtin knowledge of functions
    \\  -fno-builtin              Disable implicit builtin knowledge of functions
    \\  -ffunction-sections       Places each function in a separate section
    \\  -fno-function-sections    All functions go into same section
    \\  -fstrip                   Omit debug symbols
    \\  -fno-strip                Keep debug symbols
    \\  -fformatted-panics        Enable formatted safety panics
    \\  -fno-formatted-panics     Disable formatted safety panics
    \\  -ofmt=[mode]              Override target object format
    \\    elf                     Executable and Linking Format
    \\    c                       C source code
    \\    wasm                    WebAssembly
    \\    coff                    Common Object File Format (Windows)
    \\    macho                   macOS relocatables
    \\    spirv                   Standard, Portable Intermediate Representation V (SPIR-V)
    \\    plan9                   Plan 9 from Bell Labs object format
    \\    hex  (planned feature)  Intel IHEX
    \\    raw  (planned feature)  Dump machine code directly
    \\  -idirafter [dir]          Add directory to AFTER include search path
    \\  -isystem  [dir]           Add directory to SYSTEM include search path
    \\  -I[dir]                   Add directory to include search path
    \\  -D[macro]=[value]         Define C [macro] to [value] (1 if [value] omitted)
    \\  --libc [file]             Provide a file which specifies libc paths
    \\  -cflags [flags] --        Set extra flags for the next positional C source files
    \\
    \\Link Options:
    \\  -l[lib], --library [lib]       Link against system library (only if actually used)
    \\  -needed-l[lib],                Link against system library (even if unused)
    \\    --needed-library [lib]
    \\  -L[d], --library-directory [d] Add a directory to the library search path
    \\  -T[script], --script [script]  Use a custom linker script
    \\  --version-script [path]        Provide a version .map file
    \\  --dynamic-linker [path]        Set the dynamic interpreter path (usually ld.so)
    \\  --sysroot [path]               Set the system root directory (usually /)
    \\  --version [ver]                Dynamic library semver
    \\  --entry [name]                 Set the entrypoint symbol name
    \\  -fsoname[=name]                Override the default SONAME value
    \\  -fno-soname                    Disable emitting a SONAME
    \\  -fLLD                          Force using LLD as the linker
    \\  -fno-LLD                       Prevent using LLD as the linker
    \\  -fcompiler-rt                  Always include compiler-rt symbols in output
    \\  -fno-compiler-rt               Prevent including compiler-rt symbols in output
    \\  -rdynamic                      Add all symbols to the dynamic symbol table
    \\  -rpath [path]                  Add directory to the runtime library search path
    \\  -feach-lib-rpath               Ensure adding rpath for each used dynamic library
    \\  -fno-each-lib-rpath            Prevent adding rpath for each used dynamic library
    \\  -fallow-shlib-undefined        Allows undefined symbols in shared libraries
    \\  -fno-allow-shlib-undefined     Disallows undefined symbols in shared libraries
    \\  -fbuild-id                     Helps coordinate stripped binaries with debug symbols
    \\  -fno-build-id                  (default) Saves a bit of time linking
    \\  --eh-frame-hdr                 Enable C++ exception handling by passing --eh-frame-hdr to linker
    \\  --emit-relocs                  Enable output of relocation sections for post build tools
    \\  -z [arg]                       Set linker extension flags
    \\    nodelete                     Indicate that the object cannot be deleted from a process
    \\    notext                       Permit read-only relocations in read-only segments
    \\    defs                         Force a fatal error if any undefined symbols remain
    \\    undefs                       Reverse of -z defs
    \\    origin                       Indicate that the object must have its origin processed
    \\    nocopyreloc                  Disable the creation of copy relocations
    \\    now                          (default) Force all relocations to be processed on load
    \\    lazy                         Don't force all relocations to be processed on load
    \\    relro                        (default) Force all relocations to be read-only after processing
    \\    norelro                      Don't force all relocations to be read-only after processing
    \\    common-page-size=[bytes]     Set the common page size for ELF binaries
    \\    max-page-size=[bytes]        Set the max page size for ELF binaries
    \\  -dynamic                       Force output to be dynamically linked
    \\  -static                        Force output to be statically linked
    \\  -Bsymbolic                     Bind global references locally
    \\  --compress-debug-sections=[e]  Debug section compression settings
    \\      none                       No compression
    \\      zlib                       Compression with deflate/inflate
    \\  --gc-sections                  Force removal of functions and data that are unreachable by the entry point or exported symbols
    \\  --no-gc-sections               Don't force removal of unreachable functions and data
    \\  --sort-section=[value]         Sort wildcard section patterns by 'name' or 'alignment'
    \\  --subsystem [subsystem]        (Windows) /SUBSYSTEM:<subsystem> to the linker
    \\  --stack [size]                 Override default stack size
    \\  --image-base [addr]            Set base address for executable image
    \\  -weak-l[lib]                   (Darwin) link against system library and mark it and all referenced symbols as weak
    \\    -weak_library [lib]
    \\  -framework [name]              (Darwin) link against framework
    \\  -needed_framework [name]       (Darwin) link against framework (even if unused)
    \\  -needed_library [lib]          (Darwin) link against system library (even if unused)
    \\  -weak_framework [name]         (Darwin) link against framework and mark it and all referenced symbols as weak
    \\  -F[dir]                        (Darwin) add search path for frameworks
    \\  -install_name=[value]          (Darwin) add dylib's install name
    \\  --entitlements [path]          (Darwin) add path to entitlements file for embedding in code signature
    \\  -pagezero_size [value]         (Darwin) size of the __PAGEZERO segment in hexadecimal notation
    \\  -search_paths_first            (Darwin) search each dir in library search paths for `libx.dylib` then `libx.a`
    \\  -search_dylibs_first           (Darwin) search `libx.dylib` in each dir in library search paths, then `libx.a`
    \\  -headerpad [value]             (Darwin) set minimum space for future expansion of the load commands in hexadecimal notation
    \\  -headerpad_max_install_names   (Darwin) set enough space as if all paths were MAXPATHLEN
    \\  -dead_strip                    (Darwin) remove functions and data that are unreachable by the entry point or exported symbols
    \\  -dead_strip_dylibs             (Darwin) remove dylibs that are unreachable by the entry point or exported symbols
    \\  --import-memory                (WebAssembly) import memory from the environment
    \\  --import-symbols               (WebAssembly) import missing symbols from the host environment
    \\  --import-table                 (WebAssembly) import function table from the host environment
    \\  --export-table                 (WebAssembly) export function table to the host environment
    \\  --initial-memory=[bytes]       (WebAssembly) initial size of the linear memory
    \\  --max-memory=[bytes]           (WebAssembly) maximum size of the linear memory
    \\  --shared-memory                (WebAssembly) use shared linear memory
    \\  --global-base=[addr]           (WebAssembly) where to start to place global data
    \\  --export=[value]               (WebAssembly) Force a symbol to be exported
    \\
    \\Test Options:
    \\  --test-filter [text]           Skip tests that do not match filter
    \\  --test-name-prefix [text]      Add prefix to all tests
    \\  --test-cmd [arg]               Specify test execution command one arg at a time
    \\  --test-cmd-bin                 Appends test binary path to test cmd args
    \\  --test-evented-io              Runs the test in evented I/O mode
    \\  --test-no-exec                 Compiles test binary without running it
    \\  --test-runner [path]           Specify a custom test runner
    \\
    \\Debug Options (Zig Compiler Development):
    \\  -fopt-bisect-limit [limit]   Only run [limit] first LLVM optimization passes
    \\  -ftime-report                Print timing diagnostics
    \\  -fstack-report               Print stack size diagnostics
    \\  --verbose-link               Display linker invocations
    \\  --verbose-cc                 Display C compiler invocations
    \\  --verbose-air                Enable compiler debug output for Zig AIR
    \\  --verbose-llvm-ir            Enable compiler debug output for LLVM IR
    \\  --verbose-cimport            Enable compiler debug output for C imports
    \\  --verbose-llvm-cpu-features  Enable compiler debug output for LLVM CPU features
    \\  --debug-log [scope]          Enable printing debug/info log messages for scope
    \\  --debug-compile-errors       Crash with helpful diagnostics at the first compile error
    \\  --debug-link-snapshot        Enable dumping of the linker's state in JSON format
    \\
;

const repl_help =
    \\Commands:
    \\         update  Detect changes to source files and update output files.
    \\            run  Execute the output file, if it is an executable or test.
    \\ update-and-run  Perform an `update` followed by `run`.
    \\           help  Print this text
    \\           exit  Quit this repl
    \\
;

const SOName = union(enum) {
    no,
    yes_default_value,
    yes: []const u8,
};

const EmitBin = union(enum) {
    no,
    yes_default_path,
    yes: []const u8,
    yes_a_out,
};

const Emit = union(enum) {
    no,
    yes_default_path,
    yes: []const u8,

    const Resolved = struct {
        data: ?Compilation.EmitLoc,
        dir: ?fs.Dir,

        fn deinit(self: *Resolved) void {
            if (self.dir) |*dir| {
                dir.close();
            }
        }
    };

    fn resolve(emit: Emit, default_basename: []const u8) !Resolved {
        var resolved: Resolved = .{ .data = null, .dir = null };
        errdefer resolved.deinit();

        switch (emit) {
            .no => {},
            .yes_default_path => {
                resolved.data = Compilation.EmitLoc{
                    .directory = .{ .path = null, .handle = fs.cwd() },
                    .basename = default_basename,
                };
            },
            .yes => |full_path| {
                const basename = fs.path.basename(full_path);
                if (fs.path.dirname(full_path)) |dirname| {
                    const handle = try fs.cwd().openDir(dirname, .{});
                    resolved = .{
                        .dir = handle,
                        .data = Compilation.EmitLoc{
                            .basename = basename,
                            .directory = .{
                                .path = dirname,
                                .handle = handle,
                            },
                        },
                    };
                } else {
                    resolved.data = Compilation.EmitLoc{
                        .basename = basename,
                        .directory = .{ .path = null, .handle = fs.cwd() },
                    };
                }
            },
        }
        return resolved;
    }
};

fn optionalStringEnvVar(arena: Allocator, name: []const u8) !?[]const u8 {
    // Env vars aren't used in the bootstrap stage.
    if (build_options.only_c) {
        return null;
    }
    if (std.process.getEnvVarOwned(arena, name)) |value| {
        return value;
    } else |err| switch (err) {
        error.EnvironmentVariableNotFound => return null,
        else => |e| return e,
    }
}

const ArgMode = union(enum) {
    build: std.builtin.OutputMode,
    cc,
    cpp,
    translate_c,
    zig_test,
    run,
};

fn buildOutputType(
    gpa: Allocator,
    arena: Allocator,
    all_args: []const []const u8,
    arg_mode: ArgMode,
) !void {
    var color: Color = .auto;
    var optimize_mode: std.builtin.Mode = .Debug;
    var provided_name: ?[]const u8 = null;
    var link_mode: ?std.builtin.LinkMode = null;
    var dll_export_fns: ?bool = null;
    var single_threaded: ?bool = null;
    var root_src_file: ?[]const u8 = null;
    var version: std.builtin.Version = .{ .major = 0, .minor = 0, .patch = 0 };
    var have_version = false;
    var compatibility_version: ?std.builtin.Version = null;
    var strip: ?bool = null;
    var formatted_panics: ?bool = null;
    var function_sections = false;
    var no_builtin = false;
    var watch = false;
    var debug_compile_errors = false;
    var verbose_link = (builtin.os.tag != .wasi or builtin.link_libc) and std.process.hasEnvVarConstant("ZIG_VERBOSE_LINK");
    var verbose_cc = (builtin.os.tag != .wasi or builtin.link_libc) and std.process.hasEnvVarConstant("ZIG_VERBOSE_CC");
    var verbose_air = false;
    var verbose_llvm_ir = false;
    var verbose_cimport = false;
    var verbose_llvm_cpu_features = false;
    var time_report = false;
    var stack_report = false;
    var show_builtin = false;
    var emit_bin: EmitBin = .yes_default_path;
    var emit_asm: Emit = .no;
    var emit_llvm_ir: Emit = .no;
    var emit_llvm_bc: Emit = .no;
    var emit_docs: Emit = .no;
    var emit_analysis: Emit = .no;
    var emit_implib: Emit = .yes_default_path;
    var emit_implib_arg_provided = false;
    var target_arch_os_abi: []const u8 = "native";
    var target_mcpu: ?[]const u8 = null;
    var target_dynamic_linker: ?[]const u8 = null;
    var target_ofmt: ?[]const u8 = null;
    var output_mode: std.builtin.OutputMode = undefined;
    var emit_h: Emit = .no;
    var soname: SOName = undefined;
    var ensure_libc_on_non_freestanding = false;
    var ensure_libcpp_on_non_freestanding = false;
    var link_libc = false;
    var link_libcpp = false;
    var link_libunwind = false;
    var want_native_include_dirs = false;
    var enable_cache: ?bool = null;
    var want_pic: ?bool = null;
    var want_pie: ?bool = null;
    var want_lto: ?bool = null;
    var want_unwind_tables: ?bool = null;
    var want_sanitize_c: ?bool = null;
    var want_stack_check: ?bool = null;
    var want_stack_protector: ?u32 = null;
    var want_red_zone: ?bool = null;
    var omit_frame_pointer: ?bool = null;
    var want_valgrind: ?bool = null;
    var want_tsan: ?bool = null;
    var want_compiler_rt: ?bool = null;
    var rdynamic: bool = false;
    var linker_script: ?[]const u8 = null;
    var version_script: ?[]const u8 = null;
    var disable_c_depfile = false;
    var linker_sort_section: ?link.SortSection = null;
    var linker_gc_sections: ?bool = null;
    var linker_compress_debug_sections: ?link.CompressDebugSections = null;
    var linker_allow_shlib_undefined: ?bool = null;
    var linker_bind_global_refs_locally: ?bool = null;
    var linker_import_memory: ?bool = null;
    var linker_import_symbols: bool = false;
    var linker_import_table: bool = false;
    var linker_export_table: bool = false;
    var linker_initial_memory: ?u64 = null;
    var linker_max_memory: ?u64 = null;
    var linker_shared_memory: bool = false;
    var linker_global_base: ?u64 = null;
    var linker_print_gc_sections: bool = false;
    var linker_print_icf_sections: bool = false;
    var linker_print_map: bool = false;
    var linker_opt_bisect_limit: i32 = -1;
    var linker_z_nocopyreloc = false;
    var linker_z_nodelete = false;
    var linker_z_notext = false;
    var linker_z_defs = false;
    var linker_z_origin = false;
    var linker_z_now = true;
    var linker_z_relro = true;
    var linker_z_common_page_size: ?u64 = null;
    var linker_z_max_page_size: ?u64 = null;
    var linker_tsaware = false;
    var linker_nxcompat = false;
    var linker_dynamicbase = false;
    var linker_optimization: ?u8 = null;
    var linker_module_definition_file: ?[]const u8 = null;
    var test_evented_io = false;
    var test_no_exec = false;
    var entry: ?[]const u8 = null;
    var stack_size_override: ?u64 = null;
    var image_base_override: ?u64 = null;
    var use_llvm: ?bool = null;
    var use_lld: ?bool = null;
    var use_clang: ?bool = null;
    var link_eh_frame_hdr = false;
    var link_emit_relocs = false;
    var each_lib_rpath: ?bool = null;
    var build_id: ?bool = null;
    var sysroot: ?[]const u8 = null;
    var libc_paths_file: ?[]const u8 = try optionalStringEnvVar(arena, "ZIG_LIBC");
    var machine_code_model: std.builtin.CodeModel = .default;
    var runtime_args_start: ?usize = null;
    var test_filter: ?[]const u8 = null;
    var test_name_prefix: ?[]const u8 = null;
    var test_runner_path: ?[]const u8 = null;
    var override_local_cache_dir: ?[]const u8 = try optionalStringEnvVar(arena, "ZIG_LOCAL_CACHE_DIR");
    var override_global_cache_dir: ?[]const u8 = try optionalStringEnvVar(arena, "ZIG_GLOBAL_CACHE_DIR");
    var override_lib_dir: ?[]const u8 = try optionalStringEnvVar(arena, "ZIG_LIB_DIR");
    var main_pkg_path: ?[]const u8 = null;
    var clang_preprocessor_mode: Compilation.ClangPreprocessorMode = .no;
    var subsystem: ?std.Target.SubSystem = null;
    var major_subsystem_version: ?u32 = null;
    var minor_subsystem_version: ?u32 = null;
    var wasi_exec_model: ?std.builtin.WasiExecModel = null;
    var enable_link_snapshots: bool = false;
    var native_darwin_sdk: ?std.zig.system.darwin.DarwinSDK = null;
    var install_name: ?[]const u8 = null;
    var hash_style: link.HashStyle = .both;
    var entitlements: ?[]const u8 = null;
    var pagezero_size: ?u64 = null;
    var search_strategy: ?link.File.MachO.SearchStrategy = null;
    var headerpad_size: ?u32 = null;
    var headerpad_max_install_names: bool = false;
    var dead_strip_dylibs: bool = false;
    var reference_trace: ?u32 = null;
    var error_tracing: ?bool = null;
    var pdb_out_path: ?[]const u8 = null;

    // e.g. -m3dnow or -mno-outline-atomics. They correspond to std.Target llvm cpu feature names.
    // This array is populated by zig cc frontend and then has to be converted to zig-style
    // CPU features.
    var llvm_m_args = std.ArrayList([]const u8).init(gpa);
    defer llvm_m_args.deinit();

    var system_libs = std.StringArrayHashMap(Compilation.SystemLib).init(gpa);
    defer system_libs.deinit();

    var static_libs = std.ArrayList([]const u8).init(gpa);
    defer static_libs.deinit();

    var wasi_emulated_libs = std.ArrayList(wasi_libc.CRTFile).init(gpa);
    defer wasi_emulated_libs.deinit();

    var clang_argv = std.ArrayList([]const u8).init(gpa);
    defer clang_argv.deinit();

    var extra_cflags = std.ArrayList([]const u8).init(gpa);
    defer extra_cflags.deinit();

    var lib_dirs = std.ArrayList([]const u8).init(gpa);
    defer lib_dirs.deinit();

    var rpath_list = std.ArrayList([]const u8).init(gpa);
    defer rpath_list.deinit();

    var c_source_files = std.ArrayList(Compilation.CSourceFile).init(gpa);
    defer c_source_files.deinit();

    var link_objects = std.ArrayList(Compilation.LinkObject).init(gpa);
    defer link_objects.deinit();

    // This map is a flag per link_objects item, used to represent the
    // `-l :file.so` syntax from gcc/clang.
    // This is only exposed from the `zig cc` interface. It means that the `path`
    // field from the corresponding `link_objects` element is a suffix, and is
    // to be tried against each library path as a prefix until an existing file is found.
    // This map remains empty for the main CLI.
    var link_objects_lib_search_paths: std.AutoHashMapUnmanaged(u32, void) = .{};

    var framework_dirs = std.ArrayList([]const u8).init(gpa);
    defer framework_dirs.deinit();

    var frameworks: std.StringArrayHashMapUnmanaged(Compilation.SystemLib) = .{};

    // null means replace with the test executable binary
    var test_exec_args = std.ArrayList(?[]const u8).init(gpa);
    defer test_exec_args.deinit();

    var linker_export_symbol_names = std.ArrayList([]const u8).init(gpa);
    defer linker_export_symbol_names.deinit();

    // Contains every module specified via --mod. The dependencies are added
    // after argument parsing is completed. We use a StringArrayHashMap to make
    // error output consistent.
    var modules = std.StringArrayHashMap(struct {
        mod: *Package,
        deps_str: []const u8, // still in CLI arg format
    }).init(gpa);
    defer {
        var it = modules.iterator();
        while (it.next()) |kv| kv.value_ptr.mod.destroy(gpa);
        modules.deinit();
    }

    // The dependency string for the root package
    var root_deps_str: ?[]const u8 = null;

    // before arg parsing, check for the NO_COLOR environment variable
    // if it exists, default the color setting to .off
    // explicit --color arguments will still override this setting.
    // Disable color on WASI per https://github.com/WebAssembly/WASI/issues/162
    color = if (builtin.os.tag == .wasi or std.process.hasEnvVarConstant("NO_COLOR")) .off else .auto;

    switch (arg_mode) {
        .build, .translate_c, .zig_test, .run => {
            var optimize_mode_string: ?[]const u8 = null;
            switch (arg_mode) {
                .build => |m| {
                    output_mode = m;
                },
                .translate_c => {
                    emit_bin = .no;
                    output_mode = .Obj;
                },
                .zig_test, .run => {
                    output_mode = .Exe;
                },
                else => unreachable,
            }

            soname = .yes_default_value;

            const Iterator = struct {
                resp_file: ?ArgIteratorResponseFile = null,
                args: []const []const u8,
                i: usize = 0,
                fn next(it: *@This()) ?[]const u8 {
                    if (it.i >= it.args.len) {
                        if (it.resp_file) |*resp| return resp.next();
                        return null;
                    }
                    defer it.i += 1;
                    return it.args[it.i];
                }
                fn nextOrFatal(it: *@This()) []const u8 {
                    if (it.i >= it.args.len) {
                        if (it.resp_file) |*resp| if (resp.next()) |ret| return ret;
                        fatal("expected parameter after {s}", .{it.args[it.i - 1]});
                    }
                    defer it.i += 1;
                    return it.args[it.i];
                }
            };
            var args_iter = Iterator{
                .args = all_args[2..],
            };

            var cssan = ClangSearchSanitizer.init(gpa, &clang_argv);
            defer cssan.map.deinit();

            var file_ext: ?Compilation.FileExt = null;
            args_loop: while (args_iter.next()) |arg| {
                if (mem.startsWith(u8, arg, "@")) {
                    // This is a "compiler response file". We must parse the file and treat its
                    // contents as command line parameters.
                    const resp_file_path = arg[1..];
                    args_iter.resp_file = initArgIteratorResponseFile(arena, resp_file_path) catch |err| {
                        fatal("unable to read response file '{s}': {s}", .{ resp_file_path, @errorName(err) });
                    };
                } else if (mem.startsWith(u8, arg, "-")) {
                    if (mem.eql(u8, arg, "-h") or mem.eql(u8, arg, "--help")) {
                        try io.getStdOut().writeAll(usage_build_generic);
                        return cleanExit();
                    } else if (mem.eql(u8, arg, "--")) {
                        if (arg_mode == .run) {
                            // args_iter.i is 1, referring the next arg after "--" in ["--", ...]
                            // Add +2 to the index so it is relative to all_args
                            runtime_args_start = args_iter.i + 2;
                            break :args_loop;
                        } else {
                            fatal("unexpected end-of-parameter mark: --", .{});
                        }
                    } else if (mem.eql(u8, arg, "--mod")) {
                        const info = args_iter.nextOrFatal();
                        var info_it = mem.split(u8, info, ":");
                        const mod_name = info_it.next() orelse fatal("expected non-empty argument after {s}", .{arg});
                        const deps_str = info_it.next() orelse fatal("expected 'name:deps:path' after {s}", .{arg});
                        const root_src_orig = info_it.rest();
                        if (root_src_orig.len == 0) fatal("expected 'name:deps:path' after {s}", .{arg});
                        if (mod_name.len == 0) fatal("empty name for module at '{s}'", .{root_src_orig});

                        const root_src = try introspect.resolvePath(arena, root_src_orig);

                        for ([_][]const u8{ "std", "root", "builtin" }) |name| {
                            if (mem.eql(u8, mod_name, name)) {
                                fatal("unable to add module '{s}' -> '{s}': conflicts with builtin module", .{ mod_name, root_src });
                            }
                        }

                        var mod_it = modules.iterator();
                        while (mod_it.next()) |kv| {
                            if (std.mem.eql(u8, mod_name, kv.key_ptr.*)) {
                                fatal("unable to add module '{s}' -> '{s}': already exists as '{s}'", .{ mod_name, root_src, kv.value_ptr.mod.root_src_path });
                            }
                        }

                        try modules.ensureUnusedCapacity(1);
                        modules.put(mod_name, .{
                            .mod = try Package.create(
                                gpa,
                                fs.path.dirname(root_src),
                                fs.path.basename(root_src),
                            ),
                            .deps_str = deps_str,
                        }) catch unreachable;
                    } else if (mem.eql(u8, arg, "--deps")) {
                        if (root_deps_str != null) {
                            fatal("only one --deps argument is allowed", .{});
                        }
                        root_deps_str = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--main-pkg-path")) {
                        main_pkg_path = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "-cflags")) {
                        extra_cflags.shrinkRetainingCapacity(0);
                        while (true) {
                            const next_arg = args_iter.next() orelse {
                                fatal("expected -- after -cflags", .{});
                            };
                            if (mem.eql(u8, next_arg, "--")) break;
                            try extra_cflags.append(next_arg);
                        }
                    } else if (mem.eql(u8, arg, "--color")) {
                        const next_arg = args_iter.next() orelse {
                            fatal("expected [auto|on|off] after --color", .{});
                        };
                        color = std.meta.stringToEnum(Color, next_arg) orelse {
                            fatal("expected [auto|on|off] after --color, found '{s}'", .{next_arg});
                        };
                    } else if (mem.eql(u8, arg, "--subsystem")) {
                        subsystem = try parseSubSystem(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "-O")) {
                        optimize_mode_string = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--entry")) {
                        entry = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--stack")) {
                        const next_arg = args_iter.nextOrFatal();
                        stack_size_override = std.fmt.parseUnsigned(u64, next_arg, 0) catch |err| {
                            fatal("unable to parse stack size '{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                    } else if (mem.eql(u8, arg, "--image-base")) {
                        const next_arg = args_iter.nextOrFatal();
                        image_base_override = std.fmt.parseUnsigned(u64, next_arg, 0) catch |err| {
                            fatal("unable to parse image base override '{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                    } else if (mem.eql(u8, arg, "--name")) {
                        provided_name = args_iter.nextOrFatal();
                        if (!mem.eql(u8, provided_name.?, fs.path.basename(provided_name.?)))
                            fatal("invalid package name '{s}': cannot contain folder separators", .{provided_name.?});
                    } else if (mem.eql(u8, arg, "-rpath")) {
                        try rpath_list.append(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "--library-directory") or mem.eql(u8, arg, "-L")) {
                        try lib_dirs.append(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "-F")) {
                        try framework_dirs.append(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "-framework")) {
                        try frameworks.put(gpa, args_iter.nextOrFatal(), .{});
                    } else if (mem.eql(u8, arg, "-weak_framework")) {
                        try frameworks.put(gpa, args_iter.nextOrFatal(), .{ .weak = true });
                    } else if (mem.eql(u8, arg, "-needed_framework")) {
                        try frameworks.put(gpa, args_iter.nextOrFatal(), .{ .needed = true });
                    } else if (mem.eql(u8, arg, "-install_name")) {
                        install_name = args_iter.nextOrFatal();
                    } else if (mem.startsWith(u8, arg, "--compress-debug-sections=")) {
                        const param = arg["--compress-debug-sections=".len..];
                        linker_compress_debug_sections = std.meta.stringToEnum(link.CompressDebugSections, param) orelse {
                            fatal("expected --compress-debug-sections=[none|zlib], found '{s}'", .{param});
                        };
                    } else if (mem.eql(u8, arg, "--compress-debug-sections")) {
                        linker_compress_debug_sections = link.CompressDebugSections.zlib;
                    } else if (mem.eql(u8, arg, "-pagezero_size")) {
                        const next_arg = args_iter.nextOrFatal();
                        pagezero_size = std.fmt.parseUnsigned(u64, eatIntPrefix(next_arg, 16), 16) catch |err| {
                            fatal("unable to parse pagezero size'{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                    } else if (mem.eql(u8, arg, "-search_paths_first")) {
                        search_strategy = .paths_first;
                    } else if (mem.eql(u8, arg, "-search_dylibs_first")) {
                        search_strategy = .dylibs_first;
                    } else if (mem.eql(u8, arg, "-headerpad")) {
                        const next_arg = args_iter.nextOrFatal();
                        headerpad_size = std.fmt.parseUnsigned(u32, eatIntPrefix(next_arg, 16), 16) catch |err| {
                            fatal("unable to parse headerpat size '{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                    } else if (mem.eql(u8, arg, "-headerpad_max_install_names")) {
                        headerpad_max_install_names = true;
                    } else if (mem.eql(u8, arg, "-dead_strip")) {
                        linker_gc_sections = true;
                    } else if (mem.eql(u8, arg, "-dead_strip_dylibs")) {
                        dead_strip_dylibs = true;
                    } else if (mem.eql(u8, arg, "-T") or mem.eql(u8, arg, "--script")) {
                        linker_script = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--version-script")) {
                        version_script = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--library") or mem.eql(u8, arg, "-l")) {
                        // We don't know whether this library is part of libc or libc++ until
                        // we resolve the target, so we simply append to the list for now.
                        try system_libs.put(args_iter.nextOrFatal(), .{});
                    } else if (mem.eql(u8, arg, "--needed-library") or
                        mem.eql(u8, arg, "-needed-l") or
                        mem.eql(u8, arg, "-needed_library"))
                    {
                        const next_arg = args_iter.nextOrFatal();
                        try system_libs.put(next_arg, .{ .needed = true });
                    } else if (mem.eql(u8, arg, "-weak_library") or mem.eql(u8, arg, "-weak-l")) {
                        try system_libs.put(args_iter.nextOrFatal(), .{ .weak = true });
                    } else if (mem.eql(u8, arg, "-D")) {
                        try clang_argv.append(arg);
                        try clang_argv.append(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "-I")) {
                        try cssan.addIncludePath(.I, arg, args_iter.nextOrFatal(), false);
                    } else if (mem.eql(u8, arg, "-isystem") or mem.eql(u8, arg, "-iwithsysroot")) {
                        try cssan.addIncludePath(.isystem, arg, args_iter.nextOrFatal(), false);
                    } else if (mem.eql(u8, arg, "-idirafter")) {
                        try cssan.addIncludePath(.idirafter, arg, args_iter.nextOrFatal(), false);
                    } else if (mem.eql(u8, arg, "-iframework") or mem.eql(u8, arg, "-iframeworkwithsysroot")) {
                        try cssan.addIncludePath(.iframework, arg, args_iter.nextOrFatal(), false);
                    } else if (mem.eql(u8, arg, "--version")) {
                        const next_arg = args_iter.nextOrFatal();
                        version = std.builtin.Version.parse(next_arg) catch |err| {
                            fatal("unable to parse --version '{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                        have_version = true;
                    } else if (mem.eql(u8, arg, "-target")) {
                        target_arch_os_abi = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "-mcpu")) {
                        target_mcpu = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "-mcmodel")) {
                        machine_code_model = parseCodeModel(args_iter.nextOrFatal());
                    } else if (mem.startsWith(u8, arg, "-ofmt=")) {
                        target_ofmt = arg["-ofmt=".len..];
                    } else if (mem.startsWith(u8, arg, "-mcpu=")) {
                        target_mcpu = arg["-mcpu=".len..];
                    } else if (mem.startsWith(u8, arg, "-mcmodel=")) {
                        machine_code_model = parseCodeModel(arg["-mcmodel=".len..]);
                    } else if (mem.startsWith(u8, arg, "-O")) {
                        optimize_mode_string = arg["-O".len..];
                    } else if (mem.eql(u8, arg, "--dynamic-linker")) {
                        target_dynamic_linker = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--sysroot")) {
                        sysroot = args_iter.nextOrFatal();
                        try clang_argv.append("-isysroot");
                        try clang_argv.append(sysroot.?);
                    } else if (mem.eql(u8, arg, "--libc")) {
                        libc_paths_file = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--test-filter")) {
                        test_filter = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--test-name-prefix")) {
                        test_name_prefix = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--test-runner")) {
                        test_runner_path = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--test-cmd")) {
                        try test_exec_args.append(args_iter.nextOrFatal());
                    } else if (mem.eql(u8, arg, "--cache-dir")) {
                        override_local_cache_dir = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--global-cache-dir")) {
                        override_global_cache_dir = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--zig-lib-dir")) {
                        override_lib_dir = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "--debug-log")) {
                        if (!build_options.enable_logging) {
                            std.log.warn("Zig was compiled without logging enabled (-Dlog). --debug-log has no effect.", .{});
                        } else {
                            try log_scopes.append(gpa, args_iter.nextOrFatal());
                        }
                    } else if (mem.eql(u8, arg, "--debug-link-snapshot")) {
                        if (!build_options.enable_link_snapshots) {
                            std.log.warn("Zig was compiled without linker snapshots enabled (-Dlink-snapshot). --debug-link-snapshot has no effect.", .{});
                        } else {
                            enable_link_snapshots = true;
                        }
                    } else if (mem.eql(u8, arg, "--entitlements")) {
                        entitlements = args_iter.nextOrFatal();
                    } else if (mem.eql(u8, arg, "-fcompiler-rt")) {
                        want_compiler_rt = true;
                    } else if (mem.eql(u8, arg, "-fno-compiler-rt")) {
                        want_compiler_rt = false;
                    } else if (mem.eql(u8, arg, "-feach-lib-rpath")) {
                        each_lib_rpath = true;
                    } else if (mem.eql(u8, arg, "-fno-each-lib-rpath")) {
                        each_lib_rpath = false;
                    } else if (mem.eql(u8, arg, "-fbuild-id")) {
                        build_id = true;
                    } else if (mem.eql(u8, arg, "-fno-build-id")) {
                        build_id = false;
                    } else if (mem.eql(u8, arg, "--enable-cache")) {
                        enable_cache = true;
                    } else if (mem.eql(u8, arg, "--test-cmd-bin")) {
                        try test_exec_args.append(null);
                    } else if (mem.eql(u8, arg, "--test-evented-io")) {
                        test_evented_io = true;
                    } else if (mem.eql(u8, arg, "--test-no-exec")) {
                        test_no_exec = true;
                    } else if (mem.eql(u8, arg, "--watch")) {
                        watch = true;
                    } else if (mem.eql(u8, arg, "-ftime-report")) {
                        time_report = true;
                    } else if (mem.eql(u8, arg, "-fstack-report")) {
                        stack_report = true;
                    } else if (mem.eql(u8, arg, "-fPIC")) {
                        want_pic = true;
                    } else if (mem.eql(u8, arg, "-fno-PIC")) {
                        want_pic = false;
                    } else if (mem.eql(u8, arg, "-fPIE")) {
                        want_pie = true;
                    } else if (mem.eql(u8, arg, "-fno-PIE")) {
                        want_pie = false;
                    } else if (mem.eql(u8, arg, "-flto")) {
                        want_lto = true;
                    } else if (mem.eql(u8, arg, "-fno-lto")) {
                        want_lto = false;
                    } else if (mem.eql(u8, arg, "-funwind-tables")) {
                        want_unwind_tables = true;
                    } else if (mem.eql(u8, arg, "-fno-unwind-tables")) {
                        want_unwind_tables = false;
                    } else if (mem.eql(u8, arg, "-fstack-check")) {
                        want_stack_check = true;
                    } else if (mem.eql(u8, arg, "-fno-stack-check")) {
                        want_stack_check = false;
                    } else if (mem.eql(u8, arg, "-fstack-protector")) {
                        want_stack_protector = Compilation.default_stack_protector_buffer_size;
                    } else if (mem.eql(u8, arg, "-fno-stack-protector")) {
                        want_stack_protector = 0;
                    } else if (mem.eql(u8, arg, "-mred-zone")) {
                        want_red_zone = true;
                    } else if (mem.eql(u8, arg, "-mno-red-zone")) {
                        want_red_zone = false;
                    } else if (mem.eql(u8, arg, "-fomit-frame-pointer")) {
                        omit_frame_pointer = true;
                    } else if (mem.eql(u8, arg, "-fno-omit-frame-pointer")) {
                        omit_frame_pointer = false;
                    } else if (mem.eql(u8, arg, "