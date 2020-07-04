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

const EncodingErro