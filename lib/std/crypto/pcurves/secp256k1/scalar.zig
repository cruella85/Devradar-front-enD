const std = @import("std");
const common = @import("../common.zig");
const crypto = std.crypto;
const debug = std.debug;
const math = std.math;
const mem = std.mem;

const Field = common.Field;

const NonCanonicalError = std.crypto.errors.NonCanonicalError;
const NotSquareError = std.crypto.errors.NotSquareError;

/// Number of bytes required to encode a scalar.
pub const encoded_length = 32;

/// A compressed scalar, in canonical form.
pub const CompressedScalar = [encoded_length]u8;

const Fe = Field(.{
    .fiat = @import("secp256k1_scalar_64.zig"),
    .field_order = 115792089237316195423570985008687907852837564279074904382605163141518161494337,
    .field_bits = 256,
    .saturated_bits = 256,
    .encoded_length = encoded_length,
});

/// The scalar field order.
pub const field_order = Fe.field_order;

/// Reject a scalar whose encoding is not canonical.
pub fn rejectNonCanonical(s: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!void {
    return Fe.rejectNonCanonical(s, endian);
}

/// Reduce a 48-bytes scalar to the field size.
pub fn reduce48(s: [48]u8, endian: std.builtin.Endian) CompressedScalar {
    return Scalar.fromBytes48(s, endian).toBytes(endian);
}

/// Reduce a 64-bytes scalar to the field size.
pub fn reduce64(s: [64]u8, endian: std.builtin.Endian) CompressedScalar {
    return ScalarDouble.fromBytes64(s, endian).toBytes(endian);
}

/// Return a*b (mod L)
pub fn mul(a: CompressedScalar, b: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!CompressedScalar {
    return (try Scalar.fromBytes(a, endian)).mul(try Scalar.fromBytes(b, endian)).toBytes(endian);
}

/// Return a*b+c (mod L)
pub fn mulAdd(a: CompressedScalar, b: CompressedScalar, c: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!CompressedScalar {
    return (try Scalar.fromBytes(a, endian)).mul(try Scalar.fromBytes(b, endian)).add(try Scalar.fromBytes(c, endian)).toBytes(endian);
}

/// Return a+b (mod L)
pub fn add(a: CompressedScalar, b: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!CompressedScalar {
    return (try Scalar.fromBytes(a, endian)).add(try Scalar.fromBytes(b, endian)).toBytes(endian);
}

/// Return -s (mod L)
pub fn neg(s: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!CompressedScalar {
    return (try Scalar.fromBytes(s, endian)).neg().toBytes(endian);
}

/// Return (a-b) (mod L)
pub fn sub(a: CompressedScalar, b: CompressedScalar, endian: std.builtin.Endian) NonCanonicalError!CompressedScalar {
    return (try Scalar.fromBytes(a, endian)).sub(try Scalar.fromBytes(b, endian)).toBytes(endian);
}

/// Return a random scalar
pub fn random(endian: std.builtin.Endian) CompressedScalar {
    return Scalar.random().toBytes(endian);
}

/// A scalar in unpacked representation.
pub const Scalar = struct {
