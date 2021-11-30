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
                    } else if (mem.eql(u8, arg, "-fsanitize-c")) {
                        want_sanitize_c = true;
                    } else if (mem.eql(u8, arg, "-fno-sanitize-c")) {
                        want_sanitize_c = false;
                    } else if (mem.eql(u8, arg, "-fvalgrind")) {
                        want_valgrind = true;
                    } else if (mem.eql(u8, arg, "-fno-valgrind")) {
                        want_valgrind = false;
                    } else if (mem.eql(u8, arg, "-fsanitize-thread")) {
                        want_tsan = true;
                    } else if (mem.eql(u8, arg, "-fno-sanitize-thread")) {
                        want_tsan = false;
                    } else if (mem.eql(u8, arg, "-fLLVM")) {
                        use_llvm = true;
                    } else if (mem.eql(u8, arg, "-fno-LLVM")) {
                        use_llvm = false;
                    } else if (mem.eql(u8, arg, "-fLLD")) {
                        use_lld = true;
                    } else if (mem.eql(u8, arg, "-fno-LLD")) {
                        use_lld = false;
                    } else if (mem.eql(u8, arg, "-fClang")) {
                        use_clang = true;
                    } else if (mem.eql(u8, arg, "-fno-Clang")) {
                        use_clang = false;
                    } else if (mem.eql(u8, arg, "-freference-trace")) {
                        reference_trace = 256;
                    } else if (mem.startsWith(u8, arg, "-freference-trace=")) {
                        const num = arg["-freference-trace=".len..];
                        reference_trace = std.fmt.parseUnsigned(u32, num, 10) catch |err| {
                            fatal("unable to parse reference_trace count '{s}': {s}", .{ num, @errorName(err) });
                        };
                    } else if (mem.eql(u8, arg, "-fno-reference-trace")) {
                        reference_trace = null;
                    } else if (mem.eql(u8, arg, "-ferror-tracing")) {
                        error_tracing = true;
                    } else if (mem.eql(u8, arg, "-fno-error-tracing")) {
                        error_tracing = false;
                    } else if (mem.eql(u8, arg, "-rdynamic")) {
                        rdynamic = true;
                    } else if (mem.eql(u8, arg, "-fsoname")) {
                        soname = .yes_default_value;
                    } else if (mem.startsWith(u8, arg, "-fsoname=")) {
                        soname = .{ .yes = arg["-fsoname=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-soname")) {
                        soname = .no;
                    } else if (mem.eql(u8, arg, "-femit-bin")) {
                        emit_bin = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-bin=")) {
                        emit_bin = .{ .yes = arg["-femit-bin=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-bin")) {
                        emit_bin = .no;
                    } else if (mem.eql(u8, arg, "-femit-h")) {
                        emit_h = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-h=")) {
                        emit_h = .{ .yes = arg["-femit-h=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-h")) {
                        emit_h = .no;
                    } else if (mem.eql(u8, arg, "-femit-asm")) {
                        emit_asm = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-asm=")) {
                        emit_asm = .{ .yes = arg["-femit-asm=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-asm")) {
                        emit_asm = .no;
                    } else if (mem.eql(u8, arg, "-femit-llvm-ir")) {
                        emit_llvm_ir = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-llvm-ir=")) {
                        emit_llvm_ir = .{ .yes = arg["-femit-llvm-ir=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-llvm-ir")) {
                        emit_llvm_ir = .no;
                    } else if (mem.eql(u8, arg, "-femit-llvm-bc")) {
                        emit_llvm_bc = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-llvm-bc=")) {
                        emit_llvm_bc = .{ .yes = arg["-femit-llvm-bc=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-llvm-bc")) {
                        emit_llvm_bc = .no;
                    } else if (mem.eql(u8, arg, "-femit-docs")) {
                        emit_docs = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-docs=")) {
                        emit_docs = .{ .yes = arg["-femit-docs=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-docs")) {
                        emit_docs = .no;
                    } else if (mem.eql(u8, arg, "-femit-analysis")) {
                        emit_analysis = .yes_default_path;
                    } else if (mem.startsWith(u8, arg, "-femit-analysis=")) {
                        emit_analysis = .{ .yes = arg["-femit-analysis=".len..] };
                    } else if (mem.eql(u8, arg, "-fno-emit-analysis")) {
                        emit_analysis = .no;
                    } else if (mem.eql(u8, arg, "-femit-implib")) {
                        emit_implib = .yes_default_path;
                        emit_implib_arg_provided = true;
                    } else if (mem.startsWith(u8, arg, "-femit-implib=")) {
                        emit_implib = .{ .yes = arg["-femit-implib=".len..] };
                        emit_implib_arg_provided = true;
                    } else if (mem.eql(u8, arg, "-fno-emit-implib")) {
                        emit_implib = .no;
                        emit_implib_arg_provided = true;
                    } else if (mem.eql(u8, arg, "-dynamic")) {
                        link_mode = .Dynamic;
                    } else if (mem.eql(u8, arg, "-static")) {
                        link_mode = .Static;
                    } else if (mem.eql(u8, arg, "-fdll-export-fns")) {
                        dll_export_fns = true;
                    } else if (mem.eql(u8, arg, "-fno-dll-export-fns")) {
                        dll_export_fns = false;
                    } else if (mem.eql(u8, arg, "--show-builtin")) {
                        show_builtin = true;
                        emit_bin = .no;
                    } else if (mem.eql(u8, arg, "-fstrip")) {
                        strip = true;
                    } else if (mem.eql(u8, arg, "-fno-strip")) {
                        strip = false;
                    } else if (mem.eql(u8, arg, "-fformatted-panics")) {
                        formatted_panics = true;
                    } else if (mem.eql(u8, arg, "-fno-formatted-panics")) {
                        formatted_panics = false;
                    } else if (mem.eql(u8, arg, "-fsingle-threaded")) {
                        single_threaded = true;
                    } else if (mem.eql(u8, arg, "-fno-single-threaded")) {
                        single_threaded = false;
                    } else if (mem.eql(u8, arg, "-ffunction-sections")) {
                        function_sections = true;
                    } else if (mem.eql(u8, arg, "-fno-function-sections")) {
                        function_sections = false;
                    } else if (mem.eql(u8, arg, "-fbuiltin")) {
                        no_builtin = false;
                    } else if (mem.eql(u8, arg, "-fno-builtin")) {
                        no_builtin = true;
                    } else if (mem.startsWith(u8, arg, "-fopt-bisect-limit=")) {
                        linker_opt_bisect_limit = std.math.lossyCast(i32, parseIntSuffix(arg, "-fopt-bisect-limit=".len));
                    } else if (mem.eql(u8, arg, "--eh-frame-hdr")) {
                        link_eh_frame_hdr = true;
                    } else if (mem.eql(u8, arg, "--emit-relocs")) {
                        link_emit_relocs = true;
                    } else if (mem.eql(u8, arg, "-fallow-shlib-undefined")) {
                        linker_allow_shlib_undefined = true;
                    } else if (mem.eql(u8, arg, "-fno-allow-shlib-undefined")) {
                        linker_allow_shlib_undefined = false;
                    } else if (mem.eql(u8, arg, "-z")) {
                        const z_arg = args_iter.nextOrFatal();
                        if (mem.eql(u8, z_arg, "nodelete")) {
                            linker_z_nodelete = true;
                        } else if (mem.eql(u8, z_arg, "notext")) {
                            linker_z_notext = true;
                        } else if (mem.eql(u8, z_arg, "defs")) {
                            linker_z_defs = true;
                        } else if (mem.eql(u8, z_arg, "undefs")) {
                            linker_z_defs = false;
                        } else if (mem.eql(u8, z_arg, "origin")) {
                            linker_z_origin = true;
                        } else if (mem.eql(u8, z_arg, "nocopyreloc")) {
                            linker_z_nocopyreloc = true;
                        } else if (mem.eql(u8, z_arg, "now")) {
                            linker_z_now = true;
                        } else if (mem.eql(u8, z_arg, "lazy")) {
                            linker_z_now = false;
                        } else if (mem.eql(u8, z_arg, "relro")) {
                            linker_z_relro = true;
                        } else if (mem.eql(u8, z_arg, "norelro")) {
                            linker_z_relro = false;
                        } else if (mem.startsWith(u8, z_arg, "common-page-size=")) {
                            linker_z_common_page_size = parseIntSuffix(z_arg, "common-page-size=".len);
                        } else if (mem.startsWith(u8, z_arg, "max-page-size=")) {
                            linker_z_max_page_size = parseIntSuffix(z_arg, "max-page-size=".len);
                        } else {
                            fatal("unsupported linker extension flag: -z {s}", .{z_arg});
                        }
                    } else if (mem.eql(u8, arg, "--import-memory")) {
                        linker_import_memory = true;
                    } else if (mem.eql(u8, arg, "--import-symbols")) {
                        linker_import_symbols = true;
                    } else if (mem.eql(u8, arg, "--import-table")) {
                        linker_import_table = true;
                    } else if (mem.eql(u8, arg, "--export-table")) {
                        linker_export_table = true;
                    } else if (mem.startsWith(u8, arg, "--initial-memory=")) {
                        linker_initial_memory = parseIntSuffix(arg, "--initial-memory=".len);
                    } else if (mem.startsWith(u8, arg, "--max-memory=")) {
                        linker_max_memory = parseIntSuffix(arg, "--max-memory=".len);
                    } else if (mem.startsWith(u8, arg, "--shared-memory")) {
                        linker_shared_memory = true;
                    } else if (mem.startsWith(u8, arg, "--global-base=")) {
                        linker_global_base = parseIntSuffix(arg, "--global-base=".len);
                    } else if (mem.startsWith(u8, arg, "--export=")) {
                        try linker_export_symbol_names.append(arg["--export=".len..]);
                    } else if (mem.eql(u8, arg, "-Bsymbolic")) {
                        linker_bind_global_refs_locally = true;
                    } else if (mem.eql(u8, arg, "--gc-sections")) {
                        linker_gc_sections = true;
                    } else if (mem.eql(u8, arg, "--no-gc-sections")) {
                        linker_gc_sections = false;
                    } else if (mem.eql(u8, arg, "--debug-compile-errors")) {
                        if (!crash_report.is_enabled) {
                            std.log.warn("Zig was compiled in a release mode. --debug-compile-errors has no effect.", .{});
                        } else {
                            debug_compile_errors = true;
                        }
                    } else if (mem.eql(u8, arg, "--verbose-link")) {
                        verbose_link = true;
                    } else if (mem.eql(u8, arg, "--verbose-cc")) {
                        verbose_cc = true;
                    } else if (mem.eql(u8, arg, "--verbose-air")) {
                        verbose_air = true;
                    } else if (mem.eql(u8, arg, "--verbose-llvm-ir")) {
                        verbose_llvm_ir = true;
                    } else if (mem.eql(u8, arg, "--verbose-cimport")) {
                        verbose_cimport = true;
                    } else if (mem.eql(u8, arg, "--verbose-llvm-cpu-features")) {
                        verbose_llvm_cpu_features = true;
                    } else if (mem.startsWith(u8, arg, "-T")) {
                        linker_script = arg[2..];
                    } else if (mem.startsWith(u8, arg, "-L")) {
                        try lib_dirs.append(arg[2..]);
                    } else if (mem.startsWith(u8, arg, "-F")) {
                        try framework_dirs.append(arg[2..]);
                    } else if (mem.startsWith(u8, arg, "-l")) {
                        // We don't know whether this library is part of libc or libc++ until
                        // we resolve the target, so we simply append to the list for now.
                        try system_libs.put(arg["-l".len..], .{});
                    } else if (mem.startsWith(u8, arg, "-needed-l")) {
                        try system_libs.put(arg["-needed-l".len..], .{ .needed = true });
                    } else if (mem.startsWith(u8, arg, "-weak-l")) {
                        try system_libs.put(arg["-weak-l".len..], .{ .weak = true });
                    } else if (mem.startsWith(u8, arg, "-D")) {
                        try clang_argv.append(arg);
                    } else if (mem.startsWith(u8, arg, "-I")) {
                        try cssan.addIncludePath(.I, arg, arg[2..], true);
                    } else if (mem.eql(u8, arg, "-x")) {
                        const lang = args_iter.nextOrFatal();
                        if (mem.eql(u8, lang, "none")) {
                            file_ext = null;
                        } else if (Compilation.LangToExt.get(lang)) |got_ext| {
                            file_ext = got_ext;
                        } else {
                            fatal("language not recognized: '{s}'", .{lang});
                        }
                    } else if (mem.startsWith(u8, arg, "-mexec-model=")) {
                        wasi_exec_model = std.meta.stringToEnum(std.builtin.WasiExecModel, arg["-mexec-model=".len..]) orelse {
                            fatal("expected [command|reactor] for -mexec-mode=[value], found '{s}'", .{arg["-mexec-model=".len..]});
                        };
                    } else {
                        fatal("unrecognized parameter: '{s}'", .{arg});
                    }
                } else switch (file_ext orelse
                    Compilation.classifyFileExt(arg)) {
                    .object, .static_library, .shared_library => try link_objects.append(.{ .path = arg }),
                    .assembly, .assembly_with_cpp, .c, .cpp, .h, .ll, .bc, .m, .mm, .cu => {
                        try c_source_files.append(.{
                            .src_path = arg,
                            .extra_flags = try arena.dupe([]const u8, extra_cflags.items),
                            // duped when parsing the args.
                            .ext = file_ext,
                        });
                    },
                    .zig => {
                        if (root_src_file) |other| {
                            fatal("found another zig file '{s}' after root source file '{s}'", .{ arg, other });
                        } else root_src_file = arg;
                    },
                    .def, .unknown => {
                        fatal("unrecognized file extension of parameter '{s}'", .{arg});
                    },
                }
            }
            if (optimize_mode_string) |s| {
                optimize_mode = std.meta.stringToEnum(std.builtin.Mode, s) orelse
                    fatal("unrecognized optimization mode: '{s}'", .{s});
            }
        },
        .cc, .cpp => {
            if (build_options.only_c) unreachable;

            emit_h = .no;
            soname = .no;
            ensure_libc_on_non_freestanding = true;
            ensure_libcpp_on_non_freestanding = arg_mode == .cpp;
            want_native_include_dirs = true;
            // Clang's driver enables this switch unconditionally.
            // Disabling the emission of .eh_frame_hdr can unexpectedly break
            // some functionality that depend on it, such as C++ exceptions and
            // DWARF-based stack traces.
            link_eh_frame_hdr = true;

            const COutMode = enum {
                link,
                object,
                assembly,
                preprocessor,
            };
            var c_out_mode: COutMode = .link;
            var out_path: ?[]const u8 = null;
            var is_shared_lib = false;
            var linker_args = std.ArrayList([]const u8).init(arena);
            var it = ClangArgIterator.init(arena, all_args);
            var emit_llvm = false;
            var needed = false;
            var must_link = false;
            var force_static_libs = false;
            var file_ext: ?Compilation.FileExt = null;
            while (it.has_next) {
                it.next() catch |err| {
                    fatal("unable to parse command line parameters: {s}", .{@errorName(err)});
                };
                switch (it.zig_equivalent) {
                    .target => target_arch_os_abi = it.only_arg, // example: -target riscv64-linux-unknown
                    .o => {
                        // We handle -o /dev/null equivalent to -fno-emit-bin because
                        // otherwise our atomic rename into place will fail. This also
                        // makes Zig do less work, avoiding pointless file system operations.
                        if (mem.eql(u8, it.only_arg, "/dev/null")) {
                            emit_bin = .no;
                        } else {
                            out_path = it.only_arg;
                        }
                    },
                    .c => c_out_mode = .object, // -c
                    .asm_only => c_out_mode = .assembly, // -S
                    .preprocess_only => c_out_mode = .preprocessor, // -E
                    .emit_llvm => emit_llvm = true,
                    .x => {
                        const lang = mem.sliceTo(it.only_arg, 0);
                        if (mem.eql(u8, lang, "none")) {
                            file_ext = null;
                        } else if (Compilation.LangToExt.get(lang)) |got_ext| {
                            file_ext = got_ext;
                        } else {
                            fatal("language not recognized: '{s}'", .{lang});
                        }
                    },
                    .other => {
                        try clang_argv.appendSlice(it.other_args);
                    },
                    .positional => switch (file_ext orelse
                        Compilation.classifyFileExt(mem.sliceTo(it.only_arg, 0))) {
                        .assembly, .assembly_with_cpp, .c, .cpp, .ll, .bc, .h, .m, .mm, .cu => {
                            try c_source_files.append(.{
                                .src_path = it.only_arg,
                                .ext = file_ext, // duped while parsing the args.
                            });
                        },
                        .unknown, .shared_library, .object, .static_library => try link_objects.append(.{
                            .path = it.only_arg,
                            .must_link = must_link,
                        }),
                        .def => {
                            linker_module_definition_file = it.only_arg;
                        },
                        .zig => {
                            if (root_src_file) |other| {
                                fatal("found another zig file '{s}' after root source file '{s}'", .{ it.only_arg, other });
                            } else root_src_file = it.only_arg;
                        },
                    },
                    .l => {
                        // -l
                        // We don't know whether this library is part of libc or libc++ until
                        // we resolve the target, so we simply append to the list for now.
                        if (mem.startsWith(u8, it.only_arg, ":")) {
                            // This "feature" of gcc/clang means to treat this as a positional
                            // link object, but using the library search directories as a prefix.
                            try link_objects.append(.{
                                .path = it.only_arg[1..],
                                .must_link = must_link,
                            });
                            const index = @intCast(u32, link_objects.items.len - 1);
                            try link_objects_lib_search_paths.put(arena, index, {});
                        } else if (force_static_libs) {
                            try static_libs.append(it.only_arg);
                        } else {
                            try system_libs.put(it.only_arg, .{ .needed = needed });
                        }
                    },
                    .ignore => {},
                    .driver_punt => {
                        // Never mind what we're doing, just pass the args directly. For example --help.
                        return process.exit(try clangMain(arena, all_args));
                    },
                    .pic => want_pic = true,
                    .no_pic => want_pic = false,
                    .pie => want_pie = true,
                    .no_pie => want_pie = false,
                    .lto => want_lto = true,
                    .no_lto => want_lto = false,
                    .red_zone => want_red_zone = true,
                    .no_red_zone => want_red_zone = false,
                    .omit_frame_pointer => omit_frame_pointer = true,
                    .no_omit_frame_pointer => omit_frame_pointer = false,
                    .function_sections => function_sections = true,
                    .no_function_sections => function_sections = false,
                    .builtin => no_builtin = false,
                    .no_builtin => no_builtin = true,
                    .color_diagnostics => color = .on,
                    .no_color_diagnostics => color = .off,
                    .stack_check => want_stack_check = true,
                    .no_stack_check => want_stack_check = false,
                    .stack_protector => {
                        if (want_stack_protector == null) {
                            want_stack_protector = Compilation.default_stack_protector_buffer_size;
                        }
                    },
                    .no_stack_protector => want_stack_protector = 0,
                    .unwind_tables => want_unwind_tables = true,
                    .no_unwind_tables => want_unwind_tables = false,
                    .nostdlib => {
                        ensure_libc_on_non_freestanding = false;
                        ensure_libcpp_on_non_freestanding = false;
                    },
                    .nostdlib_cpp => ensure_libcpp_on_non_freestanding = false,
                    .shared => {
                        link_mode = .Dynamic;
                        is_shared_lib = true;
                    },
                    .rdynamic => rdynamic = true,
                    .wl => {
                        var split_it = mem.split(u8, it.only_arg, ",");
                        while (split_it.next()) |linker_arg| {
                            // Handle nested-joined args like `-Wl,-rpath=foo`.
                            // Must be prefixed with 1 or 2 dashes.
                            if (linker_arg.len >= 3 and
                                linker_arg[0] == '-' and
                                linker_arg[2] != '-')
                            {
                                if (mem.indexOfScalar(u8, linker_arg, '=')) |equals_pos| {
                                    const key = linker_arg[0..equals_pos];
                                    const value = linker_arg[equals_pos + 1 ..];
                                    if (mem.eql(u8, key, "build-id")) {
                                        build_id = true;
                                        warn("ignoring build-id style argument: '{s}'", .{value});
                                        continue;
                                    } else if (mem.eql(u8, key, "--sort-common")) {
                                        // this ignores --sort=common=<anything>; ignoring plain --sort-common
                                        // is done below.
                                        continue;
                                    }
                                    try linker_args.append(key);
                                    try linker_args.append(value);
                                    continue;
                                }
                            }
                            if (mem.eql(u8, linker_arg, "--as-needed")) {
                                needed = false;
                            } else if (mem.eql(u8, linker_arg, "--no-as-needed")) {
                                needed = true;
                            } else if (mem.eql(u8, linker_arg, "-no-pie")) {
                                want_pie = false;
                            } else if (mem.eql(u8, linker_arg, "--sort-common")) {
                                // from ld.lld(1): --sort-common is ignored for GNU compatibility,
                                // this ignores plain --sort-common
                            } else if (mem.eql(u8, linker_arg, "--whole-archive") or
                                mem.eql(u8, linker_arg, "-whole-archive"))
                            {
                                must_link = true;
                            } else if (mem.eql(u8, linker_arg, "--no-whole-archive") or
                                mem.eql(u8, linker_arg, "-no-whole-archive"))
                            {
                                must_link = false;
                            } else if (mem.eql(u8, linker_arg, "-Bdynamic") or
                                mem.eql(u8, linker_arg, "-dy") or
                                mem.eql(u8, linker_arg, "-call_shared"))
                            {
                                force_static_libs = false;
                            } else if (mem.eql(u8, linker_arg, "-Bstatic") or
                                mem.eql(u8, linker_arg, "-dn") or
                                mem.eql(u8, linker_arg, "-non_shared") or
                                mem.eql(u8, linker_arg, "-static"))
                            {
                                force_static_libs = true;
                            } else if (mem.eql(u8, linker_arg, "-search_paths_first")) {
                                search_strategy = .paths_first;
                            } else if (mem.eql(u8, linker_arg, "-search_dylibs_first")) {
                                search_strategy = .dylibs_first;
                            } else {
                                try linker_args.append(linker_arg);
                            }
                        }
                    },
                    .optimize => {
                        // Alright, what release mode do they want?
                        const level = if (it.only_arg.len >= 1 and it.only_arg[0] == 'O') it.only_arg[1..] else it.only_arg;
                        if (mem.eql(u8, level, "s") or
                            mem.eql(u8, level, "z"))
                        {
                            optimize_mode = .ReleaseSmall;
                        } else if (mem.eql(u8, level, "1") or
                            mem.eql(u8, level, "2") or
                            mem.eql(u8, level, "3") or
                            mem.eql(u8, level, "4") or
                            mem.eql(u8, level, "fast"))
                        {
                            optimize_mode = .ReleaseFast;
                        } else if (mem.eql(u8, level, "g") or
                            mem.eql(u8, level, "0"))
                        {
                            optimize_mode = .Debug;
                        } else {
                            try clang_argv.appendSlice(it.other_args);
                        }
                    },
                    .debug => {
                        strip = false;
                        if (mem.eql(u8, it.only_arg, "g")) {
                            // We handled with strip = false above.
                        } else if (mem.eql(u8, it.only_arg, "g1") or
                            mem.eql(u8, it.only_arg, "gline-tables-only"))
                        {
                            // We handled with strip = false above. but we also want reduced debug info.
                            try clang_argv.append("-gline-tables-only");
                        } else {
                            try clang_argv.appendSlice(it.other_args);
                        }
                    },
                    .sanitize => {
                        if (mem.eql(u8, it.only_arg, "undefined")) {
                            want_sanitize_c = true;
                        } else if (mem.eql(u8, it.only_arg, "thread")) {
                            want_tsan = true;
                        } else {
                            try clang_argv.appendSlice(it.other_args);
                        }
                    },
                    .linker_script => linker_script = it.only_arg,
                    .verbose => {
                        verbose_link = true;
                        // Have Clang print more infos, some tools such as CMake
                        // parse this to discover any implicit include and
                        // library dir to look-up into.
                        try clang_argv.append("-v");
                    },
                    .dry_run => {
                        verbose_link = true;
                        try clang_argv.append("-###");
                        // This flag is supposed to mean "dry run" but currently this
                        // will actually still execute. The tracking issue for this is
                        // https://github.com/ziglang/zig/issues/7170
                    },
                    .for_linker => try linker_args.append(it.only_arg),
                    .linker_input_z => {
                        try linker_args.append("-z");
                        try linker_args.append(it.only_arg);
                    },
                    .lib_dir => try lib_dirs.append(it.only_arg),
                    .mcpu => target_mcpu = it.only_arg,
                    .m => try llvm_m_args.append(it.only_arg),
                    .dep_file => {
                        disable_c_depfile = true;
                        try clang_argv.appendSlice(it.other_args);
                    },
                    .dep_file_to_stdout => { // -M, -MM
                        // "Like -MD, but also implies -E and writes to stdout by default"
                        // "Like -MMD, but also implies -E and writes to stdout by default"
                        c_out_mode = .preprocessor;
                        disable_c_depfile = true;
                        try clang_argv.appendSlice(it.other_args);
                    },
                    .framework_dir => try framework_dirs.append(it.only_arg),
                    .framework => try frameworks.put(gpa, it.only_arg, .{}),
                    .nostdlibinc => want_native_include_dirs = false,
                    .strip => strip = true,
                    .exec_model => {
                        wasi_exec_model = std.meta.stringToEnum(std.builtin.WasiExecModel, it.only_arg) orelse {
                            fatal("expected [command|reactor] for -mexec-mode=[value], found '{s}'", .{it.only_arg});
                        };
                    },
                    .sysroot => {
                        sysroot = it.only_arg;
                    },
                    .entry => {
                        entry = it.only_arg;
                    },
                    .weak_library => try system_libs.put(it.only_arg, .{ .weak = true }),
                    .weak_framework => try frameworks.put(gpa, it.only_arg, .{ .weak = true }),
                    .headerpad_max_install_names => headerpad_max_install_names = true,
                    .compress_debug_sections => {
                        if (it.only_arg.len == 0) {
                            linker_compress_debug_sections = .zlib;
                        } else {
                            linker_compress_debug_sections = std.meta.stringToEnum(link.CompressDebugSections, it.only_arg) orelse {
                                fatal("expected [none|zlib] after --compress-debug-sections, found '{s}'", .{it.only_arg});
                            };
                        }
                    },
                    .install_name => {
                        install_name = it.only_arg;
                    },
                    .undefined => {
                        if (mem.eql(u8, "dynamic_lookup", it.only_arg)) {
                            linker_allow_shlib_undefined = true;
                        } else if (mem.eql(u8, "error", it.only_arg)) {
                            linker_allow_shlib_undefined = false;
                        } else {
                            fatal("unsupported -undefined option '{s}'", .{it.only_arg});
                        }
                    },
                }
            }
            // Parse linker args.
            var i: usize = 0;
            while (i < linker_args.items.len) : (i += 1) {
                const arg = linker_args.items[i];
                if (mem.eql(u8, arg, "-soname") or
                    mem.eql(u8, arg, "--soname"))
                {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const name = linker_args.items[i];
                    soname = .{ .yes = name };
                    // Use it as --name.
                    // Example: libsoundio.so.2
                    var prefix: usize = 0;
                    if (mem.startsWith(u8, name, "lib")) {
                        prefix = 3;
                    }
                    var end: usize = name.len;
                    if (mem.endsWith(u8, name, ".so")) {
                        end -= 3;
                    } else {
                        var found_digit = false;
                        while (end > 0 and std.ascii.isDigit(name[end - 1])) {
                            found_digit = true;
                            end -= 1;
                        }
                        if (found_digit and end > 0 and name[end - 1] == '.') {
                            end -= 1;
                        } else {
                            end = name.len;
                        }
                        if (mem.endsWith(u8, name[prefix..end], ".so")) {
                            end -= 3;
                        }
                    }
                    provided_name = name[prefix..end];
                } else if (mem.eql(u8, arg, "-rpath")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try rpath_list.append(linker_args.items[i]);
                } else if (mem.eql(u8, arg, "--subsystem")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    subsystem = try parseSubSystem(linker_args.items[i]);
                } else if (mem.eql(u8, arg, "-I") or
                    mem.eql(u8, arg, "--dynamic-linker") or
                    mem.eql(u8, arg, "-dynamic-linker"))
                {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    target_dynamic_linker = linker_args.items[i];
                } else if (mem.eql(u8, arg, "-E") or
                    mem.eql(u8, arg, "--export-dynamic") or
                    mem.eql(u8, arg, "-export-dynamic"))
                {
                    rdynamic = true;
                } else if (mem.eql(u8, arg, "--version-script")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    version_script = linker_args.items[i];
                } else if (mem.eql(u8, arg, "-O")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    linker_optimization = std.fmt.parseUnsigned(u8, linker_args.items[i], 10) catch |err| {
                        fatal("unable to parse optimization level '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.startsWith(u8, arg, "-O")) {
                    linker_optimization = std.fmt.parseUnsigned(u8, arg["-O".len..], 10) catch |err| {
                        fatal("unable to parse optimization level '{s}': {s}", .{ arg, @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-pagezero_size")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const next_arg = linker_args.items[i];
                    pagezero_size = std.fmt.parseUnsigned(u64, eatIntPrefix(next_arg, 16), 16) catch |err| {
                        fatal("unable to parse pagezero size '{s}': {s}", .{ next_arg, @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-headerpad")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const next_arg = linker_args.items[i];
                    headerpad_size = std.fmt.parseUnsigned(u32, eatIntPrefix(next_arg, 16), 16) catch |err| {
                        fatal("unable to parse  headerpad size '{s}': {s}", .{ next_arg, @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-headerpad_max_install_names")) {
                    headerpad_max_install_names = true;
                } else if (mem.eql(u8, arg, "-dead_strip")) {
                    linker_gc_sections = true;
                } else if (mem.eql(u8, arg, "-dead_strip_dylibs")) {
                    dead_strip_dylibs = true;
                } else if (mem.eql(u8, arg, "--no-undefined")) {
                    linker_z_defs = true;
                } else if (mem.eql(u8, arg, "--gc-sections")) {
                    linker_gc_sections = true;
                } else if (mem.eql(u8, arg, "--no-gc-sections")) {
                    linker_gc_sections = false;
                } else if (mem.eql(u8, arg, "--print-gc-sections")) {
                    linker_print_gc_sections = true;
                } else if (mem.eql(u8, arg, "--print-icf-sections")) {
                    linker_print_icf_sections = true;
                } else if (mem.eql(u8, arg, "--print-map")) {
                    linker_print_map = true;
                } else if (mem.eql(u8, arg, "--sort-section")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const arg1 = linker_args.items[i];
                    linker_sort_section = std.meta.stringToEnum(link.SortSection, arg1) orelse {
                        fatal("expected [name|alignment] after --sort-section, found '{s}'", .{arg1});
                    };
                } else if (mem.eql(u8, arg, "--allow-shlib-undefined") or
                    mem.eql(u8, arg, "-allow-shlib-undefined"))
                {
                    linker_allow_shlib_undefined = true;
                } else if (mem.eql(u8, arg, "--no-allow-shlib-undefined") or
                    mem.eql(u8, arg, "-no-allow-shlib-undefined"))
                {
                    linker_allow_shlib_undefined = false;
                } else if (mem.eql(u8, arg, "-Bsymbolic")) {
                    linker_bind_global_refs_locally = true;
                } else if (mem.eql(u8, arg, "--import-memory")) {
                    linker_import_memory = true;
                } else if (mem.eql(u8, arg, "--import-symbols")) {
                    linker_import_symbols = true;
                } else if (mem.eql(u8, arg, "--import-table")) {
                    linker_import_table = true;
                } else if (mem.eql(u8, arg, "--export-table")) {
                    linker_export_table = true;
                } else if (mem.startsWith(u8, arg, "--initial-memory=")) {
                    linker_initial_memory = parseIntSuffix(arg, "--initial-memory=".len);
                } else if (mem.startsWith(u8, arg, "--max-memory=")) {
                    linker_max_memory = parseIntSuffix(arg, "--max-memory=".len);
                } else if (mem.startsWith(u8, arg, "--shared-memory")) {
                    linker_shared_memory = true;
                } else if (mem.startsWith(u8, arg, "--global-base=")) {
                    linker_global_base = parseIntSuffix(arg, "--global-base=".len);
                } else if (mem.startsWith(u8, arg, "--export=")) {
                    try linker_export_symbol_names.append(arg["--export=".len..]);
                } else if (mem.eql(u8, arg, "--export")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try linker_export_symbol_names.append(linker_args.items[i]);
                } else if (mem.eql(u8, arg, "--compress-debug-sections")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const arg1 = linker_args.items[i];
                    linker_compress_debug_sections = std.meta.stringToEnum(link.CompressDebugSections, arg1) orelse {
                        fatal("expected [none|zlib] after --compress-debug-sections, found '{s}'", .{arg1});
                    };
                } else if (mem.startsWith(u8, arg, "-z")) {
                    var z_arg = arg[2..];
                    if (z_arg.len == 0) {
                        i += 1;
                        if (i >= linker_args.items.len) {
                            fatal("expected linker extension flag after '{s}'", .{arg});
                        }
                        z_arg = linker_args.items[i];
                    }
                    if (mem.eql(u8, z_arg, "nodelete")) {
                        linker_z_nodelete = true;
                    } else if (mem.eql(u8, z_arg, "notext")) {
                        linker_z_notext = true;
                    } else if (mem.eql(u8, z_arg, "defs")) {
                        linker_z_defs = true;
                    } else if (mem.eql(u8, z_arg, "undefs")) {
                        linker_z_defs = false;
                    } else if (mem.eql(u8, z_arg, "origin")) {
                        linker_z_origin = true;
                    } else if (mem.eql(u8, z_arg, "nocopyreloc")) {
                        linker_z_nocopyreloc = true;
                    } else if (mem.eql(u8, z_arg, "noexecstack")) {
                        // noexecstack is the default when linking with LLD
                    } else if (mem.eql(u8, z_arg, "now")) {
                        linker_z_now = true;
                    } else if (mem.eql(u8, z_arg, "lazy")) {
                        linker_z_now = false;
                    } else if (mem.eql(u8, z_arg, "relro")) {
                        linker_z_relro = true;
                    } else if (mem.eql(u8, z_arg, "norelro")) {
                        linker_z_relro = false;
                    } else if (mem.startsWith(u8, z_arg, "stack-size=")) {
                        const next_arg = z_arg["stack-size=".len..];
                        stack_size_override = std.fmt.parseUnsigned(u64, next_arg, 0) catch |err| {
                            fatal("unable to parse stack size '{s}': {s}", .{ next_arg, @errorName(err) });
                        };
                    } else if (mem.startsWith(u8, z_arg, "common-page-size=")) {
                        linker_z_common_page_size = parseIntSuffix(z_arg, "common-page-size=".len);
                    } else if (mem.startsWith(u8, z_arg, "max-page-size=")) {
                        linker_z_max_page_size = parseIntSuffix(z_arg, "max-page-size=".len);
                    } else {
                        fatal("unsupported linker extension flag: -z {s}", .{z_arg});
                    }
                } else if (mem.eql(u8, arg, "--major-image-version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    version.major = std.fmt.parseUnsigned(u32, linker_args.items[i], 10) catch |err| {
                        fatal("unable to parse major image version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                    have_version = true;
                } else if (mem.eql(u8, arg, "--minor-image-version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    version.minor = std.fmt.parseUnsigned(u32, linker_args.items[i], 10) catch |err| {
                        fatal("unable to parse minor image version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                    have_version = true;
                } else if (mem.eql(u8, arg, "-e") or mem.eql(u8, arg, "--entry")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    entry = linker_args.items[i];
                } else if (mem.eql(u8, arg, "--stack") or mem.eql(u8, arg, "-stack_size")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    stack_size_override = std.fmt.parseUnsigned(u64, linker_args.items[i], 0) catch |err| {
                        fatal("unable to parse stack size override '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "--image-base")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    image_base_override = std.fmt.parseUnsigned(u64, linker_args.items[i], 0) catch |err| {
                        fatal("unable to parse image base override '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-T") or mem.eql(u8, arg, "--script")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    linker_script = linker_args.items[i];
                } else if (mem.eql(u8, arg, "--eh-frame-hdr")) {
                    link_eh_frame_hdr = true;
                } else if (mem.eql(u8, arg, "--no-eh-frame-hdr")) {
                    link_eh_frame_hdr = false;
                } else if (mem.eql(u8, arg, "--tsaware")) {
                    linker_tsaware = true;
                } else if (mem.eql(u8, arg, "--nxcompat")) {
                    linker_nxcompat = true;
                } else if (mem.eql(u8, arg, "--dynamicbase")) {
                    linker_dynamicbase = true;
                } else if (mem.eql(u8, arg, "--high-entropy-va")) {
                    // This option does not do anything.
                } else if (mem.eql(u8, arg, "--export-all-symbols")) {
                    rdynamic = true;
                } else if (mem.eql(u8, arg, "--color-diagnostics") or
                    mem.eql(u8, arg, "--color-diagnostics=always"))
                {
                    color = .on;
                } else if (mem.eql(u8, arg, "--no-color-diagnostics") or
                    mem.eql(u8, arg, "--color-diagnostics=never"))
                {
                    color = .off;
                } else if (mem.eql(u8, arg, "-s") or mem.eql(u8, arg, "--strip-all") or
                    mem.eql(u8, arg, "-S") or mem.eql(u8, arg, "--strip-debug"))
                {
                    // -s, --strip-all             Strip all symbols
                    // -S, --strip-debug           Strip debugging symbols
                    strip = true;
                } else if (mem.eql(u8, arg, "--start-group") or
                    mem.eql(u8, arg, "--end-group"))
                {
                    // We don't need to care about these because these args are
                    // for resolving circular dependencies but our linker takes
                    // care of this without explicit args.
                } else if (mem.eql(u8, arg, "--major-os-version") or
                    mem.eql(u8, arg, "--minor-os-version"))
                {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    // This option does not do anything.
                } else if (mem.eql(u8, arg, "--major-subsystem-version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }

                    major_subsystem_version = std.fmt.parseUnsigned(
                        u32,
                        linker_args.items[i],
                        10,
                    ) catch |err| {
                        fatal("unable to parse major subsystem version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "--minor-subsystem-version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }

                    minor_subsystem_version = std.fmt.parseUnsigned(
                        u32,
                        linker_args.items[i],
                        10,
                    ) catch |err| {
                        fatal("unable to parse minor subsystem version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-framework")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try frameworks.put(gpa, linker_args.items[i], .{});
                } else if (mem.eql(u8, arg, "-weak_framework")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try frameworks.put(gpa, linker_args.items[i], .{ .weak = true });
                } else if (mem.eql(u8, arg, "-needed_framework")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try frameworks.put(gpa, linker_args.items[i], .{ .needed = true });
                } else if (mem.eql(u8, arg, "-needed_library")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try system_libs.put(linker_args.items[i], .{ .needed = true });
                } else if (mem.startsWith(u8, arg, "-weak-l")) {
                    try system_libs.put(arg["-weak-l".len..], .{ .weak = true });
                } else if (mem.eql(u8, arg, "-weak_library")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try system_libs.put(linker_args.items[i], .{ .weak = true });
                } else if (mem.eql(u8, arg, "-compatibility_version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    compatibility_version = std.builtin.Version.parse(linker_args.items[i]) catch |err| {
                        fatal("unable to parse -compatibility_version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                } else if (mem.eql(u8, arg, "-current_version")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    version = std.builtin.Version.parse(linker_args.items[i]) catch |err| {
                        fatal("unable to parse -current_version '{s}': {s}", .{ linker_args.items[i], @errorName(err) });
                    };
                    have_version = true;
                } else if (mem.eql(u8, arg, "--out-implib") or
                    mem.eql(u8, arg, "-implib"))
                {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    emit_implib = .{ .yes = linker_args.items[i] };
                    emit_implib_arg_provided = true;
                } else if (mem.eql(u8, arg, "-undefined")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    if (mem.eql(u8, "dynamic_lookup", linker_args.items[i])) {
                        linker_allow_shlib_undefined = true;
                    } else if (mem.eql(u8, "error", linker_args.items[i])) {
                        linker_allow_shlib_undefined = false;
                    } else {
                        fatal("unsupported -undefined option '{s}'", .{linker_args.items[i]});
                    }
                } else if (mem.eql(u8, arg, "-install_name")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    install_name = linker_args.items[i];
                } else if (mem.eql(u8, arg, "-force_load")) {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    try link_objects.append(.{
                        .path = linker_args.items[i],
                        .must_link = true,
                    });
                } else if (mem.eql(u8, arg, "-hash-style") or
                    mem.eql(u8, arg, "--hash-style"))
                {
                    i += 1;
                    if (i >= linker_args.items.len) {
                        fatal("expected linker arg after '{s}'", .{arg});
                    }
                    const next_arg = linker_args.items[i];
                    hash_style = std.meta.stringToEnum(link.HashStyle, next_arg) orelse {
                        fatal("expected [sysv|gnu|both] after --hash-style, found '{s}'", .{
                            next_arg,
                        });
                    };
                } else if (mem.startsWith(u8, arg, "/subsystem:")) {
                    var split_it = mem.splitBackwards(u8, arg, ":");
                    subsystem = try parseSubSystem(split_it.first());
                } else if (mem.startsWith(u8, arg, "/implib:")) {
                    var split_it = mem.splitBackwards(u8, arg, ":");
                    emit_implib = .{ .yes = split_it.first() };
                    emit_implib_arg_provided = true;
                } else if (mem.startsWith(u8, arg, "/pdb:")) {
                    var split_it = mem.splitBackwards(u8, arg, ":");
                    pdb_out_path = split_it.first();
                } else if (mem.startsWith(u8, arg, "/version:")) {
                    var split_it = mem.splitBackwards(u8, arg, ":");
                    const version_arg = split_it.first();
                    version = std.builtin.Version.parse(version_arg) catch |err| {
                        fatal("unable to parse /version '{s}': {s}", .{ arg, @errorName(err) });
                    };

                    have_version = true;
                } else {
                    fatal("unsupported linker arg: {s}", .{arg});
                }
            }

            if (want_sanitize_c) |wsc| {
                if (wsc and optimize_mode == .ReleaseFast) {
                    optimize_mode = .ReleaseSafe;
                }
            }

            switch (c_out_mode) {
                .link => {
                    output_mode = if (is_shared_lib) .Lib else .Exe;
                    emit_bin = if (out_path) |p| .{ .yes = p } else EmitBin.yes_a_out;
                    if (emit_llvm) {
                        fatal("-emit-llvm cannot be used when linking", .{});
                    }
                },
                .object => {
                    output_mode = .Obj;
                    if (emit_llvm) {
                        emit_bin = .no;
                        if (out_path) |p| {
                            emit_llvm_bc = .{ .yes = p };
                        } else {
                            emit_llvm_bc = .yes_default_path;
                        }
                    } else {
                        if (out_path) |p| {
                            emit_bin = .{ .yes = p };
                        } else {
                            emit_bin = .yes_default_path;
                        }
                    }
                },
                .assembly => {
                    output_mode = .Obj;
                    emit_bin = .no;
                    if (emit_llvm) {
                        if (out_path) |p| {
                            emit_llvm_ir = .{ .yes = p };
                        } else {
                            emit_llvm_ir = .yes_default_path;
                        }
                    } else {
                        if (out_path) |p| {
                            emit_asm = .{ .yes = p };
                        } else {
                            emit_asm = .yes_default_path;
                        }
                    }
                },
                .preprocessor => {
                    output_mode = .Obj;
                    // An error message is generated when there is more than 1 C source file.
                    if (c_source_files.items.len != 1) {
                        // For example `zig cc` and no args should print the "no input files" message.
                        return process.exit(try clangMain(arena, all_args));
                    }
                    if (out_path) |p| {
                        emit_bin = .{ .yes = p };
                        clang_preprocessor_mode = .yes;
                    } else {
                        clang_preprocessor_mode = .stdout;
                    }
                },
            }
            if (c_source_files.items.len == 0 and
                link_objects.items.len == 0 and
                root_src_file == null)
            {
                // For example `zig cc` and no args should print the "no input files" message.
                // There could be other reasons to punt to clang, for example, --help.
                return process.exit(try clangMain(arena, all_args));
            }
        },
    }

    {
        // Resolve module dependencies
        var it = modules.iterator();
        while (it.next()) |kv| {
            const deps_str = kv.value_ptr.deps_str;
            var deps_it = ModuleDepIterator.init(deps_str);
            while (deps_it.next()) |dep| {
                if (dep.expose.len == 0) {
                    fatal("module '{s}' depends on '{s}' with a blank name", .{ kv.key_ptr.*, dep.name });
                }

                for ([_][]const u8{ "std", "root", "builtin" }) |name| {
                    if (mem.eql(u8, dep.expose, name)) {
                        fatal("unable to add module '{s}' under name '{s}': conflicts with builtin module", .{ dep.name, dep.expose });
                    }
                }

                const dep_mod = modules.get(dep.name) orelse
                    fatal("module '{s}' depends on module '{s}' which does not exist", .{ kv.key_ptr.*, dep.name });

                try kv.value_ptr.mod.add(gpa, dep.expose, dep_mod.mod);
            }
        }
    }

    if (arg_mode == .build and optimize_mode == .ReleaseSmall and strip == null)
        strip = true;

    if (arg_mode == .translate_c and c_source_files.items.len != 1) {
        fatal("translate-c expects exactly 1 source file (found {d})", .{c_source_files.items.len});
    }

    if (root_src_file == null and arg_mode == .zig_test) {
        fatal("`zig test` expects a zig source file argument", .{});
    }

    const root_name = if (provided_name) |n| n else blk: {
        if (arg_mode == .zig_test) {
            break :blk "test";
        } else if (root_src_file) |file| {
            const basename = fs.path.basename(file);
            break :blk basename[0 .. basename.len - fs.path.extension(basename).len];
        } else if (c_source_files.items.len >= 1) {
            const basename = fs.path.basename(c_source_files.items[0].src_path);
            break :blk basename[0 .. basename.len - fs.path.extension(basename).len];
        } else if (link_objects.items.len >= 1) {
            const basename = fs.path.basename(link_objects.items[0].path);
            break :blk basename[0 .. basename.len - fs.path.extension(basename).len];
        } else if (emit_bin == .yes) {
            const basename = fs.path.basename(emit_bin.yes);
            break :blk basename[0 .. basename.len - fs.path.extension(basename).len];
        } else if (show_builtin) {
            break :blk "builtin";
        } else if (arg_mode == .run) {
            fatal("`zig run` expects at least one positional argument", .{});
            // TODO once the attempt to unwrap error: LinkingWithoutZigSourceUnimplemented
            // is solved, remove the above fatal() and uncomment the `break` below.
            //break :blk "run";
        } else {
            fatal("expected a positional argument, -femit-bin=[path], --show-builtin, or --name [name]", .{});
        }
    };

    var target_parse_options: std.zig.CrossTarget.ParseOptions = .{
        .arch_os_abi = target_arch_os_abi,
        .cpu_features = target_mcpu,
        .dynamic_linker = target_dynamic_linker,
        .object_format = target_ofmt,
    };

    // Before passing the mcpu string in for parsing, we convert any -m flags that were
    // passed in via zig cc to zig-style.
    if (llvm_m_args.items.len != 0) {
        // If this returns null, we let it fall through to the case below which will
        // run the full parse function and do proper error handling.
        if (std.zig.CrossTarget.parseCpuArch(target_parse_options)) |cpu_arch| {
            var llvm_to_zig_name = std.StringHashMap([]const u8).init(gpa);
            defer llvm_to_zig_name.deinit();

            for (cpu_arch.allFeaturesList()) |feature| {
                const llvm_name = feature.llvm_name orelse continue;
                try llvm_to_zig_name.put(llvm_name, feature.name);
            }

            var mcpu_buffer = std.ArrayList(u8).init(gpa);
            defer mcpu_buffer.deinit();

            try mcpu_buffer.appendSlice(target_mcpu orelse "baseline");

            for (llvm_m_args.items) |llvm_m_arg| {
                if (mem.startsWith(u8, llvm_m_arg, "mno-")) {
                    const llvm_name = llvm_m_arg["mno-".len..];
                    const zig_name = llvm_to_zig_name.get(llvm_name) orelse {
                        fatal("target architecture {s} has no LLVM CPU feature named '{s}'", .{
                            @tagName(cpu_arch), llvm_name,
                        });
                    };
                    try mcpu_buffer.append('-');
                    try mcpu_buffer.appendSlice(zig_name);
                } else if (mem.startsWith(u8, llvm_m_arg, "m")) {
                    const llvm_name = llvm_m_arg["m".len..];
                    const zig_name = llvm_to_zig_name.get(llvm_name) orelse {
                        fatal("target architecture {s} has no LLVM CPU feature named '{s}'", .{
                            @tagName(cpu_arch), llvm_name,
                        });
                    };
                    try mcpu_buffer.append('+');
                    try mcpu_buffer.appendSlice(zig_name);
                } else {
                    unreachable;
                }
            }

            const adjusted_target_mcpu = try arena.dupe(u8, mcpu_buffer.items);
            std.log.debug("adjusted target_mcpu: {s}", .{adjusted_target_mcpu});
            target_parse_options.cpu_features = adjusted_target_mcpu;
        }
    }

    const cross_target = try parseCrossTargetOrReportFatalError(arena, target_parse_options);
    const target_info = try detectNativeTargetInfo(cross_target);

    if (target_info.target.os.tag != .freestanding) {
        if (ensure_libc_on_non_freestanding)
            link_libc = true;
        if (ensure_libcpp_on_non_freestanding)
            link_libcpp = true;
    }

    if (target_info.target.cpu.arch.isWasm() and linker_shared_memory) {
        if (output_mode == .Obj) {
            fatal("shared memory is not allowed in object files", .{});
        }

        if (!target_info.target.cpu.features.isEnabled(@enumToInt(std.Target.wasm.Feature.atomics)) or
            !target_info.target.cpu.features.isEnabled(@enumToInt(std.Target.wasm.Feature.bulk_memory)))
        {
            fatal("'atomics' and 'bulk-memory' features must be enabled to use shared memory", .{});
        }
    }

    // Now that we have target info, we can find out if any of the system libraries
    // are part of libc or libc++. We remove them from the list and communicate their
    // existence via flags instead.
    {
        // Similarly, if any libs in this list are statically provided, we remove
        // them from this list and populate the link_objects array instead.
        const sep = fs.path.sep_str;
        var test_path = std.ArrayList(u8).init(gpa);
        defer test_path.deinit();

        var i: usize = 0;
        syslib: while (i < system_libs.count()) {
            const lib_name = system_libs.keys()[i];

            if (target_util.is_libc_lib_name(target_info.target, lib_name)) {
                link_libc = true;
                system_libs.orderedRemoveAt(i);
                continue;
            }
            if (target_util.is_libcpp_lib_name(target_info.target, lib_name)) {
                link_libcpp = true;
                system_libs.orderedRemoveAt(i);
                continue;
            }
            switch (target_util.classifyCompilerRtLibName(target_info.target, lib_name)) {
                .none => {},
                .only_libunwind, .both => {
                    link_libunwind = true;
                    system_libs.orderedRemoveAt(i);
                    continue;
                },
                .only_compiler_rt => {
                    std.log.warn("ignoring superfluous library '{s}': this dependency is fulfilled instead by compiler-rt which zig unconditionally provides", .{lib_name});
                    system_libs.orderedRemoveAt(i);
                    continue;
                },
            }

            if (fs.path.isAbsolute(lib_name)) {
                fatal("cannot use absolute path as a system library: {s}", .{lib_name});
            }

            if (target_info.target.os.tag == .wasi) {
                if (wasi_libc.getEmulatedLibCRTFile(lib_name)) |crt_file| {
                    try wasi_emulated_libs.append(crt_file);
                    system_libs.orderedRemoveAt(i);
                    continue;
                }
            }

            for (lib_dirs.items) |lib_dir_path| {
                if (cross_target.isDarwin()) break; // Targeting Darwin we let the linker resolve the libraries in the correct order
                test_path.clearRetainingCapacity();
                try test_path.writer().print("{s}" ++ sep ++ "{s}{s}{s}", .{
                    lib_dir_path,
                    target_info.target.libPrefix(),
                    lib_name,
                    target_info.target.staticLibSuffix(),
                });
                fs.cwd().access(test_path.items, .{}) catch |err| switch (err) {
                    error.FileNotFound => continue,
                    else => |e| fatal("unable to search for static library '{s}': {s}", .{
                        test_path.items, @errorName(e),
                    }),
                };
                try link_objects.append(.{ .path = try arena.dupe(u8, test_path.items) });
                system_libs.orderedRemoveAt(i);
                continue :syslib;
            }

            // Unfortunately, in the case of MinGW we also need to look for `libfoo.a`.
            if (target_info.target.isMinGW()) {
                for (lib_dirs.items) |lib_dir_path| {
                    test_path.clearRetainingCapacity();
                    try test_path.writer().print("{s}" ++ sep ++ "lib{s}.a", .{
                        lib_dir_path, lib_name,
                    });
                    fs.cwd().access(test_path.items, .{}) catch |err| switch (err) {
                        error.FileNotFound => continue,
                        else => |e| fatal("unable to search for static library '{s}': {s}", .{
                            test_path.items, @errorName(e),
                        }),
                    };
                    try link_objects.append(.{ .path = try arena.dupe(u8, test_path.items) });
                    system_libs.orderedRemoveAt(i);
                    continue :syslib;
                }
            }

            std.log.scoped(.cli).debug("depending on system for -l{s}", .{lib_name});

            i += 1;
        }
    }
    // libc++ depends on libc
    if (link_libcpp) {
        link_libc = true;
    }

    if (use_lld) |opt| {
        if (opt and cross_target.isDarwin()) {
            fatal("LLD requested with Mach-O object format. Only the self-hosted linker is supported for this target.", .{});
        }
    }

    if (want_lto) |opt| {
        if (opt and cross_target.isDarwin()) {
            fatal("LTO is not yet supported with the Mach-O object format. More details: https://github.com/ziglang/zig/issues/8680", .{});
        }
    }

    if (comptime builtin.target.isDarwin()) {
        // If we want to link against frameworks, we need system headers.
        if (framework_dirs.items.len > 0 or frameworks.count() > 0)
            want_native_include_dirs = true;
    }

    if (sysroot == null and cross_target.isNativeOs() and
        (system_libs.count() != 0 or want_native_include_dirs))
    {
        const paths = std.zig.system.NativePaths.detect(arena, target_info) catch |err| {
            fatal("unable to detect native system paths: {s}", .{@errorName(err)});
        };
        for (paths.warnings.items) |warning| {
            warn("{s}", .{warning});
        }

        const has_sysroot = if (comptime builtin.target.isDarwin()) outer: {
            if (std.zig.system.darwin.isDarwinSDKInstalled(arena)) {
                const sdk = std.zig.system.darwin.getDarwinSDK(arena, target_info.target) orelse
                    break :outer false;
                native_darwin_sdk = sdk;
                try clang_argv.ensureUnusedCapacity(2);
                clang_argv.appendAssumeCapacity("-isysroot");
                clang_argv.appendAssumeCapacity(sdk.path);
                break :outer true;
            } else break :outer false;
        } else false;

        try clang_argv.ensureUnusedCapacity(paths.include_dirs.items.len * 2);
        const isystem_flag = if (has_sysroot) "-iwithsysroot" else "-isystem";
        for (paths.include_dirs.items) |include_dir| {
            clang_argv.appendAssumeCapacity(isystem_flag);
            clang_argv.appendAssumeCapacity(include_dir);
        }

        try clang_argv.ensureUnusedCapacity(paths.framework_dirs.items.len * 2);
        try framework_dirs.ensureUnusedCapacity(paths.framework_dirs.items.len);
        const iframework_flag = if (has_sysroot) "-iframeworkwithsysroot" else "-iframework";
        for (paths.framework_dirs.items) |framework_dir| {
            clang_argv.appendAssumeCapacity(iframework_flag);
            clang_argv.appendAssumeCapacity(framework_dir);
            framework_dirs.appendAssumeCapacity(framework_dir);
        }

        for (paths.lib_dirs.items) |lib_dir| {
            try lib_dirs.append(lib_dir);
        }
        for (paths.rpaths.items) |rpath| {
            try rpath_list.append(rpath);
        }
    }

    {
        // Resolve static libraries into full paths.
        const sep = fs.path.sep_str;

        var test_path = std.ArrayList(u8).init(gpa);
        defer test_path.deinit();

        for (static_libs.items) |static_lib| {
            for (lib_dirs.items) |lib_dir_path| {
                test_path.clearRetainingCapacity();
                try test_path.writer().print("{s}" ++ sep ++ "{s}{s}{s}", .{
                    lib_dir_path,
                    target_info.target.libPrefix(),
                    static_lib,
                    target_info.target.staticLibSuffix(),
                });
                fs.cwd().access(test_path.items, .{}) catch |err| switch (err) {
                    error.FileNotFound => continue,
                    else => |e| fatal("unable to search for static library '{s}': {s}", .{
                        test_path.items, @errorName(e),
                    }),
                };
                try link_objects.append(.{ .path = try arena.dupe(u8, test_path.items) });
                break;
            } else {
                var search_paths = std.ArrayList(u8).init(arena);
                for (lib_dirs.items) |lib_dir_path| {
                    try search_paths.writer().print("\n {s}" ++ sep ++ "{s}{s}{s}", .{
                        lib_dir_path,
                        target_info.target.libPrefix(),
                        static_lib,
                        target_info.target.staticLibSuffix(),
                    });
                }
                try search_paths.appendSlice("\n suggestion: use full paths to static libraries on the command line rather than using -l and -L arguments");
                fatal("static library '{s}' not found. search paths: {s}", .{
                    static_lib, search_paths.items,
                });
            }
        }
    }

    // Resolve `-l :file.so` syntax from `zig cc`. We use a separate map for this data
    // since this is an uncommon case.
    {
        var it = link_objects_lib_search_paths.iterator();
        while (it.next()) |item| {
            const link_object_i = item.key_ptr.*;
            const suffix = link_objects.items[link_object_i].path;

            for (lib_dirs.items) |lib_dir_path| {
                const test_path = try fs.path.join(arena, &.{ lib_dir_path, suffix });
                fs.cwd().access(test_path, .{}) catch |err| switch (err) {
                    error.FileNotFound => continue,
                    else => |e| fatal("unable to search for library '{s}': {s}", .{
                        test_path, @errorName(e),
                    }),
                };
                link_objects.items[link_object_i].path = test_path;
                break;
            } else {
                fatal("library '{s}' not found", .{suffix});
            }
        }
    }

    const object_format = target_info.target.ofmt;

    if (output_mode == .Obj and (object_format == .coff or object_format == .macho)) {
        const total_obj_count = c_source_files.items.len +
            @boolToInt(root_src_file != null) +
            link_objects.items.len;
        if (total_obj_count > 1) {
            fatal("{s} does not support linking multiple objects into one", .{@tagName(object_format)});
        }
    }

    var cleanup_emit_bin_dir: ?fs.Dir = null;
    defer if (cleanup_emit_bin_dir) |*dir| dir.close();

    const have_enable_cache = enable_cache orelse false;
    const optional_version = if (have_version) version else null;

    const resolved_soname: ?[]const u8 = switch (soname) {
        .yes => |explicit| explicit,
        .no => null,
        .yes_default_value => switch (object_format) {
            .elf => if (have_version)
                try std.fmt.allocPrint(arena, "lib{s}.so.{d}", .{ root_name, version.major })
            else
                try std.fmt.allocPrint(arena, "lib{s}.so", .{root_name}),
            else => null,
        },
    };

    const a_out_basename = switch (object_format) {
        .coff => "a.exe",
        else => "a.out",
    };

    const emit_bin_loc: ?Compilation.EmitLoc = switch (emit_bin) {
        .no => null,
        .yes_default_path => Compilation.EmitLoc{
            .directory = blk: {
                switch (arg_mode) {
                    .run, .zig_test => break :blk null,
                    else => {
                        if (have_enable_cache) {
                            break :blk null;
                        } else {
                            break :blk .{ .path = null, .handle = fs.cwd() };
                        }
                    },
                }
            },
            .basename = try std.zig.binNameAlloc(arena, .{
                .root_name = root_name,
                .target = target_info.target,
                .output_mode = output_mode,
                .link_mode = link_mode,
                .version = optional_version,
            }),
        },
        .yes => |full_path| b: {
            const basename = fs.path.basename(full_path);
            if (have_enable_cache) {
                break :b Compilation.EmitLoc{
                    .basename = basename,
                    .directory = null,
                };
            }
            if (fs.path.dirname(full_path)) |dirname| {
                const handle = fs.cwd().openDir(dirname, .{}) catch |err| {
                    fatal("unable to open output directory '{s}': {s}", .{ dirname, @errorName(err) });
                };
                cleanup_emit_bin_dir = handle;
                break :b Compilation.EmitLoc{
                    .basename = basename,
                    .directory = .{
                        .path = dirname,
                        .handle = handle,
                    },
                };
            } else {
                break :b Compilation.EmitLoc{
                    .basename = basename,
                    .directory = .{ .path = null, .handle = fs.cwd() },
                };
            }
        },
        .yes_a_out => Compilation.EmitLoc{
            .directory = .{ .path = null, .handle = fs.cwd() },
            .basename = a_out_basename,
        },
    };

    const default_h_basename = try std.fmt.allocPrint(arena, "{s}.h", .{root_name});
    var emit_h_resolved = emit_h.resolve(default_h_basename) catch |err| {
        switch (emit_h) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-h', '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory from arguments '--name' or '-fsoname', '{s}': {s}", .{
                    default_h_basename, @errorName(err),
                });
            },
            .no => unreachable,
        }
    };
    defer emit_h_resolved.deinit();

    const default_asm_basename = try std.fmt.allocPrint(arena, "{s}.s", .{root_name});
    var emit_asm_resolved = emit_asm.resolve(default_asm_basename) catch |err| {
        switch (emit_asm) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-asm', '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory from arguments '--name' or '-fsoname', '{s}': {s}", .{
                    default_asm_basename, @errorName(err),
                });
            },
            .no => unreachable,
        }
    };
    defer emit_asm_resolved.deinit();

    const default_llvm_ir_basename = try std.fmt.allocPrint(arena, "{s}.ll", .{root_name});
    var emit_llvm_ir_resolved = emit_llvm_ir.resolve(default_llvm_ir_basename) catch |err| {
        switch (emit_llvm_ir) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-llvm-ir', '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory from arguments '--name' or '-fsoname', '{s}': {s}", .{
                    default_llvm_ir_basename, @errorName(err),
                });
            },
            .no => unreachable,
        }
    };
    defer emit_llvm_ir_resolved.deinit();

    const default_llvm_bc_basename = try std.fmt.allocPrint(arena, "{s}.bc", .{root_name});
    var emit_llvm_bc_resolved = emit_llvm_bc.resolve(default_llvm_bc_basename) catch |err| {
        switch (emit_llvm_bc) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-llvm-bc', '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory from arguments '--name' or '-fsoname', '{s}': {s}", .{
                    default_llvm_bc_basename, @errorName(err),
                });
            },
            .no => unreachable,
        }
    };
    defer emit_llvm_bc_resolved.deinit();

    const default_analysis_basename = try std.fmt.allocPrint(arena, "{s}-analysis.json", .{root_name});
    var emit_analysis_resolved = emit_analysis.resolve(default_analysis_basename) catch |err| {
        switch (emit_analysis) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-analysis',  '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory from arguments 'name' or 'soname', '{s}': {s}", .{
                    default_analysis_basename, @errorName(err),
                });
            },
            .no => unreachable,
        }
    };
    defer emit_analysis_resolved.deinit();

    var emit_docs_resolved = emit_docs.resolve("docs") catch |err| {
        switch (emit_docs) {
            .yes => |p| {
                fatal("unable to open directory from argument '-femit-docs', '{s}': {s}", .{
                    p, @errorName(err),
                });
            },
            .yes_default_path => {
                fatal("unable to open directory 'docs': {s}", .{@errorName(err)});
            },
            .no => unreachable,
        }
    };
    defer emit_docs_resolved.deinit();

    const is_exe_or_dyn_lib = switch (output_mode) {
        .Obj => false,
        .Lib => (link_mode orelse .Static) == .Dynamic,
        .Exe => true,
    };
    // Note that cmake when targeting Windows will try to execute
    // zig cc to make an executable and output an implib too.
    const implib_eligible = is_exe_or_dyn_lib and
        emit_bin_loc != null and target_info.target.os.tag == .windows;
    if (!implib_eligible) {
        if (!emit_implib_arg_provided) {
            emit_implib = .no;
        } else if (emit_implib != .no) {
            fatal("the argument -femit-implib is allowed only when building a Windows DLL", .{});
        }
    }
    const default_implib_basename = try std.fmt.allocPrint(arena, "{s}.lib", .{root_name});
    var emit_implib_resolved = switch (emit_implib) {
        .no => Emit.Resolved{ .data = null, .dir = null },
        .yes => |p| emit_implib.resolve(default_implib_basename) catch |err| {
            fatal("unable to open directory from argument '-femit-implib', '{s}': {s}", .{
                p, @errorName(err),
            });
        },
        .yes_default_path => Emit.Resolved{
            .data = Compilation.EmitLoc{
                .directory = emit_bin_loc.?.directory,
                .basename = default_implib_basename,
            },
            .dir = null,
        },
    };
    defer emit_implib_resolved.deinit();

    const main_pkg: ?*Package = if (root_src_file) |unresolved_src_path| blk: {
        const src_path = try introspect.resolvePath(arena, unresolved_src_path);
        if (main_pkg_path) |unresolved_main_pkg_path| {
            const p = try introspect.resolvePath(arena, unresolved_main_pkg_path);
            if (p.len == 0) {
                break :blk try Package.create(gpa, null, src_path);
            } else {
                const rel_src_path = try fs.path.relative(arena, p, src_path);
                break :blk try Package.create(gpa, p, rel_src_path);
            }
        } else {
            const root_src_dir_path = fs.path.dirname(src_path);
            break :blk Package.create(gpa, root_src_dir_path, fs.path.basename(src_path)) catch |err| {
                if (root_src_dir_path) |p| {
                    fatal("unable to open '{s}': {s}", .{ p, @errorName(err) });
                } else {
                    return err;
                }
            };
        }
    } else null;
    defer if (main_pkg) |p| p.destroy(gpa);

    // Transfer packages added with --deps to the root package
    if (main_pkg) |mod| {
        var it = ModuleDepIterator.init(root_deps_str orelse "");
        while (it.next()) |dep| {
            if (dep.expose.len == 0) {
      