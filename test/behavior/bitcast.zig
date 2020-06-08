const std = @import("std");
const builtin = @import("builtin");
const expect = std.testing.expect;
const expectEqual = std.testing.expectEqual;
const math = std.math;
const maxInt = std.math.maxInt;
const minInt = std.math.minInt;
const native_endian = builtin.target.cpu.arch.endian();

test "@bitCast iX -> uX (32, 64)" {
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;

    const bit_values = [_]usize{ 32, 64 };

    inline for (bit_values) |bits| {
        try testBitCast(bits);
        comptime try testBitCast(bits);
    }
}

test "@bitCast iX -> uX (8, 16, 128)" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const bit_values = [_]usize{ 8, 16, 128 };

    inline for (bit_values) |bits| {
        try testBitCast(bits);
        comptime try testBitCast(bits);
    }
}

test "@bitCast iX -> uX exotic integers" {
    if (builtin.zig_backend == .stage2_wasm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const bit_values = [_]usize{ 1, 48, 27, 512, 493, 293, 125, 204, 112 };

    inline for (bit_values) |bits| {
        try testBitCast(bits);
        comptime try testBitCast(bits);
    }
}

fn testBitCast(comptime N: usize) !void {
    const iN = std.meta.Int(.signed, N);
    const uN = std.meta.Int(.unsigned, N);

    try expect(conv_iN(N, -1) == maxInt(uN));
    try expect(conv_uN(N, maxInt(uN)) == -1);

    try expect(conv_iN(N, maxInt(iN)) == maxInt(iN));
    try expect(conv_uN(N, maxInt(iN)) == maxInt(iN));

    try expect(conv_uN(N, 1 << (N - 1)) == minInt(iN));
    try expect(conv_iN(N, minInt(iN)) == (1 << (N - 1)));

    try expect(conv_uN(N, 0) == 0);
    try expect(conv_iN(N, 0) == 0);

    try expect(conv_iN(N, -0) == 0);

    if (N > 24) {
        try expect(conv_uN(N, 0xf23456) == 0xf23456);
    }
}

fn conv_iN(comptime N: usize, x: std.meta.Int(.signed, N)) std.meta.Int(.unsigned, N) {
    return @bitCast(std.meta.Int(.unsigned, N), x);
}

fn conv_uN(comptime N: usize, x: std.meta.Int(.unsigned, N)) std.meta.Int(.signed, N) {
    return @bitCast(std.meta.Int(.signed, N), x);
}

test "bitcast uX to bytes" {
    if (builtin.zig_backend == .stage2_wasm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const bit_values = [_]usize{ 1, 48, 27, 512, 493, 293, 125, 204, 112 };
    inline for (bit_values) |bits| {
        try testBitCast(bits);
        comptime try testBitCast(bits);
    }
}

fn testBitCastuXToBytes(comptime N: usize) !void {

    // The location of padding bits in these layouts are technically not defined
    // by LLVM, but we currently allow exotic integers to be cast (at comptime)
    // to types that expose their padding bits anyway.
    //
    // This test at least makes sure those bits are matched by the runtime behavior
    // on the platforms we target. If the above behavior is restricted after all,
    // this test should be deleted.

    const T = std.meta.Int(.unsigned, N);
    for ([_]T{ 0, ~@as(T, 0) }) |init_value| {
        var x: T = init_value;
        const bytes = std.mem.asBytes(&x);

        const byte_count = (N + 7) / 8;
        switch (native_endian) {
            .Little => {
                var byte_i = 0;
                while (byte_i < (byte_count - 1)) : (byte_i += 1) {
                    try expect(bytes[byte_i] == 0xff);
                }
                try expect(((bytes[byte_i] ^ 0xff) << -%@truncate(u3, N)) == 0);
            },
            .Big => {
                var byte_i = byte_count - 1;
                while (byte_i > 0) : (byte_i -= 1) {
                    try expect(bytes[byte_i] == 0xff);
                }
                try expect(((bytes[byte_i] ^ 0xff) << -%@truncate(u3, N)) == 0);
            },
        }
    }
}

test "nested bitcast" {
    const S = struct {
        fn moo(x: isize) !void {
            try expect(@intCast(isize, 42) == x);
        }

        fn foo(x: isize) !void {
            try @This().moo(
                @bitCast(isize, if (x != 0) @bitCast(usize, x) else @bitCast(usize, x)),
            );
        }
    };

    try S.foo(42);
    comptime try S.foo(42);
}

// issue #3010: compiler segfault
test "bitcast literal [4]u8 param to u32" {
    const ip = @bitCast(u32, [_]u8{ 255, 255, 255, 255 });
    try expect(ip == maxInt(u32));
}

test "bitcast generates a temporary value" {
    var y = @as(u16, 0x55AA);
    const x = @bitCast(u16, @bitCast([2]u8, y));
    try expect(y == x);
}

test "@bitCast packed structs at runtime and comptime" {
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const Full = packed struct {
        number: u16,
    };
    const Divided = packed struct {
        half1: u8,
        quarter3: u4,
        quarter4: u4,
    };
    const S = struct {
        fn doTheTest() !void {
            var full = Full{ .number = 0x1234 };
            var two_halves = @bitCast(Divided, full);
            try expect(two_halves.half1 == 0x34);
            try expect(two_halves.quarter3 == 0x2);
            try expect(two_halves.quarter4 == 0x1);
        }
    };
    try S.doTheTest();
    comptime try S.doTheTest();
}

test "@bitCast extern structs at runtime and comptime" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const Full = extern struct {
        number: u16,
    };
    const TwoHalves = extern struct {
        half1: u8,
        half2: u8,
    };
    const S = struct {
        fn doTheTest() !void {
            var full = Full{ .number = 0x1234 };
            var two_halves = @bitCast(TwoHalves, full);
            switch (native_endian) {
                .Big => {
                    try expect(two_halves.half1 == 0x12);
                    try expect(two_halves.half2 == 0x34);
                },
                .Little => {
                    try expect(two_halves.half1 == 0x34);
                    try expect(two_halves.half2 == 0x12);
                },
            }
        }
    };
    try S.doTheTest();
    comptime try S.doTheTest();
}

test "bitcast packed struct to integer and back" {
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const LevelUpMove = packed struct {
  