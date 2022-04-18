const std = @import("std.zig");
const builtin = @import("builtin");

const math = std.math;
const print = std.debug.print;

pub const FailingAllocator = @import("testing/failing_allocator.zig").FailingAllocator;

/// This should only be used in temporary test programs.
pub const allocator = allocator_instance.allocator();
pub var allocator_instance = b: {
    if (!builtin.is_test)
        @compileError("Cannot use testing allocator outside of test block");
    break :b std.heap.GeneralPurposeAllocator(.{}){};
};

pub const failing_allocator = failing_allocator_instance.allocator();
pub var failing_allocator_instance = FailingAllocator.init(base_allocator_instance.allocator(), 0);

pub var base_allocator_instance = std.heap.FixedBufferAllocator.init("");

/// TODO https://github.com/ziglang/zig/issues/5738
pub var log_level = std.log.Level.warn;

/// This function is intended to be used only in tests. It prints diagnostics to stderr
/// and then returns a test failure error when actual_error_union is not expected_error.
pub fn expectError(expected_error: anyerror, actual_error_union: anytype) !void {
    if (actual_error_union) |actual_payload| {
        std.debug.print("expected error.{s}, found {any}\n", .{ @errorName(expected_error), actual_payload });
        return error.TestUnexpectedError;
    } else |actual_error| {
        if (expected_error != actual_error) {
            std.debug.print("expected error.{s}, found error.{s}\n", .{
                @errorName(expected_error),
                @errorName(actual_error),
            });
            return error.TestExpectedError;
        }
    }
}

/// This function is intended to be used only in tests. When the two values are not
/// equal, prints diagnostics to stderr to show exactly how they are not equal,
/// then returns a test failure error.
/// `actual` is casted to the type of `expected`.
pub fn expectEqual(expected: anytype, actual: @TypeOf(expected)) !void {
    switch (@typeInfo(@TypeOf(actual))) {
        .NoReturn,
        .Opaque,
        .Frame,
        .AnyFrame,
        => @compileError("value of type " ++ @typeName(@TypeOf(actual)) ++ " encountered"),

        .Undefined,
        .Null,
        .Void,
        => return,

        .Type => {
            if (actual != expected) {
                std.debug.print("expected type {s}, found type {s}\n", .{ @typeName(expected), @typeName(actual) });
                return error.TestExpectedEqual;
            }
        },

        .Bool,
        .Int,
        .Float,
        .ComptimeFloat,
        .ComptimeInt,
        .EnumLiteral,
        .Enum,
        .Fn,
        .ErrorSet,
        => {
            if (actual != expected) {
                std.debug.print("expected {}, found {}\n", .{ expected, actual });
                return error.TestExpectedEqual;
            }
        },

        .Pointer => |pointer| {
            switch (pointer.size) {
                .One, .Many, .C => {
                    if (actual != expected) {
                        std.debug.print("expected {*}, found {*}\n", .{ expected, actual });
                        return error.TestExpectedEqual;
                    }
                },
                .Slice => {
                    if (actual.ptr != expected.ptr) {
                        std.debug.print("expected slice ptr {*}, found {*}\n", .{ expected.ptr, actual.ptr });
                        return error.TestExpectedEqual;
                    }
                    if (actual.len != expected.len) {
                        std.debug.print("expected slice len {}, found {}\n", .{ expected.len, actual.len });
                        return error.TestExpectedEqual;
                    }
                },
            }
        },

        .Array => |array| try expectEqualSlices(array.child, &expected, &actual),

        .Vector => |info| {
            var i: usize = 0;
            while (i < info.len) : (i += 1) {
                if (!std.meta.eql(expected[i], actual[i])) {
                    std.debug.print("index {} incorrect. expected {}, found {}\n", .{
                        i, expected[i], actual[i],
                    });
                    return error.TestExpectedEqual;
                }
            }
        },

        .Struct => |structType| {
            inline for (structType.fields) |field| {
                try expectEqual(@field(expected, field.name), @field(actual, field.name));
            }
        },

        .Union => |union_info| {
            if (union_info.tag_type == null) {
                @compileError("Unable to compare untagged union values");
            }

            const Tag = std.meta.Tag(@TypeOf(expected));

            const expectedTag = @as(Tag, expected);
            const actualTag = @as(Tag, actual);

            try expectEqual(expectedTag, actualTag);

            // we only reach this loop if the tags are equal
            inline for (std.meta.fields(@TypeOf(actual))) |fld| {
                if (std.mem.eql(u8, fld.name, @tagName(actualTag))) {
                    try expectEqual(@field(expected, fld.name), @field(actual, fld.name));
                    return;
                }
            }

            // we iterate over *all* union fields
            // => we should never get here as the loop above is
            //    including all possible values.
            unreachable;
        },

        .Optional => {
            if (expected) |expected_payload| {
                if (actual) |actual_payload| {
                    try expectEqual(expected_payload, actual_payload);
                } else {
                    std.debug.print("expected {any}, found null\n", .{expected_payload});
                    return error.TestExpectedEqual;
                }
            } else {
                if (actual) |actual_payload| {
                    std.debug.print("expected null, found {any}\n", .{actual_payload});
                    return error.TestExpectedEqual;
                }
            }
        },

        .ErrorUnion => {
            if (expected) |expected_payload| {
                if (actual) |actual_payload| {
                    try expectEqual(expected_payload, actual_payload);
                } else |actual_err| {
                    std.debug.print("expected {any}, found {}\n", .{ expected_payload, actual_err });
                    return error.TestExpectedEqual;
                }
            } else |expected_err| {
                if (actual) |actual_payload| {
                    std.debug.print("expected {}, found {any}\n", .{ expected_err, actual_payload });
                    return error.TestExpectedEqual;
                } else |actual_err| {
                    try expectEqual(expected_err, actual_err);
                }
            }
        },
    }
}

test "expectEqual.union(enum)" {
    const T = union(enum) {
        a: i32,
        b: f32,
    };

    const a10 = T{ .a = 10 };

    try expectEqual(a10, a10);
}

/// This function is intended to be used only in tests. When t