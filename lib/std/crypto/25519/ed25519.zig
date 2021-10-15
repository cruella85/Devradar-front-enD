const builtin = @import("builtin");
const std = @import("std");
const crypto = std.crypto;
const debug = std.debug;
const fmt = std.fmt;
const mem = std.mem;

const Sha512 = crypto.hash.sha2.Sha512;

const EncodingError = crypto.errors.EncodingError;
const IdentityElementError = crypto.errors.IdentityElementError;
const NonCanonicalError = crypto.errors.NonCanonicalError;
const SignatureVerificationError = crypto.errors.SignatureVerificationError;
const KeyMismatchError = crypto.errors.KeyMismatchError;
const WeakPublicKeyError = crypto.errors.WeakPublicKeyError;

/// Ed25519 (EdDSA) signatures.
pub const Ed25519 = struct {
    /// The underlying elliptic curve.
    pub const Curve = std.crypto.ecc.Edwards25519;

    /// Length (in bytes) of optional random bytes, for non-deterministic signatures.
    pub const noise_length = 32;

    const CompressedScalar = Curve.scalar.CompressedScalar;
    const Scalar = Curve.scalar.Scalar;

    /// An Ed25519 secret key.
    pub const SecretKey = struct {
        /// Length (in bytes) of a raw secret key.
        pub const encoded_length = 64;

        bytes: [encoded_length]u8,

        /// Return the seed used to generate this secret key.
        pub fn seed(self: SecretKey) [KeyPair.seed_length]u8 {
            return self.bytes[0..KeyPair.seed_length].*;
        }

        /// Return the raw public key bytes corresponding to this secret key.
        pub fn publicKeyBytes(self: SecretKey) [PublicKey.encoded_length]u8 {
            return self.bytes[KeyPair.seed_length..].*;
        }

        /// Create a secret key from raw bytes.
        pub fn fromBytes(bytes: [encoded_length]u8) !SecretKey {
            return SecretKey{ .bytes = bytes };
        }

        /// Return the secret key as raw bytes.
        pub fn toBytes(sk: SecretKey) [encoded_length]u8 {
            return sk.bytes;
        }

        // Return the clamped secret scalar and prefix for this secret key
        fn scalarAndPrefix(self: SecretKey) struct { scalar: CompressedScalar, prefix: [32]u8 } {
            var az: [Sha512.digest_length]u8 = undefined;
            var h = Sha512.init(.{});
            h.update(&self.seed());
            h.final(&az);

            var s = az[0..32].*;
            Curve.scalar.clamp(&s);

            return .{ .scalar = s, .prefix = az[32..].* };
        }
    };

    /// A Signer is used to incrementally compute a signature.
    /// It can be obtained from a `KeyPair`, using the `signer()` function.
    pub const Signer = struct {
        h: Sha512,
        scalar: CompressedScalar,
        nonce: CompressedScalar,
        r_bytes: [Curve.encoded_length]u8,

        fn init(scalar: CompressedScalar, nonce: CompressedScalar, public_key: PublicKey) (IdentityElementError || KeyMismatchError || NonCanonicalError || WeakPublicKeyError)!Signer {
            const r = try Curve.basePoint.mul(nonce);
            const r_bytes = r.toBytes();

            var t: [64]u8 = undefined;
            mem.copy(u8, t[0..32], &r_bytes);
            mem.copy(u8, t[32..], &public_key.bytes);
            var h = Sha512.init(.{});
            h.update(&t);

            return Signer{ .h = h, .scalar = scalar, .nonce = nonce, .r_bytes = r_bytes };
        }

        /// Add new data to the message being signed.
        pub fn update(self: *Signer, data: []const u8) void {
            self.h.update(data);
        }

        /// Compute a signature over the entire message.
        pub fn finalize(self: *Signer) Signature {
            var hram64: [Sha512.digest_length]u8 = undefined;
            self.h.final(&hram64);
            const hram = Curve.scalar.reduce64(hram64);

            const s = Curve.scalar.mulAdd(hram, self.scalar, self.nonce);

            return Signature{ .r = self.r_bytes, .s = s };
        }
    };

    /// An Ed25519 public key.
    pub const PublicKey = struct {
        /// Length (in bytes) of a raw public key.
        pub const encoded_length = 32;

        bytes: [encoded_length]u8,

        /// Create a public key from raw bytes.
        pub fn fromBytes(bytes: [encoded_length]u8) NonCanonicalError!PublicKey {
            try Curve.rejectNonCanonical(bytes);
            return PublicKey{ .bytes = bytes };
        }

        /// Convert a public key to raw bytes.
        pub fn toBytes(pk: PublicKey) [encoded_length]u8 {
            return pk.bytes;
        }

        fn signWithNonce(public_key: PublicKey, msg: []const u8, scalar: CompressedScalar, nonce: CompressedScalar) (IdentityElementError || NonCanonicalError || KeyMismatchError || WeakPublicKeyError)!Signature {
            var st = try Signer.init(scalar, nonce, public_key);
            st.update(msg);
            return st.finalize();
        }

        fn computeNonceAndSign(public_key: PublicKey, msg: []const u8, noise: ?[noise_length]u8, scalar: CompressedScalar, prefix: []const u8) (IdentityElementError || NonCanonicalError || KeyMismatchError || WeakPublicKeyError)!Signature {
            var h = Sha512.init(.{});
            if (noise) |*z| {
                h.update(z);
            }
            h.update(prefix);
            h.update(msg);
            v