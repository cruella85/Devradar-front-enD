const std = @import("../std.zig");
const crypto = std.crypto;
const debug = std.debug;
const mem = std.mem;

pub const HmacMd5 = Hmac(crypto.hash.Md5);
pub const HmacSha1 = Hmac(crypto.hash.Sha1);

pub const sha2 = struct {
    pub const HmacSha224 = Hmac(crypto.hash.sha2.Sha224);
    pub const HmacSha256 = Hmac(crypto.hash.sha2.Sha256);
    pub const HmacSha384 = Hmac(crypto.hash.sha2.Sha384);
    pub const HmacSha512 = Hmac(crypto.hash.sha2.Sha512);
};

pub fn Hmac(comptime Hash: type) type {
    return struct {
        const Self = @This();
        pub const mac_length = Hash.digest_length;
        pub const key_length_min = 0;
        pub const key_length = 32; // recommended key length

        o_key_pad: [Hash.block_length]u8,
        hash: Hash,

        // HMAC(k, m) = H(o_key_pad || H(i_key_pad || msg)) where || is concatenation
        pub fn create(out: *[mac_length]u8, msg: []const u8, key: []const u8) void {
      