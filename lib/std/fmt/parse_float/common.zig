const std = @import("std");

/// A custom N-bit floating point type, representing `f * 2^e`.
/// e is biased, so it be directly shifted into the exponent bits.
/// Negative exponent indicates an invalid result.
pub fn BiasedFp(comptime T: type) type {
    const MantissaT = mantissaType(T);

    return struct {
        const Self = @This();

        /// The significant digits.
        f: MantissaT,
        /// The biased, binary exponent.
        e: i32,

        pub fn zero() Self {
            return .{ .f = 0, .e = 0 };
        }

        pub fn zeroPow2(e: i32) Self {
            return .{ .f = 0, .e = e };
        }

        pub fn inf(comptime FloatT: type) Self {
            return .{ .f = 0, .e = (1 << std.math.floatExponentBits(FloatT)) - 1 };
        }

        pub fn eql(self: Self, other: Self) bool {
            return self.f == other.f and self.e == other.e;
        }

        pub fn toFloat(self: Self, comptime FloatT: type, negative: bool) FloatT {
            var word = self.f;
            word |= @intCast(MantissaT, self.e) << std.math.floatMantissaBits(FloatT);
            var f = floatFromUnsigned(FloatT, MantissaT, word);
            if (negative) f = -f;
            return f;
        }
    };
}

pub fn floatFromUnsigned(comptime T: type, comptime MantissaT: type, v: MantissaT) T {
    return switch (T) {
        f16 => @bitCast(f16, @truncate(u16, v)),
        f32 => @bitCast(f32, @truncate(u32, v)),
        f64 => @bitCast(f64, @truncate(u64, v)),
        f128 => @bitCast(f128, v),
        else => unreachable,
    };
}

/// Represents a parsed floating point value as its components.
pub fn Number(comptime T: type) type {
    return struct {
        exponent: i64,
        mantissa: mantissaType(T),
        negative: bool,
        /// More than max_mantissa digits were found during parse
        many_digits: bool,
        /// The number was a hex-float (e.g. 0x1.234p567)
        hex: boo