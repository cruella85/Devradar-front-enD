const std = @import("std");
const builtin = @import("builtin");
const native_endian = builtin.target.cpu.arch.endian();
const expect = std.testing.expect;
const expectEqual = std.testing.expectEqual;
const expectEqualSlices = std.testing.expectEqualSlices;
const maxInt = std.math.maxInt;

top_level_field: i32,

test "top level fields" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var instance = @This(){
        .top_level_field = 1234,
    };
    instance.top_level_field += 1;
    try expect(@as(i32, 1235) == instance.top_level_field);
}

const StructWithFields = struct {
    a: u8,
    b: u32,
    c: u64,
    d: u32,

    fn first(self: *const StructWithFields) u8 {
        return self.a;
    }

    fn second(self: *const StructWithFields) u32 {
        return self.b;
    }

    fn third(self: *const StructWithFields) u64 {
        return self.c;
    }

    fn fourth(self: *const StructWithFields) u32 {
        return self.d;
    }
};

test "non-packed struct has fields padded out to the required alignment" {
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;

    const foo = StructWithFields{ .a = 5, .b = 1, .c = 10, .d = 2 };
    try expect(foo.first() == 5);
    try expect(foo.second() == 1);
    try expect(foo.third() == 10);
    try expect(foo.fourth() == 2);
}

const SmallStruct = struct {
    a: u8,
    b: u8,

    fn first(self: *SmallStruct) u8 {
        return self.a;
    }

    fn second(self: *SmallStruct) u8 {
        return self.b;
    }
};

test "lower unnamed constants" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var foo = SmallStruct{ .a = 1, .b = 255 };
    try expect(foo.first() == 1);
    try expect(foo.second() == 255);
}

const StructWithNoFields = struct {
    fn add(a: i32, b: i32) i32 {
        return a + b;
    }
};

const StructFoo = struct {
    a: i32,
    b: bool,
    c: u64,
};

test "structs" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var foo: StructFoo = undefined;
    @memset(@ptrCast([*]u8, &foo), 0, @sizeOf(StructFoo));
    foo.a += 1;
    foo.b = foo.a == 1;
    try testFoo(foo);
    testMutation(&foo);
    try expect(foo.c == 100);
}
fn testFoo(foo: StructFoo) !void {
    try expect(foo.b);
}
fn testMutation(foo: *StructFoo) void {
    foo.c = 100;
}

test "struct byval assign" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var foo1: StructFoo = undefined;
    var foo2: StructFoo = undefined;

    foo1.a = 1234;
    foo2.a = 0;
    try expect(foo2.a == 0);
    foo2 = foo1;
    try expect(foo2.a == 1234);
}

test "call struct static method" {
    const result = StructWithNoFields.add(3, 4);
    try expect(result == 7);
}

const should_be_11 = StructWithNoFields.add(5, 6);

test "invoke static method in global scope" {
    try expect(should_be_11 == 11);
}

const empty_global_instance = StructWithNoFields{};

test "return empty struct instance" {
    _ = returnEmptyStructInstance();
}
fn returnEmptyStructInstance() StructWithNoFields {
    return empty_global_instance;
}

test "fn call of struct field" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const Foo = struct {
        ptr: fn () i32,
    };
    const S = struct {
        fn aFunc() i32 {
            return 13;
        }

        fn callStructField(comptime foo: Foo) i32 {
            return foo.ptr();
        }
    };

    try expect(S.callStructField(Foo{ .ptr = S.aFunc }) == 13);
}

test "struct initializer" {
    const val = Val{ .x = 42 };
    try expect(val.x == 42);
}

const MemberFnTestFoo = struct {
    x: i32,
    fn member(foo: MemberFnTestFoo) i32 {
        return foo.x;
    }
};

test "call member function directly" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const instance = MemberFnTestFoo{ .x = 1234 };
    const result = MemberFnTestFoo.member(instance);
    try expect(result == 1234);
}

test "store member function in variable" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const instance = MemberFnTestFoo{ .x = 1234 };
    const memberFn = MemberFnTestFoo.member;
    const result = memberFn(instance);
    try expect(result == 1234);
}

test "member functions" {
    const r = MemberFnRand{ .seed = 1234 };
    try expect(r.getSeed() == 1234);
}
const MemberFnRand = struct {
    seed: u32,
    pub fn getSeed(r: *const MemberFnRand) u32 {
        return r.seed;
    }
};

test "return struct byval from function" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const bar = makeBar2(1234, 5678);
    try expect(bar.y == 5678);
}
const Bar = struct {
    x: i32,
    y: i32,
};
fn makeBar2(x: i32, y: i32) Bar {
    return Bar{
        .x = x,
        .y = y,
    };
}

test "call method with mutable reference to struct with no fields" {
    const S = struct {
        fn doC(s: *const @This()) bool {
            _ = s;
            return true;
        }
        fn do(s: *@This()) bool {
            _ = s;
            return true;
        }
    };

    var s = S{};
    try expect(S.doC(&s));
    try expect(s.doC());
    try expect(S.do(&s));
    try expect(s.do());
}

test "usingnamespace within struct scope" {
    const S = struct {
        usingnamespace struct {
            pub fn inner() i32 {
                return 42;
            }
        };
    };
    try expect(@as(i32, 42) == S.inner());
}

test "struct field init with catch" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const S = struct {
        fn doTheTest() !void {
            var x: anyerror!isize = 1;
            var req = Foo{
                .field = x catch undefined,
            };
            try expect(req.field == 1);
        }

        pub const Foo = extern struct {
            field: isize,
        };
    };
    try S.doTheTest();
    comptime try S.doTheTest();
}

const blah: packed struct {
    a: u3,
    b: u3,
    c: u2,
} = undefined;

test "bit field alignment" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO
    try expect(@TypeOf(&blah.b) == *align(1:3:1) const u3);
}

const Node = struct {
    val: Val,
    next: *Node,
};

const Val = struct {
    x: i32,
};

test "struct point to self" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var root: Node = undefined;
    root.val.x = 1;

    var node: Node = undefined;
    node.next = &root;
    node.val.x = 2;

    root.next = &node;

    try expect(node.next.next.next.val.x == 1);
}

test "void struct fields" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const foo = VoidStructFieldsFoo{
        .a = void{},
        .b = 1,
        .c = void{},
    };
    try expect(foo.b == 1);
    try expect(@sizeOf(VoidStructFieldsFoo) == 4);
}
const VoidStructFieldsFoo = struct {
    a: void,
    b: i32,
    c: void,
};

test "return empty struct from fn" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    _ = testReturnEmptyStructFromFn();
}
const EmptyStruct2 = struct {};
fn testReturnEmptyStructFromFn() EmptyStruct2 {
    return EmptyStruct2{};
}

test "pass slice of empty struct to fn" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    try expect(testPassSliceOfEmptyStructToFn(&[_]EmptyStruct2{EmptyStruct2{}}) == 1);
}
fn testPassSliceOfEmptyStructToFn(slice: []const EmptyStruct2) usize {
    return slice.len;
}

test "self-referencing struct via array member" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const T = struct {
        children: [1]*@This(),
    };
    var x: T = undefined;
    x = T{ .children = .{&x} };
    try expect(x.children[0] == &x);
}

test "empty struct method call" {
    const es = EmptyStruct{};
    try expect(es.method() == 1234);
}
const EmptyStruct = struct {
    fn method(es: *const EmptyStruct) i32 {
        _ = es;
        return 1234;
    }
};

test "align 1 field before self referential align 8 field as slice return type" {
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const result = alloc(Expr);
    try expect(result.len == 0);
}

const Expr = union(enum) {
    Literal: u8,
    Question: *Expr,
};

fn alloc(comptime T: type) []T {
    return &[_]T{};
}

const APackedStruct = packed struct {
    x: u8,
    y: u8,
};

test "packed struct" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var foo = APackedStruct{
        .x = 1,
        .y = 2,
    };
    foo.y += 1;
    const four = foo.x + foo.y;
    try expect(four == 4);
}

const Foo24Bits = packed struct {
    field: u24,
};
const Foo96Bits = packed struct {
    a: u24,
    b: u24,
    c: u24,
    d: u24,
};

test "packed struct 24bits" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_wasm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.cpu.arch == .wasm32) return error.SkipZigTest; // TODO
    if (builtin.cpu.arch == .arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    comptime {
        std.debug.assert(@sizeOf(Foo24Bits) == @sizeOf(u24));
        std.debug.assert(@sizeOf(Foo96Bits) == @sizeOf(u96));
    }

    var value = Foo96Bits{
        .a = 0,
        .b = 0,
        .c = 0,
        .d = 0,
    };
    value.a += 1;
    try expect(value.a == 1);
    try expect(value.b == 0);
    try expect(value.c == 0);
    try expect(value.d == 0);

    value.b += 1;
    try expect(value.a == 1);
    try expect(value.b == 1);
    try expect(value.c == 0);
    try expect(value.d == 0);

    value.c += 1;
    try expect(value.a == 1);
    try expect(value.b == 1);
    try expect(value.c == 1);
    try expect(value.d == 0);

    value.d += 1;
    try expect(value.a == 1);
    try expect(value.b == 1);
    try expect(value.c == 1);
    try expect(value.d == 1);
}

test "runtime struct initialization of bitfield" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const s1 = Nibbles{
        .x = x1,
        .y = x1,
    };
    const s2 = Nibbles{
        .x = @intCast(u4, x2),
        .y = @intCast(u4, x2),
    };

    try expect(s1.x == x1);
    try expect(s1.y == x1);
    try expect(s2.x == @intCast(u4, x2));
    try expect(s2.y == @intCast(u4, x2));
}

var x1 = @as(u4, 1);
var x2 = @as(u8, 2);

const Nibbles = packed struct {
    x: u4,
    y: u4,
};

const Bitfields = packed struct {
    f1: u16,
    f2: u16,
    f3: u8,
    f4: u8,
    f5: u4,
    f6: u4,
    f7: u8,
};

test "packed struct fields are ordered from LSB to MSB" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    var all: u64 = 0x7765443322221111;
    var bytes: [8]u8 align(@alignOf(Bitfields)) = undefined;
    @memcpy(&bytes, @ptrCast([*]u8, &all), 8);
    var bitfields = @ptrCast(*Bitfields, &bytes).*;

    try expect(bitfields.f1 == 0x1111);
    try expect(bitfields.f2 == 0x2222);
    try expect(bitfields.f3 == 0x33);
    try expect(bitfields.f4 == 0x44);
    try expect(bitfields.f5 == 0x5);
    try expect(bitfields.f6 == 0x6);
    try expect(bitfields.f7 == 0x77);
}

test "implicit cast packed struct field to const ptr" {
    if (builtin.zig_backend == .stage2_aarch64) return error.SkipZigTest;
    if (builtin.zig_backend == .stage2_arm) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_x86_64) return error.SkipZigTest; // TODO
    if (builtin.zig_backend == .stage2_sparc64) return error.SkipZigTest; // TODO

    const LevelUpMove = packed struct {
        move_id: u9,
        level: u7,

        fn toInt(value: u7) u7 {
            return value;
        }
    };

    var lup: LevelUpMove = undefined;
    lup.level = 12;
    const res = LevelUpMove.toInt(