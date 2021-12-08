//! Ported from musl, which is licensed under the MIT license:
//! https://git.musl-libc.org/cgit/musl/tree/COPYRIGHT
//!
//! https://git.musl-libc.org/cgit/musl/tree/src/math/log2f.c
//! https://git.musl-libc.org/cgit/musl/tree/src/math/log2.c

const std = @import("std");
const builtin = @import("builtin");
const math = std.math;
const expect = std.testing.expect;
const maxInt = std.math.maxInt;
const arch = builtin.cpu.arch;
const common = @import("common.zig");

pub const panic = common.panic;

comptime {
    @export(__log2h, .{ .name = "__log2h", .linkage = common.linkage, .visibility = common.visibility });
    @export(log2f, .{ .name = "log2f", .linkage = common.linkage, .visibility = common.visibility });
    @export(log2, .{ .name = "log2", .linkage = common.linkage, .visibility = common.visibility });
    @export(__log2x, .{ .name = "__log2x", .linkage = common.linkage, .visibility = common.visibility });
    if (common.want_ppc_abi) {
        @export(log2q, .{ .name = "log2f128", .linkage = common.linkage, .visibility = common.visibility });
    }
    @export(log2q, .{ .name = "log2q", .linkage = common.linkage, .visibility = common.visibility });
    @export(log2l, .{ .name = "log2l", .linkage = common.linkage, .visibility = common.visibility });
}

pub fn __log2h(a: f16) callconv(.C) f16 {
    // TODO: more efficient implementation
    return @floatCast(f16, log2f(a));
}

pub fn log2f(x_: f32) callconv(.C) f32 {
    const ivln2hi: f32 = 1.4428710938e+00;
    const ivln2lo: f32 = -1.7605285393e-04;
    const Lg1: f32 = 0xaaaaaa.0p-24;
    const Lg2: f32 = 0xccce13.0p-25;
    const Lg3: f32 = 0x91e9ee.0p-25;
    const Lg4: f32 = 0xf89e26.0p-26;

    var x = x_;
    var u = @bitCast(u32, x);
    var ix = u;
    var k: i32 = 0;

    // x < 2^(-126)
    if (ix < 0x00800000 or ix >> 31 != 0) {
        // log(+-0) = -inf
        if (ix << 1 == 0) {
            return -math.inf(f32);
        }
        // log(-#) = nan
        if (ix >> 31 != 0) {
            return math.nan(f32);
        }

        k -= 25;
        x *= 0x1.0p25;
        ix = @bitCast(u32, x);
    } else if (ix >= 0x7F800000) {
        return x;
    } else if (ix == 0x3F800000) {
        return 0;
    }

    // x into [sqrt(2) / 2, sqrt(2)]
    ix += 0x3F800000 - 0x3F3504F3;
    k += @intCast(i32, ix >> 23) - 0x7F;
    ix = (ix & 0x007FFFFF) + 0x3F3504F3;
    x = @bitCast(f32, ix);

    const f = x - 1.0;
    const s = f / (2.0 + f);
    const z = s * s;
    const w = z * z;
    const t1 = w * (Lg2 + w * Lg4);
    const t2 = z * (Lg1 + w * Lg3);
    const R = t2 + t1;
    const hfsq = 0.5 * f * f;

    var hi = f - hfsq;
    u = @bitCast(u32, hi);
    u &= 0xFFFFF000;
    hi = @bitCast(f32, u);
    const lo = f - hi - hfsq + s * (hfsq + R);
    return (lo + hi) * ivln2lo + lo * ivln2hi + hi * ivln2hi + @intToFloat(f32, k);
}

pub fn log2(x_: f64) callconv(.C) f64 {
    const ivln2hi: f64 = 1.44269504072144627571e+00;
    const ivln2lo: f64 = 1.67517131648865118353e-10;
    const Lg1: f64 = 6.666666666666735130e-01;
    const Lg2: f64 = 3.999999999940941908e-01;
    const Lg3: f64 = 2.857142874366239149e-01;
    c