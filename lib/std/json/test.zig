// RFC 8529 conformance tests.
//
// Tests are taken from https://github.com/nst/JSONTestSuite
// Read also http://seriot.ch/parsing_json.php for a good overview.

const std = @import("../std.zig");
const json = std.json;
const testing = std.testing;
const TokenStream = std.json.TokenStream;
const parse = std.json.parse;
const ParseOptions = std.json.ParseOptions;
const parseFree = std.json.parseFree;
const Parser = std.json.Parser;
const mem = std.mem;
const writeStream = std.json.writeStream;
const Value = std.json.Value;
const StringifyOptions = std.json.StringifyOptions;
const stringify = std.json.stringify;
const stringifyAlloc = std.json.stringifyAlloc;
const StreamingParser = std.json.StreamingParser;
const Token = std.json.Token;
const validate = std.json.validate;
const Array = std.json.Array;
const ObjectMap = std.json.ObjectMap;
const assert = std.debug.assert;

fn testNonStreaming(s: []const u8) !void {
    var p = json.Parser.init(testing.allocator, false);
    defer p.deinit();

    var tree = try p.parse(s);
    defer tree.deinit();
}

fn ok(s: []const u8) !void {
    try testing.expect(json.validate(s));

    try testNonStreaming(s);
}

fn err(s: []const u8) !void {
    try testing.expect(!json.validate(s));

    try testing.expect(std.meta.isError(testNonStreaming(s)));
}

fn utf8Error(s: []const u8) !void {
    try testing.expect(!json.validate(s));

    try testing.expectError(error.InvalidUtf8Byte, testNonStreaming(s));
}

fn any(s: []const u8) !void {
    _ = json.validate(s);

    testNonStreaming(s) catch {};
}

fn anyStreamingErrNonStreaming(s: []const u8) !void {
    _ = json.validate(s);

    try testing.expect(std.meta.isError(testNonStreaming(s)));
}

fn roundTrip(s: []const u8) !void {
    try testing.expect(json.validate(s));

    var p = json.Parser.init(testing.allocator, false);
    defer p.deinit();

    var tree = try p.parse(s);
    defer tree.deinit();

    var buf: [256]u8 = undefined;
    var fbs = std.io.fixedBufferStream(&buf);
    try tree.root.jsonStringify(.{}, fbs.writer());

    try testing.expectEqualStrings(s, fbs.getWritten());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Additional tests not part of test JSONTestSuite.

test "y_trailing_comma_after_empty" {
    try roundTrip(
        \\{"1":[],"2":{},"3":"4"}
    );
}

test "n_object_closed_missing_value" {
    try err(
        \\{"a":}
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

test "y_array_arraysWithSpaces" {
    try ok(
        \\[[]   ]
    );
}

test "y_array_empty" {
    try roundTrip(
        \\[]
    );
}

test "y_array_empty-string" {
    try roundTrip(
        \\[""]
    );
}

test "y_array_ending_with_newline" {
    try roundTrip(
        \\["a"]
    );
}

test "y_array_false" {
    try roundTrip(
        \\[false]
    );
}

test "y_array_heterogeneous" {
    try ok(
        \\[null, 1, "1", {}]
    );
}

test "y_array_null" {
    try roundTrip(
        \\[null]
    );
}

test "y_array_with_1_and_newline" {
    try ok(
        \\[1
        \\]
    );
}

test "y_array_with_leading_space" {
    try ok(
        \\ [1]
    );
}

test "y_array_with_several_null" {
    try roundTrip(
        \\[1,null,null,null,2]
    );
}

test "y_array_with_trailing_space" {
    try ok("[2] ");
}

test "y_number_0e+1" {
    try ok(
        \\[0e+1]
    );
}

test "y_number_0e1" {
    try ok(
        \\[0e1]
    );
}

test "y_number_after_space" {
    try ok(
        \\[ 4]
    );
}

test "y_number_double_close_to_zero" {
    try ok(
        \\[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]
    );
}

test "y_number_int_with_exp" {
    try ok(
        \\[20e1]
    );
}

test "y_number" {
    try ok(
        \\[123e65]
    );
}

test "y_number_minus_zero" {
    try ok(
        \\[-0]
    );
}

test "y_number_negative_int" {
    try roundTrip(
        \\[-123]
    );
}

test "y_number_negative_one" {
    try roundTrip(
        \\[-1]
    );
}

test "y_number_negative_zero" {
    try ok(
        \\[-0]
    );
}

test "y_number_real_capital_e" {
    try ok(
        \\[1E22]
    );
}

test "y_number_real_capital_e_neg_exp" {
    try ok(
        \\[1E-2]
    );
}

test "y_number_real_capital_e_pos_exp" {
    try ok(
        \\[1E+2]
    );
}

test "y_number_real_exponent" {
    try ok(
        \\[123e45]
    );
}

test "y_number_real_fraction_exponent" {
    try ok(
        \\[123.456e78]
    );
}

test "y_number_real_neg_exp" {
    try ok(
        \\[1e-2]