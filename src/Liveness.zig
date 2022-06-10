//! For each AIR instruction, we want to know:
//! * Is the instruction unreferenced (e.g. dies immediately)?
//! * For each of its operands, does the operand die with this instruction (e.g. is
//!   this the last reference to it)?
//! Some instructions are special, such as:
//! * Conditional Branches
//! * Switch Branches
const Liveness = @This();
const std = @import("std");
const trace = @import("tracy.zig").trace;
const log = std.log.scoped(.liveness);
const assert = std.debug.assert;
const Allocator = std.mem.Allocator;
const Air = @import("Air.zig");
const Log2Int = std.math.Log2Int;

/// This array is split into sets of 4 bits per AIR instruction.
/// The MSB (0bX000) is whether the instruction is unreferenced.
/// The LSB (0b000X) is the first operand, and so on, up to 3 operands. A set bit means the
/// operand dies after this instruction.
/// Instructions which need more data to track liveness have special handling via the
/// `special` table.
tomb_bits: []usize,
/// Sparse table of specially handled instructions. The value is an index into the `extra`
/// array. The meaning of the data depends on the AIR tag.
///  * `cond_br` - points to a `CondBr` in `extra` at this index.
///  * `switch_br` - points to a `SwitchBr` in `extra` at this index.
///  * `asm`, `call`, `aggregate_init` - the value is a set of bits which are the extra tomb
///    bits of operands.
///    The main tomb bits are still used and the extra ones are starting with the lsb of the
///    value here.
special: std.AutoHashMapUnmanaged(Air.Inst.Index, u32),
/// Auxiliary data. The way this data is interpreted is determined contextually.
extra: []const u32,

/// Trailing is the set of instructions whose lifetimes end at the start of the then branch,
/// followed by the set of instructions whose lifetimes end at the start of the else branch.
pub const CondBr = struct {
    then_death_count: u32,
    else_death_count: u32,
};

/// Trailing is:
/// * For each case in the same order as in the AIR:
///   - case_death_count: u32
///   - Air.Inst.Index for each `case_death_count`: set of instructions whose lifetimes
///     end at the start of this case.
/// * Air.Inst.Index for each `else_death_count`: set of instructions whose lifetimes
///   end at the start of the else case.
pub const SwitchBr = struct {
    else_death_count: u32,
};

pub fn analyze(gpa: Allocator, air: Air) Allocator.Error!Liveness {
    const tracy = trace(@src());
    defer tracy.end();

    var a: Analysis = .{
        .gpa = gpa,
        .air = air,
        .table = .{},
        .tomb_bits = try gpa.alloc(
            usize,
            (air.instructions.len * bpi + @bitSizeOf(usize) - 1) / @bitSizeOf(usize),
        ),
        .extra = .{},
        .special = .{},
    };
    errdefer gpa.free(a.tomb_bits);
    errdefer a.special.deinit(gpa);
    defer a.extra.deinit(gpa);
    defer a.table.deinit(gpa);

    std.mem.set(usize, a.tomb_bits, 0);

    const main_body = air.getMainBody();
    try a.table.ensureTotalCapacity(gpa, @intCast(u32, main_body.len));
    try analyzeWithContext(&a, null, main_body);
    return Liveness{
        .tomb_bits = a.tomb_bits,
        .special = a.special,
        .extra = try a.extra.toOwnedSlice(gpa),
    };
}

pub fn getTombBits(l: Liveness, inst: Air.Inst.Index) Bpi {
    const usize_index = (inst * bpi) / @bitSizeOf(usize);
    return @truncate(Bpi, l.tomb_bits[usize_index] >>
        @intCast(Log2Int(usize), (inst % (@bitSizeOf(usize) / bpi)) * bpi));
}

pub fn isUnused(l: Liveness, inst: Air.Inst.Index) bool {
    const usize_index = (inst * bpi) / @bitSizeOf(usize);
    const mask = @as(usize, 1) <<
        @intCast(Log2Int(usize), (inst % (@bitSizeOf(usize) / bpi)) * bpi + (bpi - 1));
    return (l.tomb_bits[usize_index] & mask) != 0;
}

pub fn operandDies(l: Liveness, inst: Air.Inst.Index, operand: OperandInt) bool {
    assert(operand < bpi - 1);
    const usize_index = (inst * bpi) / @bitSizeOf(usize);
    const mask = @as(usize, 1) <<
        @intCast(Log2Int(usize), (inst % (@bitSizeOf(usize) / bpi)) * bpi + operand);
    return (l.tomb_bits[usize_index] & mask) != 0;
}

pub fn clearOperandDeath(l: Liveness, inst: Air.Inst.Index, operand: OperandInt) void {
    assert(operand < bpi - 1);
    const usize_index = (inst * bpi) / @bitSizeOf(usize);
    const mask = @as(usize, 1) <<
        @intCast(Log2Int(usize), (inst % (@bitSizeOf(usize) / bpi)) * bpi + operand);
    l.tomb_bits[usize_index] &= ~mask;
}

const OperandCategory = enum {
    /// The operand lives on, but this instruction cannot possibly mutate memory.
    none,
    /// The operand lives on and this instruction can mutate memory.
    write,
    /// The operand dies at this instruction.
    tomb,
    /// The operand lives on, and this instruction is noreturn.
    noret,
    /// This instruction is too complicated for analysis, no information is available.
    complex,
};

/// Given an instruction that we are examining, and an operand that we are looking for,
/// returns a classification.
pub fn categorizeOperand(
    l: Liveness,
    air: Air,
    inst: Air.Inst.Index,
    operand: Air.Inst.Index,
) OperandCategory {
    const air_tags = air.instructions.items(.tag);
    const air_datas = air.instructions.items(.data);
    const operand_ref = Air.indexToRef(operand);
    switch (air_tags[inst]) {
        .add,
        .addwrap,
        .add_sat,
        .sub,
        .subwrap,
        .sub_sat,
        .mul,
        .mulwrap,
        .mul_sat,
        .div_float,
        .div_trunc,
        .div_floor,
        .div_exact,
        .rem,
        .mod,
        .bit_and,
        .bit_or,
        .xor,
        .cmp_lt,
        .cmp_lte,
        .cmp_eq,
        .cmp_gte,
        .cmp_gt,
        .cmp_neq,
        .bool_and,
        .bool_or,
        .array_elem_val,
        .slice_elem_val,
        .ptr_elem_val,
        .shl,
        .shl_exact,
        .shl_sat,
        .shr,
        .shr_exact,
        .min,
        .max,
        .add_optimized,
        .addwrap_optimized,
        .sub_optimized,
        .subwrap_optimized,
        .mul_optimized,
        .mulwrap_optimized,
        .div_float_optimized,
        .div_trunc_optimized,
        .div_floor_optimized,
        .div_exact_optimized,
        .rem_optimized,
        .mod_optimized,
        .neg_optimized,
        .cmp_lt_optimized,
        .cmp_lte_optimized,
        .cmp_eq_optimized,
        .cmp_gte_optimized,
        .cmp_gt_optimized,
        .cmp_neq_optimized,
        => {
            const o = air_datas[inst].bin_op;
            if (o.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (o.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            return .none;
        },

        .store,
        .atomic_store_unordered,
        .atomic_store_monotonic,
        .atomic_store_release,
        .atomic_store_seq_cst,
        .set_union_tag,
        => {
            const o = air_datas[inst].bin_op;
            if (o.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            if (o.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .write);
            return .write;
        },

        .vector_store_elem => {
            const o = air_datas[inst].vector_store_elem;
            const extra = air.extraData(Air.Bin, o.payload).data;
            if (o.vector_ptr == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 2, .none);
            return .write;
        },

        .arg,
        .alloc,
        .ret_ptr,
        .constant,
        .const_ty,
        .trap,
        .breakpoint,
        .dbg_stmt,
        .dbg_inline_begin,
        .dbg_inline_end,
        .dbg_block_begin,
        .dbg_block_end,
        .unreach,
        .ret_addr,
        .frame_addr,
        .wasm_memory_size,
        .err_return_trace,
        .save_err_return_trace_index,
        .c_va_start,
        => return .none,

        .fence => return .write,

        .not,
        .bitcast,
        .load,
        .fpext,
        .fptrunc,
        .intcast,
        .trunc,
        .optional_payload,
        .optional_payload_ptr,
        .wrap_optional,
        .unwrap_errunion_payload,
        .unwrap_errunion_err,
        .unwrap_errunion_payload_ptr,
        .unwrap_errunion_err_ptr,
        .wrap_errunion_payload,
        .wrap_errunion_err,
        .slice_ptr,
        .slice_len,
        .ptr_slice_len_ptr,
        .ptr_slice_ptr_ptr,
        .struct_field_ptr_index_0,
        .struct_field_ptr_index_1,
        .struct_field_ptr_index_2,
        .struct_field_ptr_index_3,
        .array_to_slice,
        .float_to_int,
        .float_to_int_optimized,
        .int_to_float,
        .get_union_tag,
        .clz,
        .ctz,
        .popcount,
        .byte_swap,
        .bit_reverse,
        .splat,
        .error_set_has_value,
        .addrspace_cast,
        .c_va_arg,
        .c_va_copy,
        => {
            const o = air_datas[inst].ty_op;
            if (o.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },

        .optional_payload_ptr_set,
        .errunion_payload_ptr_set,
        => {
            const o = air_datas[inst].ty_op;
            if (o.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            return .write;
        },

        .is_null,
        .is_non_null,
        .is_null_ptr,
        .is_non_null_ptr,
        .is_err,
        .is_non_err,
        .is_err_ptr,
        .is_non_err_ptr,
        .ptrtoint,
        .bool_to_int,
        .is_named_enum_value,
        .tag_name,
        .error_name,
        .sqrt,
        .sin,
        .cos,
        .tan,
        .exp,
        .exp2,
        .log,
        .log2,
        .log10,
        .fabs,
        .floor,
        .ceil,
        .round,
        .trunc_float,
        .neg,
        .cmp_lt_errors_len,
        .c_va_end,
        => {
            const o = air_datas[inst].un_op;
            if (o == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },

        .ret,
        .ret_load,
        => {
            const o = air_datas[inst].un_op;
            if (o == operand_ref) return matchOperandSmallIndex(l, inst, 0, .noret);
            return .noret;
        },

        .set_err_return_trace => {
            const o = air_datas[inst].un_op;
            if (o == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            return .write;
        },

        .add_with_overflow,
        .sub_with_overflow,
        .mul_with_overflow,
        .shl_with_overflow,
        .ptr_add,
        .ptr_sub,
        .ptr_elem_ptr,
        .slice_elem_ptr,
        .slice,
        => {
            const ty_pl = air_datas[inst].ty_pl;
            const extra = air.extraData(Air.Bin, ty_pl.payload).data;
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            return .none;
        },

        .dbg_var_ptr,
        .dbg_var_val,
        => {
            const o = air_datas[inst].pl_op.operand;
            if (o == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },

        .prefetch => {
            const prefetch = air_datas[inst].prefetch;
            if (prefetch.ptr == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },

        .call, .call_always_tail, .call_never_tail, .call_never_inline => {
            const inst_data = air_datas[inst].pl_op;
            const callee = inst_data.operand;
            const extra = air.extraData(Air.Call, inst_data.payload);
            const args = @ptrCast([]const Air.Inst.Ref, air.extra[extra.end..][0..extra.data.args_len]);
            if (args.len + 1 <= bpi - 1) {
                if (callee == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
                for (args, 0..) |arg, i| {
                    if (arg == operand_ref) return matchOperandSmallIndex(l, inst, @intCast(OperandInt, i + 1), .write);
                }
                return .write;
            }
            var bt = l.iterateBigTomb(inst);
            if (bt.feed()) {
                if (callee == operand_ref) return .tomb;
            } else {
                if (callee == operand_ref) return .write;
            }
            for (args) |arg| {
                if (bt.feed()) {
                    if (arg == operand_ref) return .tomb;
                } else {
                    if (arg == operand_ref) return .write;
                }
            }
            return .write;
        },
        .select => {
            const pl_op = air_datas[inst].pl_op;
            const extra = air.extraData(Air.Bin, pl_op.payload).data;
            if (pl_op.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 2, .none);
            return .none;
        },
        .shuffle => {
            const extra = air.extraData(Air.Shuffle, air_datas[inst].ty_pl.payload).data;
            if (extra.a == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (extra.b == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            return .none;
        },
        .reduce, .reduce_optimized => {
            const reduce = air_datas[inst].reduce;
            if (reduce.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
        .cmp_vector, .cmp_vector_optimized => {
            const extra = air.extraData(Air.VectorCmp, air_datas[inst].ty_pl.payload).data;
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            return .none;
        },
        .aggregate_init => {
            const ty_pl = air_datas[inst].ty_pl;
            const aggregate_ty = air.getRefType(ty_pl.ty);
            const len = @intCast(usize, aggregate_ty.arrayLen());
            const elements = @ptrCast([]const Air.Inst.Ref, air.extra[ty_pl.payload..][0..len]);

            if (elements.len <= bpi - 1) {
                for (elements, 0..) |elem, i| {
                    if (elem == operand_ref) return matchOperandSmallIndex(l, inst, @intCast(OperandInt, i), .none);
                }
                return .none;
            }

            var bt = l.iterateBigTomb(inst);
            for (elements) |elem| {
                if (bt.feed()) {
                    if (elem == operand_ref) return .tomb;
                } else {
                    if (elem == operand_ref) return .write;
                }
            }
            return .write;
        },
        .union_init => {
            const extra = air.extraData(Air.UnionInit, air_datas[inst].ty_pl.payload).data;
            if (extra.init == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
        .struct_field_ptr, .struct_field_val => {
            const extra = air.extraData(Air.StructField, air_datas[inst].ty_pl.payload).data;
            if (extra.struct_operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
        .field_parent_ptr => {
            const extra = air.extraData(Air.FieldParentPtr, air_datas[inst].ty_pl.payload).data;
            if (extra.field_ptr == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
        .cmpxchg_strong, .cmpxchg_weak => {
            const extra = air.extraData(Air.Cmpxchg, air_datas[inst].ty_pl.payload).data;
            if (extra.ptr == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            if (extra.expected_value == operand_ref) return matchOperandSmallIndex(l, inst, 1, .write);
            if (extra.new_value == operand_ref) return matchOperandSmallIndex(l, inst, 2, .write);
            return .write;
        },
        .mul_add => {
            const pl_op = air_datas[inst].pl_op;
            const extra = air.extraData(Air.Bin, pl_op.payload).data;
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .none);
            if (pl_op.operand == operand_ref) return matchOperandSmallIndex(l, inst, 2, .none);
            return .none;
        },
        .atomic_load => {
            const ptr = air_datas[inst].atomic_load.ptr;
            if (ptr == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
        .atomic_rmw => {
            const pl_op = air_datas[inst].pl_op;
            const extra = air.extraData(Air.AtomicRmw, pl_op.payload).data;
            if (pl_op.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            if (extra.operand == operand_ref) return matchOperandSmallIndex(l, inst, 1, .write);
            return .write;
        },
        .memset,
        .memcpy,
        => {
            const pl_op = air_datas[inst].pl_op;
            const extra = air.extraData(Air.Bin, pl_op.payload).data;
            if (pl_op.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .write);
            if (extra.lhs == operand_ref) return matchOperandSmallIndex(l, inst, 1, .write);
            if (extra.rhs == operand_ref) return matchOperandSmallIndex(l, inst, 2, .write);
            return .write;
        },

        .br => {
            const br = air_datas[inst].br;
            if (br.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .noret);
            return .noret;
        },
        .assembly => {
            return .complex;
        },
        .block => {
            const extra = air.extraData(Air.Block, air_datas[inst].ty_pl.payload);
            const body = air.extra[extra.end..][0..extra.data.body_len];

            if (body.len == 1 and air_tags[body[0]] == .cond_br) {
                // Peephole optimization for "panic-like" conditionals, which have
                // one empty branch and another which calls a `noreturn` function.
                // This allows us to infer that safety checks do not modify memory,
                // as far as control flow successors are concerned.

                const inst_data = air_datas[body[0]].pl_op;
                const cond_extra = air.extraData(Air.CondBr, inst_data.payload);
                if (inst_data.operand == operand_ref and operandDies(l, body[0], 0))
                    return .tomb;

                if (cond_extra.data.then_body_len != 1 or cond_extra.data.else_body_len != 1)
                    return .complex;

                var operand_live: bool = true;
                for (air.extra[cond_extra.end..][0..2]) |cond_inst| {
                    if (l.categorizeOperand(air, cond_inst, operand) == .tomb)
                        operand_live = false;

                    switch (air_tags[cond_inst]) {
                        .br => { // Breaks immediately back to block
                            const br = air_datas[cond_inst].br;
                            if (br.block_inst != inst)
                                return .complex;
                        },
                        .call => {}, // Calls a noreturn function
                        else => return .complex,
                    }
                }
                return if (operand_live) .none else .tomb;
            }

            return .complex;
        },
        .@"try" => {
            return .complex;
        },
        .try_ptr => {
            return .complex;
        },
        .loop => {
            return .complex;
        },
        .cond_br => {
            return .complex;
        },
        .switch_br => {
            return .complex;
        },
        .wasm_memory_grow => {
            const pl_op = air_datas[inst].pl_op;
            if (pl_op.operand == operand_ref) return matchOperandSmallIndex(l, inst, 0, .none);
            return .none;
        },
    }
}

fn matchOperandSmallIndex(
    l: Liveness,
    inst: Air.Inst.Index,
    operand: OperandInt,
    default: OperandCategory,
) OperandCategory {
    if (operandDies(l, inst, operand)) {
        return .tomb;
    } else {
        return default;
    }
}

/// Higher level API.
pub const CondBrSlices = struct {
    then_deaths: []const Air.Inst.Index,
    else_deaths: []const Air.Inst.Index,
};

pub fn getCondBr(l: Liveness, inst: Air.Inst.Index) CondBrSlices {
    var index: usize = l.special.get(inst) orelse return .{
        .then_deaths = &.{},
        .else_deaths = &.{},
    };
    const then_death_count = l.extra[index];
    index += 1;
    const else_death_count = l.extra[index];
    index += 1;
    const then_deaths = l.extra[index..][0..then_death_count];
    index += then_death_count;
    return .{
        .then_deaths = then_deaths,
        .else_deaths = l.extra[index..][0..else_death_count],
    };
}

/// Indexed by case number as they appear in AIR.
/// Else is the last element.
pub const SwitchBrTable = struct {
    deaths: []const []const Air.Inst.Index,
};

/// Caller owns the memory.
pub fn getSwitchBr(l: Liveness, gpa: Allocator, inst: Air.Inst.Index, cases_len: u32) Allocator.Error!SwitchBrTable {
    var index: usize = l.special.get(inst) orelse return SwitchBrTable{
        .deaths = &.{},
    };
    const else_death_count = l.extra[index];
    index += 1;

    var deaths = std.ArrayList([]const Air.Inst.Index).init(gpa);
    defer deaths.deinit();
    try deaths.ensureTotalCapacity(cases_len + 1);

    var case_i: u32 = 0;
    while (case_i < cases_len - 1) : (case_i += 1) {
        const case_death_count: u32 = l.extra[index];
        index += 1;
        const case_deaths = l.extra[index..][0..case_death_count];
        index += case_death_count;
        deaths.appendAssumeCapacity(case_deaths);
    }
    {
        // Else
        const else_deaths = l.extra[index..][0..else_death_count];
        deaths.appendAssumeCapacity(else_deaths);
    }
    return SwitchBrTable{
        .deaths = try deaths.toOwnedSlice(),
    };
}

pub fn deinit(l: *Liveness, gpa: Allocator) void {
    gpa.free(l.tomb_bits);
    gpa.free(l.extra);
    l.special.deinit(gpa);
    l.* = undefined;
}

pub fn iterateBigTomb(l: Liveness, inst: Air.Inst.Index) BigTomb {
    return .{
        .tomb_bits = l.getTombBits(inst),
        .extra_start = l.special.get(inst) orelse 0,
        .extra_offset = 0,
        .extra = l.extra,
        .bit_index = 0,
    };
}

/// How many tomb bits per AIR instruction.
pub const bpi = 4;
pub const Bpi = std.meta.Int(.unsigned, bpi);
pub const OperandInt = std.math.Log2Int(Bpi);

/// Useful for 