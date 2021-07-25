const std = @import("std");
const mem = std.mem;
const testing = std.testing;
const parse = @import("../parse.zig");

const Node = parse.Node;
const Tree = parse.Tree;

test "explicit doc" {
    const source =
        \\--- !tapi-tbd
        \\tbd-version: 4
        \\abc-version: 5
        \\...
    ;

    var tree = Tree.init(testing.allocator);
    defer tree.deinit();
    try tree.parse(source);

    try testing.expectEqual(tree.docs.items.len, 1);

    const doc = tree.docs.items[0].cast(Node.Doc).?;
    try testing.expectEqual(doc.start.?, 0);
    try testing.expectEqual(doc.end.?, tree.tokens.len - 2);

    const directive = tree.tokens[doc.directive.?];
    try testing.expectEqual(directive.id, .Literal);
    try testing.expect(mem.eql(u8, "tapi-tbd", tree.source[directive.start..directive.end]));

    try testing.expect(doc.value != null);
    try testing.expectEqual(doc.value.?.tag, .map);

    const map = doc.value.?.cast(Node.Map).?;
    try testing.expectEqual(map.start.?, 5);
    try testing.expectEqual(map.end.?, 14);
    try testing.expectEqual(map.values.items.len, 2);

    {
        const entry = map.values.items[0];

        const key = tree.tokens[entry.key];
        try testing.expectEqual(key.id, .Literal);
        try testing.expect(mem.eql(u8, "tbd-version", tree.source[key.start..key.end]));

        const value = entry.value.cast(Node.Value).?;
        const value_tok = tree.tokens[value.start.?];
        try testing.expectEqual(value_tok.id, .Literal);
        try testing.expect(mem.eql(u8, "4", tree.source[value_tok.start..value_tok.end]));
    }

    {
        const entry = map.values.items[1];

        const key = tree.tokens[entry.key];
        try testing.expectEqual(key.id, .Literal);
        try testing.expect(mem.eql(u8, "abc-version", tree.source[key.start..key.end]));

        const value = entry.value.cast(Node.Value).?;
        const value_tok = tree.tokens[value.start.?];
        try testing.expectEqual(value_tok.id, .Literal);
        try testing.expect(mem.eql(u8, "5", tree.source[value_tok.start..value_tok.end]));
    }
}

test "leaf in qu