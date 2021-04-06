//! The engines provided here should be initialized from an external source.
//! For a thread-local cryptographically secure pseudo random number generator,
//! use `std.crypto.random`.
//! Be sure to use a CSPRNG when required, otherwise using a normal PRNG will
//! be faster and use substantially less stack space.
//!
//! TODO(tiehuis): Benchmark these against other reference implementations.

const std = @import("std.zig");
const builtin = @import("builtin");
const assert = std.debug.assert;
const mem = std.mem;
const math = std.math;
const ziggurat = @import("rand/ziggurat.zig");
const maxInt = std.math.maxInt;

/// Fast unbiased random numbers.
pub const DefaultPrng = Xoshiro256;

/// Cryptographically secure random numbers.
pub const DefaultCsprng = Ascon;

pub const Ascon = @import("rand/Ascon.zig");
pub const Isaac64 = @import("rand/Isaac64.zig");
pub const Xoodoo = @import("rand/Xoodoo.zig");
pub const Pcg = @import("rand/Pcg.zig");
pub const Xoroshiro128 = @import("rand/Xoroshiro128.zig");
pub const Xoshiro256 = @import("rand/Xoshiro256.zig");
pub const Sfc64 = @import("rand/Sfc64.zig");
pub const RomuTrio = @import("rand/RomuTrio.zig");

pub const Random = struct {
    ptr: *anyopaque,
    fillFn: *const fn (ptr: *anyopaque, buf: []u8) void,

    pub fn init(pointer: anytype, comptime fillFn: fn (ptr: @TypeOf(pointer), buf: []u8) void) Random {
        const Ptr = @TypeOf(pointer);
        assert(@typeInfo(Ptr) == .Pointer); // Must be a pointer
        assert(@typeInfo(Ptr).Pointer.size == .One); // Must be a single-item pointer
        assert(@typeInfo(@typeInfo(Ptr).Pointer.child) == .Struct); // Must point to a struct
        const gen = struct {
            fn fill(ptr: *anyopaque, buf: []u8) void {
                const alignment = @typeInfo(Ptr).Pointer.alignment;
                const self = @ptrCast(Ptr, @alignCast(alignment, ptr));
                fillFn(self, buf);
            }
        };

        return .{
            .ptr = pointer,
            .fillFn = gen.fill,
        };
    }

    /// Read random bytes into the specified buffer until full.
    pub fn bytes(r: Random, buf: []u8) void {
        r.fillFn(r.ptr, buf);
    }

    pub fn boolean(r: Random) bool {
        return r.int(u1) != 0;
    }

    /// Returns a random value from an enum, evenly distributed.
    ///
    /// Note that this will not yield consistent results across all targets
    /// due to dependence on the representation of `usize` as an index.
    /// See `enumValueWithIndex` for further commentary.
    pub inline fn enumValue(r: Random, comptime EnumType: type) EnumType {
        return r.enumValueWithIndex(EnumType, usize);
    }

    /// Returns a random value from an enum, evenly distributed.
    ///
    /// An index into an array of all named values is generated using the
    /// specified `Index` type to determine the return value.
    /// This allows for results to be independent of `usize` representation.
    ///
    /// Prefer `enumValue` if this isn't important.
    ///
    /// See `uintLessThan`, which this function uses in most cases,
    /// for commentary on the runtime of this function.
    pub fn enumValueWithIndex(r: Random, comptime EnumType: type, comptime Index: type) EnumType {
        comptime assert(@typeInfo(EnumType) == .Enum);

        // We won't use int -> enum casting because enum elements can have
        //  arbitrary values.  Instead we'll randomly pick one of the type's values.
        const values = comptime std.enums.values(EnumType);
        comptime assert(values.len > 0); // can't return anything
        comptime assert(maxInt(Index) >= values.len - 1); // can't access all values
        comptime if (values.len == 1) return values[0];

        const index = if (comptime values.len - 1 == maxInt(Index))
            r.int(Index)
        else
            r.uintLessThan(Index, values.len);

        const MinInt = MinArrayIndex(Index);
        return values[@intCast(MinInt, index)];
    }

    /// Returns a random int `i` such that `minInt(T) <= i <= maxInt(T)`.
    /// `i` is evenly distributed.
    pub fn int(r: Random, comptime T: type) T {
        const bits = @typeInfo(T).Int.bits;
        const UnsignedT = std.meta.Int(.unsigned, bits);
        const ByteAlignedT = std.meta.Int(.unsigned, @divTrunc(bits + 7, 8) * 8);

        var rand_bytes: [@sizeOf(ByteAlignedT)]u8 = undefined;
        r.bytes(rand_bytes[0..]);

        // use LE instead of native endian for better portability maybe?
        // TODO: endian portability is pointless if the underlying prng isn't endian portable.
        // TODO: document the endian portability of this library.
        const byte_aligned_result = mem.readIntSliceLittle(ByteAlignedT, &rand_bytes);
        const unsigned_result = @truncate(UnsignedT, byte_aligned_result);
        return @bitCast(T, unsigned_result);
    }

    /// Constant-time implementation off `uintLessThan`.
    /// The results of this function may be biased.
    pub fn uintLessThanBiased(r: Random, comptime T: type, less_than: T) T {
        comptime assert(@typeInfo(T).Int.signedness == .unsigned);
        const bits = @typeInfo(T).Int.bits;
        comptime assert(bits <= 64); // TODO: workaround: LLVM ERROR: Unsupported library call operation!
        assert(0 < less_than);
        if (bits <= 32) {
            return @intCast(T, limitRangeBiased(u32, r.int(u32), less_than));
        } else {
            return @intCast(T, limitRangeBiased(u64, r.int(u64), less_than));
        }
    }

    /// Returns an evenly distributed random unsigned integer `0 <= i < less_than`.
    /// This function assumes that the underlying `fillFn` produces evenly distributed values.
    /// Within this assumption, the runtime of this function is exponentially distributed.
    /// If `fillFn` were backed by a true random generator,
    /// the runtime of this function would technically be unbounded.
    /// However, if `fillFn` is backed by any evenly distributed pseudo random number generator,
    /// this function is guaranteed to return.
    /// If you need deterministic runtime bounds, use `uintLessThanBiased`.
    pub fn uintLessThan(r: Random, comptime T: type, less_than: T) T {
        comptime assert(@typeInfo(T).Int.signedness == .unsigned);
        const bits = @typeInfo(T).Int.bits;
        comptime assert(bits <= 64); // TODO: workaround: LLVM ERROR: Unsupported library call operation!
        assert(0 < less_than);
        // Small is typically u32
        const small_bits = @divTrunc(bits + 31, 32) * 32;
        const Small = std.meta.Int(.unsigned, small_bits);
        // Large is typically u64
        const Large = std.meta.Int(.unsigned, small_bits * 2);

        // adapted from:
        //   http://www.pcg-random.org/posts/bounded-rands.html
        //   "Lemire's (with an extra tweak from me)"
        var x: Small = r.int(Small);
        var m: Large = @as(Large, x) * @as(Large, less_than);
        var l: Small = @truncate(Small, m);
        if (l < less_than) {
            var t: Small = -%less_than;

            if (t >= less_than) {
                t -= less_than;
                if (t >= less_than) {
                    t %= less_than;
                }
            }
            while (l < t) {
                x = r.int(Small);
                m = @as(Large, x) * @as(Large, less_than);
                l = @truncate(Small, m);
            }
        }
        return @intCast(T, m >> small_bits);
    }

    /// Constant-time implementation off `uintAtMost`.
    /// The results of this function may be biased.
    pub fn uintAtMostBiased(r: Random, comptime T: type, at_most: T) T {
        assert(@typeInfo(T).Int.signedness == .unsigned);
        if (at_most == maxInt(T)) {
            // have the full range
            return r.int(T);
        }
        return r.uintLessThanBiased(T, at_most + 1);
    }

    /// Returns an evenly distributed random unsigned integer `0 <= i <= at_most`.
    /// See `uintLessThan`, which this function uses in most cases,
    /// for commentary on the runtime of this function.
    pub fn uintAtMost(r: Random, comptime T: type, at_most: T) T {
        assert(@typeInfo(T).Int.signedness == .unsigned);
        if (at_most == maxInt(T)) {
            // have the full range
            return r.int(T);
        }
        return r.uintLessThan(T, at_most + 1);
    }

    /// Constant-time implementation off `intRangeLessThan`.
    /// The results of this function may be biased.
    pub fn intRangeLessThanBiased(r: Random, comptime T: type, at_least: T, less_than: T) T {
        assert(at_least < less_than);
        const info = @typeInfo(T).Int;
        if (info.signedness == .signed) {
            // Two's complement makes this math pretty easy.
            const UnsignedT = std.meta.Int(.unsigned, info.bits);
            const lo = @bitCast(UnsignedT, at_least);
            const hi = @bit