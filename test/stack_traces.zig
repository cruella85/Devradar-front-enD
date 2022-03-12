const std = @import("std");
const os = std.os;
const tests = @import("tests.zig");

pub fn addCases(cases: *tests.StackTracesContext) void {
    cases.addCase(.{
        .name = "return",
        .source =
        \\pub fn main() !void {
        \\    return error.TheSkyIsFalling;
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in main (test)
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
                .linux, // defeated by aggressive inlining
            },
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in [function]
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\
            ,
        },
        .ReleaseFast = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
        .ReleaseSmall = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
    });

    cases.addCase(.{
        .name = "try return",
        .source =
        \\fn foo() !void {
        \\    return error.TheSkyIsFalling;
        \\}
        \\
        \\pub fn main() !void {
        \\    try foo();
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in foo (test)
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\source.zig:6:5: [address] in main (test)
            \\    try foo();
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
            },
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in [function]
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\source.zig:6:5: [address] in [function]
            \\    try foo();
            \\    ^
            \\
            ,
        },
        .ReleaseFast = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
        .ReleaseSmall = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
    });
    cases.addCase(.{
        .name = "non-error return pops error trace",
        .source =
        \\fn bar() !void {
        \\    return error.UhOh;
        \\}
        \\
        \\fn foo() !void {
        \\    bar() catch {
        \\        return; // non-error result: success
        \\    };
        \\}
        \\
        \\pub fn main() !void {
        \\    try foo();
        \\    return error.UnrelatedError;
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: UnrelatedError
            \\source.zig:13:5: [address] in main (test)
            \\    return error.UnrelatedError;
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
                .linux, // defeated by aggressive inlining
            },
            .expect =
            \\error: UnrelatedError
            \\source.zig:13:5: [address] in [function]
            \\    return error.UnrelatedError;
            \\    ^
            \\
            ,
        },
        .ReleaseFast = .{
            .expect =
            \\error: UnrelatedError
            \\
            ,
        },
        .ReleaseSmall = .{
            .expect =
            \\error: UnrelatedError
            \\
            ,
        },
    });

    cases.addCase(.{
        .name = "continue in while loop",
        .source =
        \\fn foo() !void {
        \\    return error.UhOh;
        \\}
        \\
        \\pub fn main() !void {
        \\    var i: usize = 0;
        \\    while (i < 3) : (i += 1) {
        \\        foo() catch continue;
        \\    }
        \\    return error.UnrelatedError;
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: UnrelatedError
            \\source.zig:10:5: [address] in main (test)
            \\    return error.UnrelatedError;
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
                .linux, // defeated by aggressive inlining
            },
            .expect =
            \\error: UnrelatedError
            \\source.zig:10:5: [address] in [function]
            \\    return error.UnrelatedError;
            \\    ^
            \\
            ,
        },
        .ReleaseFast = .{
            .expect =
            \\error: UnrelatedError
            \\
            ,
        },
        .ReleaseSmall = .{
            .expect =
            \\error: UnrelatedError
            \\
            ,
        },
    });

    cases.addCase(.{
        .name = "try return + handled catch/if-else",
        .source =
        \\fn foo() !void {
        \\    return error.TheSkyIsFalling;
        \\}
        \\
        \\pub fn main() !void {
        \\    foo() catch {}; // should not affect error trace
        \\    if (foo()) |_| {} else |_| {
        \\        // should also not affect error trace
        \\    }
        \\    try foo();
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in foo (test)
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\source.zig:10:5: [address] in main (test)
            \\    try foo();
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
                .linux, // defeated by aggressive inlining
            },
            .expect =
            \\error: TheSkyIsFalling
            \\source.zig:2:5: [address] in [function]
            \\    return error.TheSkyIsFalling;
            \\    ^
            \\source.zig:10:5: [address] in [function]
            \\    try foo();
            \\    ^
            \\
            ,
        },
        .ReleaseFast = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
        .ReleaseSmall = .{
            .expect =
            \\error: TheSkyIsFalling
            \\
            ,
        },
    });

    cases.addCase(.{
        .name = "break from inline loop pops error return trace",
        .source =
        \\fn foo() !void { return error.FooBar; }
        \\
        \\pub fn main() !void {
        \\    comptime var i: usize = 0;
        \\    b: inline while (i < 5) : (i += 1) {
        \\        foo() catch {
        \\            break :b; // non-error break, success
        \\        };
        \\    }
        \\    // foo() was successfully handled, should not appear in trace
        \\
        \\    return error.BadTime;
        \\}
        ,
        .Debug = .{
            .expect =
            \\error: BadTime
            \\source.zig:12:5: [address] in main (test)
            \\    return error.BadTime;
            \\    ^
            \\
            ,
        },
        .ReleaseSafe = .{
            .exclude_os = .{
                .windows, // TODO
                .linux, // defeated by aggressive inlining
            },
            .expect =
            \\error: BadTime
            \\source.zig:12:5: [address] in [function]
            \\    return error.BadTime;
            \\    ^
            \\
            ,
        },
        .ReleaseFa