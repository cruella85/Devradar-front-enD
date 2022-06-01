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
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    await_seq('a');
    var p = async await_amain();
    _ = p;
    await_seq('f');
    resume await_a_promise;
    await_seq('i');
    try expect(await_final_result == 1234);
    try expect(std.mem.eql(u8, &await_points, "abcdefghi"));
}
fn await_amain() callconv(.Async) void {
    await_seq('b');
    var p = async await_another();
    await_seq('e');
    await_final_result = await p;
    await_seq('h');
}
fn await_another() callconv(.Async) i32 {
    await_seq('c');
    suspend {
        await_seq('d');
        await_a_promise = @frame();
    }
    await_seq('g');
    return 1234;
}

var await_points = [_]u8{0} ** "abcdefghi".len;
var await_seq_index: usize = 0;

fn await_seq(c: u8) void {
    await_points[await_seq_index] = c;
    await_seq_index += 1;
}

var early_final_result: i32 = 0;

test "coroutine await early return" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    early_seq('a');
    var p = async early_amain();
    _ = p;
    early_seq('f');
    try expect(early_final_result == 1234);
    try expect(std.mem.eql(u8, &early_points, "abcdef"));
}
fn early_amain() callconv(.Async) void {
    early_seq('b');
    var p = async early_another();
    early_seq('d');
    early_final_result = await p;
    early_seq('e');
}
fn early_another() callconv(.Async) i32 {
    early_seq('c');
    return 1234;
}

var early_points = [_]u8{0} ** "abcdef".len;
var early_seq_index: usize = 0;

fn early_seq(c: u8) void {
    early_points[early_seq_index] = c;
    early_seq_index += 1;
}

test "async function with dot syntax" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var y: i32 = 1;
        fn foo() callconv(.Async) void {
            y += 1;
            suspend {}
        }
    };
    const p = async S.foo();
    _ = p;
    try expect(S.y == 2);
}

test "async fn pointer in a struct field" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    var data: i32 = 1;
    const Foo = struct {
        bar: fn (*i32) callconv(.Async) void,
    };
    var foo = Foo{ .bar = simpleAsyncFn2 };
    var bytes: [64]u8 align(16) = undefined;
    const f = @asyncCall(&bytes, {}, foo.bar, .{&data});
    comptime try expect(@TypeOf(f) == anyframe->void);
    try expect(data == 2);
    resume f;
    try expect(data == 4);
    _ = async doTheAwait(f);
    try expect(data == 4);
}

fn doTheAwait(f: anyframe->void) void {
    await f;
}
fn simpleAsyncFn2(y: *i32) callconv(.Async) void {
    defer y.* += 2;
    y.* += 1;
    suspend {}
}

test "@asyncCall with return type" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const Foo = struct {
        bar: fn () callconv(.Async) i32,

        var global_frame: anyframe = undefined;
        fn middle() callconv(.Async) i32 {
            return afunc();
        }

        fn afunc() i32 {
            global_frame = @frame();
            suspend {}
            return 1234;
        }
    };
    var foo = Foo{ .bar = Foo.middle };
    var bytes: [150]u8 align(16) = undefined;
    var aresult: i32 = 0;
    _ = @asyncCall(&bytes, &aresult, foo.bar, .{});
    try expect(aresult == 0);
    resume Foo.global_frame;
    try expect(aresult == 1234);
}

test "async fn with inferred error set" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var global_frame: anyframe = undefined;

        fn doTheTest() !void {
            var frame: [1]@Frame(middle) = undefined;
            var fn_ptr = middle;
            var result: @typeInfo(@typeInfo(@TypeOf(fn_ptr)).Fn.return_type.?).ErrorUnion.error_set!void = undefined;
            _ = @asyncCall(std.mem.sliceAsBytes(frame[0..]), &result, fn_ptr, .{});
            resume global_frame;
            try std.testing.expectError(error.Fail, result);
        }
        fn middle() callconv(.Async) !void {
            var f = async middle2();
            return await f;
        }

        fn middle2() !void {
            return failing();
        }

        fn failing() !void {
            global_frame = @frame();
            suspend {}
            return error.Fail;
        }
    };
    try S.doTheTest();
}

test "error return trace across suspend points - early return" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const p = nonFailing();
    resume p;
    const p2 = async printTrace(p);
    _ = p2;
}

test "error return trace across suspend points - async return" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const p = nonFailing();
    const p2 = async printTrace(p);
    _ = p2;
    resume p;
}

fn nonFailing() (anyframe->anyerror!void) {
    const Static = struct {
        var frame: @Frame(suspendThenFail) = undefined;
    };
    Static.frame = async suspendThenFail();
    return &Static.frame;
}
fn suspendThenFail() callconv(.Async) anyerror!void {
    suspend {}
    return error.Fail;
}
fn printTrace(p: anyframe->(anyerror!void)) callconv(.Async) void {
    (await p) catch |e| {
        std.testing.expect(e == error.Fail) catch @panic("test failure");
        if (@errorReturnTrace()) |trace| {
            expect(trace.index == 1) catch @panic("test failure");
        } else switch (builtin.mode) {
            .Debug, .ReleaseSafe => @panic("expected return trace"),
            .ReleaseFast, .ReleaseSmall => {},
        }
    };
}

test "break from suspend" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    var my_result: i32 = 1;
    const p = async testBreakFromSuspend(&my_result);
    _ = p;
    try std.testing.expect(my_result == 2);
}
fn testBreakFromSuspend(my_result: *i32) callconv(.Async) void {
    suspend {
        resume @frame();
    }
    my_result.* += 1;
    suspend {}
    my_result.* += 1;
}

test "heap allocated async function frame" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var x: i32 = 42;

        fn doTheTest() !void {
            const frame = try std.testing.allocator.create(@Frame(someFunc));
            defer std.testing.allocator.destroy(frame);

            try expect(x == 42);
            frame.* = async someFunc();
            try expect(x == 43);
            resume frame;
            try expect(x == 44);
        }

        fn someFunc() void {
            x += 1;
            suspend {}
            x += 1;
        }
    };
    try S.doTheTest();
}

test "async function call return value" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var pt = Point{ .x = 10, .y = 11 };

        fn doTheTest() !void {
            try expectEqual(pt.x, 10);
            try expectEqual(pt.y, 11);
            _ = async first();
            try expectEqual(pt.x, 10);
            try expectEqual(pt.y, 11);
            resume frame;
            try expectEqual(pt.x, 1);
            try expectEqual(pt.y, 2);
        }

        fn first() void {
            pt = second(1, 2);
        }

        fn second(x: i32, y: i32) Point {
            return other(x, y);
        }

        fn other(x: i32, y: i32) Point {
            frame = @frame();
            suspend {}
            return Point{
                .x = x,
                .y = y,
            };
        }

        const Point = struct {
            x: i32,
            y: i32,
        };
    };
    try S.doTheTest();
}

test "suspension points inside branching control flow" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var result: i32 = 10;

        fn doTheTest() !void {
            try expect(10 == result);
            var frame = async func(true);
            try expect(10 == result);
            resume frame;
            try expect(11 == result);
            resume frame;
            try expect(12 == result);
            resume frame;
            try expect(13 == result);
        }

        fn func(b: bool) void {
            while (b) {
                suspend {}
                result += 1;
            }
        }
    };
    try S.doTheTest();
}

test "call async function which has struct return type" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return er