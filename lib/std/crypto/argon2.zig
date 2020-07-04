// https://datatracker.ietf.org/doc/rfc9106
// https://github.com/golang/crypto/tree/master/argon2
// https://github.com/P-H-C/phc-winner-argon2

const std = @import("std");
const builtin = @import("builtin");

const blake2 = crypto.hash.blake2;
const crypto = std.crypto;
const math = std.math;
const mem = std.mem;
const phc_format = pwhash.phc_format;
const pwhash = crypto.pwhash;

const Thread = std.Thread;
const Blake2b512 = blake2.Blake2b512;
const Blocks = std.ArrayListAligned([block_length]u64, 16);
const H0 = [Blake2b512.digest_length + 8]u8;

const EncodingError = crypto.errors.EncodingError;
const KdfError = pwhash.KdfError;
const HasherError = pwhash.HasherError;
const Error = pwhash.Error;

const version = 0x13;
const block_length = 128;
const sync_points = 4;
const max_int = 0xffff_ffff;

const default_salt_len = 32;
const default_hash_len = 32;
const max_salt_len = 64;
const max_hash_len = 64;

/// Argon2 type
pub const Mode = enum {
    /// Argon2d is faster and uses data-depending memory access, which makes it highly resistant
    /// against GPU cracking attacks and suitable for applications with no threats from side-channel
    /// timing attacks (eg. cryptocurrencies).
    argon2d,

    /// Argon2i instead uses data-independent memory access, which is preferred for password
    /// hashing and password-based key derivation, but it is slower as it makes more passes over
    /// the memory to protect from tradeoff attacks.
    argon2i,

    /// Argon2id is a hybrid of Argon2i and Argon2d, using a combination of data-depending and
    /// data-independent memory accesses, which gives some of Argon2i's resistance to side-channel
    /// cache timing attacks and much of Argon2d's resistance to GPU cracking attacks.
    argon2id,
};

/// Argon2 parameters
pub const Params = struct {
    const Self = @This();

    /// A [t]ime cost, which defines the amount of computation realized and therefore the execution
    /// time, given in number of iterations.
    t: u32,

    /// A [m]emory cost, which defines the memory usage, given in kibibytes.
    m: u32,

    /// A [p]arallelism degree, which defines the number of parallel threads.
    p: u24,

    /// The [secret] parameter, which is used for keyed hashing. This allows a secret key to be input
    /// at hashing time (from some external location) and be folded into the value of the hash. This
    /// means that even if your salts and hashes are compromised, an attacker cannot brute-force to
    /// find the password without the key.
    secret: ?[]const u8 = null,

    /// The [ad] parameter, which is used to fold any additional data into the hash value. Functionally,
    /// this behaves almost exactly like the secret or salt parameters; the ad parameter is folding
    /// into the value of the hash. However, this parameter is used for different data. The salt
    /// should be a random string stored alongside your password. The secret should be a random key
    /// only usable at hashing time. The ad is for any other data.
    ad: ?[]const u8 = null,

    /// Baseline parameters for interactive logins using argon2i type
    pub const interactive_2i = Self.fromLimits(4, 33554432);
    /// Baseline parameters for normal usage using argon2i type
    pub const moderate_2i = Self.fromLimits(6, 134217728);
    /// Baseline parameters for offline usage using argon2i type
    pub const sensitive_2i = Self.fromLimits(8, 536870912);

    /// Baseline parameters for interactive logins using argon2id type
    pub const interactive_2id = Self.fromLimits(2, 67108864);
    /// Baseline parameters for normal usage using argon2id type
    pub const moderate_2id = Self.fromLimits(3, 268435456);
    /// Baseline parameters for offline usage using argon2id type
    pub const sensitive_2id = Self.fromLimits(4, 1073741824);

    /// Create parameters from ops and mem limits, where mem_limit given in bytes
    pub fn fromLimits(ops_limit: u32, mem_limit: usize) Self {
        const m = mem_limit / 1024;
        std.debug.assert(m <= max_int);
        return .{ .t = ops_limit, .m = @intCast(u32, m), .p = 1 };
    }
};

fn initHash(
    password: []const u8,
    salt: []const u8,
    params: Params,
    dk_len: usize,
    mode: Mode,
) H0 {
    var h0: H0 = undefined;
    var parameters: [24]u8 = undefined;
    var tmp: [4]u8 = undefined;
    var b2 = Blake2b512.init(.{});
    mem.writeIntLittle(u32, parameters[0..4], params.p);
    mem.writeIntLittle(u32, parameters[4..8], @intCast(u32, dk_len));
    mem.writeIntLittle(u32, parameters[8..12], params.m);
    mem.writeIntLittle(u32, parameters[12..16], params.t);
    mem.writeIntLittle(u32, parameters[16..20], version);
    mem.writeIntLittle(u32, parameters[20..24], @enumToInt(mode));
    b2.update(&parameters);
    mem.writeIntLittle(u32, &tmp, @intCast(u32, password.len));
    b2.update(&tmp);
    b2.update(password);
    mem.writeIntLittle(u32, &tmp, @intCast(u32, salt.len));
    b2.update(&tmp);
    b2.update(salt);
    const secret = params.secret orelse "";
    std.debug.assert(secret.len <= max_int);
    mem.writeIntLittle(u32, &tmp, @intCast(u32, secret.len));
    b2.update(&tmp);
    b2.update(secret);
    const ad = params.ad orelse "";
    std.debug.assert(ad.len <= max_int);
    mem.writeIntLittle(u32, &tmp, @intCast(u32, ad.len));
    b2.update(&tmp);
    b2.update(ad);
    b2.final(h0[0..Blake2b512.digest_length]);
    return h0;
}

fn blake2bLong(out: []u8, in: []const u8) void {
    var b2 = Blake2b512.init(.{ .expected_out_bits = math.min(512, out.len * 8) });

    var buffer: [Blake2b512.digest_length]u8 = undefined;
    mem.writeIntLittle(u32, buffer[0..4], @intCast(u32, out.len));
    b2.update(buffer[0..4]);
    b2.update(in);
    b2.final(&buffer);

    if (out.len <= Blake2b512.digest_length) {
        mem.copy(u8, out, buffer[0..out.len]);
        return;
    }

    b2 = Blake2b512.init(.{});
    mem.copy(u8, out, buffer[0..32]);
    var out_slice = out[32..];
    while (out_slice.len > Blake2b512.digest_length) : ({
        out_slice = out_slice[32..];
        b2 = Blake2b512.init(.{});
    }) {
        b2.update(&buffer);
        b2.final(&buffer);
        mem.copy(u8, out_slice, buffer[0..32]);
    }

    var r = Blake2b512.digest_length;
    if (out.len % Blake2b512.digest_length > 0) {
        r = ((out.len + 31) / 32) - 2;
        b2 = Blake2b512.init(.{ .expected_out_bits = r * 8 });
    }

    b2.update(&buffer);
    b2.final(&buffer);
    mem.copy(u8, out_slice, buffer[0..r]);
}

fn initBlocks(
    blocks: *Blocks,
    h0: *H0,
    memory: u32,
    threads: u24,
) void {
    var block0: [1024]u8 = undefined;
    var lane: u24 = 0;
    while (lane < threads) : (lane += 1) {
        const j = lane * (memory / threads);
        mem.writeIntLittle(u32, h0[Blake2b512.digest_length + 4 ..][0..4], lane);

        mem.writeIntLittle(u32, h0[Blake2b512.digest_length..][0..4], 0);
        blake2bLong(&block0, h0);
        for (&blocks.items[j + 0], 0..) |*v, i| {
            v.* = mem.readIntLittle(u64, block0[i * 8 ..][0..8]);
        }

        mem.writeIntLittle(u32, h0[Blake2b512.digest_length..][0..4], 1);
        blake2bLong(&block0, h0);
        for (&blocks.items[j + 1], 0..) |*v, i| {
            v.* = mem.readIntLittle(u64, block0[i * 8 ..][0..8]);
        }
    }
}

fn processBlocks(
    allocator: mem.Allocator,
    blocks: *Blocks,
    time: u32,
    memory: u32,
    threads: u24,
    mode: Mode,
) KdfError!void {
    const lanes = memory / threads;
    const segments = lanes / sync_points;

    if (builtin.single_threaded or threads == 1) {
        processBlocksSt(blocks, time, memory, threads, mode, lanes, segments);
    } else {
        try processBlocksMt(allocator, blocks, time, memory, threads, mode, lanes, segments);
    }
}

fn processBlocksSt(
    blocks: *Blocks,
    time: u32,
    memory: u32,
    threads: u24,
    mode: Mode,
    lanes: u32,
    segments: u32,
) void {
    var n: u32 = 0;
    while (n < time) : (n += 1) {
        var slice: u32 = 0;
        while (slice < sync_points) : (slice += 1) {
            var lane: u24 = 0;
            while (lane < threads) : (lane += 1) {
                processSegment(blocks, time, memory, threads, mode, lanes, segments, n, slice, lane);
            }
        }
    }
}

fn processBlocksMt(
    allocator: mem.Allocator,
    blocks: *Blocks,
    time: u32,
    memory: u32,
    threads: u24,
    mode: Mode,
    lanes: u32,
    segments: u32,
) KdfError!void {
    var threads_list = try std.ArrayList(Thread).initCapacity(allocator, threads);
    defer threads_list.deinit();

    var n: u32 = 0;
    while (n < time) : (n += 1) {
        var slice: u32 = 0;
        while (slice < sync_points) : (slice += 1) {
            var lane: u24 = 0;
            while (lane < threads) : (lane += 1) {
                const thread = try Thread.spawn(.{}, processSegment, .{
                    blocks, time, memory, threads, mode, lanes, segments, n, slice, lane,
                });
                threads_list.appendAssumeCapacity(thread);
            }
            lane = 0;
            while (lane < threads) : (lane += 1) {
                threads_list.items[lane].join();
            }
            threads_list.clearRetainingCapacity();
        }
    }
}

fn processSegment(
    blocks: *Blocks,
    passes: u32,
    memory: u32,
    threads: u24,
    m