//! Ported from:
//!
//! https://github.com/llvm/llvm-project/commit/d674d96bc56c0f377879d01c9d8dfdaaa7859cdb/compiler-rt/lib/builtins/divdf3.c

const std = @import("std");
const builtin = @import("builtin");
const arch = builtin.cpu.arch;
const is_test = builtin.is_test;
const common = @import("common.zig");

const normalize = common.normalize;
const wideMultiply = common.wideMultiply;

pub const panic = common.panic;

comptime {
    if (common.want_aeabi) {
        @export(__aeabi_ddiv, .{ .name = "__aeabi_ddiv", .linkage = common.linkage, .visibility = common.visibility });
    } else {
        @export(__divdf3, .{ .name = "__divdf3", .linkage = common.linkage, .visibility = common.visibility });
    }
}

pub fn __divdf3(a: f64, b: f64) callconv(.C) f64 {
    return div(a, b);
}

fn __aeabi_ddiv(a: f64, b: f64) callconv(.AAPCS) f64 {
    return div(a, b);
}

inline fn div(a: f64, b: f64) f64 {
    const Z = std.meta.Int(.unsigned, 64);
    const SignedZ = std.meta.Int(.signed, 64);

    const significandBits = std.math.floatMantissaBits(f64);
    const exponentBits = std.math.floatExponentBits(f64);

    const signBit = (@as(Z, 1) << (significandBits + exponentBits));
    const maxExponent = ((1 << exponentBits) - 1);
    const exponentBias = (maxExponent >> 1);

    const implicitBit = (@as(Z, 1) << significandBits);
    const quietBit = implicitBit >> 1;
    const significandMask = implicitBit - 1;

    const absMask = signBit - 1;
    const exponentMask = absMask ^ significandMask;
    const qnanRep = exponentMask | quietBit;
    const infRep = @bitCast(Z, std.math.inf(f64));

    const aExponent = @truncate(u32, (@bitCast(Z, a) >> significandBits) & maxExponent);
    const bExponent = @truncate(u32, (@bitCast(Z, b) >> significandBits) & maxExponent);
    const quotientSign: Z = (@bitCast(Z, a) ^ @bitCast(Z, b)) & signBit;

    var aSignificand: Z = @bitCast(Z, a) & significandMask;
    var bSignificand: Z = @bitCast(Z, b) & significandMask;
    var scale: i32 = 0;

    // Detect if a or b is zero, denormal, infinity, or NaN.
    if (aExponent -% 1 >= maxExponent - 1 or bExponent -% 1 >= maxExponent - 1) {
        const aAbs: Z = @bitCast(Z, a) & absMask;
        const bAbs: Z = @bitCast(Z, b) & absMask;

        // NaN / anything = qNaN
        if (aAbs > infRep) return @bitCast(f64, @bitCast(Z, a) | quietBit);
        // anything / NaN = qNaN
        if (bAbs > infRep) return @bitCast(f64, @bitCast(Z, b) | quietBit);

        if (aAbs == infRep) {
            // infinity / infinity = NaN
            if (bAbs == infRep) {
                return @bitCast(f64, qnanRep);
            }
            // infinity / anything else = +/- infinity
            else {
                return @bitCast(f64, aAbs | quotientSign);
            }
        }

        // anything else / infinity = +/- 0
        if (bAbs == infRep) return @bitCast(f64, quotientSign);

        if (aAbs == 0) {
            // zero / zero = NaN
            if (bAbs == 0) {
                return @bitCast(f64, qnanRep);
            }
            // zero / anything else = +/- zero
            else {
                return @bitCast(f64, quotientSign)