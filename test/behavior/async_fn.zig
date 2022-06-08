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
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;

        fn doTheTest() void {
            _ = async atest();
            resume frame;
        }

        fn atest() void {
            const result = func();
            expect(result.x == 5) catch @panic("test failed");
            expect(result.y == 6) catch @panic("test failed");
        }

        const Point = struct {
            x: usize,
            y: usize,
        };

        fn func() Point {
            suspend {
                frame = @frame();
            }
            return Point{
                .x = 5,
                .y = 6,
            };
        }
    };
    S.doTheTest();
}

test "pass string literal to async function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var ok: bool = false;

        fn doTheTest() !void {
            _ = async hello("hello");
            resume frame;
            try expect(ok);
        }

        fn hello(msg: []const u8) void {
            frame = @frame();
            suspend {}
            expectEqualStrings("hello", msg) catch @panic("test failed");
            ok = true;
        }
    };
    try S.doTheTest();
}

test "await inside an errdefer" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;

        fn doTheTest() !void {
            _ = async amainWrap();
            resume frame;
        }

        fn amainWrap() !void {
            var foo = async func();
            errdefer await foo;
            return error.Bad;
        }

        fn func() void {
            frame = @frame();
            suspend {}
        }
    };
    try S.doTheTest();
}

test "try in an async function with error union and non-zero-bit payload" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var ok = false;

        fn doTheTest() !void {
            _ = async amain();
            resume frame;
            try expect(ok);
        }

        fn amain() void {
            std.testing.expectError(error.Bad, theProblem()) catch @panic("test failed");
            ok = true;
        }

        fn theProblem() ![]u8 {
            frame = @frame();
            suspend {}
            const result = try other();
            return result;
        }

        fn other() ![]u8 {
            return error.Bad;
        }
    };
    try S.doTheTest();
}

test "returning a const error from async function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var ok = false;

        fn doTheTest() !void {
            _ = async amain();
            resume frame;
            try expect(ok);
        }

        fn amain() !void {
            var download_frame = async fetchUrl(10, "a string");
            const download_text = try await download_frame;
            _ = download_text;

            @panic("should not get here");
        }

        fn fetchUrl(unused: i32, url: []const u8) ![]u8 {
            _ = unused;
            _ = url;
            frame = @frame();
            suspend {}
            ok = true;
            return error.OutOfMemory;
        }
    };
    try S.doTheTest();
}

test "async/await typical usage" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    inline for ([_]bool{ false, true }) |b1| {
        inline for ([_]bool{ false, true }) |b2| {
            inline for ([_]bool{ false, true }) |b3| {
                inline for ([_]bool{ false, true }) |b4| {
                    testAsyncAwaitTypicalUsage(b1, b2, b3, b4).doTheTest();
                }
            }
        }
    }
}

fn testAsyncAwaitTypicalUsage(
    comptime simulate_fail_download: bool,
    comptime simulate_fail_file: bool,
    comptime suspend_download: bool,
    comptime suspend_file: bool,
) type {
    return struct {
        fn doTheTest() void {
            _ = async amainWrap();
            if (suspend_file) {
                resume global_file_frame;
            }
            if (suspend_download) {
                resume global_download_frame;
            }
        }
        fn amainWrap() void {
            if (amain()) |_| {
                expect(!simulate_fail_download) catch @panic("test failure");
                expect(!simulate_fail_file) catch @panic("test failure");
            } else |e| switch (e) {
                error.NoResponse => expect(simulate_fail_download) catch @panic("test failure"),
                error.FileNotFound => expect(simulate_fail_file) catch @panic("test failure"),
                else => @panic("test failure"),
            }
        }

        fn amain() !void {
            const allocator = std.testing.allocator;
            var download_frame = async fetchUrl(allocator, "https://example.com/");
            var download_awaited = false;
            errdefer if (!download_awaited) {
                if (await download_frame) |x| allocator.free(x) else |_| {}
            };

            var file_frame = async readFile(allocator, "something.txt");
            var file_awaited = false;
            errdefer if (!file_awaited) {
                if (await file_frame) |x| allocator.free(x) else |_| {}
            };

            download_awaited = true;
            const download_text = try await download_frame;
            defer allocator.free(download_text);

            file_awaited = true;
            const file_text = try await file_frame;
            defer allocator.free(file_text);

            try expect(std.mem.eql(u8, "expected download text", download_text));
            try expect(std.mem.eql(u8, "expected file text", file_text));
        }

        var global_download_frame: anyframe = undefined;
        fn fetchUrl(allocator: std.mem.Allocator, url: []const u8) anyerror![]u8 {
            _ = url;
            const result = try allocator.dupe(u8, "expected download text");
            errdefer allocator.free(result);
            if (suspend_download) {
                suspend {
                    global_download_frame = @frame();
                }
            }
            if (simulate_fail_download) return error.NoResponse;
            return result;
        }

        var global_file_frame: anyframe = undefined;
        fn readFile(allocator: std.mem.Allocator, filename: []const u8) anyerror![]u8 {
            _ = filename;
            const result = try allocator.dupe(u8, "expected file text");
            errdefer allocator.free(result);
            if (suspend_file) {
                suspend {
                    global_file_frame = @frame();
                }
            }
            if (simulate_fail_file) return error.FileNotFound;
            return result;
        }
    };
}

test "alignment of local variables in async functions" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn doTheTest() !void {
            var y: u8 = 123;
            _ = y;
            var x: u8 align(128) = 1;
            try expect(@ptrToInt(&x) % 128 == 0);
        }
    };
    try S.doTheTest();
}

test "no reason to resolve frame still works" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    _ = async simpleNothing();
}
fn simpleNothing() void {
    var x: i32 = 1234;
    _ = x;
}

test "async call a generic function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn doTheTest() !void {
            var f = async func(i32, 2);
            const result = await f;
            try expect(result == 3);
        }

        fn func(comptime T: type, inc: T) T {
            var x: T = 1;
            suspend {
                resume @frame();
            }
            x += inc;
            return x;
        }
    };
    _ = async S.doTheTest();
}

test "return from suspend block" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn doTheTest() !void {
            expect(func() == 1234) catch @panic("test failure");
        }
        fn func() i32 {
            suspend {
                return 1234;
            }
        }
    };
    _ = async S.doTheTest();
}

test "struct parameter to async function is copied to the frame" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        const Point = struct {
            x: i32,
            y: i32,
        };

        var frame: anyframe = undefined;

        fn doTheTest() void {
            _ = async atest();
            resume frame;
        }

        fn atest() void {
            var f: @Frame(foo) = undefined;
            bar(&f);
            clobberStack(10);
        }

        fn clobberStack(x: i32) void {
            if (x == 0) return;
            clobberStack(x - 1);
            var y: i32 = x;
            _ = y;
        }

        fn bar(f: *@Frame(foo)) void {
            var pt = Point{ .x = 1, .y = 2 };
            f.* = async foo(pt);
            var result = await f;
            expect(result == 1) catch @panic("test failure");
        }

        fn foo(point: Point) i32 {
            suspend {
                frame = @frame();
            }
            return point.x;
        }
    };
    S.doTheTest();
}

test "cast fn to async fn when it is inferred to be async" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var ok = false;

        fn doTheTest() void {
            var ptr: fn () callconv(.Async) i32 = undefined;
            ptr = func;
            var buf: [100]u8 align(16) = undefined;
            var result: i32 = undefined;
            const f = @asyncCall(&buf, &result, ptr, .{});
            _ = await f;
            expect(result == 1234) catch @panic("test failure");
            ok = true;
        }

        fn func() i32 {
            suspend {
                frame = @frame();
            }
            return 1234;
        }
    };
    _ = async S.doTheTest();
    resume S.frame;
    try expect(S.ok);
}

test "cast fn to async fn when it is inferred to be async, awaited directly" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var frame: anyframe = undefined;
        var ok = false;

        fn doTheTest() void {
            var ptr: fn () callconv(.Async) i32 = undefined;
            ptr = func;
            var buf: [100]u8 align(16) = undefined;
            var result: i32 = undefined;
            _ = await @asyncCall(&buf, &result, ptr, .{});
            expect(result == 1234) catch @panic("test failure");
            ok = true;
        }

        fn func() i32 {
            suspend {
                frame = @frame();
            }
            return 1234;
        }
    };
    _ = async S.doTheTest();
    resume S.frame;
    try expect(S.ok);
}

test "await does not force async if callee is blocking" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn simple() i32 {
            return 1234;
        }
    };
    var x = async S.simple();
    try expect(await x == 1234);
}

test "recursive async function" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    try expect(recursiveAsyncFunctionTest(false).doTheTest() == 55);
    try expect(recursiveAsyncFunctionTest(true).doTheTest() == 55);
}

fn recursiveAsyncFunctionTest(comptime suspending_implementation: bool) type {
    return struct {
        fn fib(allocator: std.mem.Allocator, x: u32) error{OutOfMemory}!u32 {
            if (x <= 1) return x;

            if (suspending_implementation) {
                suspend {
                    resume @frame();
                }
            }

            const f1 = try allocator.create(@Frame(fib));
            defer allocator.destroy(f1);

            const f2 = try allocator.create(@Frame(fib));
            defer allocator.destroy(f2);

            f1.* = async fib(allocator, x - 1);
            var f1_awaited = false;
            errdefer if (!f1_awaited) {
                _ = await f1;
            };

            f2.* = async fib(allocator, x - 2);
            var f2_awaited = false;
            errdefer if (!f2_awaited) {
                _ = await f2;
            };

            var sum: u32 = 0;

            f1_awaited = true;
            sum += try await f1;

            f2_awaited = true;
            sum += try await f2;

            return sum;
        }

        fn doTheTest() u32 {
            if (suspending_implementation) {
                var result: u32 = undefined;
                _ = async amain(&result);
                return result;
            } else {
                return fib(std.testing.allocator, 10) catch unreachable;
            }
        }

        fn amain(result: *u32) void {
            var x = async fib(std.testing.allocator, 10);
            result.* = (await x) catch unreachable;
        }
    };
}

test "@asyncCall with comptime-known function, but not awaited directly" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var global_frame: anyframe = undefined;

        fn doTheTest() !void {
            var frame: [1]@Frame(middle) = undefined;
            var result: @typeInfo(@typeInfo(@TypeOf(middle)).Fn.return_type.?).ErrorUnion.error_set!void = undefined;
            _ = @asyncCall(std.mem.sliceAsBytes(frame[0..]), &result, middle, .{});
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

test "@asyncCall with actual frame instead of byte buffer" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn func() i32 {
            suspend {}
            return 1234;
        }
    };
    var frame: @Frame(S.func) = undefined;
    var result: i32 = undefined;
    const ptr = @asyncCall(&frame, &result, S.func, .{});
    resume ptr;
    try expect(result == 1234);
}

test "@asyncCall using the result location inside the frame" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn simple2(y: *i32) callconv(.Async) i32 {
            defer y.* += 2;
            y.* += 1;
            suspend {}
            return 1234;
        }
        fn getAnswer(f: anyframe->i32, out: *i32) void {
            out.* = await f;
        }
    };
    var data: i32 = 1;
    const Foo = struct {
        bar: fn (*i32) callconv(.Async) i32,
    };
    var foo = Foo{ .bar = S.simple2 };
    var bytes: [64]u8 align(16) = undefined;
    const f = @asyncCall(&bytes, {}, foo.bar, .{&data});
    comptime try expect(@TypeOf(f) == anyframe->i32);
    try expect(data == 2);
    resume f;
    try expect(data == 4);
    _ = async S.getAnswer(f, &data);
    try expect(data == 1234);
}

test "@TypeOf an async function call of generic fn with error union type" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        fn func(comptime x: anytype) anyerror!i32 {
            const T = @TypeOf(async func(x));
            comptime try expect(T == @typeInfo(@TypeOf(@frame())).Pointer.child);
            return undefined;
        }
    };
    _ = async S.func(i32);
}

test "using @TypeOf on a generic function call" {
    if (true) return error.SkipZigTest; // TODO
    if (builtin.os.tag == .wasi) return error.SkipZigTest; // TODO

    const S = struct {
        var global_frame: anyframe = undefined;
        var global_ok = false;

        var buf: [100]u8 align(16) = undefined;

        fn amain(x: anytype) void {
            if (x == 0) {
                global_ok = true;
                return;
            }
            suspend {
                global_frame = @frame();
            }
            const F = @TypeOf(async amain(x - 1));
            const frame = @intToPtr(*F, @ptrToInt(&buf));
            return await @asyncCall(frame, {}, amain, .{x - 1});
        }
    };
    _ = async S.amain(@as(u32, 1));
    resume S.global_frame;
    try expect(S.global_ok);
}

test "recursive call of await @asyncCall with struct