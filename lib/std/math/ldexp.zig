const std = @import("std");
const math = std.math;
const Log2Int = std.math.Log2Int;
const assert = std.debug.assert;
const expect = std.testing.expect;

/// Returns x * 2^n.
pub fn ldexp(x: anytype, n: i32) @TypeOf(x) {
    const T = @TypeOf(x);
    const TBits = std.meta.Int(.unsigned, @typeInfo(T).Float.bits);

    const exponent_bits = math.floatExponentBits(T);
    const mantissa_bits = math.floatMantissaBits(T);
    const fractional_bits = math.floatFractionalBits(T);

    const max_biased_exponent = 2 * math.floatExponentMax(T);
    const mantissa_mask = @as(TBits, (1 << mantissa_bits) - 1);

    const repr = @bitCast(TBits, x);
    const sign_bit = repr & (1 << (exponent_bits + mantissa_bits));

    if (math.isNan(x) or !math.isFinite(x))
        return x;

    var exponent: i32 = @intCast(i32, (repr << 1) >> (mantissa_bits + 1));
    if (exponent == 0)
        exponent += (@as(i32, exponent_bits) + @boolToInt(T == f80)) - @clz(repr << 1);

    if (n >= 0) {
        if (n > max_biased_exponent - exponent) {
            // Overflow. Return +/- inf
            return @bitCast(T, @bitCast(TBits, math.inf(T)) | sign_bit);
        } else if (exponent + n <= 0) {
            // Result is subnormal
            return @bitCast(T, (repr << @intCast(Log2Int(TBits), n)) | sign_bit);
        } else if (exponent <= 0) {
            // Result is normal, but needs shifting
            var result = @intCast(TBits, n + exponent) << mantissa_bits;
            result |= (repr << @intCast(Log2Int(TBits), 1 - exponent)) & mantissa_mask;
            return @bitCast(T, result | sign_bit);
        }

        // Result needs no shifting
        return @bitCast(T, 