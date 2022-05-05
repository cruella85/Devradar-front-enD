const std = @import("std");
const builtin = @import("builtin");
const DW = std.dwarf;
const assert = std.debug.assert;
const testing = std.testing;

/// Disjoint sets of registers. Every register must belong to
/// exactly one register class.
pub const RegisterClass = enum {
    general_purpose,
    stack_pointer,
    floating_point,
};

/// Registers in the AArch64 instruction set
pub const Register = enum(u8) {
    // zig fmt: off
    // 64-bit general-purpose registers
    x0, x1, x2, x3, x4, x5, x6, x7,
    x8, x9, x10, x11, x12, x13, x14, x15,
    x16, x17, x18, x19, x20, x21, x22, x23,
    x24, x25, x26, x27, x28, x29, x30, xzr,

    // 32-bit general-purpose registers
    w0, w1, w2, w3, w4, w5, w6, w7,
    w8, w9, w10, w11, w12, w13, w14, w15,
    w16, w17, w18, w19, w20, w21, w22, w23,
    w24, w25, w26, w27, w28, w29, w30, wzr,

    // Stack pointer
    sp, wsp,

    // 128-bit floating-point registers
    q0, q1, q2, q3, q4, q5, q6, q7,
    q8, q9, q10, q11, q12, q13, q14, q15,
    q16, q17, q18, q19, q20, q21, q22, q23,
    q24, q25, q26, q27, q28, q29, q30, q31,

    // 64-bit floating-point registers
    d0, d1, d2, d3, d4, d5, d6, d7,
    d8, d9, d10, d11, d12, d13, d14, d15,
    d16, d17, d18, d19, d20, d21, d22, d23,
    d24, d25, d26, d27, d28, d29, d30, d31,

    // 32-bit floating-point registers
    s0, s1, s2, s3, s4, s5, s6, s7,
    s8, s9, s10, s11, s12, s13, s14, s15,
    s16, s17, s18, s19, s20, s21, s22, s23,
    s24, s25, s26, s27, s28, s29, s30, s31,

    // 16-bit floating-point registers
    h0, h1, h2, h3, h4, h5, h6, h7,
    h8, h9, h10, h11, h12, h13, h14, h15,
    h16, h17, h18, h19, h20, h21, h22, h23,
    h24, h25, h26, h27, h28, h29, h30, h31,

    // 8-bit floating-point registers
    b0, b1, b2, b3, b4, b5, b6, b7,
    b8, b9, b10, b11, b12, b13, b14, b15,
    b16, b17, b18, b19, b20, b21, b22, b23,
    b24, b25, b26, b27, b28, b29, b30, b31,
    // zig fmt: on

    pub fn class(self: Register) RegisterClass {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => .general_purpose,
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => .general_purpose,

            @enumToInt(Register.sp) => .stack_pointer,
            @enumToInt(Register.wsp) => .stack_pointer,

            @enumToInt(Register.q0)...@enumToInt(Register.q31) => .floating_point,
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => .floating_point,
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => .floating_point,
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => .floating_point,
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => .floating_point,
            else => unreachable,
        };
    }

    pub fn id(self: Register) u6 {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.x0)),
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.w0)),

            @enumToInt(Register.sp) => 32,
            @enumToInt(Register.wsp) => 32,

            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.q0) + 33),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.d0) + 33),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.s0) + 33),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.h0) + 33),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intCast(u6, @enumToInt(self) - @enumToInt(Register.b0) + 33),
            else => unreachable,
        };
    }

    pub fn enc(self: Register) u5 {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.x0)),
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.w0)),

            @enumToInt(Register.sp) => 31,
            @enumToInt(Register.wsp) => 31,

            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.q0)),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.d0)),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.s0)),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.h0)),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intCast(u5, @enumToInt(self) - @enumToInt(Register.b0)),
            else => unreachable,
        };
    }

    /// Returns the bit-width of the register.
    pub fn size(self: Register) u8 {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => 64,
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => 32,

            @enumToInt(Register.sp) => 64,
            @enumToInt(Register.wsp) => 32,

            @enumToInt(Register.q0)...@enumToInt(Register.q31) => 128,
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => 64,
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => 32,
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => 16,
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => 8,
            else => unreachable,
        };
    }

    /// Convert from a general-purpose register to its 64 bit alias.
    pub fn toX(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.x0) + @enumToInt(Register.x0),
            ),
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.w0) + @enumToInt(Register.x0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a general-purpose register to its 32 bit alias.
    pub fn toW(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.x0)...@enumToInt(Register.xzr) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.x0) + @enumToInt(Register.w0),
            ),
            @enumToInt(Register.w0)...@enumToInt(Register.wzr) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.w0) + @enumToInt(Register.w0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a floating-point register to its 128 bit alias.
    pub fn toQ(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.q0) + @enumToInt(Register.q0),
            ),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.d0) + @enumToInt(Register.q0),
            ),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.s0) + @enumToInt(Register.q0),
            ),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.h0) + @enumToInt(Register.q0),
            ),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.b0) + @enumToInt(Register.q0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a floating-point register to its 64 bit alias.
    pub fn toD(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.q0) + @enumToInt(Register.d0),
            ),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.d0) + @enumToInt(Register.d0),
            ),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.s0) + @enumToInt(Register.d0),
            ),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.h0) + @enumToInt(Register.d0),
            ),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.b0) + @enumToInt(Register.d0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a floating-point register to its 32 bit alias.
    pub fn toS(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.q0) + @enumToInt(Register.s0),
            ),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.d0) + @enumToInt(Register.s0),
            ),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.s0) + @enumToInt(Register.s0),
            ),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.h0) + @enumToInt(Register.s0),
            ),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.b0) + @enumToInt(Register.s0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a floating-point register to its 16 bit alias.
    pub fn toH(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.q0) + @enumToInt(Register.h0),
            ),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.d0) + @enumToInt(Register.h0),
            ),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.s0) + @enumToInt(Register.h0),
            ),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.h0) + @enumToInt(Register.h0),
            ),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.b0) + @enumToInt(Register.h0),
            ),
            else => unreachable,
        };
    }

    /// Convert from a floating-point register to its 8 bit alias.
    pub fn toB(self: Register) Register {
        return switch (@enumToInt(self)) {
            @enumToInt(Register.q0)...@enumToInt(Register.q31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.q0) + @enumToInt(Register.b0),
            ),
            @enumToInt(Register.d0)...@enumToInt(Register.d31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.d0) + @enumToInt(Register.b0),
            ),
            @enumToInt(Register.s0)...@enumToInt(Register.s31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.s0) + @enumToInt(Register.b0),
            ),
            @enumToInt(Register.h0)...@enumToInt(Register.h31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.h0) + @enumToInt(Register.b0),
            ),
            @enumToInt(Register.b0)...@enumToInt(Register.b31) => @intToEnum(
                Register,
                @enumToInt(self) - @enumToInt(Register.b0) + @enumToInt(Register.b0),
            ),
            else => unreachable,
        };
    }

    pub fn dwarfLocOp(self: Register) u8 {
        return @as(u8, self.enc()) + DW.OP.reg0;
    }

    /// DWARF encodings that push a value onto the DWARF stack that is either
    /// the contents of a register or the result of adding the contents a given
    /// register to a given signed offset.
    pub fn dwarfLocOpDeref(self: Register) u8 {
        return @as(u8, self.enc()) + DW.OP.breg0;
    }
};

test "Register.enc" {
    try testing.expectEqual(@as(u5, 0), Register.x0.enc());
    try testing.expectEqual(@as(u5, 0), Register.w0.enc());

    try testing.expectEqual(@as(u5, 31), Register.xzr.enc());
    try testing.expectEqual(@as(u5, 31), Register.wzr.enc());

    try testing.expectEqual(@as(u5, 31), Register.sp.enc());
    try testing.expectEqual(@as(u5, 31), Register.sp.enc());
}

test "Register.size" {
    try testing.expectEqual(@as(u8, 64), Register.x19.size());
    try testing.expectEqual(@as(u8, 32), Register.w3.size());
}

test "Register.toX/toW" {
    try testing.expectEqual(Register.x0, Register.w0.toX());
    try testing.expectEqual(Register.x0, Register.x0.toX());

    try testing.expectEqual(Register.w3, Register.w3.toW());
    try testing.expectEqual(Register.w3, Register.x3.toW());
}

/// Represents an instruction in the AArch64 instruction set
pub const Instruction = union(enum) {
    move_wide_immediate: packed struct {
        rd: u5,
        imm16: u16,
        hw: u2,
        fixed: u6 = 0b100101,
        opc: u2,
        sf: u1,
    },
    pc_relative_address: packed struct {
        rd: u5,
        immhi: u19,
        fixed: u5 = 0b10000,
        immlo: u2,
        op: u1,
    },
    load_store_register: packed struct {
        rt: u5,
        rn: u5,
        offset: u12,
        opc: u2,
        op1: u2,
        v: u1,
        fixed: u3 = 0b111,
        size: u2,
    },
    load_store_register_pair: packed struct {
        rt1: u5,
        rn: u5,
        rt2: u5,
        imm7: u7,
        load: u1,
        encoding: u2,
        fixed: u5 = 0b101_0_0,
        opc: u2,
    },
    load_literal: packed struct {
        rt: u5,
        imm19: u19,
        fixed: u6 = 0b011_0_00,
        opc: u2,
    },
    exception_generation: packed struct {
        ll: u2,
        op2: u3,
        imm16: u16,
        opc: u3,
        fixed: u8 = 0b1101_0100,
    },
    unconditional_branch_register: packed struct {
        op4: u5,
        rn: u5,
        op3: u6,
        op2: u5,
        opc: u4,
        fixed: u7 = 0b1101_011,
    },
    unconditional_branch_immediate: packed struct {
        imm26: u26,
        fixed: u5 = 0b00101,
        op: u1,
    },
    no_operation: packed struct {
        fixed: u32 = 0b1101010100_0_00_011_0010_0000_000_11111,
    },
    logical_shifted_register: packed struct {
        rd: u5,
        rn: u5,
        imm6: u6,
        rm: u5,
        n: u1,
        shift: u2,
        fixed: u5 = 0b01010,
        opc: u2,
        sf: u1,
    },
    add_subtract_immediate: packed struct {
        rd: u5,
        rn: u5,
        imm12: u12,
        sh: u1,
        fixed: u6 = 0b100010,
        s: u1,
        op: u1,
        sf: u1,
    },
    logical_immediate: packed struct {
        rd: u5,
        rn: u5,
        imms: u6,
        immr: u6,
        n: u1,
        fixed: u6 = 0b100100,
        opc: u2,
        sf: u1,
    },
    bitfield: packed struct {
        rd: u5,
        rn: u5,
        imms: u6,
        immr: u6,
        n: u1,
        fixed: u6 = 0b100110,
        opc: u2,
        sf: u1,
    },
    add_subtract_shifted_register: packed struct {
        rd: u5,
        rn: u5,
        imm6: u6,
        rm: u5,
        fixed_1: u1 = 0b0,
        shift: u2,
        fixed_2: u5 = 0b01011,
        s: u1,
        op: u1,
        sf: u1,
    },
    add_subtract_extended_register: packed struct {
        rd: u5,
        rn: u5,
        imm3: u3,
        option: u3,
        rm: u5,
        fixed: u8 = 0b01011_00_1,
        s: u1,
        op: u1,
        sf: u1,
    },
    conditional_branch: struct {
        cond: u4,
        o0: u1,
        imm19: u19,
        o1: u1,
        fixed: u7 = 0b0101010,
    },
    compare_and_branch: struct {
        rt: u5,
        imm19: u19,
        op: u1,
        fixed: u6 = 0b011010,
        sf: u1,
    },
    conditional_select: struct {
        rd: u5,
        rn: u5,
        op2: u2,
        cond: u4,
        rm: u5,
        fixed: u8 = 0b11010100,
        s: u1,
        op: u1,
        sf: u1,
    },
    data_processing_3_source: packed struct {
        rd: u5,
        rn: u5,
        ra: u5,
        o0: u1,
        rm: u5,
        op31: u3,
        fixed: u5 = 0b11011,
        op54: u2,
        sf: u1,
    },
    data_processing_2_source: packed struct {
        rd: u5,
        rn: u5,
        opcode: u6,
        rm: u5,
        fixed_1: u8 = 0b11010110,
        s: u1,
        fixed_2: u1 = 0b0,
        sf: u1,
    },

    pub const Condition = enum(u4) {
        /// Integer: Equal
        /// Floating point: Equal
        eq,
        /// Integer: Not equal
        /// Floating point: Not equal or unordered
        ne,
        /// Integer: Carry set
        /// Floating point: Greater than, equal, or unordered
        cs,
        /// Integer: Carry clear
        /// Floating point: Less than
        cc,
        /// Integer: Minus, negative
        /// Floating point: Less than
        mi,
        /// Integer: Plus, positive or zero
        /// Floating point: Greater than, equal, or unordered
        pl,
        /// Integer: Overflow
        /// Floating point: Unordered
        vs,
        /// Integer: No overflow
        /// Floating point: Ordered
        vc,
        /// Integer: Unsigned higher
        /// Floating point: Greater than, or unordered
        hi,
        /// Integer: Unsigned lower or same
        /// Floating point: Less than or equal
        ls,
        /// Integer: Signed greater than or equal
        /// Floating point: Greater than or equal
        ge,
        /// Integer: Signed less than
        /// Floating point: Less than, or unordered
        lt,
        /// Integer: Signed greater than
        /// Floating point: Greater than
        gt,
        /// Integer: Signed less than or equal
        /// Floating point: Less than, equal, or unordered
        le,
        /// Integer: Always
        /// Floating point: Always
        al,
        /// Integer: Always
        /// Floating point: Always
        nv,

        /// Converts a std.math.CompareOperator into a condition flag,
        /// i.e. returns the condition that is true iff the result of the
        /// comparison is true. Assumes signed comparison
        pub fn fromCompareOperatorSigned(op: std.math.CompareOperator) Condition {
            return switch (op) {
                .gte => .ge,
                .gt => .gt,
                .neq => .ne,
                .lt => .lt,
                .lte => .le,
                .eq => .eq,
            };
        }

        /// Converts a std.math.CompareOperator into a condition flag,
        /// i.e. returns the condition that is true iff the result of the
        /// comparison is true. Assumes unsigned comparison
        pub fn fromCompareOperatorUnsigned(op: std.math.CompareOperator) Condition {
            return switch (op) {
                .gte => .cs,
                .gt => .hi,
                .neq => .ne,
                .lt => .cc,
                .lte => .ls,
                .eq => .eq,
            };
        }

        /// Returns the condition which is true iff the given condition is
        /// false (if such a condition exists)
        pub fn negate(cond: Condition) Condition {
            return switch (cond) {
                .eq => .ne,
                .ne => .eq,
                .cs => .cc,
                .cc => .cs,
                .mi => .pl,
                .pl => .mi,
                .vs => .vc,
                .vc => .vs,
                .hi => .ls,
                .ls => .hi,
                .ge => .lt,
                .lt => .ge,
                .gt => .le,
                .le => .gt,
                .al => unreachable,
                .nv => unreachable,
            };
        }
    };

    pub fn toU32(self: Instruction) u32 {
        return switch (self) {
            .move_wide_immediate => |v| @bitCast(u32, v),
            .pc_relative_address => |v| @bitCast(u32, v),
            .load_store_register => |v| @bitCast(u32, v),
            .load_store_register_pair => |v| @bitCast(u32, v),
            .load_literal => |v| @bitCast(u32, v),
            .exception_generation => |v| @bitCast(u32, v),
            .unconditional_branch_register => |v| @bitCast(u32, v),
            .unconditional_branch_immediate => |v| @bitCast(u32, v),
            .no_operation => |v| @bitCast(u32, v),
            .logical_shifted_register => |v| @bitCast(u32, v),
            .add_subtract_immediate => |v| @bitCast(u32, v),
            .logical_immediate => |v| @bitCast(u32, v),
            .bitfield => |v| @bitCast(u32, v),
            .add_subtract_shifted_register => |v| @bitCast(u32, v),
            .add_subtract_extended_register => |v| @bitCast(u32, v),
            // TODO once packed structs work, this can be refactored
            .conditional_branch => |v| @as(u32, v.cond) | (@as(u32, v.o0) << 4) | (@as(u32, v.imm19) << 5) | (@as(u32, v.o1) << 24) | (@as(u32, v.fixed) << 25),
            .compare_and_branch => |v| @as(u32, v.rt) | (@as(u32, v.imm19) << 5) | (@as(u32, v.op) << 24) | (@as(u32, v.fixed) << 25) | (@as(u32, v.sf) << 31),
            .conditional_select => |v| @as(u32, v.rd) | @as(u32, v.rn) << 5 | @as(u32, v.op2) << 10 | @as(u32, v.cond) << 12 | @as(u32, v.rm) << 16 | @as(u32, v.fixed) << 21 | @as(u32, v.s) << 29 | @as(u32, v.op) << 30 | @as(u32, v.sf) << 31,
            .data_processing_3_source => |v| @bitCast(u32, v),
            .data_processing_2_source => |v| @bitCast(u32, v),
        };
    }

    fn moveWideImmediate(
        opc: u2,
        rd: Register,
        imm16: u16,
        shift: u6,
    ) Instruction {
        assert(shift % 16 == 0);
        assert(!(rd.size() == 32 and shift > 16));
        assert(!(rd.size() == 64 and shift > 48));

        return Instruction{
            .move_wide_immediate = .{
                .rd = rd.enc(),
                .imm16 = imm16,
                .hw = @intCast(u2, shift / 16),
                .opc = opc,
                .sf = switch (rd.size()) {
                    32 => 0,
                    64 => 1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn pcRelativeAddress(rd: Register, imm21: i21, op: u1) Instruction {
        assert(rd.size() == 64);
        const imm21_u = @bitCast(u21, imm21);
        return Instruction{
            .pc_relative_address = .{
                .rd = rd.enc(),
                .immlo = @truncate(u2, imm21_u),
                .immhi = @truncate(u19, imm21_u >> 2),
                .op = op,
            },
        };
    }

    pub const LoadStoreOffsetImmediate = union(enum) {
        post_index: i9,
        pre_index: i9,
        unsigned: u12,
    };

    pub const LoadStoreOffsetRegister = struct {
        rm: u5,
        shift: union(enum) {
            uxtw: u2,
            lsl: u2,
            sxtw: u2,
            sxtx: u2,
        },
    };

    /// Represents the offset operand of a load or store instruction.
    /// Data can be loaded from memory with either an immediate offset
    /// or an offset that is stored in some register.
    pub const LoadStoreOffset = union(enum) {
        immediate: LoadStoreOffsetImmediate,
        register: LoadStoreOffsetRegister,

        pub const none = LoadStoreOffset{
            .immediate = .{ .unsigned = 0 },
        };

        pub fn toU12(self: LoadStoreOffset) u12 {
            return switch (self) {
                .immediate => |imm_type| switch (imm_type) {
                    .post_index => |v| (@intCast(u12, @bitCast(u9, v)) << 2) + 1,
                    .pre_index => |v| (@intCast(u12, @bitCast(u9, v)) << 2) + 3,
                    .unsigned => |v| v,
                },
                .register => |r| switch (r.shift) {
                    .uxtw => |v| (@intCast(u12, r.rm) << 6) + (@intCast(u12, v) << 2) + 16 + 2050,
                    .lsl => |v| (@intCast(u12, r.rm) << 6) + (@intCast(u12, v) << 2) + 24 + 2050,
                    .sxtw => |v| (@intCast(u12, r.rm) << 6) + (@intCast(u12, v) << 2) + 48 + 2050,
                    .sxtx => |v| (@intCast(u12, r.rm) << 6) + (@intCast(u12, v) << 2) + 56 + 2050,
                },
            };
        }

        pub fn imm(offset: u12) LoadStoreOffset {
            return .{
                .immediate = .{ .unsigned = offset },
            };
        }

        pub fn imm_post_index(offset: i9) LoadStoreOffset {
            return .{
                .immediate = .{ .post_index = offset },
            };
        }

        pub fn imm_pre_index(offset: i9) LoadStoreOffset {
            return .{
                .immediate = .{ .pre_index = offset },
            };
        }

        pub fn reg(rm: Register) LoadStoreOffset {
            return .{
                .register = .{
                    .rm = rm.enc(),
                    .shift = .{
                        .lsl = 0,
                    },
                },
            };
        }

        pub fn reg_uxtw(rm: Register, shift: u2) LoadStoreOffset {
            assert(rm.size() == 32 and (shift == 0 or shift == 2));
            return .{
                .register = .{
                    .rm = rm.enc(),
                    .shift = .{
                        .uxtw = shift,
                    },
                },
            };
        }

        pub fn reg_lsl(rm: Register, shift: u2) LoadStoreOffset {
            assert(rm.size() == 64 and (shift == 0 or shift == 3));
            return .{
                .register = .{
                    .rm = rm.enc(),
                    .shift = .{
                        .lsl = shift,
                    },
                },
            };
        }

        pub fn reg_sxtw(rm: Register, shift: u2) LoadStoreOffset {
            assert(rm.size() == 32 and (shift == 0 or shift == 2));
            return .{
                .register = .{
                    .rm = rm.enc(),
                    .shift = .{
                        .sxtw = shift,
                    },
                },
            };
        }

        pub fn reg_sxtx(rm: Register, shift: u2) LoadStoreOffset {
            assert(rm.size() == 64 and (shift == 0 or shift == 3));
            return .{
                .register = .{
                    .rm = rm.enc(),
                    .shift = .{
                        .sxtx = shift,
                    },
                },
            };
        }
    };

    /// Which kind of load/store to perform
    const LoadStoreVariant = enum {
        /// 32 bits or 64 bits
        str,
        /// 8 bits, zero-extended
        strb,
        /// 16 bits, zero-extended
        strh,
        /// 32 bits or 64 bits
        ldr,
        /// 8 bits, zero-extended
        ldrb,
        /// 16 bits, zero-extended
        ldrh,
        /// 8 bits, sign extended
        ldrsb,
        /// 16 bits, sign extended
        ldrsh,
        /// 32 bits, sign extended
        ldrsw,
    };

    fn loadStoreRegister(
        rt: Register,
        rn: Register,
        offset: LoadStoreOffset,
        variant: LoadStoreVariant,
    ) Instruction {
        assert(rn.size() == 64);
        assert(rn.id() != Register.xzr.id());

        const off = offset.toU12();

        const op1: u2 = blk: {
            switch (offset) {
                .immediate => |imm| switch (imm) {
                    .unsigned => break :blk 0b01,
                    else => {},
                },
                else => {},
            }
            break :blk 0b00;
        };

        const opc: u2 = blk: {
            switch (variant) {
                .ldr, .ldrh, .ldrb => break :blk 0b01,
                .str, .strh, .strb => break :blk 0b00,
                .ldrsb,
                .ldrsh,
                => switch (rt.size()) {
                    32 => break :blk 0b11,
                    64 => break :blk 0b10,
                    else => unreachable, // unexpected register size
                },
                .ldrsw => break :blk 0b10,
            }
        };

        const size: u2 = blk: {
            switch (variant) {
                .ldr, .str => switch (rt.size()) {
                    32 => break :blk 0b10,
                    64 => break :blk 0b11,
                    else => unreachable, // unexpected register size
                },
                .ldrsw => break :blk 0b10,
                .ldrh, .ldrsh, .strh => break :blk 0b01,
                .ldrb, .ldrsb, .strb => break :blk 0b00,
            }
        };

        return Instruction{
            .load_store_register = .{
                .rt = rt.enc(),
                .rn = rn.enc(),
                .offset = off,
                .opc = opc,
                .op1 = op1,
                .v = 0,
                .size = size,
            },
        };
    }

    fn loadStoreRegisterPair(
        rt1: Register,
        rt2: Register,
        rn: Register,
        offset: i9,
        encoding: u2,
        load: bool,
    ) Instruction {
        assert(rn.size() == 64);
        assert(rn.id() != Register.xzr.id());

        switch (rt1.size()) {
            32 => {
                assert(-256 <= offset and offset <= 252);
                const imm7 = @truncate(u7, @bitCast(u9, offset >> 2));
                return Instruction{
                    .load_store_register_pair = .{
                        .rt1 = rt1.enc(),
                        .rn = rn.enc(),
                        .rt2 = rt2.enc(),
                        .imm7 = imm7,
                        .load = @boolToInt(load),
                        .encoding = encoding,
                        .opc = 0b00,
                    },
                };
            },
            64 => {
                assert(-512 <= offset and offset <= 504);
                const imm7 = @truncate(u7, @bitCast(u9, offset >> 3));
                return Instruction{
                    .load_store_register_pair = .{
                        .rt1 = rt1.enc(),
                        .rn = rn.enc(),
                        .rt2 = rt2.enc(),
                        .imm7 = imm7,
                        .load = @boolToInt(load),
                        .encoding = encoding,
                        .opc = 0b10,
                    },
                };
            },
            else => unreachable, // unexpected register size
        }
    }

    fn loadLiteral(rt: Register, imm19: u19) Instruction {
        return Instruction{
            .load_literal = .{
                .rt = rt.enc(),
                .imm19 = imm19,
                .opc = switch (rt.size()) {
                    32 => 0b00,
                    64 => 0b01,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn exceptionGeneration(
        opc: u3,
        op2: u3,
        ll: u2,
        imm16: u16,
    ) Instruction {
        return Instruction{
            .exception_generation = .{
                .ll = ll,
                .op2 = op2,
                .imm16 = imm16,
                .opc = opc,
            },
        };
    }

    fn unconditionalBranchRegister(
        opc: u4,
        op2: u5,
        op3: u6,
        rn: Register,
        op4: u5,
    ) Instruction {
        assert(rn.size() == 64);

        return Instruction{
            .unconditional_branch_register = .{
                .op4 = op4,
                .rn = rn.enc(),
                .op3 = op3,
                .op2 = op2,
                .opc = opc,
            },
        };
    }

    fn unconditionalBranchImmediate(
        op: u1,
        offset: i28,
    ) Instruction {
        return Instruction{
            .unconditional_branch_immediate = .{
                .imm26 = @bitCast(u26, @intCast(i26, offset >> 2)),
                .op = op,
            },
        };
    }

    pub const LogicalShiftedRegisterShift = enum(u2) { lsl, lsr, asr, ror };

    fn logicalShiftedRegister(
        opc: u2,
        n: u1,
        rd: Register,
        rn: Register,
        rm: Register,
        shift: LogicalShiftedRegisterShift,
        amount: u6,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(rd.size() == rm.size());
        if (rd.size() == 32) assert(amount < 32);

        return Instruction{
            .logical_shifted_register = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imm6 = amount,
                .rm = rm.enc(),
                .n = n,
                .shift = @enumToInt(shift),
                .opc = opc,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable,
                },
            },
        };
    }

    fn addSubtractImmediate(
        op: u1,
        s: u1,
        rd: Register,
        rn: Register,
        imm12: u12,
        shift: bool,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(rn.id() != Register.xzr.id());

        return Instruction{
            .add_subtract_immediate = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imm12 = imm12,
                .sh = @boolToInt(shift),
                .s = s,
                .op = op,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn logicalImmediate(
        opc: u2,
        rd: Register,
        rn: Register,
        imms: u6,
        immr: u6,
        n: u1,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(!(rd.size() == 32 and n != 0));

        return Instruction{
            .logical_immediate = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imms = imms,
                .immr = immr,
                .n = n,
                .opc = opc,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn bitfield(
        opc: u2,
        n: u1,
        rd: Register,
        rn: Register,
        immr: u6,
        imms: u6,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(!(rd.size() == 64 and n != 1));
        assert(!(rd.size() == 32 and (n != 0 or immr >> 5 != 0 or immr >> 5 != 0)));

        return Instruction{
            .bitfield = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imms = imms,
                .immr = immr,
                .n = n,
                .opc = opc,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    pub const AddSubtractShiftedRegisterShift = enum(u2) { lsl, lsr, asr, _ };

    fn addSubtractShiftedRegister(
        op: u1,
        s: u1,
        shift: AddSubtractShiftedRegisterShift,
        rd: Register,
        rn: Register,
        rm: Register,
        imm6: u6,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(rd.size() == rm.size());

        return Instruction{
            .add_subtract_shifted_register = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imm6 = imm6,
                .rm = rm.enc(),
                .shift = @enumToInt(shift),
                .s = s,
                .op = op,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    pub const AddSubtractExtendedRegisterOption = enum(u3) {
        uxtb,
        uxth,
        uxtw,
        uxtx, // serves also as lsl
        sxtb,
        sxth,
        sxtw,
        sxtx,
    };

    fn addSubtractExtendedRegister(
        op: u1,
        s: u1,
        rd: Register,
        rn: Register,
        rm: Register,
        extend: AddSubtractExtendedRegisterOption,
        imm3: u3,
    ) Instruction {
        return Instruction{
            .add_subtract_extended_register = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .imm3 = imm3,
                .option = @enumToInt(extend),
                .rm = rm.enc(),
                .s = s,
                .op = op,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn conditionalBranch(
        o0: u1,
        o1: u1,
        cond: Condition,
        offset: i21,
    ) Instruction {
        assert(offset & 0b11 == 0b00);

        return Instruction{
            .conditional_branch = .{
                .cond = @enumToInt(cond),
                .o0 = o0,
                .imm19 = @bitCast(u19, @intCast(i19, offset >> 2)),
                .o1 = o1,
            },
        };
    }

    fn compareAndBranch(
        op: u1,
        rt: Register,
        offset: i21,
    ) Instruction {
        assert(offset & 0b11 == 0b00);

        return Instruction{
            .compare_and_branch = .{
                .rt = rt.enc(),
                .imm19 = @bitCast(u19, @intCast(i19, offset >> 2)),
                .op = op,
                .sf = switch (rt.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn conditionalSelect(
        op2: u2,
        op: u1,
        s: u1,
        rd: Register,
        rn: Register,
        rm: Register,
        cond: Condition,
    ) Instruction {
        assert(rd.size() == rn.size());
        assert(rd.size() == rm.size());

        return Instruction{
            .conditional_select = .{
                .rd = rd.enc(),
                .rn = rn.enc(),
                .op2 = op2,
                .cond = @enumToInt(cond),
                .rm = rm.enc(),
                .s = s,
                .op = op,
                .sf = switch (rd.size()) {
                    32 => 0b0,
                    64 => 0b1,
                    else => unreachable, // unexpected register size
                },
            },
        };
    }

    fn dataProcessing3Source(
       