const std = @import("../std.zig");
const builtin = @import("builtin");
const assert = std.debug.assert;
const testing = std.testing;
const mem = std.mem;
const Loop = std.event.Loop;
const Allocator = std.mem.Allocator;

/// Thread-safe async/await lock.
/// Functions which are waiting for the lock are suspended, and
/// are resumed when the lock is released, in order.
/// Many readers can hold the lock at the same time; however locking for writing is exclusive.
/// When a read lock is held, it will not be released until the reader queue is empty.
/// When a write lock is held, it will not be released until the writer queue is empty.
/// TODO: make this API also work in blocking I/O mode
pub const RwLock = struct {
    shared_state: State,
    writer_queue: Queue,
    reader_queue: Queue,
    writer_queue_empty: bool,
    reader_queue_empty: bool,
    reader_lock_count: usize,

    const State = enum(u8) {
        Unlock