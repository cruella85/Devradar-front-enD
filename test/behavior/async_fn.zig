const std = @import("std");
const builtin = @import("builtin");
const expect = std.testing.expect;
const expectEqual = std.testing.expectEqual;
const expectEqualStrings = std.testing.expectEqualStrings;
const expectError = std.testing.expectError;

var global_x: i32 = 1;

test "simple coroutine suspend and resume" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    var frame = async simpleAsyncFn();
    try expect(global_x == 2);
    resume frame;
    try expect(global_x == 3);
    const af: anyframe->void = &frame;
    _ = af;
    resume frame;
    try expect(global_x == 4);
}
fn simpleAsyncFn() void {
    global_x += 1;
    suspend {}
    global_x += 1;
    suspend {}
    global_x += 1;
}

var global_y: i32 = 1;

test "pass parameter to coroutine" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    var p = async simpleAsyncFnWithArg(2);
    try expect(global_y == 3);
    resume p;
    try expect(global_y == 5);
}
fn simpleAsyncFnWithArg(delta: i32) void {
    global_y += delta;
    suspend {}
    global_y += delta;
}

test "suspend at end of function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var x: i32 = 1;

        fn doTheTest() !void {
            try expect(x == 1);
            const p = async suspendAtEnd();
            _ = p;
            try expect(x == 2);
        }

        fn suspendAtEnd() void {
            x += 1;
            suspend {}
        }
    };
    try S.doTheTest();
}

test "local variable in async function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var x: i32 = 0;

        fn doTheTest() !void {
            try expect(x == 0);
            var p = async add(1, 2);
            try expect(x == 0);
            resume p;
            try expect(x == 0);
            resume p;
            try expect(x == 0);
            resume p;
            try expect(x == 3);
        }

        fn add(a: i32, b: i32) void {
            var accum: i32 = 0;
            suspend {}
            accum += a;
            suspend {}
            accum += b;
            suspend {}
            x = accum;
        }
    };
    try S.doTheTest();
}

test "calling an inferred async function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var x: i32 = 1;
        var other_frame: *@Frame(other) = undefined;

        fn doTheTest() !void {
            _ = async first();
            try expect(x == 1);
            resume other_frame.*;
            try expect(x == 2);
        }

        fn first() void {
            other();
        }
        fn other() void {
            other_frame = @frame();
            suspend {}
            x += 1;
        }
    };
    try S.doTheTest();
}

test "@frameSize" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    if (builtin.target.cpu.arch == .thumb or builtin.target.cpu.arch == .thumbeb)
        return error.SkipZigTest;

    const S = struct {
        fn doTheTest() !void {
            {
                var ptr = @ptrCast(fn (i32) callconv(.Async) void, other);
                const size = @frameSize(ptr);
                try expect(size == @sizeOf(@Frame(other)));
            }
            {
                var ptr = @ptrCast(fn () callconv(.Async) void, first);
                const size = @frameSize(ptr);
                try expect(size == @sizeOf(@Frame(first)));
            }
        }

        fn first() void {
            other(1);
        }
        fn other(param: i32) void {
            _ = param;
            var local: i32 = undefined;
            _ = local;
            suspend {}
        }
    };
    try S.doTheTest();
}

test "coroutine suspend, resume" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;

        fn doTheTest() !void {
            _ = async amain();
            seq('d');
            resume frame;
            seq('h');

            try expect(std.mem.eql(u8, &points, "abcdefgh"));
        }

        fn amain() void {
            seq('a');
            var f = async testAsyncSeq();
            seq('c');
            await f;
            seq('g');
        }

        fn testAsyncSeq() void {
            defer seq('f');

            seq('b');
            suspend {
                frame = @frame();
            }
            seq('e');
        }
        var points = [_]u8{'x'} ** "abcdefgh".len;
        var index: usize = 0;

        fn seq(c: u8) void {
            points[index] = c;
            index += 1;
        }
    };
    try S.doTheTest();
}

test "coroutine suspend with block" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const p = async testSuspendBlock();
    _ = p;
    try expect(!global_result);
    resume a_promise;
    try expect(global_result);
}

var a_promise: anyframe = undefined;
var global_result = false;
fn testSuspendBlock() callconv(.Async) void {
    suspend {
        comptime expect(@TypeOf(@frame()) == *@Frame(testSuspendBlock)) catch unreachable;
        a_promise = @frame();
    }

    // Test to make sure that @frame() works as advertised (issue #1296)
    // var our_handle: anyframe = @frame();
    expect(a_promise == @as(anyframe, @frame())) catch @panic("test failed");

    global_result = true;
}

var await_a_promise: anyframe = undefined;
var await_final_result: i32 = 0;

test "coroutine await" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.