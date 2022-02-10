
//! Semantic analysis of ZIR instructions.
//! Shared to every Block. Stored on the stack.
//! State used for compiling a ZIR into AIR.
//! Transforms untyped ZIR instructions into semantically-analyzed AIR instructions.
//! Does type checking, comptime control flow, and safety-check generation.
//! This is the the heart of the Zig compiler.

mod: *Module,
/// Alias to `mod.gpa`.
gpa: Allocator,
/// Points to the temporary arena allocator of the Sema.
/// This arena will be cleared when the sema is destroyed.
arena: Allocator,
/// Points to the arena allocator for the owner_decl.
/// This arena will persist until the decl is invalidated.
perm_arena: Allocator,
code: Zir,
air_instructions: std.MultiArrayList(Air.Inst) = .{},
air_extra: std.ArrayListUnmanaged(u32) = .{},
air_values: std.ArrayListUnmanaged(Value) = .{},
/// Maps ZIR to AIR.
inst_map: InstMap = .{},
/// When analyzing an inline function call, owner_decl is the Decl of the caller
/// and `src_decl` of `Block` is the `Decl` of the callee.
/// This `Decl` owns the arena memory of this `Sema`.
owner_decl: *Decl,
owner_decl_index: Decl.Index,
/// For an inline or comptime function call, this will be the root parent function
/// which contains the callsite. Corresponds to `owner_decl`.
owner_func: ?*Module.Fn,
/// The function this ZIR code is the body of, according to the source code.
/// This starts out the same as `owner_func` and then diverges in the case of
/// an inline or comptime function call.
func: ?*Module.Fn,
/// Used to restore the error return trace when returning a non-error from a function.
error_return_trace_index_on_fn_entry: Air.Inst.Ref = .none,
/// When semantic analysis needs to know the return type of the function whose body
/// is being analyzed, this `Type` should be used instead of going through `func`.
/// This will correctly handle the case of a comptime/inline function call of a
/// generic function which uses a type expression for the return type.
/// The type will be `void` in the case that `func` is `null`.
fn_ret_ty: Type,
branch_quota: u32 = default_branch_quota,
branch_count: u32 = 0,
/// Populated when returning `error.ComptimeBreak`. Used to communicate the
/// break instruction up the stack to find the corresponding Block.
comptime_break_inst: Zir.Inst.Index = undefined,
/// This field is updated when a new source location becomes active, so that
/// instructions which do not have explicitly mapped source locations still have
/// access to the source location set by the previous instruction which did
/// contain a mapped source location.
src: LazySrcLoc = .{ .token_offset = 0 },
decl_val_table: std.AutoHashMapUnmanaged(Decl.Index, Air.Inst.Ref) = .{},
/// When doing a generic function instantiation, this array collects a
/// `Value` object for each parameter that is comptime-known and thus elided
/// from the generated function. This memory is allocated by a parent `Sema` and
/// owned by the values arena of the Sema owner_decl.
comptime_args: []TypedValue = &.{},
/// Marks the function instruction that `comptime_args` applies to so that we
/// don't accidentally apply it to a function prototype which is used in the
/// type expression of a generic function parameter.
comptime_args_fn_inst: Zir.Inst.Index = 0,
/// When `comptime_args` is provided, this field is also provided. It was used as
/// the key in the `monomorphed_funcs` set. The `func` instruction is supposed
/// to use this instead of allocating a fresh one. This avoids an unnecessary
/// extra hash table lookup in the `monomorphed_funcs` set.
/// Sema will set this to null when it takes ownership.
preallocated_new_func: ?*Module.Fn = null,
/// The key is `constant` AIR instructions to types that must be fully resolved
/// after the current function body analysis is done.
/// TODO: after upgrading to use InternPool change the key here to be an
/// InternPool value index.
types_to_resolve: std.ArrayListUnmanaged(Air.Inst.Ref) = .{},
/// These are lazily created runtime blocks from block_inline instructions.
/// They are created when an break_inline passes through a runtime condition, because
/// Sema must convert comptime control flow to runtime control flow, which means
/// breaking from a block.
post_hoc_blocks: std.AutoHashMapUnmanaged(Air.Inst.Index, *LabeledBlock) = .{},
/// Populated with the last compile error created.
err: ?*Module.ErrorMsg = null,
/// True when analyzing a generic instantiation. Used to suppress some errors.
is_generic_instantiation: bool = false,
/// Set to true when analyzing a func type instruction so that nested generic
/// function types will emit generic poison instead of a partial type.
no_partial_func_ty: bool = false,

unresolved_inferred_allocs: std.AutoHashMapUnmanaged(Air.Inst.Index, void) = .{},

const std = @import("std");
const math = std.math;
const mem = std.mem;
const Allocator = std.mem.Allocator;
const assert = std.debug.assert;
const log = std.log.scoped(.sema);

const Sema = @This();
const Value = @import("value.zig").Value;
const Type = @import("type.zig").Type;
const TypedValue = @import("TypedValue.zig");
const Air = @import("Air.zig");
const Zir = @import("Zir.zig");
const Module = @import("Module.zig");
const trace = @import("tracy.zig").trace;
const Namespace = Module.Namespace;
const CompileError = Module.CompileError;
const SemaError = Module.SemaError;
const Decl = Module.Decl;
const CaptureScope = Module.CaptureScope;
const WipCaptureScope = Module.WipCaptureScope;
const LazySrcLoc = Module.LazySrcLoc;
const RangeSet = @import("RangeSet.zig");
const target_util = @import("target.zig");
const Package = @import("Package.zig");
const crash_report = @import("crash_report.zig");
const build_options = @import("build_options");
const Compilation = @import("Compilation.zig");

pub const default_branch_quota = 1000;
pub const default_reference_trace_len = 2;

/// Stores the mapping from `Zir.Inst.Index -> Air.Inst.Ref`, which is used by sema to resolve
/// instructions during analysis.
/// Instead of a hash table approach, InstMap is simply a slice that is indexed into using the
/// zir instruction index and a start offset. An index is not pressent in the map if the value
/// at the index is `Air.Inst.Ref.none`.
/// `ensureSpaceForInstructions` can be called to force InstMap to have a mapped range that
/// includes all instructions in a slice. After calling this function, `putAssumeCapacity*` can
/// be called safely for any of the instructions passed in.
pub const InstMap = struct {
    items: []Air.Inst.Ref = &[_]Air.Inst.Ref{},
    start: Zir.Inst.Index = 0,

    pub fn deinit(map: InstMap, allocator: mem.Allocator) void {
        allocator.free(map.items);
    }

    pub fn get(map: InstMap, key: Zir.Inst.Index) ?Air.Inst.Ref {
        if (!map.contains(key)) return null;
        return map.items[key - map.start];
    }

    pub fn putAssumeCapacity(
        map: *InstMap,
        key: Zir.Inst.Index,
        ref: Air.Inst.Ref,
    ) void {
        map.items[key - map.start] = ref;
    }

    pub fn putAssumeCapacityNoClobber(
        map: *InstMap,
        key: Zir.Inst.Index,
        ref: Air.Inst.Ref,
    ) void {
        assert(!map.contains(key));
        map.putAssumeCapacity(key, ref);
    }

    pub const GetOrPutResult = struct {
        value_ptr: *Air.Inst.Ref,
        found_existing: bool,
    };

    pub fn getOrPutAssumeCapacity(
        map: *InstMap,
        key: Zir.Inst.Index,
    ) GetOrPutResult {
        const index = key - map.start;
        return GetOrPutResult{
            .value_ptr = &map.items[index],
            .found_existing = map.items[index] != .none,
        };
    }

    pub fn remove(map: InstMap, key: Zir.Inst.Index) bool {
        if (!map.contains(key)) return false;
        map.items[key - map.start] = .none;
        return true;
    }

    pub fn contains(map: InstMap, key: Zir.Inst.Index) bool {
        return map.items[key - map.start] != .none;
    }

    pub fn ensureSpaceForInstructions(
        map: *InstMap,
        allocator: mem.Allocator,
        insts: []const Zir.Inst.Index,
    ) !void {
        const min_max = mem.minMax(Zir.Inst.Index, insts);
        const start = min_max.min;
        const end = min_max.max;
        if (map.start <= start and end < map.items.len + map.start)
            return;

        const old_start = if (map.items.len == 0) start else map.start;
        var better_capacity = map.items.len;
        var better_start = old_start;
        while (true) {
            const extra_capacity = better_capacity / 2 + 16;
            better_capacity += extra_capacity;
            better_start -|= @intCast(Zir.Inst.Index, extra_capacity / 2);
            if (better_start <= start and end < better_capacity + better_start)
                break;
        }

        const start_diff = old_start - better_start;
        const new_items = try allocator.alloc(Air.Inst.Ref, better_capacity);
        mem.set(Air.Inst.Ref, new_items[0..start_diff], .none);
        mem.copy(Air.Inst.Ref, new_items[start_diff..], map.items);
        mem.set(Air.Inst.Ref, new_items[start_diff + map.items.len ..], .none);

        allocator.free(map.items);
        map.items = new_items;
        map.start = @intCast(Zir.Inst.Index, better_start);
    }
};

/// This is the context needed to semantically analyze ZIR instructions and
/// produce AIR instructions.
/// This is a temporary structure stored on the stack; references to it are valid only
/// during semantic analysis of the block.
pub const Block = struct {
    parent: ?*Block,
    /// Shared among all child blocks.
    sema: *Sema,
    /// The namespace to use for lookups from this source block
    /// When analyzing fields, this is different from src_decl.src_namespace.
    namespace: *Namespace,
    /// The AIR instructions generated for this block.
    instructions: std.ArrayListUnmanaged(Air.Inst.Index),
    // `param` instructions are collected here to be used by the `func` instruction.
    params: std.ArrayListUnmanaged(Param) = .{},

    wip_capture_scope: *CaptureScope,

    label: ?*Label = null,
    inlining: ?*Inlining,
    /// If runtime_index is not 0 then one of these is guaranteed to be non null.
    runtime_cond: ?LazySrcLoc = null,
    runtime_loop: ?LazySrcLoc = null,
    /// This Decl is the Decl according to the Zig source code corresponding to this Block.
    /// This can vary during inline or comptime function calls. See `Sema.owner_decl`
    /// for the one that will be the same for all Block instances.
    src_decl: Decl.Index,
    /// Non zero if a non-inline loop or a runtime conditional have been encountered.
    /// Stores to comptime variables are only allowed when var.runtime_index <= runtime_index.
    runtime_index: Value.RuntimeIndex = .zero,
    inline_block: Zir.Inst.Index = 0,

    comptime_reason: ?*const ComptimeReason = null,
    // TODO is_comptime and comptime_reason should probably be merged together.
    is_comptime: bool,
    is_typeof: bool = false,
    is_coerce_result_ptr: bool = false,

    /// Keep track of the active error return trace index around blocks so that we can correctly
    /// pop the error trace upon block exit.
    error_return_trace_index: Air.Inst.Ref = .none,

    /// when null, it is determined by build mode, changed by @setRuntimeSafety
    want_safety: ?bool = null,

    /// What mode to generate float operations in, set by @setFloatMode
    float_mode: std.builtin.FloatMode = .Strict,

    c_import_buf: ?*std.ArrayList(u8) = null,

    /// type of `err` in `else => |err|`
    switch_else_err_ty: ?Type = null,

    /// Value for switch_capture in an inline case
    inline_case_capture: Air.Inst.Ref = .none,

    const ComptimeReason = union(enum) {
        c_import: struct {
            block: *Block,
            src: LazySrcLoc,
        },
        comptime_ret_ty: struct {
            block: *Block,
            func: Air.Inst.Ref,
            func_src: LazySrcLoc,
            return_ty: Type,
        },

        fn explain(cr: ComptimeReason, sema: *Sema, msg: ?*Module.ErrorMsg) !void {
            const parent = msg orelse return;
            const prefix = "expression is evaluated at comptime because ";
            switch (cr) {
                .c_import => |ci| {
                    try sema.errNote(ci.block, ci.src, parent, prefix ++ "it is inside a @cImport", .{});
                },
                .comptime_ret_ty => |rt| {
                    const src_loc = if (try sema.funcDeclSrc(rt.func)) |fn_decl| blk: {
                        var src_loc = fn_decl.srcLoc();
                        src_loc.lazy = .{ .node_offset_fn_type_ret_ty = 0 };
                        break :blk src_loc;
                    } else blk: {
                        const src_decl = sema.mod.declPtr(rt.block.src_decl);
                        break :blk rt.func_src.toSrcLoc(src_decl);
                    };
                    if (rt.return_ty.tag() == .generic_poison) {
                        return sema.mod.errNoteNonLazy(src_loc, parent, prefix ++ "the generic function was instantiated with a comptime-only return type", .{});
                    }
                    try sema.mod.errNoteNonLazy(
                        src_loc,
                        parent,
                        prefix ++ "the function returns a comptime-only type '{}'",
                        .{rt.return_ty.fmt(sema.mod)},
                    );
                    try sema.explainWhyTypeIsComptime(rt.block, rt.func_src, parent, src_loc, rt.return_ty);
                },
            }
        }
    };

    const Param = struct {
        /// `noreturn` means `anytype`.
        ty: Type,
        is_comptime: bool,
        name: []const u8,
    };

    /// This `Block` maps a block ZIR instruction to the corresponding
    /// AIR instruction for break instruction analysis.
    pub const Label = struct {
        zir_block: Zir.Inst.Index,
        merges: Merges,
    };

    /// This `Block` indicates that an inline function call is happening
    /// and return instructions should be analyzed as a break instruction
    /// to this AIR block instruction.
    /// It is shared among all the blocks in an inline or comptime called
    /// function.
    pub const Inlining = struct {
        func: ?*Module.Fn,
        comptime_result: Air.Inst.Ref,
        merges: Merges,
    };

    pub const Merges = struct {
        block_inst: Air.Inst.Index,
        /// Separate array list from break_inst_list so that it can be passed directly
        /// to resolvePeerTypes.
        results: std.ArrayListUnmanaged(Air.Inst.Ref),
        /// Keeps track of the break instructions so that the operand can be replaced
        /// if we need to add type coercion at the end of block analysis.
        /// Same indexes, capacity, length as `results`.
        br_list: std.ArrayListUnmanaged(Air.Inst.Index),
    };

    /// For debugging purposes.
    pub fn dump(block: *Block, mod: Module) void {
        Zir.dumpBlock(mod, block);
    }

    pub fn makeSubBlock(parent: *Block) Block {
        return .{
            .parent = parent,
            .sema = parent.sema,
            .src_decl = parent.src_decl,
            .namespace = parent.namespace,
            .instructions = .{},
            .wip_capture_scope = parent.wip_capture_scope,
            .label = null,
            .inlining = parent.inlining,
            .is_comptime = parent.is_comptime,
            .comptime_reason = parent.comptime_reason,
            .is_typeof = parent.is_typeof,
            .runtime_cond = parent.runtime_cond,
            .runtime_loop = parent.runtime_loop,
            .runtime_index = parent.runtime_index,
            .want_safety = parent.want_safety,
            .float_mode = parent.float_mode,
            .c_import_buf = parent.c_import_buf,
            .switch_else_err_ty = parent.switch_else_err_ty,
            .error_return_trace_index = parent.error_return_trace_index,
        };
    }

    pub fn wantSafety(block: *const Block) bool {
        return block.want_safety orelse switch (block.sema.mod.optimizeMode()) {
            .Debug => true,
            .ReleaseSafe => true,
            .ReleaseFast => false,
            .ReleaseSmall => false,
        };
    }

    pub fn getFileScope(block: *Block) *Module.File {
        return block.namespace.file_scope;
    }

    fn addTy(
        block: *Block,
        tag: Air.Inst.Tag,
        ty: Type,
    ) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = tag,
            .data = .{ .ty = ty },
        });
    }

    fn addTyOp(
        block: *Block,
        tag: Air.Inst.Tag,
        ty: Type,
        operand: Air.Inst.Ref,
    ) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = tag,
            .data = .{ .ty_op = .{
                .ty = try block.sema.addType(ty),
                .operand = operand,
            } },
        });
    }

    fn addBitCast(block: *Block, ty: Type, operand: Air.Inst.Ref) Allocator.Error!Air.Inst.Ref {
        return block.addInst(.{
            .tag = .bitcast,
            .data = .{ .ty_op = .{
                .ty = try block.sema.addType(ty),
                .operand = operand,
            } },
        });
    }

    fn addNoOp(block: *Block, tag: Air.Inst.Tag) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = tag,
            .data = .{ .no_op = {} },
        });
    }

    fn addUnOp(
        block: *Block,
        tag: Air.Inst.Tag,
        operand: Air.Inst.Ref,
    ) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = tag,
            .data = .{ .un_op = operand },
        });
    }

    fn addBr(
        block: *Block,
        target_block: Air.Inst.Index,
        operand: Air.Inst.Ref,
    ) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = .br,
            .data = .{ .br = .{
                .block_inst = target_block,
                .operand = operand,
            } },
        });
    }

    fn addBinOp(
        block: *Block,
        tag: Air.Inst.Tag,
        lhs: Air.Inst.Ref,
        rhs: Air.Inst.Ref,
    ) error{OutOfMemory}!Air.Inst.Ref {
        return block.addInst(.{
            .tag = tag,
            .data = .{ .bin_op = .{
                .lhs = lhs,
                .rhs = rhs,
            } },
        });
    }

    fn addStructFieldPtr(
        block: *Block,
        struct_ptr: Air.Inst.Ref,
        field_index: u32,
        ptr_field_ty: Type,
    ) !Air.Inst.Ref {
        const ty = try block.sema.addType(ptr_field_ty);
        const tag: Air.Inst.Tag = switch (field_index) {
            0 => .struct_field_ptr_index_0,
            1 => .struct_field_ptr_index_1,
            2 => .struct_field_ptr_index_2,
            3 => .struct_field_ptr_index_3,
            else => {
                return block.addInst(.{
                    .tag = .struct_field_ptr,
                    .data = .{ .ty_pl = .{
                        .ty = ty,
                        .payload = try block.sema.addExtra(Air.StructField{
                            .struct_operand = struct_ptr,
                            .field_index = field_index,
                        }),
                    } },
                });
            },
        };
        return block.addInst(.{
            .tag = tag,
            .data = .{ .ty_op = .{
                .ty = ty,
                .operand = struct_ptr,
            } },
        });
    }

    fn addStructFieldVal(
        block: *Block,
        struct_val: Air.Inst.Ref,
        field_index: u32,
        field_ty: Type,
    ) !Air.Inst.Ref {
        return block.addInst(.{
            .tag = .struct_field_val,
            .data = .{ .ty_pl = .{
                .ty = try block.sema.addType(field_ty),
                .payload = try block.sema.addExtra(Air.StructField{
                    .struct_operand = struct_val,
                    .field_index = field_index,
                }),
            } },
        });
    }

    fn addSliceElemPtr(
        block: *Block,
        slice: Air.Inst.Ref,
        elem_index: Air.Inst.Ref,
        elem_ptr_ty: Type,
    ) !Air.Inst.Ref {
        return block.addInst(.{
            .tag = .slice_elem_ptr,
            .data = .{ .ty_pl = .{
                .ty = try block.sema.addType(elem_ptr_ty),
                .payload = try block.sema.addExtra(Air.Bin{
                    .lhs = slice,
                    .rhs = elem_index,
                }),
            } },
        });
    }

    fn addPtrElemPtr(
        block: *Block,
        array_ptr: Air.Inst.Ref,
        elem_index: Air.Inst.Ref,
        elem_ptr_ty: Type,
    ) !Air.Inst.Ref {
        const ty_ref = try block.sema.addType(elem_ptr_ty);
        return block.addPtrElemPtrTypeRef(array_ptr, elem_index, ty_ref);
    }

    fn addPtrElemPtrTypeRef(
        block: *Block,
        array_ptr: Air.Inst.Ref,
        elem_index: Air.Inst.Ref,
        elem_ptr_ty: Air.Inst.Ref,
    ) !Air.Inst.Ref {
        return block.addInst(.{
            .tag = .ptr_elem_ptr,
            .data = .{ .ty_pl = .{
                .ty = elem_ptr_ty,
                .payload = try block.sema.addExtra(Air.Bin{
                    .lhs = array_ptr,
                    .rhs = elem_index,
                }),
            } },
        });
    }

    fn addCmpVector(block: *Block, lhs: Air.Inst.Ref, rhs: Air.Inst.Ref, cmp_op: std.math.CompareOperator) !Air.Inst.Ref {
        return block.addInst(.{
            .tag = if (block.float_mode == .Optimized) .cmp_vector_optimized else .cmp_vector,
            .data = .{ .ty_pl = .{
                .ty = try block.sema.addType(
                    try Type.vector(block.sema.arena, block.sema.typeOf(lhs).vectorLen(), Type.bool),
                ),
                .payload = try block.sema.addExtra(Air.VectorCmp{
                    .lhs = lhs,
                    .rhs = rhs,
                    .op = Air.VectorCmp.encodeOp(cmp_op),
                }),
            } },
        });
    }

    fn addAggregateInit(
        block: *Block,
        aggregate_ty: Type,
        elements: []const Air.Inst.Ref,
    ) !Air.Inst.Ref {
        const sema = block.sema;
        const ty_ref = try sema.addType(aggregate_ty);
        try sema.air_extra.ensureUnusedCapacity(sema.gpa, elements.len);
        const extra_index = @intCast(u32, sema.air_extra.items.len);
        sema.appendRefsAssumeCapacity(elements);

        return block.addInst(.{
            .tag = .aggregate_init,
            .data = .{ .ty_pl = .{
                .ty = ty_ref,
                .payload = extra_index,
            } },
        });
    }

    fn addUnionInit(
        block: *Block,
        union_ty: Type,
        field_index: u32,
        init: Air.Inst.Ref,
    ) !Air.Inst.Ref {
        return block.addInst(.{
            .tag = .union_init,
            .data = .{ .ty_pl = .{
                .ty = try block.sema.addType(union_ty),
                .payload = try block.sema.addExtra(Air.UnionInit{
                    .field_index = field_index,
                    .init = init,
                }),
            } },
        });
    }

    pub fn addInst(block: *Block, inst: Air.Inst) error{OutOfMemory}!Air.Inst.Ref {
        return Air.indexToRef(try block.addInstAsIndex(inst));
    }

    pub fn addInstAsIndex(block: *Block, inst: Air.Inst) error{OutOfMemory}!Air.Inst.Index {
        const sema = block.sema;
        const gpa = sema.gpa;

        try sema.air_instructions.ensureUnusedCapacity(gpa, 1);
        try block.instructions.ensureUnusedCapacity(gpa, 1);

        const result_index = @intCast(Air.Inst.Index, sema.air_instructions.len);
        sema.air_instructions.appendAssumeCapacity(inst);
        block.instructions.appendAssumeCapacity(result_index);
        return result_index;
    }

    /// Insert an instruction into the block at `index`. Moves all following
    /// instructions forward in the block to make room. Operation is O(N).
    pub fn insertInst(block: *Block, index: Air.Inst.Index, inst: Air.Inst) error{OutOfMemory}!Air.Inst.Ref {
        return Air.indexToRef(try block.insertInstAsIndex(index, inst));
    }

    pub fn insertInstAsIndex(block: *Block, index: Air.Inst.Index, inst: Air.Inst) error{OutOfMemory}!Air.Inst.Index {
        const sema = block.sema;
        const gpa = sema.gpa;

        try sema.air_instructions.ensureUnusedCapacity(gpa, 1);

        const result_index = @intCast(Air.Inst.Index, sema.air_instructions.len);
        sema.air_instructions.appendAssumeCapacity(inst);

        try block.instructions.insert(gpa, index, result_index);
        return result_index;
    }

    fn addUnreachable(block: *Block, safety_check: bool) !void {
        if (safety_check and block.wantSafety()) {
            try block.sema.safetyPanic(block, .unreach);
        } else {
            _ = try block.addNoOp(.unreach);
        }
    }

    pub fn startAnonDecl(block: *Block) !WipAnonDecl {
        return WipAnonDecl{
            .block = block,
            .new_decl_arena = std.heap.ArenaAllocator.init(block.sema.gpa),
            .finished = false,
        };
    }

    pub const WipAnonDecl = struct {
        block: *Block,
        new_decl_arena: std.heap.ArenaAllocator,
        finished: bool,

        pub fn arena(wad: *WipAnonDecl) Allocator {
            return wad.new_decl_arena.allocator();
        }

        pub fn deinit(wad: *WipAnonDecl) void {
            if (!wad.finished) {
                wad.new_decl_arena.deinit();
            }
            wad.* = undefined;
        }

        /// `alignment` value of 0 means to use ABI alignment.
        pub fn finish(wad: *WipAnonDecl, ty: Type, val: Value, alignment: u32) !Decl.Index {
            const sema = wad.block.sema;
            // Do this ahead of time because `createAnonymousDecl` depends on calling
            // `type.hasRuntimeBits()`.
            _ = try sema.typeHasRuntimeBits(ty);
            const new_decl_index = try sema.mod.createAnonymousDecl(wad.block, .{
                .ty = ty,
                .val = val,
            });
            const new_decl = sema.mod.declPtr(new_decl_index);
            new_decl.@"align" = alignment;
            errdefer sema.mod.abortAnonDecl(new_decl_index);
            try new_decl.finalizeNewArena(&wad.new_decl_arena);
            wad.finished = true;
            return new_decl_index;
        }
    };
};

const LabeledBlock = struct {
    block: Block,
    label: Block.Label,

    fn destroy(lb: *LabeledBlock, gpa: Allocator) void {
        lb.block.instructions.deinit(gpa);
        lb.label.merges.results.deinit(gpa);
        lb.label.merges.br_list.deinit(gpa);
        gpa.destroy(lb);
    }
};

pub fn deinit(sema: *Sema) void {
    const gpa = sema.gpa;
    sema.air_instructions.deinit(gpa);
    sema.air_extra.deinit(gpa);
    sema.air_values.deinit(gpa);
    sema.inst_map.deinit(gpa);
    sema.decl_val_table.deinit(gpa);
    sema.types_to_resolve.deinit(gpa);
    {
        var it = sema.post_hoc_blocks.iterator();
        while (it.next()) |entry| {
            const labeled_block = entry.value_ptr.*;
            labeled_block.destroy(gpa);
        }
        sema.post_hoc_blocks.deinit(gpa);
    }
    sema.unresolved_inferred_allocs.deinit(gpa);
    sema.* = undefined;
}

/// Returns only the result from the body that is specified.
/// Only appropriate to call when it is determined at comptime that this body
/// has no peers.
fn resolveBody(
    sema: *Sema,
    block: *Block,
    body: []const Zir.Inst.Index,
    /// This is the instruction that a break instruction within `body` can
    /// use to return from the body.
    body_inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const break_data = (try sema.analyzeBodyBreak(block, body)) orelse
        return Air.Inst.Ref.unreachable_value;
    // For comptime control flow, we need to detect when `analyzeBody` reports
    // that we need to break from an outer block. In such case we
    // use Zig's error mechanism to send control flow up the stack until
    // we find the corresponding block to this break.
    if (block.is_comptime and break_data.block_inst != body_inst) {
        sema.comptime_break_inst = break_data.inst;
        return error.ComptimeBreak;
    }
    return try sema.resolveInst(break_data.operand);
}

fn analyzeBodyRuntimeBreak(sema: *Sema, block: *Block, body: []const Zir.Inst.Index) !void {
    _ = sema.analyzeBodyInner(block, body) catch |err| switch (err) {
        error.ComptimeBreak => {
            const zir_datas = sema.code.instructions.items(.data);
            const break_data = zir_datas[sema.comptime_break_inst].@"break";
            try sema.addRuntimeBreak(block, .{
                .block_inst = break_data.block_inst,
                .operand = break_data.operand,
                .inst = sema.comptime_break_inst,
            });
        },
        else => |e| return e,
    };
}

pub fn analyzeBody(
    sema: *Sema,
    block: *Block,
    body: []const Zir.Inst.Index,
) !void {
    _ = sema.analyzeBodyInner(block, body) catch |err| switch (err) {
        error.ComptimeBreak => unreachable, // unexpected comptime control flow
        else => |e| return e,
    };
}

const BreakData = struct {
    block_inst: Zir.Inst.Index,
    operand: Zir.Inst.Ref,
    inst: Zir.Inst.Index,
};

pub fn analyzeBodyBreak(
    sema: *Sema,
    block: *Block,
    body: []const Zir.Inst.Index,
) CompileError!?BreakData {
    const break_inst = sema.analyzeBodyInner(block, body) catch |err| switch (err) {
        error.ComptimeBreak => sema.comptime_break_inst,
        else => |e| return e,
    };
    if (block.instructions.items.len != 0 and
        sema.typeOf(Air.indexToRef(block.instructions.items[block.instructions.items.len - 1])).isNoReturn())
        return null;
    const break_data = sema.code.instructions.items(.data)[break_inst].@"break";
    return BreakData{
        .block_inst = break_data.block_inst,
        .operand = break_data.operand,
        .inst = break_inst,
    };
}

/// ZIR instructions which are always `noreturn` return this. This matches the
/// return type of `analyzeBody` so that we can tail call them.
/// Only appropriate to return when the instruction is known to be NoReturn
/// solely based on the ZIR tag.
const always_noreturn: CompileError!Zir.Inst.Index = @as(Zir.Inst.Index, undefined);

/// This function is the main loop of `Sema` and it can be used in two different ways:
/// * The traditional way where there are N breaks out of the block and peer type
///   resolution is done on the break operands. In this case, the `Zir.Inst.Index`
///   part of the return value will be `undefined`, and callsites should ignore it,
///   finding the block result value via the block scope.
/// * The "flat" way. There is only 1 break out of the block, and it is with a `break_inline`
///   instruction. In this case, the `Zir.Inst.Index` part of the return value will be
///   the break instruction. This communicates both which block the break applies to, as
///   well as the operand. No block scope needs to be created for this strategy.
fn analyzeBodyInner(
    sema: *Sema,
    block: *Block,
    body: []const Zir.Inst.Index,
) CompileError!Zir.Inst.Index {
    // No tracy calls here, to avoid interfering with the tail call mechanism.

    try sema.inst_map.ensureSpaceForInstructions(sema.gpa, body);

    const parent_capture_scope = block.wip_capture_scope;

    var wip_captures = WipCaptureScope{
        .finalized = true,
        .scope = parent_capture_scope,
        .perm_arena = sema.perm_arena,
        .gpa = sema.gpa,
    };
    defer if (wip_captures.scope != parent_capture_scope) {
        wip_captures.deinit();
    };

    const map = &sema.inst_map;
    const tags = sema.code.instructions.items(.tag);
    const datas = sema.code.instructions.items(.data);

    var orig_captures: usize = parent_capture_scope.captures.count();

    var crash_info = crash_report.prepAnalyzeBody(sema, block, body);
    crash_info.push();
    defer crash_info.pop();

    var dbg_block_begins: u32 = 0;

    // We use a while(true) loop here to avoid a redundant way of breaking out of
    // the loop. The only way to break out of the loop is with a `noreturn`
    // instruction.
    var i: usize = 0;
    const result = while (true) {
        crash_info.setBodyIndex(i);
        const inst = body[i];
        std.log.scoped(.sema_zir).debug("sema ZIR {s} %{d}", .{
            sema.mod.declPtr(block.src_decl).src_namespace.file_scope.sub_file_path, inst,
        });
        const air_inst: Air.Inst.Ref = switch (tags[inst]) {
            // zig fmt: off
            .alloc                        => try sema.zirAlloc(block, inst),
            .alloc_inferred               => try sema.zirAllocInferred(block, inst, Type.initTag(.inferred_alloc_const)),
            .alloc_inferred_mut           => try sema.zirAllocInferred(block, inst, Type.initTag(.inferred_alloc_mut)),
            .alloc_inferred_comptime      => try sema.zirAllocInferredComptime(inst, Type.initTag(.inferred_alloc_const)),
            .alloc_inferred_comptime_mut  => try sema.zirAllocInferredComptime(inst, Type.initTag(.inferred_alloc_mut)),
            .alloc_mut                    => try sema.zirAllocMut(block, inst),
            .alloc_comptime_mut           => try sema.zirAllocComptime(block, inst),
            .make_ptr_const               => try sema.zirMakePtrConst(block, inst),
            .anyframe_type                => try sema.zirAnyframeType(block, inst),
            .array_cat                    => try sema.zirArrayCat(block, inst),
            .array_mul                    => try sema.zirArrayMul(block, inst),
            .array_type                   => try sema.zirArrayType(block, inst),
            .array_type_sentinel          => try sema.zirArrayTypeSentinel(block, inst),
            .vector_type                  => try sema.zirVectorType(block, inst),
            .as                           => try sema.zirAs(block, inst),
            .as_node                      => try sema.zirAsNode(block, inst),
            .as_shift_operand             => try sema.zirAsShiftOperand(block, inst),
            .bit_and                      => try sema.zirBitwise(block, inst, .bit_and),
            .bit_not                      => try sema.zirBitNot(block, inst),
            .bit_or                       => try sema.zirBitwise(block, inst, .bit_or),
            .bitcast                      => try sema.zirBitcast(block, inst),
            .suspend_block                => try sema.zirSuspendBlock(block, inst),
            .bool_not                     => try sema.zirBoolNot(block, inst),
            .bool_br_and                  => try sema.zirBoolBr(block, inst, false),
            .bool_br_or                   => try sema.zirBoolBr(block, inst, true),
            .c_import                     => try sema.zirCImport(block, inst),
            .call                         => try sema.zirCall(block, inst),
            .closure_get                  => try sema.zirClosureGet(block, inst),
            .cmp_lt                       => try sema.zirCmp(block, inst, .lt),
            .cmp_lte                      => try sema.zirCmp(block, inst, .lte),
            .cmp_eq                       => try sema.zirCmpEq(block, inst, .eq, Air.Inst.Tag.fromCmpOp(.eq, block.float_mode == .Optimized)),
            .cmp_gte                      => try sema.zirCmp(block, inst, .gte),
            .cmp_gt                       => try sema.zirCmp(block, inst, .gt),
            .cmp_neq                      => try sema.zirCmpEq(block, inst, .neq, Air.Inst.Tag.fromCmpOp(.neq, block.float_mode == .Optimized)),
            .coerce_result_ptr            => try sema.zirCoerceResultPtr(block, inst),
            .decl_ref                     => try sema.zirDeclRef(block, inst),
            .decl_val                     => try sema.zirDeclVal(block, inst),
            .load                         => try sema.zirLoad(block, inst),
            .elem_ptr                     => try sema.zirElemPtr(block, inst),
            .elem_ptr_node                => try sema.zirElemPtrNode(block, inst),
            .elem_ptr_imm                 => try sema.zirElemPtrImm(block, inst),
            .elem_val                     => try sema.zirElemVal(block, inst),
            .elem_val_node                => try sema.zirElemValNode(block, inst),
            .elem_type_index              => try sema.zirElemTypeIndex(block, inst),
            .enum_literal                 => try sema.zirEnumLiteral(block, inst),
            .enum_to_int                  => try sema.zirEnumToInt(block, inst),
            .int_to_enum                  => try sema.zirIntToEnum(block, inst),
            .err_union_code               => try sema.zirErrUnionCode(block, inst),
            .err_union_code_ptr           => try sema.zirErrUnionCodePtr(block, inst),
            .err_union_payload_unsafe     => try sema.zirErrUnionPayload(block, inst),
            .err_union_payload_unsafe_ptr => try sema.zirErrUnionPayloadPtr(block, inst),
            .error_union_type             => try sema.zirErrorUnionType(block, inst),
            .error_value                  => try sema.zirErrorValue(block, inst),
            .field_ptr                    => try sema.zirFieldPtr(block, inst, false),
            .field_ptr_init               => try sema.zirFieldPtr(block, inst, true),
            .field_ptr_named              => try sema.zirFieldPtrNamed(block, inst),
            .field_val                    => try sema.zirFieldVal(block, inst),
            .field_val_named              => try sema.zirFieldValNamed(block, inst),
            .field_call_bind              => try sema.zirFieldCallBind(block, inst),
            .func                         => try sema.zirFunc(block, inst, false),
            .func_inferred                => try sema.zirFunc(block, inst, true),
            .func_fancy                   => try sema.zirFuncFancy(block, inst),
            .import                       => try sema.zirImport(block, inst),
            .indexable_ptr_len            => try sema.zirIndexablePtrLen(block, inst),
            .int                          => try sema.zirInt(block, inst),
            .int_big                      => try sema.zirIntBig(block, inst),
            .float                        => try sema.zirFloat(block, inst),
            .float128                     => try sema.zirFloat128(block, inst),
            .int_type                     => try sema.zirIntType(block, inst),
            .is_non_err                   => try sema.zirIsNonErr(block, inst),
            .is_non_err_ptr               => try sema.zirIsNonErrPtr(block, inst),
            .ret_is_non_err               => try sema.zirRetIsNonErr(block, inst),
            .is_non_null                  => try sema.zirIsNonNull(block, inst),
            .is_non_null_ptr              => try sema.zirIsNonNullPtr(block, inst),
            .merge_error_sets             => try sema.zirMergeErrorSets(block, inst),
            .negate                       => try sema.zirNegate(block, inst),
            .negate_wrap                  => try sema.zirNegateWrap(block, inst),
            .optional_payload_safe        => try sema.zirOptionalPayload(block, inst, true),
            .optional_payload_safe_ptr    => try sema.zirOptionalPayloadPtr(block, inst, true),
            .optional_payload_unsafe      => try sema.zirOptionalPayload(block, inst, false),
            .optional_payload_unsafe_ptr  => try sema.zirOptionalPayloadPtr(block, inst, false),
            .optional_type                => try sema.zirOptionalType(block, inst),
            .ptr_type                     => try sema.zirPtrType(block, inst),
            .ref                          => try sema.zirRef(block, inst),
            .ret_err_value_code           => try sema.zirRetErrValueCode(inst),
            .shr                          => try sema.zirShr(block, inst, .shr),
            .shr_exact                    => try sema.zirShr(block, inst, .shr_exact),
            .slice_end                    => try sema.zirSliceEnd(block, inst),
            .slice_sentinel               => try sema.zirSliceSentinel(block, inst),
            .slice_start                  => try sema.zirSliceStart(block, inst),
            .str                          => try sema.zirStr(block, inst),
            .switch_block                 => try sema.zirSwitchBlock(block, inst),
            .switch_cond                  => try sema.zirSwitchCond(block, inst, false),
            .switch_cond_ref              => try sema.zirSwitchCond(block, inst, true),
            .switch_capture               => try sema.zirSwitchCapture(block, inst, false, false),
            .switch_capture_ref           => try sema.zirSwitchCapture(block, inst, false, true),
            .switch_capture_multi         => try sema.zirSwitchCapture(block, inst, true, false),
            .switch_capture_multi_ref     => try sema.zirSwitchCapture(block, inst, true, true),
            .switch_capture_tag           => try sema.zirSwitchCaptureTag(block, inst),
            .type_info                    => try sema.zirTypeInfo(block, inst),
            .size_of                      => try sema.zirSizeOf(block, inst),
            .bit_size_of                  => try sema.zirBitSizeOf(block, inst),
            .typeof                       => try sema.zirTypeof(block, inst),
            .typeof_builtin               => try sema.zirTypeofBuiltin(block, inst),
            .typeof_log2_int_type         => try sema.zirTypeofLog2IntType(block, inst),
            .xor                          => try sema.zirBitwise(block, inst, .xor),
            .struct_init_empty            => try sema.zirStructInitEmpty(block, inst),
            .struct_init                  => try sema.zirStructInit(block, inst, false),
            .struct_init_ref              => try sema.zirStructInit(block, inst, true),
            .struct_init_anon             => try sema.zirStructInitAnon(block, inst, false),
            .struct_init_anon_ref         => try sema.zirStructInitAnon(block, inst, true),
            .array_init                   => try sema.zirArrayInit(block, inst, false),
            .array_init_ref               => try sema.zirArrayInit(block, inst, true),
            .array_init_anon              => try sema.zirArrayInitAnon(block, inst, false),
            .array_init_anon_ref          => try sema.zirArrayInitAnon(block, inst, true),
            .union_init                   => try sema.zirUnionInit(block, inst),
            .field_type                   => try sema.zirFieldType(block, inst),
            .field_type_ref               => try sema.zirFieldTypeRef(block, inst),
            .ptr_to_int                   => try sema.zirPtrToInt(block, inst),
            .align_of                     => try sema.zirAlignOf(block, inst),
            .bool_to_int                  => try sema.zirBoolToInt(block, inst),
            .embed_file                   => try sema.zirEmbedFile(block, inst),
            .error_name                   => try sema.zirErrorName(block, inst),
            .tag_name                     => try sema.zirTagName(block, inst),
            .type_name                    => try sema.zirTypeName(block, inst),
            .frame_type                   => try sema.zirFrameType(block, inst),
            .frame_size                   => try sema.zirFrameSize(block, inst),
            .float_to_int                 => try sema.zirFloatToInt(block, inst),
            .int_to_float                 => try sema.zirIntToFloat(block, inst),
            .int_to_ptr                   => try sema.zirIntToPtr(block, inst),
            .float_cast                   => try sema.zirFloatCast(block, inst),
            .int_cast                     => try sema.zirIntCast(block, inst),
            .ptr_cast                     => try sema.zirPtrCast(block, inst),
            .truncate                     => try sema.zirTruncate(block, inst),
            .align_cast                   => try sema.zirAlignCast(block, inst),
            .has_decl                     => try sema.zirHasDecl(block, inst),
            .has_field                    => try sema.zirHasField(block, inst),
            .byte_swap                    => try sema.zirByteSwap(block, inst),
            .bit_reverse                  => try sema.zirBitReverse(block, inst),
            .bit_offset_of                => try sema.zirBitOffsetOf(block, inst),
            .offset_of                    => try sema.zirOffsetOf(block, inst),
            .splat                        => try sema.zirSplat(block, inst),
            .reduce                       => try sema.zirReduce(block, inst),
            .shuffle                      => try sema.zirShuffle(block, inst),
            .atomic_load                  => try sema.zirAtomicLoad(block, inst),
            .atomic_rmw                   => try sema.zirAtomicRmw(block, inst),
            .mul_add                      => try sema.zirMulAdd(block, inst),
            .builtin_call                 => try sema.zirBuiltinCall(block, inst),
            .field_parent_ptr             => try sema.zirFieldParentPtr(block, inst),
            .@"resume"                    => try sema.zirResume(block, inst),
            .@"await"                     => try sema.zirAwait(block, inst),
            .array_base_ptr               => try sema.zirArrayBasePtr(block, inst),
            .field_base_ptr               => try sema.zirFieldBasePtr(block, inst),
            .for_len                      => try sema.zirForLen(block, inst),

            .clz       => try sema.zirBitCount(block, inst, .clz,      Value.clz),
            .ctz       => try sema.zirBitCount(block, inst, .ctz,      Value.ctz),
            .pop_count => try sema.zirBitCount(block, inst, .popcount, Value.popCount),

            .sqrt  => try sema.zirUnaryMath(block, inst, .sqrt, Value.sqrt),
            .sin   => try sema.zirUnaryMath(block, inst, .sin, Value.sin),
            .cos   => try sema.zirUnaryMath(block, inst, .cos, Value.cos),
            .tan   => try sema.zirUnaryMath(block, inst, .tan, Value.tan),
            .exp   => try sema.zirUnaryMath(block, inst, .exp, Value.exp),
            .exp2  => try sema.zirUnaryMath(block, inst, .exp2, Value.exp2),
            .log   => try sema.zirUnaryMath(block, inst, .log, Value.log),
            .log2  => try sema.zirUnaryMath(block, inst, .log2, Value.log2),
            .log10 => try sema.zirUnaryMath(block, inst, .log10, Value.log10),
            .fabs  => try sema.zirUnaryMath(block, inst, .fabs, Value.fabs),
            .floor => try sema.zirUnaryMath(block, inst, .floor, Value.floor),
            .ceil  => try sema.zirUnaryMath(block, inst, .ceil, Value.ceil),
            .round => try sema.zirUnaryMath(block, inst, .round, Value.round),
            .trunc => try sema.zirUnaryMath(block, inst, .trunc_float, Value.trunc),

            .error_set_decl      => try sema.zirErrorSetDecl(block, inst, .parent),
            .error_set_decl_anon => try sema.zirErrorSetDecl(block, inst, .anon),
            .error_set_decl_func => try sema.zirErrorSetDecl(block, inst, .func),

            .add       => try sema.zirArithmetic(block, inst, .add,        true),
            .addwrap   => try sema.zirArithmetic(block, inst, .addwrap,    true),
            .add_sat   => try sema.zirArithmetic(block, inst, .add_sat,    true),
            .add_unsafe=> try sema.zirArithmetic(block, inst, .add_unsafe, false),
            .mul       => try sema.zirArithmetic(block, inst, .mul,        true),
            .mulwrap   => try sema.zirArithmetic(block, inst, .mulwrap,    true),
            .mul_sat   => try sema.zirArithmetic(block, inst, .mul_sat,    true),
            .sub       => try sema.zirArithmetic(block, inst, .sub,        true),
            .subwrap   => try sema.zirArithmetic(block, inst, .subwrap,    true),
            .sub_sat   => try sema.zirArithmetic(block, inst, .sub_sat,    true),

            .div       => try sema.zirDiv(block, inst),
            .div_exact => try sema.zirDivExact(block, inst),
            .div_floor => try sema.zirDivFloor(block, inst),
            .div_trunc => try sema.zirDivTrunc(block, inst),

            .mod_rem   => try sema.zirModRem(block, inst),
            .mod       => try sema.zirMod(block, inst),
            .rem       => try sema.zirRem(block, inst),

            .max => try sema.zirMinMax(block, inst, .max),
            .min => try sema.zirMinMax(block, inst, .min),

            .shl       => try sema.zirShl(block, inst, .shl),
            .shl_exact => try sema.zirShl(block, inst, .shl_exact),
            .shl_sat   => try sema.zirShl(block, inst, .shl_sat),

            .ret_ptr  => try sema.zirRetPtr(block),
            .ret_type => try sema.addType(sema.fn_ret_ty),

            // Instructions that we know to *always* be noreturn based solely on their tag.
            // These functions match the return type of analyzeBody so that we can
            // tail call them here.
            .compile_error  => break sema.zirCompileError(block, inst),
            .ret_implicit   => break sema.zirRetImplicit(block, inst),
            .ret_node       => break sema.zirRetNode(block, inst),
            .ret_load       => break sema.zirRetLoad(block, inst),
            .ret_err_value  => break sema.zirRetErrValue(block, inst),
            .@"unreachable" => break sema.zirUnreachable(block, inst),
            .panic          => break sema.zirPanic(block, inst, false),
            .panic_comptime => break sema.zirPanic(block, inst, true),
            .trap           => break sema.zirTrap(block, inst),
            // zig fmt: on

            .extended => ext: {
                const extended = datas[inst].extended;
                break :ext switch (extended.opcode) {
                    // zig fmt: off
                    .variable              => try sema.zirVarExtended(       block, extended),
                    .struct_decl           => try sema.zirStructDecl(        block, extended, inst),
                    .enum_decl             => try sema.zirEnumDecl(          block, extended, inst),
                    .union_decl            => try sema.zirUnionDecl(         block, extended, inst),
                    .opaque_decl           => try sema.zirOpaqueDecl(        block, extended, inst),
                    .this                  => try sema.zirThis(              block, extended),
                    .ret_addr              => try sema.zirRetAddr(           block, extended),
                    .builtin_src           => try sema.zirBuiltinSrc(        block, extended),
                    .error_return_trace    => try sema.zirErrorReturnTrace(  block),
                    .frame                 => try sema.zirFrame(             block, extended),
                    .frame_address         => try sema.zirFrameAddress(      block, extended),
                    .alloc                 => try sema.zirAllocExtended(     block, extended),
                    .builtin_extern        => try sema.zirBuiltinExtern(     block, extended),
                    .@"asm"                => try sema.zirAsm(               block, extended, false),
                    .asm_expr              => try sema.zirAsm(               block, extended, true),
                    .typeof_peer           => try sema.zirTypeofPeer(        block, extended),
                    .compile_log           => try sema.zirCompileLog(               extended),
                    .add_with_overflow     => try sema.zirOverflowArithmetic(block, extended, extended.opcode),
                    .sub_with_overflow     => try sema.zirOverflowArithmetic(block, extended, extended.opcode),
                    .mul_with_overflow     => try sema.zirOverflowArithmetic(block, extended, extended.opcode),
                    .shl_with_overflow     => try sema.zirOverflowArithmetic(block, extended, extended.opcode),
                    .c_undef               => try sema.zirCUndef(            block, extended),
                    .c_include             => try sema.zirCInclude(          block, extended),
                    .c_define              => try sema.zirCDefine(           block, extended),
                    .wasm_memory_size      => try sema.zirWasmMemorySize(    block, extended),
                    .wasm_memory_grow      => try sema.zirWasmMemoryGrow(    block, extended),
                    .prefetch              => try sema.zirPrefetch(          block, extended),
                    .field_call_bind_named => try sema.zirFieldCallBindNamed(block, extended),
                    .err_set_cast          => try sema.zirErrSetCast(        block, extended),
                    .await_nosuspend       => try sema.zirAwaitNosuspend(    block, extended),
                    .select                => try sema.zirSelect(            block, extended),
                    .error_to_int          => try sema.zirErrorToInt(        block, extended),
                    .int_to_error          => try sema.zirIntToError(        block, extended),
                    .reify                 => try sema.zirReify(             block, extended, inst),
                    .builtin_async_call    => try sema.zirBuiltinAsyncCall(  block, extended),
                    .cmpxchg               => try sema.zirCmpxchg(           block, extended),
                    .addrspace_cast        => try sema.zirAddrSpaceCast(     block, extended),
                    .c_va_arg              => try sema.zirCVaArg(            block, extended),
                    .c_va_copy             => try sema.zirCVaCopy(           block, extended),
                    .c_va_end              => try sema.zirCVaEnd(            block, extended),
                    .c_va_start            => try sema.zirCVaStart(          block, extended),
                    .const_cast,           => try sema.zirConstCast(         block, extended),
                    .volatile_cast,        => try sema.zirVolatileCast(      block, extended),
                    // zig fmt: on

                    .fence => {
                        try sema.zirFence(block, extended);
                        i += 1;
                        continue;
                    },
                    .set_float_mode => {
                        try sema.zirSetFloatMode(block, extended);
                        i += 1;
                        continue;
                    },
                    .set_align_stack => {
                        try sema.zirSetAlignStack(block, extended);
                        i += 1;
                        continue;
                    },
                    .set_cold => {
                        try sema.zirSetCold(block, extended);
                        i += 1;
                        continue;
                    },
                    .breakpoint => {
                        if (!block.is_comptime) {
                            _ = try block.addNoOp(.breakpoint);
                        }
                        i += 1;
                        continue;
                    },
                };
            },

            // Instructions that we know can *never* be noreturn based solely on
            // their tag. We avoid needlessly checking if they are noreturn and
            // continue the loop.
            // We also know that they cannot be referenced later, so we avoid
            // putting them into the map.
            .dbg_stmt => {
                try sema.zirDbgStmt(block, inst);
                i += 1;
                continue;
            },
            .dbg_var_ptr => {
                try sema.zirDbgVar(block, inst, .dbg_var_ptr);
                i += 1;
                continue;
            },
            .dbg_var_val => {
                try sema.zirDbgVar(block, inst, .dbg_var_val);
                i += 1;
                continue;
            },
            .dbg_block_begin => {
                dbg_block_begins += 1;
                try sema.zirDbgBlockBegin(block);
                i += 1;
                continue;
            },
            .dbg_block_end => {
                dbg_block_begins -= 1;
                try sema.zirDbgBlockEnd(block);
                i += 1;
                continue;
            },
            .ensure_err_union_payload_void => {
                try sema.zirEnsureErrUnionPayloadVoid(block, inst);
                i += 1;
                continue;
            },
            .ensure_result_non_error => {
                try sema.zirEnsureResultNonError(block, inst);
                i += 1;
                continue;
            },
            .ensure_result_used => {
                try sema.zirEnsureResultUsed(block, inst);
                i += 1;
                continue;
            },
            .set_eval_branch_quota => {
                try sema.zirSetEvalBranchQuota(block, inst);
                i += 1;
                continue;
            },
            .atomic_store => {
                try sema.zirAtomicStore(block, inst);
                i += 1;
                continue;
            },
            .store => {
                try sema.zirStore(block, inst);
                i += 1;
                continue;
            },
            .store_node => {
                try sema.zirStoreNode(block, inst);
                i += 1;
                continue;
            },
            .store_to_block_ptr => {
                try sema.zirStoreToBlockPtr(block, inst);
                i += 1;
                continue;
            },
            .store_to_inferred_ptr => {
                try sema.zirStoreToInferredPtr(block, inst);
                i += 1;
                continue;
            },
            .resolve_inferred_alloc => {
                try sema.zirResolveInferredAlloc(block, inst);
                i += 1;
                continue;
            },
            .validate_array_init_ty => {
                try sema.validateArrayInitTy(block, inst);
                i += 1;
                continue;
            },
            .validate_struct_init_ty => {
                try sema.validateStructInitTy(block, inst);
                i += 1;
                continue;
            },
            .validate_struct_init => {
                try sema.zirValidateStructInit(block, inst, false);
                i += 1;
                continue;
            },
            .validate_struct_init_comptime => {
                try sema.zirValidateStructInit(block, inst, true);
                i += 1;
                continue;
            },
            .validate_array_init => {
                try sema.zirValidateArrayInit(block, inst, false);
                i += 1;
                continue;
            },
            .validate_array_init_comptime => {
                try sema.zirValidateArrayInit(block, inst, true);
                i += 1;
                continue;
            },
            .validate_deref => {
                try sema.zirValidateDeref(block, inst);
                i += 1;
                continue;
            },
            .@"export" => {
                try sema.zirExport(block, inst);
                i += 1;
                continue;
            },
            .export_value => {
                try sema.zirExportValue(block, inst);
                i += 1;
                continue;
            },
            .set_runtime_safety => {
                try sema.zirSetRuntimeSafety(block, inst);
                i += 1;
                continue;
            },
            .param => {
                try sema.zirParam(block, inst, false);
                i += 1;
                continue;
            },
            .param_comptime => {
                try sema.zirParam(block, inst, true);
                i += 1;
                continue;
            },
            .param_anytype => {
                try sema.zirParamAnytype(block, inst, false);
                i += 1;
                continue;
            },
            .param_anytype_comptime => {
                try sema.zirParamAnytype(block, inst, true);
                i += 1;
                continue;
            },
            .closure_capture => {
                try sema.zirClosureCapture(block, inst);
                i += 1;
                continue;
            },
            .memcpy => {
                try sema.zirMemcpy(block, inst);
                i += 1;
                continue;
            },
            .memset => {
                try sema.zirMemset(block, inst);
                i += 1;
                continue;
            },
            .check_comptime_control_flow => {
                if (!block.is_comptime) {
                    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
                    const src = inst_data.src();
                    const inline_block = Zir.refToIndex(inst_data.operand).?;

                    var check_block = block;
                    const target_runtime_index = while (true) {
                        if (check_block.inline_block == inline_block) {
                            break check_block.runtime_index;
                        }
                        check_block = check_block.parent.?;
                    };

                    if (@enumToInt(target_runtime_index) < @enumToInt(block.runtime_index)) {
                        const runtime_src = block.runtime_cond orelse block.runtime_loop.?;
                        const msg = msg: {
                            const msg = try sema.errMsg(block, src, "comptime control flow inside runtime block", .{});
                            errdefer msg.destroy(sema.gpa);

                            try sema.errNote(block, runtime_src, msg, "runtime control flow here", .{});
                            break :msg msg;
                        };
                        return sema.failWithOwnedErrorMsg(msg);
                    }
                }
                i += 1;
                continue;
            },
            .save_err_ret_index => {
                try sema.zirSaveErrRetIndex(block, inst);
                i += 1;
                continue;
            },
            .restore_err_ret_index => {
                try sema.zirRestoreErrRetIndex(block, inst);
                i += 1;
                continue;
            },

            // Special case instructions to handle comptime control flow.
            .@"break" => {
                if (block.is_comptime) {
                    break inst; // same as break_inline
                } else {
                    break sema.zirBreak(block, inst);
                }
            },
            .break_inline => {
                if (block.is_comptime) {
                    break inst;
                } else {
                    sema.comptime_break_inst = inst;
                    return error.ComptimeBreak;
                }
            },
            .repeat => {
                if (block.is_comptime) {
                    // Send comptime control flow back to the beginning of this block.
                    const src = LazySrcLoc.nodeOffset(datas[inst].node);
                    try sema.emitBackwardBranch(block, src);
                    if (wip_captures.scope.captures.count() != orig_captures) {
                        try wip_captures.reset(parent_capture_scope);
                        block.wip_capture_scope = wip_captures.scope;
                        orig_captures = 0;
                    }
                    i = 0;
                    continue;
                } else {
                    break always_noreturn;
                }
            },
            .repeat_inline => {
                // Send comptime control flow back to the beginning of this block.
                const src = LazySrcLoc.nodeOffset(datas[inst].node);
                try sema.emitBackwardBranch(block, src);
                if (wip_captures.scope.captures.count() != orig_captures) {
                    try wip_captures.reset(parent_capture_scope);
                    block.wip_capture_scope = wip_captures.scope;
                    orig_captures = 0;
                }
                i = 0;
                continue;
            },
            .loop => blk: {
                if (!block.is_comptime) break :blk try sema.zirLoop(block, inst);
                // Same as `block_inline`. TODO https://github.com/ziglang/zig/issues/8220
                const inst_data = datas[inst].pl_node;
                const extra = sema.code.extraData(Zir.Inst.Block, inst_data.payload_index);
                const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            .block => blk: {
                if (!block.is_comptime) break :blk try sema.zirBlock(block, inst);
                // Same as `block_inline`. TODO https://github.com/ziglang/zig/issues/8220
                const inst_data = datas[inst].pl_node;
                const extra = sema.code.extraData(Zir.Inst.Block, inst_data.payload_index);
                const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
                // If this block contains a function prototype, we need to reset the
                // current list of parameters and restore it later.
                // Note: this probably needs to be resolved in a more general manner.
                const prev_params = block.params;
                block.params = .{};
                defer {
                    block.params.deinit(sema.gpa);
                    block.params = prev_params;
                }
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            .block_inline => blk: {
                // Directly analyze the block body without introducing a new block.
                // However, in the case of a corresponding break_inline which reaches
                // through a runtime conditional branch, we must retroactively emit
                // a block, so we remember the block index here just in case.
                const block_index = block.instructions.items.len;
                const inst_data = datas[inst].pl_node;
                const extra = sema.code.extraData(Zir.Inst.Block, inst_data.payload_index);
                const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
                const gpa = sema.gpa;

                const opt_break_data = b: {
                    // Create a temporary child block so that this inline block is properly
                    // labeled for any .restore_err_ret_index instructions
                    var child_block = block.makeSubBlock();

                    // If this block contains a function prototype, we need to reset the
                    // current list of parameters and restore it later.
                    // Note: this probably needs to be resolved in a more general manner.
                    child_block.inline_block =
                        if (tags[inline_body[inline_body.len - 1]] == .repeat_inline) inline_body[0] else inst;

                    var label: Block.Label = .{
                        .zir_block = inst,
                        .merges = undefined,
                    };
                    child_block.label = &label;
                    defer child_block.params.deinit(gpa);

                    // Write these instructions directly into the parent block
                    child_block.instructions = block.instructions;
                    defer block.instructions = child_block.instructions;

                    break :b try sema.analyzeBodyBreak(&child_block, inline_body);
                };

                // A runtime conditional branch that needs a post-hoc block to be
                // emitted communicates this by mapping the block index into the inst map.
                if (map.get(inst)) |new_block_ref| ph: {
                    // Comptime control flow populates the map, so we don't actually know
                    // if this is a post-hoc runtime block until we check the
                    // post_hoc_block map.
                    const new_block_inst = Air.refToIndex(new_block_ref) orelse break :ph;
                    const labeled_block = sema.post_hoc_blocks.get(new_block_inst) orelse
                        break :ph;

                    // In this case we need to move all the instructions starting at
                    // block_index from the current block into this new one.

                    if (opt_break_data) |break_data| {
                        // This is a comptime break which we now change to a runtime break
                        // since it crosses a runtime branch.
                        // It may pass through our currently being analyzed block_inline or it
                        // may point directly to it. In the latter case, this modifies the
                        // block that we are about to look up in the post_hoc_blocks map below.
                        try sema.addRuntimeBreak(block, break_data);
                    } else {
                        // Here the comptime control flow ends with noreturn; however
                        // we have runtime control flow continuing after this block.
                        // This branch is therefore handled by the `i += 1; continue;`
                        // logic below.
                    }

                    try labeled_block.block.instructions.appendSlice(gpa, block.instructions.items[block_index..]);
                    block.instructions.items.len = block_index;

                    const block_result = try sema.analyzeBlockBody(block, inst_data.src(), &labeled_block.block, &labeled_block.label.merges);
                    {
                        // Destroy the ad-hoc block entry so that it does not interfere with
                        // the next iteration of comptime control flow, if any.
                        labeled_block.destroy(gpa);
                        assert(sema.post_hoc_blocks.remove(new_block_inst));
                    }
                    map.putAssumeCapacity(inst, block_result);
                    i += 1;
                    continue;
                }

                const break_data = opt_break_data orelse break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            .condbr => blk: {
                if (!block.is_comptime) break sema.zirCondbr(block, inst);
                // Same as condbr_inline. TODO https://github.com/ziglang/zig/issues/8220
                const inst_data = datas[inst].pl_node;
                const cond_src: LazySrcLoc = .{ .node_offset_if_cond = inst_data.src_node };
                const extra = sema.code.extraData(Zir.Inst.CondBr, inst_data.payload_index);
                const then_body = sema.code.extra[extra.end..][0..extra.data.then_body_len];
                const else_body = sema.code.extra[extra.end + then_body.len ..][0..extra.data.else_body_len];
                const cond = sema.resolveInstConst(block, cond_src, extra.data.condition, "condition in comptime branch must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and block.comptime_reason != null) try block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                const inline_body = if (cond.val.toBool()) then_body else else_body;

                try sema.maybeErrorUnwrapCondbr(block, inline_body, extra.data.condition, cond_src);
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            .condbr_inline => blk: {
                const inst_data = datas[inst].pl_node;
                const cond_src: LazySrcLoc = .{ .node_offset_if_cond = inst_data.src_node };
                const extra = sema.code.extraData(Zir.Inst.CondBr, inst_data.payload_index);
                const then_body = sema.code.extra[extra.end..][0..extra.data.then_body_len];
                const else_body = sema.code.extra[extra.end + then_body.len ..][0..extra.data.else_body_len];
                const cond = sema.resolveInstConst(block, cond_src, extra.data.condition, "condition in comptime branch must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and block.comptime_reason != null) try block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                const inline_body = if (cond.val.toBool()) then_body else else_body;

                try sema.maybeErrorUnwrapCondbr(block, inline_body, extra.data.condition, cond_src);
                const old_runtime_index = block.runtime_index;
                defer block.runtime_index = old_runtime_index;
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            .@"try" => blk: {
                if (!block.is_comptime) break :blk try sema.zirTry(block, inst);
                const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
                const src = inst_data.src();
                const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
                const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
                const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
                const err_union = try sema.resolveInst(extra.data.operand);
                const is_non_err = try sema.analyzeIsNonErrComptimeOnly(block, operand_src, err_union);
                assert(is_non_err != .none);
                const is_non_err_tv = sema.resolveInstConst(block, operand_src, is_non_err, "try operand inside comptime block must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and block.comptime_reason != null) try block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                if (is_non_err_tv.val.toBool()) {
                    const err_union_ty = sema.typeOf(err_union);
                    break :blk try sema.analyzeErrUnionPayload(block, src, err_union_ty, err_union, operand_src, false);
                }
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            //.try_inline => blk: {
            //    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
            //    const src = inst_data.src();
            //    const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
            //    const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
            //    const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
            //    const operand = try sema.resolveInst(extra.data.operand);
            //    const operand_ty = sema.typeOf(operand);
            //    const is_ptr = operand_ty.zigTypeTag() == .Pointer;
            //    const err_union = if (is_ptr)
            //        try sema.analyzeLoad(block, src, operand, operand_src)
            //    else
            //        operand;
            //    const is_non_err = try sema.analyzeIsNonErrComptimeOnly(block, operand_src, err_union);
            //    assert(is_non_err != .none);
            //    const is_non_err_tv = try sema.resolveInstConst(block, operand_src, is_non_err);
            //    if (is_non_err_tv.val.toBool()) {
            //        if (is_ptr) {
            //            break :blk try sema.analyzeErrUnionPayloadPtr(block, src, operand, false, false);
            //        } else {
            //            const err_union_ty = sema.typeOf(err_union);
            //            break :blk try sema.analyzeErrUnionPayload(block, src, err_union_ty, operand, operand_src, false);
            //        }
            //    }
            //    const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
            //        break always_noreturn;
            //    if (inst == break_data.block_inst) {
            //        break :blk try sema.resolveInst(break_data.operand);
            //    } else {
            //        break break_data.inst;
            //    }
            //},
            .try_ptr => blk: {
                if (!block.is_comptime) break :blk try sema.zirTryPtr(block, inst);
                const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
                const src = inst_data.src();
                const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
                const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
                const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
                const operand = try sema.resolveInst(extra.data.operand);
                const err_union = try sema.analyzeLoad(block, src, operand, operand_src);
                const is_non_err = try sema.analyzeIsNonErrComptimeOnly(block, operand_src, err_union);
                assert(is_non_err != .none);
                const is_non_err_tv = sema.resolveInstConst(block, operand_src, is_non_err, "try operand inside comptime block must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and block.comptime_reason != null) try block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                if (is_non_err_tv.val.toBool()) {
                    break :blk try sema.analyzeErrUnionPayloadPtr(block, src, operand, false, false);
                }
                const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
                    break always_noreturn;
                if (inst == break_data.block_inst) {
                    break :blk try sema.resolveInst(break_data.operand);
                } else {
                    break break_data.inst;
                }
            },
            //.try_ptr_inline => blk: {
            //    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
            //    const src = inst_data.src();
            //    const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
            //    const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
            //    const inline_body = sema.code.extra[extra.end..][0..extra.data.body_len];
            //    const operand = try sema.resolveInst(extra.data.operand);
            //    const err_union = try sema.analyzeLoad(block, src, operand, operand_src);
            //    const is_non_err = try sema.analyzeIsNonErrComptimeOnly(block, operand_src, err_union);
            //    assert(is_non_err != .none);
            //    const is_non_err_tv = try sema.resolveInstConst(block, operand_src, is_non_err);
            //    if (is_non_err_tv.val.toBool()) {
            //        break :blk try sema.analyzeErrUnionPayloadPtr(block, src, operand, false, false);
            //    }
            //    const break_data = (try sema.analyzeBodyBreak(block, inline_body)) orelse
            //        break always_noreturn;
            //    if (inst == break_data.block_inst) {
            //        break :blk try sema.resolveInst(break_data.operand);
            //    } else {
            //        break break_data.inst;
            //    }
            //},
            .@"defer" => blk: {
                const inst_data = sema.code.instructions.items(.data)[inst].@"defer";
                const defer_body = sema.code.extra[inst_data.index..][0..inst_data.len];
                const break_inst = sema.analyzeBodyInner(block, defer_body) catch |err| switch (err) {
                    error.ComptimeBreak => sema.comptime_break_inst,
                    else => |e| return e,
                };
                if (break_inst != defer_body[defer_body.len - 1]) break always_noreturn;
                break :blk Air.Inst.Ref.void_value;
            },
            .defer_err_code => blk: {
                const inst_data = sema.code.instructions.items(.data)[inst].defer_err_code;
                const extra = sema.code.extraData(Zir.Inst.DeferErrCode, inst_data.payload_index).data;
                const defer_body = sema.code.extra[extra.index..][0..extra.len];
                const err_code = try sema.resolveInst(inst_data.err_code);
                sema.inst_map.putAssumeCapacity(extra.remapped_err_code, err_code);
                const break_inst = sema.analyzeBodyInner(block, defer_body) catch |err| switch (err) {
                    error.ComptimeBreak => sema.comptime_break_inst,
                    else => |e| return e,
                };
                if (break_inst != defer_body[defer_body.len - 1]) break always_noreturn;
                break :blk Air.Inst.Ref.void_value;
            },
        };
        if (sema.typeOf(air_inst).isNoReturn())
            break always_noreturn;
        map.putAssumeCapacity(inst, air_inst);
        i += 1;
    };

    // balance out dbg_block_begins in case of early noreturn
    const noreturn_inst = block.instructions.popOrNull();
    while (dbg_block_begins > 0) {
        dbg_block_begins -= 1;
        if (block.is_comptime or sema.mod.comp.bin_file.options.strip) continue;

        _ = try block.addInst(.{
            .tag = .dbg_block_end,
            .data = undefined,
        });
    }
    if (noreturn_inst) |some| try block.instructions.append(sema.gpa, some);

    if (!wip_captures.finalized) {
        try wip_captures.finalize();
        block.wip_capture_scope = parent_capture_scope;
    }

    return result;
}

pub fn resolveInst(sema: *Sema, zir_ref: Zir.Inst.Ref) !Air.Inst.Ref {
    var i: usize = @enumToInt(zir_ref);

    // First section of indexes correspond to a set number of constant values.
    if (i < Zir.Inst.Ref.typed_value_map.len) {
        // We intentionally map the same indexes to the same values between ZIR and AIR.
        return zir_ref;
    }
    i -= Zir.Inst.Ref.typed_value_map.len;

    // Finally, the last section of indexes refers to the map of ZIR=>AIR.
    const inst = sema.inst_map.get(@intCast(u32, i)).?;
    const ty = sema.typeOf(inst);
    if (ty.tag() == .generic_poison) return error.GenericPoison;
    return inst;
}

fn resolveConstBool(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
    reason: []const u8,
) !bool {
    const air_inst = try sema.resolveInst(zir_ref);
    const wanted_type = Type.bool;
    const coerced_inst = try sema.coerce(block, wanted_type, air_inst, src);
    const val = try sema.resolveConstValue(block, src, coerced_inst, reason);
    return val.toBool();
}

pub fn resolveConstString(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
    reason: []const u8,
) ![]u8 {
    const air_inst = try sema.resolveInst(zir_ref);
    const wanted_type = Type.initTag(.const_slice_u8);
    const coerced_inst = try sema.coerce(block, wanted_type, air_inst, src);
    const val = try sema.resolveConstValue(block, src, coerced_inst, reason);
    return val.toAllocatedBytes(wanted_type, sema.arena, sema.mod);
}

pub fn resolveType(sema: *Sema, block: *Block, src: LazySrcLoc, zir_ref: Zir.Inst.Ref) !Type {
    const air_inst = try sema.resolveInst(zir_ref);
    const ty = try sema.analyzeAsType(block, src, air_inst);
    if (ty.tag() == .generic_poison) return error.GenericPoison;
    return ty;
}

fn analyzeAsType(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_inst: Air.Inst.Ref,
) !Type {
    const wanted_type = Type.initTag(.type);
    const coerced_inst = try sema.coerce(block, wanted_type, air_inst, src);
    const val = try sema.resolveConstValue(block, src, coerced_inst, "types must be comptime-known");
    var buffer: Value.ToTypeBuffer = undefined;
    const ty = val.toType(&buffer);
    return ty.copy(sema.arena);
}

pub fn setupErrorReturnTrace(sema: *Sema, block: *Block, last_arg_index: usize) !void {
    if (!sema.mod.backendSupportsFeature(.error_return_trace)) return;

    assert(!block.is_comptime);
    var err_trace_block = block.makeSubBlock();
    defer err_trace_block.instructions.deinit(sema.gpa);

    const src: LazySrcLoc = .unneeded;

    // var addrs: [err_return_trace_addr_count]usize = undefined;
    const err_return_trace_addr_count = 32;
    const addr_arr_ty = try Type.array(sema.arena, err_return_trace_addr_count, null, Type.usize, sema.mod);
    const addrs_ptr = try err_trace_block.addTy(.alloc, try Type.Tag.single_mut_pointer.create(sema.arena, addr_arr_ty));

    // var st: StackTrace = undefined;
    const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
    const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
    const st_ptr = try err_trace_block.addTy(.alloc, try Type.Tag.single_mut_pointer.create(sema.arena, stack_trace_ty));

    // st.instruction_addresses = &addrs;
    const addr_field_ptr = try sema.fieldPtr(&err_trace_block, src, st_ptr, "instruction_addresses", src, true);
    try sema.storePtr2(&err_trace_block, src, addr_field_ptr, src, addrs_ptr, src, .store);

    // st.index = 0;
    const index_field_ptr = try sema.fieldPtr(&err_trace_block, src, st_ptr, "index", src, true);
    try sema.storePtr2(&err_trace_block, src, index_field_ptr, src, .zero_usize, src, .store);

    // @errorReturnTrace() = &st;
    _ = try err_trace_block.addUnOp(.set_err_return_trace, st_ptr);

    try block.instructions.insertSlice(sema.gpa, last_arg_index, err_trace_block.instructions.items);
}

/// May return Value Tags: `variable`, `undef`.
/// See `resolveConstValue` for an alternative.
/// Value Tag `generic_poison` causes `error.GenericPoison` to be returned.
fn resolveValue(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_ref: Air.Inst.Ref,
    reason: []const u8,
) CompileError!Value {
    if (try sema.resolveMaybeUndefValAllowVariables(air_ref)) |val| {
        if (val.tag() == .generic_poison) return error.GenericPoison;
        return val;
    }
    return sema.failWithNeededComptime(block, src, reason);
}

/// Value Tag `variable` will cause a compile error.
/// Value Tag `undef` may be returned.
fn resolveConstMaybeUndefVal(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    inst: Air.Inst.Ref,
    reason: []const u8,
) CompileError!Value {
    if (try sema.resolveMaybeUndefValAllowVariables(inst)) |val| {
        switch (val.tag()) {
            .variable => return sema.failWithNeededComptime(block, src, reason),
            .generic_poison => return error.GenericPoison,
            else => return val,
        }
    }
    return sema.failWithNeededComptime(block, src, reason);
}

/// Will not return Value Tags: `variable`, `undef`. Instead they will emit compile errors.
/// See `resolveValue` for an alternative.
fn resolveConstValue(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_ref: Air.Inst.Ref,
    reason: []const u8,
) CompileError!Value {
    if (try sema.resolveMaybeUndefValAllowVariables(air_ref)) |val| {
        switch (val.tag()) {
            .undef => return sema.failWithUseOfUndef(block, src),
            .variable => return sema.failWithNeededComptime(block, src, reason),
            .generic_poison => return error.GenericPoison,
            else => return val,
        }
    }
    return sema.failWithNeededComptime(block, src, reason);
}

/// Value Tag `variable` causes this function to return `null`.
/// Value Tag `undef` causes this function to return a compile error.
fn resolveDefinedValue(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_ref: Air.Inst.Ref,
) CompileError!?Value {
    if (try sema.resolveMaybeUndefVal(air_ref)) |val| {
        if (val.isUndef()) {
            if (block.is_typeof) return null;
            return sema.failWithUseOfUndef(block, src);
        }
        return val;
    }
    return null;
}

/// Value Tag `variable` causes this function to return `null`.
/// Value Tag `undef` causes this function to return the Value.
/// Value Tag `generic_poison` causes `error.GenericPoison` to be returned.
fn resolveMaybeUndefVal(
    sema: *Sema,
    inst: Air.Inst.Ref,
) CompileError!?Value {
    const val = (try sema.resolveMaybeUndefValAllowVariables(inst)) orelse return null;
    switch (val.tag()) {
        .variable => return null,
        .generic_poison => return error.GenericPoison,
        else => return val,
    }
}

/// Value Tag `variable` results in `null`.
/// Value Tag `undef` results in the Value.
/// Value Tag `generic_poison` causes `error.GenericPoison` to be returned.
/// Value Tag `decl_ref` and `decl_ref_mut` or any nested such value results in `null`.
fn resolveMaybeUndefValIntable(
    sema: *Sema,
    inst: Air.Inst.Ref,
) CompileError!?Value {
    const val = (try sema.resolveMaybeUndefValAllowVariables(inst)) orelse return null;
    var check = val;
    while (true) switch (check.tag()) {
        .variable, .decl_ref, .decl_ref_mut, .comptime_field_ptr => return null,
        .field_ptr => check = check.castTag(.field_ptr).?.data.container_ptr,
        .elem_ptr => check = check.castTag(.elem_ptr).?.data.array_ptr,
        .eu_payload_ptr, .opt_payload_ptr => check = check.cast(Value.Payload.PayloadPtr).?.data.container_ptr,
        .generic_poison => return error.GenericPoison,
        else => {
            try sema.resolveLazyValue(val);
            return val;
        },
    };
}

/// Returns all Value tags including `variable` and `undef`.
fn resolveMaybeUndefValAllowVariables(sema: *Sema, inst: Air.Inst.Ref) CompileError!?Value {
    var make_runtime = false;
    if (try sema.resolveMaybeUndefValAllowVariablesMaybeRuntime(inst, &make_runtime)) |val| {
        if (make_runtime) return null;
        return val;
    }
    return null;
}

/// Returns all Value tags including `variable`, `undef` and `runtime_value`.
fn resolveMaybeUndefValAllowVariablesMaybeRuntime(
    sema: *Sema,
    inst: Air.Inst.Ref,
    make_runtime: *bool,
) CompileError!?Value {
    // First section of indexes correspond to a set number of constant values.
    var i: usize = @enumToInt(inst);
    if (i < Air.Inst.Ref.typed_value_map.len) {
        return Air.Inst.Ref.typed_value_map[i].val;
    }
    i -= Air.Inst.Ref.typed_value_map.len;

    const air_tags = sema.air_instructions.items(.tag);
    if (try sema.typeHasOnePossibleValue(sema.typeOf(inst))) |opv| {
        if (air_tags[i] == .constant) {
            const ty_pl = sema.air_instructions.items(.data)[i].ty_pl;
            const val = sema.air_values.items[ty_pl.payload];
            if (val.tag() == .variable) return val;
        }
        return opv;
    }
    switch (air_tags[i]) {
        .constant => {
            const ty_pl = sema.air_instructions.items(.data)[i].ty_pl;
            const val = sema.air_values.items[ty_pl.payload];
            if (val.tag() == .runtime_value) make_runtime.* = true;
            if (val.isPtrToThreadLocal(sema.mod)) make_runtime.* = true;
            return val;
        },
        .const_ty => {
            return try sema.air_instructions.items(.data)[i].ty.toValue(sema.arena);
        },
        else => return null,
    }
}

fn failWithNeededComptime(sema: *Sema, block: *Block, src: LazySrcLoc, reason: []const u8) CompileError {
    const msg = msg: {
        const msg = try sema.errMsg(block, src, "unable to resolve comptime value", .{});
        errdefer msg.destroy(sema.gpa);

        try sema.errNote(block, src, msg, "{s}", .{reason});
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn failWithUseOfUndef(sema: *Sema, block: *Block, src: LazySrcLoc) CompileError {
    return sema.fail(block, src, "use of undefined value here causes undefined behavior", .{});
}

fn failWithDivideByZero(sema: *Sema, block: *Block, src: LazySrcLoc) CompileError {
    return sema.fail(block, src, "division by zero here causes undefined behavior", .{});
}

fn failWithModRemNegative(sema: *Sema, block: *Block, src: LazySrcLoc, lhs_ty: Type, rhs_ty: Type) CompileError {
    return sema.fail(block, src, "remainder division with '{}' and '{}': signed integers and floats must use @rem or @mod", .{
        lhs_ty.fmt(sema.mod), rhs_ty.fmt(sema.mod),
    });
}

fn failWithExpectedOptionalType(sema: *Sema, block: *Block, src: LazySrcLoc, optional_ty: Type) CompileError {
    return sema.fail(block, src, "expected optional type, found '{}'", .{optional_ty.fmt(sema.mod)});
}

fn failWithArrayInitNotSupported(sema: *Sema, block: *Block, src: LazySrcLoc, ty: Type) CompileError {
    const msg = msg: {
        const msg = try sema.errMsg(block, src, "type '{}' does not support array initialization syntax", .{
            ty.fmt(sema.mod),
        });
        errdefer msg.destroy(sema.gpa);
        if (ty.isSlice()) {
            try sema.errNote(block, src, msg, "inferred array length is specified with an underscore: '[_]{}'", .{ty.elemType2().fmt(sema.mod)});
        }
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn failWithStructInitNotSupported(sema: *Sema, block: *Block, src: LazySrcLoc, ty: Type) CompileError {
    return sema.fail(block, src, "type '{}' does not support struct initialization syntax", .{
        ty.fmt(sema.mod),
    });
}

fn failWithErrorSetCodeMissing(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    dest_err_set_ty: Type,
    src_err_set_ty: Type,
) CompileError {
    return sema.fail(block, src, "expected type '{}', found type '{}'", .{
        dest_err_set_ty.fmt(sema.mod), src_err_set_ty.fmt(sema.mod),
    });
}

fn failWithIntegerOverflow(sema: *Sema, block: *Block, src: LazySrcLoc, int_ty: Type, val: Value, vector_index: usize) CompileError {
    if (int_ty.zigTypeTag() == .Vector) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "overflow of vector type '{}' with value '{}'", .{
                int_ty.fmt(sema.mod), val.fmtValue(int_ty, sema.mod),
            });
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, src, msg, "when computing vector element at index '{d}'", .{vector_index});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    return sema.fail(block, src, "overflow of integer type '{}' with value '{}'", .{
        int_ty.fmt(sema.mod), val.fmtValue(int_ty, sema.mod),
    });
}

fn failWithInvalidComptimeFieldStore(sema: *Sema, block: *Block, init_src: LazySrcLoc, container_ty: Type, field_index: usize) CompileError {
    const msg = msg: {
        const msg = try sema.errMsg(block, init_src, "value stored in comptime field does not match the default value of the field", .{});
        errdefer msg.destroy(sema.gpa);

        const struct_ty = container_ty.castTag(.@"struct") orelse break :msg msg;
        const default_value_src = struct_ty.data.fieldSrcLoc(sema.mod, .{
            .index = field_index,
            .range = .value,
        });
        try sema.mod.errNoteNonLazy(default_value_src, msg, "default value set here", .{});
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn failWithUseOfAsync(sema: *Sema, block: *Block, src: LazySrcLoc) CompileError {
    const msg = msg: {
        const msg = try sema.errMsg(block, src, "async has not been implemented in the self-hosted compiler yet", .{});
        errdefer msg.destroy(sema.gpa);
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

/// We don't return a pointer to the new error note because the pointer
/// becomes invalid when you add another one.
fn errNote(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    parent: *Module.ErrorMsg,
    comptime format: []const u8,
    args: anytype,
) error{OutOfMemory}!void {
    const mod = sema.mod;
    const src_decl = mod.declPtr(block.src_decl);
    return mod.errNoteNonLazy(src.toSrcLoc(src_decl), parent, format, args);
}

fn addFieldErrNote(
    sema: *Sema,
    container_ty: Type,
    field_index: usize,
    parent: *Module.ErrorMsg,
    comptime format: []const u8,
    args: anytype,
) !void {
    @setCold(true);
    const mod = sema.mod;
    const decl_index = container_ty.getOwnerDecl();
    const decl = mod.declPtr(decl_index);

    const field_src = blk: {
        const tree = decl.getFileScope().getTree(sema.gpa) catch |err| {
            log.err("unable to load AST to report compile error: {s}", .{@errorName(err)});
            break :blk decl.srcLoc();
        };

        const container_node = decl.relativeToNodeIndex(0);
        const node_tags = tree.nodes.items(.tag);
        var buf: [2]std.zig.Ast.Node.Index = undefined;
        const container_decl = tree.fullContainerDecl(&buf, container_node) orelse break :blk decl.srcLoc();

        var it_index: usize = 0;
        for (container_decl.ast.members) |member_node| {
            switch (node_tags[member_node]) {
                .container_field_init,
                .container_field_align,
                .container_field,
                => {
                    if (it_index == field_index) {
                        break :blk decl.nodeOffsetSrcLoc(decl.nodeIndexToRelative(member_node));
                    }
                    it_index += 1;
                },
                else => continue,
            }
        }
        unreachable;
    };
    try mod.errNoteNonLazy(field_src, parent, format, args);
}

fn errMsg(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    comptime format: []const u8,
    args: anytype,
) error{OutOfMemory}!*Module.ErrorMsg {
    const mod = sema.mod;
    const src_decl = mod.declPtr(block.src_decl);
    return Module.ErrorMsg.create(sema.gpa, src.toSrcLoc(src_decl), format, args);
}

pub fn fail(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    comptime format: []const u8,
    args: anytype,
) CompileError {
    const err_msg = try sema.errMsg(block, src, format, args);
    return sema.failWithOwnedErrorMsg(err_msg);
}

fn failWithOwnedErrorMsg(sema: *Sema, err_msg: *Module.ErrorMsg) CompileError {
    @setCold(true);

    if (crash_report.is_enabled and sema.mod.comp.debug_compile_errors) {
        if (err_msg.src_loc.lazy == .unneeded) return error.NeededSourceLocation;
        var arena = std.heap.ArenaAllocator.init(sema.gpa);
        errdefer arena.deinit();
        var errors = std.ArrayList(Compilation.AllErrors.Message).init(sema.gpa);
        defer errors.deinit();

        Compilation.AllErrors.add(sema.mod, &arena, &errors, err_msg.*) catch unreachable;

        std.debug.print("compile error during Sema:\n", .{});
        Compilation.AllErrors.Message.renderToStdErr(errors.items[0], .no_color);
        crash_report.compilerPanic("unexpected compile error occurred", null, null);
    }

    const mod = sema.mod;
    ref: {
        errdefer err_msg.destroy(mod.gpa);
        if (err_msg.src_loc.lazy == .unneeded) {
            return error.NeededSourceLocation;
        }
        try mod.failed_decls.ensureUnusedCapacity(mod.gpa, 1);
        try mod.failed_files.ensureUnusedCapacity(mod.gpa, 1);

        const max_references = blk: {
            if (sema.mod.comp.reference_trace) |num| break :blk num;
            // Do not add multiple traces without explicit request.
            if (sema.mod.failed_decls.count() != 0) break :ref;
            break :blk default_reference_trace_len;
        };

        var referenced_by = if (sema.func) |some| some.owner_decl else sema.owner_decl_index;
        var reference_stack = std.ArrayList(Module.ErrorMsg.Trace).init(sema.gpa);
        defer reference_stack.deinit();

        // Avoid infinite loops.
        var seen = std.AutoHashMap(Module.Decl.Index, void).init(sema.gpa);
        defer seen.deinit();

        var cur_reference_trace: u32 = 0;
        while (sema.mod.reference_table.get(referenced_by)) |ref| : (cur_reference_trace += 1) {
            const gop = try seen.getOrPut(ref.referencer);
            if (gop.found_existing) break;
            if (cur_reference_trace < max_references) {
                const decl = sema.mod.declPtr(ref.referencer);
                try reference_stack.append(.{ .decl = decl.name, .src_loc = ref.src.toSrcLoc(decl) });
            }
            referenced_by = ref.referencer;
        }
        if (sema.mod.comp.reference_trace == null and cur_reference_trace > 0) {
            try reference_stack.append(.{
                .decl = null,
                .src_loc = undefined,
                .hidden = 0,
            });
        } else if (cur_reference_trace > max_references) {
            try reference_stack.append(.{
                .decl = undefined,
                .src_loc = undefined,
                .hidden = cur_reference_trace - max_references,
            });
        }
        err_msg.reference_trace = try reference_stack.toOwnedSlice();
    }
    if (sema.owner_func) |func| {
        func.state = .sema_failure;
    } else {
        sema.owner_decl.analysis = .sema_failure;
        sema.owner_decl.generation = mod.generation;
    }
    if (sema.func) |func| {
        func.state = .sema_failure;
    }
    const gop = mod.failed_decls.getOrPutAssumeCapacity(sema.owner_decl_index);
    if (gop.found_existing) {
        // If there are multiple errors for the same Decl, prefer the first one added.
        sema.err = null;
        err_msg.destroy(mod.gpa);
    } else {
        sema.err = err_msg;
        gop.value_ptr.* = err_msg;
    }
    return error.AnalysisFail;
}

const align_ty = Type.u29;

fn analyzeAsAlign(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_ref: Air.Inst.Ref,
) !u32 {
    const alignment_big = try sema.analyzeAsInt(block, src, air_ref, align_ty, "alignment must be comptime-known");
    const alignment = @intCast(u32, alignment_big); // We coerce to u16 in the prev line.
    try sema.validateAlign(block, src, alignment);
    return alignment;
}

fn validateAlign(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    alignment: u32,
) !void {
    if (alignment == 0) return sema.fail(block, src, "alignment must be >= 1", .{});
    if (!std.math.isPowerOfTwo(alignment)) {
        return sema.fail(block, src, "alignment value '{d}' is not a power of two", .{
            alignment,
        });
    }
}

pub fn resolveAlign(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
) !u32 {
    const air_ref = try sema.resolveInst(zir_ref);
    return sema.analyzeAsAlign(block, src, air_ref);
}

fn resolveInt(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
    dest_ty: Type,
    reason: []const u8,
) !u64 {
    const air_ref = try sema.resolveInst(zir_ref);
    return sema.analyzeAsInt(block, src, air_ref, dest_ty, reason);
}

fn analyzeAsInt(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    air_ref: Air.Inst.Ref,
    dest_ty: Type,
    reason: []const u8,
) !u64 {
    const coerced = try sema.coerce(block, dest_ty, air_ref, src);
    const val = try sema.resolveConstValue(block, src, coerced, reason);
    const target = sema.mod.getTarget();
    return (try val.getUnsignedIntAdvanced(target, sema)).?;
}

// Returns a compile error if the value has tag `variable`. See `resolveInstValue` for
// a function that does not.
pub fn resolveInstConst(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
    reason: []const u8,
) CompileError!TypedValue {
    const air_ref = try sema.resolveInst(zir_ref);
    const val = try sema.resolveConstValue(block, src, air_ref, reason);
    return TypedValue{
        .ty = sema.typeOf(air_ref),
        .val = val,
    };
}

// Value Tag may be `undef` or `variable`.
// See `resolveInstConst` for an alternative.
pub fn resolveInstValue(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_ref: Zir.Inst.Ref,
    reason: []const u8,
) CompileError!TypedValue {
    const air_ref = try sema.resolveInst(zir_ref);
    const val = try sema.resolveValue(block, src, air_ref, reason);
    return TypedValue{
        .ty = sema.typeOf(air_ref),
        .val = val,
    };
}

fn zirCoerceResultPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const pointee_ty = try sema.resolveType(block, src, extra.lhs);
    const ptr = try sema.resolveInst(extra.rhs);
    const target = sema.mod.getTarget();
    const addr_space = target_util.defaultAddressSpace(target, .local);

    if (Air.refToIndex(ptr)) |ptr_inst| {
        if (sema.air_instructions.items(.tag)[ptr_inst] == .constant) {
            const air_datas = sema.air_instructions.items(.data);
            const ptr_val = sema.air_values.items[air_datas[ptr_inst].ty_pl.payload];
            switch (ptr_val.tag()) {
                .inferred_alloc => {
                    const inferred_alloc = &ptr_val.castTag(.inferred_alloc).?.data;
                    // Add the stored instruction to the set we will use to resolve peer types
                    // for the inferred allocation.