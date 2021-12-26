const std = @import("std");

pub inline fn __builtin_bswap16(val: u16) u16 {
    return @byteSwap(val);
}
pub inline fn __builtin_bswap32(val: u32) u32 {
    return @byteSwap(val);
}
pub inline fn __builtin_bswap64(val: u64) u64 {
    return @byteSwap(val);
}

pub inline fn __builtin_signbit(val: f64) c_int {
    return @boolToInt(std.math.signbit(val));
}
pub inline fn __builtin_signbitf(val: f32) c_int {
    return @boolToInt(std.math.signbit(val));
}

pub inline fn __builtin_popcount(val: c_uint) c_int {
    // popcount of a c_uint will never exceed the capacity of a c_int
    @setRuntimeSafety(false);
    return @bitCast(c_int, @as(c_uint, @popCount(val)));
}
pub inline fn __builtin_ctz(val: c_uint) c_int {
    // Returns the number of trailing 0-bits in val, starting at the least significant bit position.
    // In C if `val` is 0, the result is undefined; in zig it's the number of bits in a c_uint
    @setRuntimeSafety(false);
    return @bitCast(c_int, @as(c_uint, @ctz(val)));
}
pub inline fn __builtin_clz(val: c_uint) c_int {
    // Returns the number of leading 0-bits in x, starting at the most significant bit position.
    // In C if `val` is 0, the result is undefined; in zig it's the number of bits in a c_uint
    @setRuntimeSafety(false);
    return @bitCast(c_int, @as(c_uint, @clz(val)));
}

pub inline fn __builtin_sqrt(val: f64) f64 {
    return @sqrt(val);
}
pub inline fn __builtin_sqrtf(val: f32) f32 {
    return @sqrt(val);
}

pub inline fn __builtin_sin(val: f64) f64 {
    return @sin(val);
}
pub inline fn __builtin_sinf(val: f32) f32 {
    return @sin(val);
}
pub inline fn __builtin_cos(val: f64) f64 {
    return @cos(val);
}
pub inline fn __builtin_cosf(val: f32) f32 {
    return @cos(val);
}

pub inline fn __builtin_exp(val: f64) f64 {
    return @exp(val);
}
pub inline fn __builtin_expf(val: f32) f32 {
    return @exp(