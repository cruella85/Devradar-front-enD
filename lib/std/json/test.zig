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
    );
}

test "y_number_real_pos_exponent" {
    try ok(
        \\[1e+2]
    );
}

test "y_number_simple_int" {
    try roundTrip(
        \\[123]
    );
}

test "y_number_simple_real" {
    try ok(
        \\[123.456789]
    );
}

test "y_object_basic" {
    try roundTrip(
        \\{"asd":"sdf"}
    );
}

test "y_object_duplicated_key_and_value" {
    try ok(
        \\{"a":"b","a":"b"}
    );
}

test "y_object_duplicated_key" {
    try ok(
        \\{"a":"b","a":"c"}
    );
}

test "y_object_empty" {
    try roundTrip(
        \\{}
    );
}

test "y_object_empty_key" {
    try roundTrip(
        \\{"":0}
    );
}

test "y_object_escaped_null_in_key" {
    try ok(
        \\{"foo\u0000bar": 42}
    );
}

test "y_object_extreme_numbers" {
    try ok(
        \\{ "min": -1.0e+28, "max": 1.0e+28 }
    );
}

test "y_object" {
    try ok(
        \\{"asd":"sdf", "dfg":"fgh"}
    );
}

test "y_object_long_strings" {
    try ok(
        \\{"x":[{"id": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}], "id": "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}
    );
}

test "y_object_simple" {
    try roundTrip(
        \\{"a":[]}
    );
}

test "y_object_string_unicode" {
    try ok(
        \\{"title":"\u041f\u043e\u043b\u0442\u043e\u0440\u0430 \u0417\u0435\u043c\u043b\u0435\u043a\u043e\u043f\u0430" }
    );
}

test "y_object_with_newlines" {
    try ok(
        \\{
        \\"a": "b"
        \\}
    );
}

test "y_string_1_2_3_bytes_UTF-8_sequences" {
    try ok(
        \\["\u0060\u012a\u12AB"]
    );
}

test "y_string_accepted_surrogate_pair" {
    try ok(
        \\["\uD801\udc37"]
    );
}

test "y_string_accepted_surrogate_pairs" {
    try ok(
        \\["\ud83d\ude39\ud83d\udc8d"]
    );
}

test "y_string_allowed_escapes" {
    try ok(
        \\["\"\\\/\b\f\n\r\t"]
    );
}

test "y_string_backslash_and_u_escaped_zero" {
    try ok(
        \\["\\u0000"]
    );
}

test "y_string_backslash_doublequotes" {
    try roundTrip(
        \\["\""]
    );
}

test "y_string_comments" {
    try ok(
        \\["a/*b*/c/*d//e"]
    );
}

test "y_string_double_escape_a" {
    try ok(
        \\["\\a"]
    );
}

test "y_string_double_escape_n" {
    try roundTrip(
        \\["\\n"]
    );
}

test "y_string_escaped_control_character" {
    try ok(
        \\["\u0012"]
    );
}

test "y_string_escaped_noncharacter" {
    try ok(
        \\["\uFFFF"]
    );
}

test "y_string_in_array" {
    try ok(
        \\["asd"]
    );
}

test "y_string_in_array_with_leading_space" {
    try ok(
        \\[ "asd"]
    );
}

test "y_string_last_surrogates_1_and_2" {
    try ok(
        \\["\uDBFF\uDFFF"]
    );
}

test "y_string_nbsp_uescaped" {
    try ok(
        \\["new\u00A0line"]
    );
}

test "y_string_nonCharacterInUTF-8_U+10FFFF" {
    try ok(
        \\["􏿿"]
    );
}

test "y_string_nonCharacterInUTF-8_U+FFFF" {
    try ok(
        \\["￿"]
    );
}

test "y_string_null_escape" {
    try ok(
        \\["\u0000"]
    );
}

test "y_string_one-byte-utf-8" {
    try ok(
        \\["\u002c"]
    );
}

test "y_string_pi" {
    try ok(
        \\["π"]
    );
}

test "y_string_reservedCharacterInUTF-8_U+1BFFF" {
    try ok(
        \\["𛿿"]
    );
}

test "y_string_simple_ascii" {
    try ok(
        \\["asd "]
    );
}

test "y_string_space" {
    try roundTrip(
        \\" "
    );
}

test "y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF" {
    try ok(
        \\["\uD834\uDd1e"]
    );
}

test "y_string_three-byte-utf-8" {
    try ok(
        \\["\u0821"]
    );
}

test "y_string_two-byte-utf-8" {
    try ok(
        \\["\u0123"]
    );
}

test "y_string_u+2028_line_sep" {
    try ok("[\"\xe2\x80\xa8\"]");
}

test "y_string_u+2029_par_sep" {
    try ok("[\"\xe2\x80\xa9\"]");
}

test "y_string_uescaped_newline" {
    try ok(
        \\["new\u000Aline"]
    );
}

test "y_string_uEscape" {
    try ok(
        \\["\u0061\u30af\u30EA\u30b9"]
    );
}

test "y_string_unescaped_char_delete" {
    try ok("[\"\x7f\"]");
}

test "y_string_unicode_2" {
    try ok(
        \\["⍂㈴⍂"]
    );
}

test "y_string_unicodeEscapedBackslash" {
    try ok(
        \\["\u005C"]
    );
}

test "y_string_unicode_escaped_double_quote" {
    try ok(
        \\["\u0022"]
    );
}

test "y_string_unicode" {
    try ok(
        \\["\uA66D"]
    );
}

test "y_string_unicode_U+10FFFE_nonchar" {
    try ok(
        \\["\uDBFF\uDFFE"]
    );
}

test "y_string_unicode_U+1FFFE_nonchar" {
    try ok(
        \\["\uD83F\uDFFE"]
    );
}

test "y_string_unicode_U+200B_ZERO_WIDTH_SPACE" {
    try ok(
        \\["\u200B"]
    );
}

test "y_string_unicode_U+2064_invisible_plus" {
    try ok(
        \\["\u2064"]
    );
}

test "y_string_unicode_U+FDD0_nonchar" {
    try ok(
        \\["\uFDD0"]
    );
}

test "y_string_unicode_U+FFFE_nonchar" {
    try ok(
        \\["\uFFFE"]
    );
}

test "y_string_utf8" {
    try ok(
        \\["€𝄞"]
    );
}

test "y_string_with_del_character" {
    try ok("[\"a\x7fa\"]");
}

test "y_structure_lonely_false" {
    try roundTrip(
        \\false
    );
}

test "y_structure_lonely_int" {
    try roundTrip(
        \\42
    );
}

test "y_structure_lonely_negative_real" {
    try ok(
        \\-0.1
    );
}

test "y_structure_lonely_null" {
    try roundTrip(
        \\null
    );
}

test "y_structure_lonely_string" {
    try roundTrip(
        \\"asd"
    );
}

test "y_structure_lonely_true" {
    try roundTrip(
        \\true
    );
}

test "y_structure_string_empty" {
    try roundTrip(
        \\""
    );
}

test "y_structure_trailing_newline" {
    try roundTrip(
        \\["a"]
    );
}

test "y_structure_true_in_array" {
    try roundTrip(
        \\[true]
    );
}

test "y_structure_whitespace_array" {
    try ok(" [] ");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

test "n_array_1_true_without_comma" {
    try err(
        \\[1 true]
    );
}

test "n_array_a_invalid_utf8" {
    try err(
        \\[aå]
    );
}

test "n_array_colon_instead_of_comma" {
    try err(
        \\["": 1]
    );
}

test "n_array_comma_after_close" {
    try err(
        \\[""],
    );
}

test "n_array_comma_and_number" {
    try err(
        \\[,1]
    );
}

test "n_array_double_comma" {
    try err(
        \\[1,,2]
    );
}

test "n_array_double_extra_comma" {
    try err(
        \\["x",,]
    );
}

test "n_array_extra_close" {
    try err(
        \\["x"]]
    );
}

test "n_array_extra_comma" {
    try err(
        \\["",]
    );
}

test "n_array_incomplete_invalid_value" {
    try err(
        \\[x
    );
}

test "n_array_incomplete" {
    try err(
        \\["x"
    );
}

test "n_array_inner_array_no_comma" {
    try err(
        \\[3[4]]
    );
}

test "n_array_invalid_utf8" {
    try err(
        \\[ÿ]
    );
}

test "n_array_items_separated_by_semicolon" {
    try err(
        \\[1:2]
    );
}

test "n_array_just_comma" {
    try err(
        \\[,]
    );
}

test "n_array_just_minus" {
    try err(
        \\[-]
    );
}

test "n_array_missing_value" {
    try err(
        \\[   , ""]
    );
}

test "n_array_newlines_unclosed" {
    try err(
        \\["a",
        \\4
        \\,1,
    );
}

test "n_array_number_and_comma" {
    try err(
        \\[1,]
    );
}

test "n_array_number_and_several_commas" {
    try err(
        \\[1,,]
    );
}

test "n_array_spaces_vertical_tab_formfeed" {
    try err("[\"\x0aa\"\\f]");
}

test "n_array_star_inside" {
    try err(
        \\[*]
    );
}

test "n_array_unclosed" {
    try err(
        \\[""
    );
}

test "n_array_unclosed_trailing_comma" {
    try err(
        \\[1,
    );
}

test "n_array_unclosed_with_new_lines" {
    try err(
        \\[1,
        \\1
        \\,1
    );
}

test "n_array_unclosed_with_object_inside" {
    try err(
        \\[{}
    );
}

test "n_incomplete_false" {
    try err(
        \\[fals]
    );
}

test "n_incomplete_null" {
    try err(
        \\[nul]
    );
}

test "n_incomplete_true" {
    try err(
        \\[tru]
    );
}

test "n_multidigit_number_then_00" {
    try err("123\x00");
}

test "n_number_0.1.2" {
    try err(
        \\[0.1.2]
    );
}

test "n_number_-01" {
    try err(
        \\[-01]
    );
}

test "n_number_0.3e" {
    try err(
        \\[0.3e]
    );
}

test "n_number_0.3e+" {
    try err(
        \\[0.3e+]
    );
}

test "n_number_0_capital_E" {
    try err(
        \\[0E]
    );
}

test "n_number_0_capital_E+" {
    try err(
        \\[0E+]
    );
}

test "n_number_0.e1" {
    try err(
        \\[0.e1]
    );
}

test "n_number_0e" {
    try err(
        \\[0e]
    );
}

test "n_number_0e+" {
    try err(
        \\[0e+]
    );
}

test "n_number_1_000" {
    try err(
        \\[1 000.0