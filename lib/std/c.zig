const std = @import("std");
const builtin = @import("builtin");
const c = @This();
const page_size = std.mem.page_size;
const iovec = std.os.iovec;
const iovec_const = std.os.iovec_const;

test {
    _ = tokenizer;
}

pub const tokenizer = @import("c/tokenizer.zig");
pub const Token = tokenizer.Token;
pub const Tokenizer = tokenizer.Tokenizer;

/// The return type is `type` to force comptime function call execution.
/// TODO: https://github.com/ziglang/zig/issues/425
/// If not linking libc, returns struct{pub const ok = false;}
/// If linking musl libc, returns struct{pub const ok = true;}
/// If linking gnu libc (glibc), the `ok` value will be true if the target
/// version is greater than or equal to `glibc_version`.
/// If linking a libc other than these, returns `false`.
pub fn versionCheck(comptime glibc_version: std.builtin.Version) type {
    return struct {
        pub const ok = blk: {
            if (!builtin.link_libc) break :blk false;
            if (builtin.abi.isMusl()) break :blk true;
            if (builtin.target.isGnuLibC()) {
                const ver = builtin.os.version_range.linux.glibc;
                const order = ver.order(glibc_version);
                break :blk switch (order) {
                    .gt, .eq => true,
                    .lt => false,
                };
            } else {
                break :blk false;
            }
        };
    };
}

pub usingnamespace switch (builtin.os.tag) {
    .linux => @import("c/linux.zig"),
    .windows => @import("c/windows.zig"),
    .macos, .ios, .tvos, .watchos => @import("c/darwin.zig"),
    .freebsd, .kfreebsd => @import("c/freebsd.zig"),
    .netbsd => @import("c/netbsd.zig"),
    .dragonfly => @import("c/dragonfly.zig"),
    .openbsd => @import("c/openbsd.zig"),
    .haiku => @import("c/haiku.zig"),
    .hermit => @import("c/hermit.zig"),
    .solaris => @import("c/solaris.zig"),
    .fuchsia => @import("c/fuchsia.zig"),
    .minix => @import("c/minix.zig"),
    .emscripten => @import("c/emscripten.zig"),
    .wasi => @import("c/wasi.zig"),
    else => struct {},
};

pub cons