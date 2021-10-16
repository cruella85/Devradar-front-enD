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
            var nonce64: [64]u8 = undefined;
            h.final(&nonce64);

            const nonce = Curve.scalar.reduce64(nonce64);

            return public_key.signWithNonce(msg, scalar, nonce);
        }
    };

    /// A Verifier is used to incrementally verify a signature.
    /// It can be obtained from a `Signature`, using the `verifier()` function.
    pub const Verifier = struct {
        h: Sha512,
        s: CompressedScalar,
        a: Curve,
        expected_r: Curve,

        fn init(sig: Signature, public_key: PublicKey) (NonCanonicalError || EncodingError || IdentityElementError)!Verifier {
            const r = sig.r;
            const s = sig.s;
            try Curve.scalar.rejectNonCanonical(s);
            const a = try Curve.fromBytes(public_key.bytes);
            try a.rejectIdentity();
            try Curve.rejectNonCanonical(r);
            const expected_r = try Curve.fromBytes(r);
            try expected_r.rejectIdentity();

            var h = Sha512.init(.{});
            h.update(&r);
            h.update(&public_key.bytes);

            return Verifier{ .h = h, .s = s, .a = a, .expected_r = expected_r };
        }

        /// Add new content to the message to be verified.
        pub fn update(self: *Verifier, msg: []const u8) void {
            self.h.update(msg);
        }

        /// Verify that the signature is valid for the entire message.
        pub fn verify(self: *Verifier) (SignatureVerificationError || WeakPublicKeyError || IdentityElementError)!void {
            var hram64: [Sha512.digest_length]u8 = undefined;
            self.h.final(&hram64);
            const hram = Curve.scalar.reduce64(hram64);

            const sb_ah = try Curve.basePoint.mulDoubleBasePublic(self.s, self.a.neg(), hram);
            if (self.expected_r.sub(sb_ah).rejectLowOrder()) {
                return error.SignatureVerificationFailed;
            } else |_| {}
        }
    };

    /// An Ed25519 signature.
    pub const Signature = struct {
        /// Length (in bytes) of a raw signature.
        pub const encoded_length = Curve.encoded_length + @sizeOf(CompressedScalar);

        /// The R component of an EdDSA signature.
        r: [Curve.encoded_length]u8,
        /// The S component of an EdDSA signature.
        s: CompressedScalar,

        /// Return the raw signature (r, s) in little-endian format.
        pub fn toBytes(self: Signature) [encoded_length]u8 {
            var bytes: [encoded_length]u8 = undefined;
            mem.copy(u8, bytes[0 .. encoded_length / 2], &self.r);
            mem.copy(u8, bytes[encoded_length / 2 ..], &self.s);
            return bytes;
        }

        /// Create a signature from a raw encoding of (r, s).
        /// EdDSA always assumes little-endian.
        pub fn fromBytes(bytes: [encoded_length]u8) Signature {
            return Signature{
                .r = bytes[0 .. encoded_length / 2].*,
                .s = bytes[encoded_length / 2 ..].*,
            };
        }

        /// Create a Verifier for incremental verification of a signature.
        pub fn verifier(self: Signature, public_key: PublicKey) (NonCanonicalError || EncodingError || IdentityElementError)!Verifier {
            return Verifier.init(self, public_key);
        }

        /// Verify the signature against a message and public key.
        /// Return IdentityElement or NonCanonical if the public key or signature are not in the expected range,
        /// or SignatureVerificationError if the signature is invalid for the given message and key.
        pub fn verify(self: Signature, msg: []const u8, public_key: PublicKey) (IdentityElementError || NonCanonicalError || SignatureVerificationError || EncodingError || WeakPublicKeyError)!void {
            var st = try Verifier.init(self, public_key);
            st.update(msg);
            return st.verify();
        }
    };

    /// An Ed25519 key pair.
    pub const KeyPair = struct {
        /// Length (in bytes) of a seed required to create a key pair.
        pub const seed_length = noise_length;

        /// Public part.
        public_key: PublicKey,
        /// Secret scalar.
        secret_key: SecretKey,

        /// Derive a key pair from an optional secret seed.
        ///
        /// As in RFC 8032, an Ed25519 public key is generated by hashing
        /// the secret key using the SHA-512 function, and interpreting the
        /// bit-swapped, clamped lower-half of the output as the secret scalar.
        ///
        /// For this reason, an EdDSA secret key is commonly called a seed,
        /// from which the actual secret is derived.
        pub fn create(seed: ?[seed_length]u8) IdentityElementError!KeyPair {
            const ss = seed orelse ss: {
                var random_seed: [seed_length]u8 = undefined;
                crypto.random.bytes(&random_seed);
                break :ss random_seed;
            };
            var az: [Sha512.digest_length]u8 = undefined;
            var h = Sha512.init(.{});
            h.update(&ss);
            h.final(&az);
            const pk_p = Curve.basePoint.clampedMul(az[0..32].*) catch return error.IdentityElement;
            const pk_bytes = pk_p.toBytes();
            var sk_bytes: [SecretKey.encoded_length]u8 = undefined;
            mem.copy(u8, &sk_bytes, &ss);
            mem.copy(u8, sk_bytes[seed_length..], &pk_bytes);
            return KeyPair{
                .public_key = PublicKey.fromBytes(pk_bytes) catch unreachable,
                .secret_key = try SecretKey.fromBytes(sk_bytes),
            };
        }

        /// Create a KeyPair from a secret key.
        /// Note that with EdDSA, storing the seed, and recovering the key pair
        /// from it is recommended over storing the entire secret key.
        /// The seed of an exiting key pair can be obtained with
        /// `key_pair.secret_key.seed()`.
        pub fn fromSecretKey(secret_key: SecretKey) (NonCanonicalError || EncodingError || IdentityElementError)!KeyPair {
            // It is critical for EdDSA to use the correct public key.
            // In order to enforce this, a SecretKey implicitly includes a copy of the public key.
            // In Debug mode, we can still afford checking that the public key is correct for extra safety.
            if (builtin.mode == .Debug) {
                const pk_p = try Curve.fromBytes(secret_key.publicKeyBytes());
                const recomputed_kp = try create(secret_key.seed());
                debug.assert(mem.eql(u8, &recomputed_kp.public_key.toBytes(), &pk_p.toBytes()));
            }
            return KeyPair{
                .public_key = try PublicKey.fromBytes(secret_key.publicKeyBytes()),
                .secret_key = secret_key,
            };
        }

        /// Sign a message using the key pair.
        /// The noise can be null in order to create deterministic signatures.
        /// If deterministic signatures are not required, the noise should be randomly generated instead.
        /// This helps defend against fault attacks.
        pub fn sign(key_pair: KeyPair, msg: []const u8, noise: ?[noise_length]u8) (IdentityElementError || NonCanonicalError || KeyMismatchError || WeakPublicKeyError)!Signature {
            if (!mem.eql(u8, &key_pair.secret_key.publicKeyBytes(), &key_pair.public_key.toBytes())) {
                return error.KeyMismatch;
            }
            const scalar_and_prefix = key_pair.secret_key.scalarAndPrefix();
            return key_pair.public_key.computeNonceAndSign(
                msg,
                noise,
                scalar_and_prefix.scalar,
                &scalar_and_prefix.prefix,
            );
        }

        /// Create a Signer, that can be used for incremental signing.
        /// Note that the signature is not deterministic.
        /// The noise parameter, if set, should be something unique for each message,
        /// such as a random nonce, or a counter.
        pub fn signer(key_pair: KeyPair, noise: ?[noise_length]u8) (IdentityElementError || KeyMismatchError || NonCanonicalError || WeakPublicKeyError)!Signer {
            if (!mem.eql(u8, &key_pair.secret_key.publicKeyBytes(), &key_pair.public_key.toBytes())) {
                return error.KeyMismatch;
            }
            const scalar_and_prefix = key_pair.secret_key.scalarAndPrefix();
            var h = Sha512.init(.{});
            h.update(&scalar_and_prefix.prefix);
            var noise2: [noise_length]u8 = undefined;
            crypto.random.bytes(&noise2);
            h.update(&noise2);
            if (noise) |*z| {
                h.update(z);
            }
            var nonce64: [64]u8 = undefined;
            h.final(&nonce64);
            const nonce = Curve.scalar.reduce64(nonce64);

            return Signer.init(scalar_and_prefix.scalar, nonce, key_pair.public_key);
      