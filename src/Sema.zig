
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
                    // This instruction will not make it to codegen; it is only to participate
                    // in the `stored_inst_list` of the `inferred_alloc`.
                    var trash_block = block.makeSubBlock();
                    defer trash_block.instructions.deinit(sema.gpa);
                    const operand = try trash_block.addBitCast(pointee_ty, .void_value);

                    const ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                        .pointee_type = pointee_ty,
                        .@"align" = inferred_alloc.alignment,
                        .@"addrspace" = addr_space,
                    });
                    const bitcasted_ptr = try block.addBitCast(ptr_ty, ptr);

                    try inferred_alloc.prongs.append(sema.arena, .{
                        .stored_inst = operand,
                        .placeholder = Air.refToIndex(bitcasted_ptr).?,
                    });

                    return bitcasted_ptr;
                },
                .inferred_alloc_comptime => {
                    const iac = ptr_val.castTag(.inferred_alloc_comptime).?;
                    // There will be only one coerce_result_ptr because we are running at comptime.
                    // The alloc will turn into a Decl.
                    var anon_decl = try block.startAnonDecl();
                    defer anon_decl.deinit();
                    iac.data.decl_index = try anon_decl.finish(
                        try pointee_ty.copy(anon_decl.arena()),
                        Value.undef,
                        iac.data.alignment,
                    );
                    if (iac.data.alignment != 0) {
                        try sema.resolveTypeLayout(pointee_ty);
                    }
                    const ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                        .pointee_type = pointee_ty,
                        .@"align" = iac.data.alignment,
                        .@"addrspace" = addr_space,
                    });
                    return sema.addConstant(
                        ptr_ty,
                        try Value.Tag.decl_ref_mut.create(sema.arena, .{
                            .decl_index = iac.data.decl_index,
                            .runtime_index = block.runtime_index,
                        }),
                    );
                },
                else => {},
            }
        }
    }

    // Make a dummy store through the pointer to test the coercion.
    // We will then use the generated instructions to decide what
    // kind of transformations to make on the result pointer.
    var trash_block = block.makeSubBlock();
    trash_block.is_comptime = false;
    trash_block.is_coerce_result_ptr = true;
    defer trash_block.instructions.deinit(sema.gpa);

    const dummy_ptr = try trash_block.addTy(.alloc, sema.typeOf(ptr));
    const dummy_operand = try trash_block.addBitCast(pointee_ty, .void_value);
    return sema.coerceResultPtr(block, src, ptr, dummy_ptr, dummy_operand, &trash_block);
}

fn coerceResultPtr(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    ptr: Air.Inst.Ref,
    dummy_ptr: Air.Inst.Ref,
    dummy_operand: Air.Inst.Ref,
    trash_block: *Block,
) CompileError!Air.Inst.Ref {
    const target = sema.mod.getTarget();
    const addr_space = target_util.defaultAddressSpace(target, .local);
    const pointee_ty = sema.typeOf(dummy_operand);
    const prev_trash_len = trash_block.instructions.items.len;

    try sema.storePtr2(trash_block, src, dummy_ptr, src, dummy_operand, src, .bitcast);

    {
        const air_tags = sema.air_instructions.items(.tag);

        //std.debug.print("dummy storePtr instructions:\n", .{});
        //for (trash_block.instructions.items) |item| {
        //    std.debug.print("  {s}\n", .{@tagName(air_tags[item])});
        //}

        // The last one is always `store`.
        const trash_inst = trash_block.instructions.items[trash_block.instructions.items.len - 1];
        if (air_tags[trash_inst] != .store) {
            // no store instruction is generated for zero sized types
            assert((try sema.typeHasOnePossibleValue(pointee_ty)) != null);
        } else {
            trash_block.instructions.items.len -= 1;
            assert(trash_inst == sema.air_instructions.len - 1);
            sema.air_instructions.len -= 1;
        }
    }

    const ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = pointee_ty,
        .@"addrspace" = addr_space,
    });

    var new_ptr = ptr;

    while (true) {
        const air_tags = sema.air_instructions.items(.tag);
        const air_datas = sema.air_instructions.items(.data);

        if (trash_block.instructions.items.len == prev_trash_len) {
            if (try sema.resolveDefinedValue(block, src, new_ptr)) |ptr_val| {
                return sema.addConstant(ptr_ty, ptr_val);
            }
            if (pointee_ty.eql(Type.null, sema.mod)) {
                const opt_ty = sema.typeOf(new_ptr).childType();
                const null_inst = try sema.addConstant(opt_ty, Value.null);
                _ = try block.addBinOp(.store, new_ptr, null_inst);
                return Air.Inst.Ref.void_value;
            }
            return sema.bitCast(block, ptr_ty, new_ptr, src, null);
        }

        const trash_inst = trash_block.instructions.pop();

        switch (air_tags[trash_inst]) {
            .bitcast => {
                const ty_op = air_datas[trash_inst].ty_op;
                const operand_ty = sema.typeOf(ty_op.operand);
                const ptr_operand_ty = try Type.ptr(sema.arena, sema.mod, .{
                    .pointee_type = operand_ty,
                    .@"addrspace" = addr_space,
                });
                if (try sema.resolveDefinedValue(block, src, new_ptr)) |ptr_val| {
                    new_ptr = try sema.addConstant(ptr_operand_ty, ptr_val);
                } else {
                    new_ptr = try sema.bitCast(block, ptr_operand_ty, new_ptr, src, null);
                }
            },
            .wrap_optional => {
                new_ptr = try sema.analyzeOptionalPayloadPtr(block, src, new_ptr, false, true);
            },
            .wrap_errunion_err => {
                return sema.fail(block, src, "TODO coerce_result_ptr wrap_errunion_err", .{});
            },
            .wrap_errunion_payload => {
                new_ptr = try sema.analyzeErrUnionPayloadPtr(block, src, new_ptr, false, true);
            },
            .array_to_slice => {
                return sema.fail(block, src, "TODO coerce_result_ptr array_to_slice", .{});
            },
            else => {
                if (std.debug.runtime_safety) {
                    std.debug.panic("unexpected AIR tag for coerce_result_ptr: {}", .{
                        air_tags[trash_inst],
                    });
                } else {
                    unreachable;
                }
            },
        }
    }
}

pub fn analyzeStructDecl(
    sema: *Sema,
    new_decl: *Decl,
    inst: Zir.Inst.Index,
    struct_obj: *Module.Struct,
) SemaError!void {
    const extended = sema.code.instructions.items(.data)[inst].extended;
    assert(extended.opcode == .struct_decl);
    const small = @bitCast(Zir.Inst.StructDecl.Small, extended.small);

    struct_obj.known_non_opv = small.known_non_opv;
    if (small.known_comptime_only) {
        struct_obj.requires_comptime = .yes;
    }

    var extra_index: usize = extended.operand;
    extra_index += @boolToInt(small.has_src_node);
    extra_index += @boolToInt(small.has_fields_len);
    const decls_len = if (small.has_decls_len) blk: {
        const decls_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk decls_len;
    } else 0;

    if (small.has_backing_int) {
        const backing_int_body_len = sema.code.extra[extra_index];
        extra_index += 1; // backing_int_body_len
        if (backing_int_body_len == 0) {
            extra_index += 1; // backing_int_ref
        } else {
            extra_index += backing_int_body_len; // backing_int_body_inst
        }
    }

    _ = try sema.mod.scanNamespace(&struct_obj.namespace, extra_index, decls_len, new_decl);
}

fn zirStructDecl(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const small = @bitCast(Zir.Inst.StructDecl.Small, extended.small);
    const src: LazySrcLoc = if (small.has_src_node) blk: {
        const node_offset = @bitCast(i32, sema.code.extra[extended.operand]);
        break :blk LazySrcLoc.nodeOffset(node_offset);
    } else sema.src;

    var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
    errdefer new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const mod = sema.mod;
    const struct_obj = try new_decl_arena_allocator.create(Module.Struct);
    const struct_ty = try Type.Tag.@"struct".create(new_decl_arena_allocator, struct_obj);
    const struct_val = try Value.Tag.ty.create(new_decl_arena_allocator, struct_ty);
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = struct_val,
    }, small.name_strategy, "struct", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer mod.abortAnonDecl(new_decl_index);
    struct_obj.* = .{
        .owner_decl = new_decl_index,
        .fields = .{},
        .zir_index = inst,
        .layout = small.layout,
        .status = .none,
        .known_non_opv = undefined,
        .is_tuple = small.is_tuple,
        .namespace = .{
            .parent = block.namespace,
            .ty = struct_ty,
            .file_scope = block.getFileScope(),
        },
    };
    std.log.scoped(.module).debug("create struct {*} owned by {*} ({s})", .{
        &struct_obj.namespace, new_decl, new_decl.name,
    });
    try sema.analyzeStructDecl(new_decl, inst, struct_obj);
    try new_decl.finalizeNewArena(&new_decl_arena);
    return sema.analyzeDeclVal(block, src, new_decl_index);
}

fn createAnonymousDeclTypeNamed(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    typed_value: TypedValue,
    name_strategy: Zir.Inst.NameStrategy,
    anon_prefix: []const u8,
    inst: ?Zir.Inst.Index,
) !Decl.Index {
    const mod = sema.mod;
    const namespace = block.namespace;
    const src_scope = block.wip_capture_scope;
    const src_decl = mod.declPtr(block.src_decl);
    const src_node = src_decl.relativeToNodeIndex(src.node_offset.x);
    const new_decl_index = try mod.allocateNewDecl(namespace, src_node, src_scope);
    errdefer mod.destroyDecl(new_decl_index);

    switch (name_strategy) {
        .anon => {
            // It would be neat to have "struct:line:column" but this name has
            // to survive incremental updates, where it may have been shifted down
            // or up to a different line, but unchanged, and thus not unnecessarily
            // semantically analyzed.
            // This name is also used as the key in the parent namespace so it cannot be
            // renamed.
            const name = try std.fmt.allocPrintZ(sema.gpa, "{s}__{s}_{d}", .{
                src_decl.name, anon_prefix, @enumToInt(new_decl_index),
            });
            errdefer sema.gpa.free(name);
            try mod.initNewAnonDecl(new_decl_index, src_decl.src_line, namespace, typed_value, name);
            return new_decl_index;
        },
        .parent => {
            const name = try sema.gpa.dupeZ(u8, mem.sliceTo(sema.mod.declPtr(block.src_decl).name, 0));
            errdefer sema.gpa.free(name);
            try mod.initNewAnonDecl(new_decl_index, src_decl.src_line, namespace, typed_value, name);
            return new_decl_index;
        },
        .func => {
            const fn_info = sema.code.getFnInfo(sema.func.?.zir_body_inst);
            const zir_tags = sema.code.instructions.items(.tag);

            var buf = std.ArrayList(u8).init(sema.gpa);
            defer buf.deinit();
            try buf.appendSlice(mem.sliceTo(sema.mod.declPtr(block.src_decl).name, 0));
            try buf.appendSlice("(");

            var arg_i: usize = 0;
            for (fn_info.param_body) |zir_inst| switch (zir_tags[zir_inst]) {
                .param, .param_comptime, .param_anytype, .param_anytype_comptime => {
                    const arg = sema.inst_map.get(zir_inst).?;
                    // The comptime call code in analyzeCall already did this, so we're
                    // just repeating it here and it's guaranteed to work.
                    const arg_val = sema.resolveConstMaybeUndefVal(block, .unneeded, arg, "") catch unreachable;

                    if (arg_i != 0) try buf.appendSlice(",");
                    try buf.writer().print("{}", .{arg_val.fmtValue(sema.typeOf(arg), sema.mod)});

                    arg_i += 1;
                    continue;
                },
                else => continue,
            };

            try buf.appendSlice(")");
            const name = try buf.toOwnedSliceSentinel(0);
            errdefer sema.gpa.free(name);
            try mod.initNewAnonDecl(new_decl_index, src_decl.src_line, namespace, typed_value, name);
            return new_decl_index;
        },
        .dbg_var => {
            const ref = Zir.indexToRef(inst.?);
            const zir_tags = sema.code.instructions.items(.tag);
            const zir_data = sema.code.instructions.items(.data);
            var i = inst.?;
            while (i < zir_tags.len) : (i += 1) switch (zir_tags[i]) {
                .dbg_var_ptr, .dbg_var_val => {
                    if (zir_data[i].str_op.operand != ref) continue;

                    const name = try std.fmt.allocPrintZ(sema.gpa, "{s}.{s}", .{
                        src_decl.name, zir_data[i].str_op.getStr(sema.code),
                    });
                    errdefer sema.gpa.free(name);

                    try mod.initNewAnonDecl(new_decl_index, src_decl.src_line, namespace, typed_value, name);
                    return new_decl_index;
                },
                else => {},
            };
            return sema.createAnonymousDeclTypeNamed(block, src, typed_value, .anon, anon_prefix, null);
        },
    }
}

fn zirEnumDecl(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const mod = sema.mod;
    const gpa = sema.gpa;
    const small = @bitCast(Zir.Inst.EnumDecl.Small, extended.small);
    var extra_index: usize = extended.operand;

    const src: LazySrcLoc = if (small.has_src_node) blk: {
        const node_offset = @bitCast(i32, sema.code.extra[extra_index]);
        extra_index += 1;
        break :blk LazySrcLoc.nodeOffset(node_offset);
    } else sema.src;
    const tag_ty_src: LazySrcLoc = .{ .node_offset_container_tag = src.node_offset.x };

    const tag_type_ref = if (small.has_tag_type) blk: {
        const tag_type_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
        extra_index += 1;
        break :blk tag_type_ref;
    } else .none;

    const body_len = if (small.has_body_len) blk: {
        const body_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk body_len;
    } else 0;

    const fields_len = if (small.has_fields_len) blk: {
        const fields_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk fields_len;
    } else 0;

    const decls_len = if (small.has_decls_len) blk: {
        const decls_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk decls_len;
    } else 0;

    var done = false;

    var new_decl_arena = std.heap.ArenaAllocator.init(gpa);
    errdefer if (!done) new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const enum_obj = try new_decl_arena_allocator.create(Module.EnumFull);
    const enum_ty_payload = try new_decl_arena_allocator.create(Type.Payload.EnumFull);
    enum_ty_payload.* = .{
        .base = .{ .tag = if (small.nonexhaustive) .enum_nonexhaustive else .enum_full },
        .data = enum_obj,
    };
    const enum_ty = Type.initPayload(&enum_ty_payload.base);
    const enum_val = try Value.Tag.ty.create(new_decl_arena_allocator, enum_ty);
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = enum_val,
    }, small.name_strategy, "enum", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer if (!done) mod.abortAnonDecl(new_decl_index);

    enum_obj.* = .{
        .owner_decl = new_decl_index,
        .tag_ty = Type.null,
        .tag_ty_inferred = true,
        .fields = .{},
        .values = .{},
        .namespace = .{
            .parent = block.namespace,
            .ty = enum_ty,
            .file_scope = block.getFileScope(),
        },
    };
    std.log.scoped(.module).debug("create enum {*} owned by {*} ({s})", .{
        &enum_obj.namespace, new_decl, new_decl.name,
    });

    try new_decl.finalizeNewArena(&new_decl_arena);
    const decl_val = try sema.analyzeDeclVal(block, src, new_decl_index);
    done = true;

    var decl_arena = new_decl.value_arena.?.promote(gpa);
    defer new_decl.value_arena.?.* = decl_arena.state;
    const decl_arena_allocator = decl_arena.allocator();

    extra_index = try mod.scanNamespace(&enum_obj.namespace, extra_index, decls_len, new_decl);

    const body = sema.code.extra[extra_index..][0..body_len];
    extra_index += body.len;

    const bit_bags_count = std.math.divCeil(usize, fields_len, 32) catch unreachable;
    const body_end = extra_index;
    extra_index += bit_bags_count;

    {
        // We create a block for the field type instructions because they
        // may need to reference Decls from inside the enum namespace.
        // Within the field type, default value, and alignment expressions, the "owner decl"
        // should be the enum itself.

        const prev_owner_decl = sema.owner_decl;
        const prev_owner_decl_index = sema.owner_decl_index;
        sema.owner_decl = new_decl;
        sema.owner_decl_index = new_decl_index;
        defer {
            sema.owner_decl = prev_owner_decl;
            sema.owner_decl_index = prev_owner_decl_index;
        }

        const prev_owner_func = sema.owner_func;
        sema.owner_func = null;
        defer sema.owner_func = prev_owner_func;

        const prev_func = sema.func;
        sema.func = null;
        defer sema.func = prev_func;

        var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, new_decl.src_scope);
        defer wip_captures.deinit();

        var enum_block: Block = .{
            .parent = null,
            .sema = sema,
            .src_decl = new_decl_index,
            .namespace = &enum_obj.namespace,
            .wip_capture_scope = wip_captures.scope,
            .instructions = .{},
            .inlining = null,
            .is_comptime = true,
        };
        defer enum_block.instructions.deinit(sema.gpa);

        if (body.len != 0) {
            try sema.analyzeBody(&enum_block, body);
        }

        try wip_captures.finalize();

        if (tag_type_ref != .none) {
            const ty = try sema.resolveType(block, tag_ty_src, tag_type_ref);
            if (ty.zigTypeTag() != .Int and ty.zigTypeTag() != .ComptimeInt) {
                return sema.fail(block, tag_ty_src, "expected integer tag type, found '{}'", .{ty.fmt(sema.mod)});
            }
            enum_obj.tag_ty = try ty.copy(decl_arena_allocator);
            enum_obj.tag_ty_inferred = false;
        } else if (fields_len == 0) {
            enum_obj.tag_ty = try Type.Tag.int_unsigned.create(decl_arena_allocator, 0);
            enum_obj.tag_ty_inferred = true;
        } else {
            const bits = std.math.log2_int_ceil(usize, fields_len);
            enum_obj.tag_ty = try Type.Tag.int_unsigned.create(decl_arena_allocator, bits);
            enum_obj.tag_ty_inferred = true;
        }
    }

    if (small.nonexhaustive and enum_obj.tag_ty.zigTypeTag() != .ComptimeInt) {
        if (fields_len > 1 and std.math.log2_int(u64, fields_len) == enum_obj.tag_ty.bitSize(sema.mod.getTarget())) {
            return sema.fail(block, src, "non-exhaustive enum specifies every value", .{});
        }
    }

    try enum_obj.fields.ensureTotalCapacity(decl_arena_allocator, fields_len);
    const any_values = for (sema.code.extra[body_end..][0..bit_bags_count]) |bag| {
        if (bag != 0) break true;
    } else false;
    if (any_values) {
        try enum_obj.values.ensureTotalCapacityContext(decl_arena_allocator, fields_len, .{
            .ty = enum_obj.tag_ty,
            .mod = mod,
        });
    }

    var bit_bag_index: usize = body_end;
    var cur_bit_bag: u32 = undefined;
    var field_i: u32 = 0;
    var last_tag_val: ?Value = null;
    var tag_val_buf: Value.Payload.U64 = undefined;
    while (field_i < fields_len) : (field_i += 1) {
        if (field_i % 32 == 0) {
            cur_bit_bag = sema.code.extra[bit_bag_index];
            bit_bag_index += 1;
        }
        const has_tag_value = @truncate(u1, cur_bit_bag) != 0;
        cur_bit_bag >>= 1;

        const field_name_zir = sema.code.nullTerminatedString(sema.code.extra[extra_index]);
        extra_index += 1;

        // doc comment
        extra_index += 1;

        // This string needs to outlive the ZIR code.
        const field_name = try decl_arena_allocator.dupe(u8, field_name_zir);

        const gop_field = enum_obj.fields.getOrPutAssumeCapacity(field_name);
        if (gop_field.found_existing) {
            const field_src = enum_obj.fieldSrcLoc(sema.mod, .{ .index = field_i }).lazy;
            const other_field_src = enum_obj.fieldSrcLoc(sema.mod, .{ .index = gop_field.index }).lazy;
            const msg = msg: {
                const msg = try sema.errMsg(block, field_src, "duplicate enum field '{s}'", .{field_name});
                errdefer msg.destroy(gpa);
                try sema.errNote(block, other_field_src, msg, "other field here", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }

        if (has_tag_value) {
            const tag_val_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
            extra_index += 1;
            const tag_inst = try sema.resolveInst(tag_val_ref);
            const tag_val = sema.resolveConstValue(block, .unneeded, tag_inst, "") catch |err| switch (err) {
                error.NeededSourceLocation => {
                    const value_src = enum_obj.fieldSrcLoc(sema.mod, .{
                        .index = field_i,
                        .range = .value,
                    }).lazy;
                    _ = try sema.resolveConstValue(block, value_src, tag_inst, "enum tag value must be comptime-known");
                    unreachable;
                },
                else => |e| return e,
            };
            last_tag_val = tag_val;
            const copied_tag_val = try tag_val.copy(decl_arena_allocator);
            const gop_val = enum_obj.values.getOrPutAssumeCapacityContext(copied_tag_val, .{
                .ty = enum_obj.tag_ty,
                .mod = mod,
            });
            if (gop_val.found_existing) {
                const value_src = enum_obj.fieldSrcLoc(sema.mod, .{
                    .index = field_i,
                    .range = .value,
                }).lazy;
                const other_field_src = enum_obj.fieldSrcLoc(sema.mod, .{ .index = gop_val.index }).lazy;
                const msg = msg: {
                    const msg = try sema.errMsg(block, value_src, "enum tag value {} already taken", .{tag_val.fmtValue(enum_obj.tag_ty, sema.mod)});
                    errdefer msg.destroy(gpa);
                    try sema.errNote(block, other_field_src, msg, "other occurrence here", .{});
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
        } else if (any_values) {
            const tag_val = if (last_tag_val) |val|
                try sema.intAdd(val, Value.one, enum_obj.tag_ty)
            else
                Value.zero;
            last_tag_val = tag_val;
            const copied_tag_val = try tag_val.copy(decl_arena_allocator);
            const gop_val = enum_obj.values.getOrPutAssumeCapacityContext(copied_tag_val, .{
                .ty = enum_obj.tag_ty,
                .mod = mod,
            });
            if (gop_val.found_existing) {
                const field_src = enum_obj.fieldSrcLoc(sema.mod, .{ .index = field_i }).lazy;
                const other_field_src = enum_obj.fieldSrcLoc(sema.mod, .{ .index = gop_val.index }).lazy;
                const msg = msg: {
                    const msg = try sema.errMsg(block, field_src, "enum tag value {} already taken", .{tag_val.fmtValue(enum_obj.tag_ty, sema.mod)});
                    errdefer msg.destroy(gpa);
                    try sema.errNote(block, other_field_src, msg, "other occurrence here", .{});
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
        } else {
            tag_val_buf = .{
                .base = .{ .tag = .int_u64 },
                .data = field_i,
            };
            last_tag_val = Value.initPayload(&tag_val_buf.base);
        }

        if (!(try sema.intFitsInType(last_tag_val.?, enum_obj.tag_ty, null))) {
            const value_src = enum_obj.fieldSrcLoc(sema.mod, .{
                .index = field_i,
                .range = if (has_tag_value) .value else .name,
            }).lazy;
            const msg = try sema.errMsg(block, value_src, "enumeration value '{}' too large for type '{}'", .{
                last_tag_val.?.fmtValue(enum_obj.tag_ty, mod), enum_obj.tag_ty.fmt(mod),
            });
            return sema.failWithOwnedErrorMsg(msg);
        }
    }
    return decl_val;
}

fn zirUnionDecl(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const small = @bitCast(Zir.Inst.UnionDecl.Small, extended.small);
    var extra_index: usize = extended.operand;

    const src: LazySrcLoc = if (small.has_src_node) blk: {
        const node_offset = @bitCast(i32, sema.code.extra[extra_index]);
        extra_index += 1;
        break :blk LazySrcLoc.nodeOffset(node_offset);
    } else sema.src;

    extra_index += @boolToInt(small.has_tag_type);
    extra_index += @boolToInt(small.has_body_len);
    extra_index += @boolToInt(small.has_fields_len);

    const decls_len = if (small.has_decls_len) blk: {
        const decls_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk decls_len;
    } else 0;

    var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
    errdefer new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const union_obj = try new_decl_arena_allocator.create(Module.Union);
    const type_tag = if (small.has_tag_type or small.auto_enum_tag)
        Type.Tag.union_tagged
    else if (small.layout != .Auto)
        Type.Tag.@"union"
    else switch (block.sema.mod.optimizeMode()) {
        .Debug, .ReleaseSafe => Type.Tag.union_safety_tagged,
        .ReleaseFast, .ReleaseSmall => Type.Tag.@"union",
    };
    const union_payload = try new_decl_arena_allocator.create(Type.Payload.Union);
    union_payload.* = .{
        .base = .{ .tag = type_tag },
        .data = union_obj,
    };
    const union_ty = Type.initPayload(&union_payload.base);
    const union_val = try Value.Tag.ty.create(new_decl_arena_allocator, union_ty);
    const mod = sema.mod;
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = union_val,
    }, small.name_strategy, "union", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer mod.abortAnonDecl(new_decl_index);
    union_obj.* = .{
        .owner_decl = new_decl_index,
        .tag_ty = Type.initTag(.null),
        .fields = .{},
        .zir_index = inst,
        .layout = small.layout,
        .status = .none,
        .namespace = .{
            .parent = block.namespace,
            .ty = union_ty,
            .file_scope = block.getFileScope(),
        },
    };
    std.log.scoped(.module).debug("create union {*} owned by {*} ({s})", .{
        &union_obj.namespace, new_decl, new_decl.name,
    });

    _ = try mod.scanNamespace(&union_obj.namespace, extra_index, decls_len, new_decl);

    try new_decl.finalizeNewArena(&new_decl_arena);
    return sema.analyzeDeclVal(block, src, new_decl_index);
}

fn zirOpaqueDecl(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const mod = sema.mod;
    const gpa = sema.gpa;
    const small = @bitCast(Zir.Inst.OpaqueDecl.Small, extended.small);
    var extra_index: usize = extended.operand;

    const src: LazySrcLoc = if (small.has_src_node) blk: {
        const node_offset = @bitCast(i32, sema.code.extra[extra_index]);
        extra_index += 1;
        break :blk LazySrcLoc.nodeOffset(node_offset);
    } else sema.src;

    const decls_len = if (small.has_decls_len) blk: {
        const decls_len = sema.code.extra[extra_index];
        extra_index += 1;
        break :blk decls_len;
    } else 0;

    var new_decl_arena = std.heap.ArenaAllocator.init(gpa);
    errdefer new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const opaque_obj = try new_decl_arena_allocator.create(Module.Opaque);
    const opaque_ty_payload = try new_decl_arena_allocator.create(Type.Payload.Opaque);
    opaque_ty_payload.* = .{
        .base = .{ .tag = .@"opaque" },
        .data = opaque_obj,
    };
    const opaque_ty = Type.initPayload(&opaque_ty_payload.base);
    const opaque_val = try Value.Tag.ty.create(new_decl_arena_allocator, opaque_ty);
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = opaque_val,
    }, small.name_strategy, "opaque", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer mod.abortAnonDecl(new_decl_index);

    opaque_obj.* = .{
        .owner_decl = new_decl_index,
        .namespace = .{
            .parent = block.namespace,
            .ty = opaque_ty,
            .file_scope = block.getFileScope(),
        },
    };
    std.log.scoped(.module).debug("create opaque {*} owned by {*} ({s})", .{
        &opaque_obj.namespace, new_decl, new_decl.name,
    });

    extra_index = try mod.scanNamespace(&opaque_obj.namespace, extra_index, decls_len, new_decl);

    try new_decl.finalizeNewArena(&new_decl_arena);
    return sema.analyzeDeclVal(block, src, new_decl_index);
}

fn zirErrorSetDecl(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    name_strategy: Zir.Inst.NameStrategy,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const gpa = sema.gpa;
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.ErrorSetDecl, inst_data.payload_index);

    var new_decl_arena = std.heap.ArenaAllocator.init(gpa);
    errdefer new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const error_set = try new_decl_arena_allocator.create(Module.ErrorSet);
    const error_set_ty = try Type.Tag.error_set.create(new_decl_arena_allocator, error_set);
    const error_set_val = try Value.Tag.ty.create(new_decl_arena_allocator, error_set_ty);
    const mod = sema.mod;
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = error_set_val,
    }, name_strategy, "error", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer mod.abortAnonDecl(new_decl_index);

    var names = Module.ErrorSet.NameMap{};
    try names.ensureUnusedCapacity(new_decl_arena_allocator, extra.data.fields_len);

    var extra_index = @intCast(u32, extra.end);
    const extra_index_end = extra_index + (extra.data.fields_len * 2);
    while (extra_index < extra_index_end) : (extra_index += 2) { // +2 to skip over doc_string
        const str_index = sema.code.extra[extra_index];
        const kv = try mod.getErrorValue(sema.code.nullTerminatedString(str_index));
        const result = names.getOrPutAssumeCapacity(kv.key);
        assert(!result.found_existing); // verified in AstGen
    }

    // names must be sorted.
    Module.ErrorSet.sortNames(&names);

    error_set.* = .{
        .owner_decl = new_decl_index,
        .names = names,
    };
    try new_decl.finalizeNewArena(&new_decl_arena);
    return sema.analyzeDeclVal(block, src, new_decl_index);
}

fn zirRetPtr(sema: *Sema, block: *Block) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    if (block.is_comptime or try sema.typeRequiresComptime(sema.fn_ret_ty)) {
        const fn_ret_ty = try sema.resolveTypeFields(sema.fn_ret_ty);
        return sema.analyzeComptimeAlloc(block, fn_ret_ty, 0);
    }

    const target = sema.mod.getTarget();
    const ptr_type = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = sema.fn_ret_ty,
        .@"addrspace" = target_util.defaultAddressSpace(target, .local),
    });

    if (block.inlining != null) {
        // We are inlining a function call; this should be emitted as an alloc, not a ret_ptr.
        // TODO when functions gain result location support, the inlining struct in
        // Block should contain the return pointer, and we would pass that through here.
        return block.addTy(.alloc, ptr_type);
    }

    return block.addTy(.ret_ptr, ptr_type);
}

fn zirRef(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_tok;
    const operand = try sema.resolveInst(inst_data.operand);
    return sema.analyzeRef(block, inst_data.src(), operand);
}

fn zirEnsureResultUsed(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const src = inst_data.src();

    return sema.ensureResultUsed(block, sema.typeOf(operand), src);
}

fn ensureResultUsed(
    sema: *Sema,
    block: *Block,
    ty: Type,
    src: LazySrcLoc,
) CompileError!void {
    switch (ty.zigTypeTag()) {
        .Void, .NoReturn => return,
        .ErrorSet, .ErrorUnion => {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "error is ignored", .{});
                errdefer msg.destroy(sema.gpa);
                try sema.errNote(block, src, msg, "consider using 'try', 'catch', or 'if'", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        else => {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "value of type '{}' ignored", .{ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);
                try sema.errNote(block, src, msg, "all non-void values must be used", .{});
                try sema.errNote(block, src, msg, "this error can be suppressed by assigning the value to '_'", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
    }
}

fn zirEnsureResultNonError(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const src = inst_data.src();
    const operand_ty = sema.typeOf(operand);
    switch (operand_ty.zigTypeTag()) {
        .ErrorSet, .ErrorUnion => {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "error is discarded", .{});
                errdefer msg.destroy(sema.gpa);
                try sema.errNote(block, src, msg, "consider using 'try', 'catch', or 'if'", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        else => return,
    }
}

fn zirEnsureErrUnionPayloadVoid(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    const err_union_ty = if (operand_ty.zigTypeTag() == .Pointer)
        operand_ty.childType()
    else
        operand_ty;
    if (err_union_ty.zigTypeTag() != .ErrorUnion) return;
    const payload_ty = err_union_ty.errorUnionPayload().zigTypeTag();
    if (payload_ty != .Void and payload_ty != .NoReturn) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "error union payload is ignored", .{});
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, src, msg, "payload value can be explicitly ignored with '|_|'", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
}

fn zirIndexablePtrLen(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const object = try sema.resolveInst(inst_data.operand);
    const object_ty = sema.typeOf(object);

    const is_pointer_to = object_ty.isSinglePointer();

    const array_ty = if (is_pointer_to)
        object_ty.childType()
    else
        object_ty;

    try checkIndexable(sema, block, src, array_ty);

    return sema.fieldVal(block, src, object, "len", src);
}

fn zirAllocExtended(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.AllocExtended, extended.operand);
    const ty_src: LazySrcLoc = .{ .node_offset_var_decl_ty = extra.data.src_node };
    const align_src: LazySrcLoc = .{ .node_offset_var_decl_align = extra.data.src_node };
    const small = @bitCast(Zir.Inst.AllocExtended.Small, extended.small);

    var extra_index: usize = extra.end;

    const var_ty: Type = if (small.has_type) blk: {
        const type_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
        extra_index += 1;
        break :blk try sema.resolveType(block, ty_src, type_ref);
    } else undefined;

    const alignment: u32 = if (small.has_align) blk: {
        const align_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
        extra_index += 1;
        const alignment = try sema.resolveAlign(block, align_src, align_ref);
        break :blk alignment;
    } else 0;

    const inferred_alloc_ty = if (small.is_const)
        Type.initTag(.inferred_alloc_const)
    else
        Type.initTag(.inferred_alloc_mut);

    if (block.is_comptime or small.is_comptime) {
        if (small.has_type) {
            return sema.analyzeComptimeAlloc(block, var_ty, alignment);
        } else {
            return sema.addConstant(
                inferred_alloc_ty,
                try Value.Tag.inferred_alloc_comptime.create(sema.arena, .{
                    .decl_index = undefined,
                    .alignment = alignment,
                }),
            );
        }
    }

    if (small.has_type) {
        if (!small.is_const) {
            try sema.validateVarType(block, ty_src, var_ty, false);
        }
        const target = sema.mod.getTarget();
        try sema.resolveTypeLayout(var_ty);
        const ptr_type = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = var_ty,
            .@"align" = alignment,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
        });
        return block.addTy(.alloc, ptr_type);
    }

    // `Sema.addConstant` does not add the instruction to the block because it is
    // not needed in the case of constant values. However here, we plan to "downgrade"
    // to a normal instruction when we hit `resolve_inferred_alloc`. So we append
    // to the block even though it is currently a `.constant`.
    const result = try sema.addConstant(
        inferred_alloc_ty,
        try Value.Tag.inferred_alloc.create(sema.arena, .{ .alignment = alignment }),
    );
    try block.instructions.append(sema.gpa, Air.refToIndex(result).?);
    try sema.unresolved_inferred_allocs.putNoClobber(sema.gpa, Air.refToIndex(result).?, {});
    return result;
}

fn zirAllocComptime(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const ty_src: LazySrcLoc = .{ .node_offset_var_decl_ty = inst_data.src_node };
    const var_ty = try sema.resolveType(block, ty_src, inst_data.operand);
    return sema.analyzeComptimeAlloc(block, var_ty, 0);
}

fn zirMakePtrConst(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const alloc = try sema.resolveInst(inst_data.operand);
    const alloc_ty = sema.typeOf(alloc);

    var ptr_info = alloc_ty.ptrInfo().data;
    const elem_ty = ptr_info.pointee_type;

    // Detect if all stores to an `.alloc` were comptime-known.
    ct: {
        var search_index: usize = block.instructions.items.len;
        const air_tags = sema.air_instructions.items(.tag);
        const air_datas = sema.air_instructions.items(.data);

        const store_inst = while (true) {
            if (search_index == 0) break :ct;
            search_index -= 1;

            const candidate = block.instructions.items[search_index];
            switch (air_tags[candidate]) {
                .dbg_stmt, .dbg_block_begin, .dbg_block_end => continue,
                .store => break candidate,
                else => break :ct,
            }
        };

        while (true) {
            if (search_index == 0) break :ct;
            search_index -= 1;

            const candidate = block.instructions.items[search_index];
            switch (air_tags[candidate]) {
                .dbg_stmt, .dbg_block_begin, .dbg_block_end => continue,
                .alloc => {
                    if (Air.indexToRef(candidate) != alloc) break :ct;
                    break;
                },
                else => break :ct,
            }
        }

        const store_op = air_datas[store_inst].bin_op;
        const store_val = (try sema.resolveMaybeUndefVal(store_op.rhs)) orelse break :ct;
        if (store_op.lhs != alloc) break :ct;

        // Remove all the unnecessary runtime instructions.
        block.instructions.shrinkRetainingCapacity(search_index);

        var anon_decl = try block.startAnonDecl();
        defer anon_decl.deinit();
        return sema.analyzeDeclRef(try anon_decl.finish(
            try elem_ty.copy(anon_decl.arena()),
            try store_val.copy(anon_decl.arena()),
            ptr_info.@"align",
        ));
    }

    ptr_info.mutable = false;
    const const_ptr_ty = try Type.ptr(sema.arena, sema.mod, ptr_info);

    // Detect if a comptime value simply needs to have its type changed.
    if (try sema.resolveMaybeUndefVal(alloc)) |val| {
        return sema.addConstant(const_ptr_ty, val);
    }

    return block.addBitCast(const_ptr_ty, alloc);
}

fn zirAllocInferredComptime(
    sema: *Sema,
    inst: Zir.Inst.Index,
    inferred_alloc_ty: Type,
) CompileError!Air.Inst.Ref {
    const src_node = sema.code.instructions.items(.data)[inst].node;
    const src = LazySrcLoc.nodeOffset(src_node);
    sema.src = src;
    return sema.addConstant(
        inferred_alloc_ty,
        try Value.Tag.inferred_alloc_comptime.create(sema.arena, .{
            .decl_index = undefined,
            .alignment = 0,
        }),
    );
}

fn zirAlloc(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const ty_src: LazySrcLoc = .{ .node_offset_var_decl_ty = inst_data.src_node };
    const var_ty = try sema.resolveType(block, ty_src, inst_data.operand);
    if (block.is_comptime) {
        return sema.analyzeComptimeAlloc(block, var_ty, 0);
    }
    const target = sema.mod.getTarget();
    const ptr_type = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = var_ty,
        .@"addrspace" = target_util.defaultAddressSpace(target, .local),
    });
    try sema.queueFullTypeResolution(var_ty);
    return block.addTy(.alloc, ptr_type);
}

fn zirAllocMut(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const ty_src: LazySrcLoc = .{ .node_offset_var_decl_ty = inst_data.src_node };
    const var_ty = try sema.resolveType(block, ty_src, inst_data.operand);
    if (block.is_comptime) {
        return sema.analyzeComptimeAlloc(block, var_ty, 0);
    }
    try sema.validateVarType(block, ty_src, var_ty, false);
    const target = sema.mod.getTarget();
    const ptr_type = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = var_ty,
        .@"addrspace" = target_util.defaultAddressSpace(target, .local),
    });
    try sema.queueFullTypeResolution(var_ty);
    return block.addTy(.alloc, ptr_type);
}

fn zirAllocInferred(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    inferred_alloc_ty: Type,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const src_node = sema.code.instructions.items(.data)[inst].node;
    const src = LazySrcLoc.nodeOffset(src_node);
    sema.src = src;

    if (block.is_comptime) {
        return sema.addConstant(
            inferred_alloc_ty,
            try Value.Tag.inferred_alloc_comptime.create(sema.arena, .{
                .decl_index = undefined,
                .alignment = 0,
            }),
        );
    }

    // `Sema.addConstant` does not add the instruction to the block because it is
    // not needed in the case of constant values. However here, we plan to "downgrade"
    // to a normal instruction when we hit `resolve_inferred_alloc`. So we append
    // to the block even though it is currently a `.constant`.
    const result = try sema.addConstant(
        inferred_alloc_ty,
        try Value.Tag.inferred_alloc.create(sema.arena, .{ .alignment = 0 }),
    );
    try block.instructions.append(sema.gpa, Air.refToIndex(result).?);
    try sema.unresolved_inferred_allocs.putNoClobber(sema.gpa, Air.refToIndex(result).?, {});
    return result;
}

fn zirResolveInferredAlloc(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ty_src: LazySrcLoc = .{ .node_offset_var_decl_ty = inst_data.src_node };
    const ptr = try sema.resolveInst(inst_data.operand);
    const ptr_inst = Air.refToIndex(ptr).?;
    assert(sema.air_instructions.items(.tag)[ptr_inst] == .constant);
    const value_index = sema.air_instructions.items(.data)[ptr_inst].ty_pl.payload;
    const ptr_val = sema.air_values.items[value_index];
    const var_is_mut = switch (sema.typeOf(ptr).tag()) {
        .inferred_alloc_const => false,
        .inferred_alloc_mut => true,
        else => unreachable,
    };
    const target = sema.mod.getTarget();

    switch (ptr_val.tag()) {
        .inferred_alloc_comptime => {
            const iac = ptr_val.castTag(.inferred_alloc_comptime).?;
            const decl_index = iac.data.decl_index;
            try sema.mod.declareDeclDependency(sema.owner_decl_index, decl_index);

            const decl = sema.mod.declPtr(decl_index);
            const final_elem_ty = try decl.ty.copy(sema.arena);
            const final_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                .pointee_type = final_elem_ty,
                .mutable = true,
                .@"align" = iac.data.alignment,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
            });
            const final_ptr_ty_inst = try sema.addType(final_ptr_ty);
            sema.air_instructions.items(.data)[ptr_inst].ty_pl.ty = final_ptr_ty_inst;

            if (var_is_mut) {
                sema.air_values.items[value_index] = try Value.Tag.decl_ref_mut.create(sema.arena, .{
                    .decl_index = decl_index,
                    .runtime_index = block.runtime_index,
                });
            } else {
                sema.air_values.items[value_index] = try Value.Tag.decl_ref.create(sema.arena, decl_index);
            }
        },
        .inferred_alloc => {
            assert(sema.unresolved_inferred_allocs.remove(ptr_inst));
            const inferred_alloc = ptr_val.castTag(.inferred_alloc).?;
            const peer_inst_list = inferred_alloc.data.prongs.items(.stored_inst);
            const final_elem_ty = try sema.resolvePeerTypes(block, ty_src, peer_inst_list, .none);

            const final_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                .pointee_type = final_elem_ty,
                .mutable = true,
                .@"align" = inferred_alloc.data.alignment,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
            });

            if (var_is_mut) {
                try sema.validateVarType(block, ty_src, final_elem_ty, false);
            } else ct: {
                // Detect if the value is comptime-known. In such case, the
                // last 3 AIR instructions of the block will look like this:
                //
                //   %a = constant
                //   %b = bitcast(%a)
                //   %c = store(%b, %d)
                //
                // If `%d` is comptime-known, then we want to store the value
                // inside an anonymous Decl and then erase these three AIR
                // instructions from the block, replacing the inst_map entry
                // corresponding to the ZIR alloc instruction with a constant
                // decl_ref pointing at our new Decl.
                // dbg_stmt instructions may be interspersed into this pattern
                // which must be ignored.
                if (block.instructions.items.len < 3) break :ct;
                var search_index: usize = block.instructions.items.len;
                const air_tags = sema.air_instructions.items(.tag);
                const air_datas = sema.air_instructions.items(.data);

                const store_inst = while (true) {
                    if (search_index == 0) break :ct;
                    search_index -= 1;

                    const candidate = block.instructions.items[search_index];
                    switch (air_tags[candidate]) {
                        .dbg_stmt, .dbg_block_begin, .dbg_block_end => continue,
                        .store => break candidate,
                        else => break :ct,
                    }
                };

                const bitcast_inst = while (true) {
                    if (search_index == 0) break :ct;
                    search_index -= 1;

                    const candidate = block.instructions.items[search_index];
                    switch (air_tags[candidate]) {
                        .dbg_stmt, .dbg_block_begin, .dbg_block_end => continue,
                        .bitcast => break candidate,
                        else => break :ct,
                    }
                };

                const const_inst = while (true) {
                    if (search_index == 0) break :ct;
                    search_index -= 1;

                    const candidate = block.instructions.items[search_index];
                    switch (air_tags[candidate]) {
                        .dbg_stmt, .dbg_block_begin, .dbg_block_end => continue,
                        .constant => break candidate,
                        else => break :ct,
                    }
                };

                const store_op = air_datas[store_inst].bin_op;
                const store_val = (try sema.resolveMaybeUndefVal(store_op.rhs)) orelse break :ct;
                if (store_op.lhs != Air.indexToRef(bitcast_inst)) break :ct;
                if (air_datas[bitcast_inst].ty_op.operand != Air.indexToRef(const_inst)) break :ct;

                const new_decl_index = d: {
                    var anon_decl = try block.startAnonDecl();
                    defer anon_decl.deinit();
                    const new_decl_index = try anon_decl.finish(
                        try final_elem_ty.copy(anon_decl.arena()),
                        try store_val.copy(anon_decl.arena()),
                        inferred_alloc.data.alignment,
                    );
                    break :d new_decl_index;
                };
                try sema.mod.declareDeclDependency(sema.owner_decl_index, new_decl_index);

                // Even though we reuse the constant instruction, we still remove it from the
                // block so that codegen does not see it.
                block.instructions.shrinkRetainingCapacity(search_index);
                sema.air_values.items[value_index] = try Value.Tag.decl_ref.create(sema.arena, new_decl_index);
                // if bitcast ty ref needs to be made const, make_ptr_const
                // ZIR handles it later, so we can just use the ty ref here.
                air_datas[ptr_inst].ty_pl.ty = air_datas[bitcast_inst].ty_op.ty;

                // Unless the block is comptime, `alloc_inferred` always produces
                // a runtime constant. The final inferred type needs to be
                // fully resolved so it can be lowered in codegen.
                try sema.resolveTypeFully(final_elem_ty);

                return;
            }

            try sema.queueFullTypeResolution(final_elem_ty);

            // Change it to a normal alloc.
            sema.air_instructions.set(ptr_inst, .{
                .tag = .alloc,
                .data = .{ .ty = final_ptr_ty },
            });

            // Now we need to go back over all the coerce_result_ptr instructions, which
            // previously inserted a bitcast as a placeholder, and do the logic as if
            // the new result ptr type was available.
            const placeholders = inferred_alloc.data.prongs.items(.placeholder);
            const gpa = sema.gpa;

            var trash_block = block.makeSubBlock();
            trash_block.is_comptime = false;
            trash_block.is_coerce_result_ptr = true;
            defer trash_block.instructions.deinit(gpa);

            const mut_final_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                .pointee_type = final_elem_ty,
                .mutable = true,
                .@"align" = inferred_alloc.data.alignment,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
            });
            const dummy_ptr = try trash_block.addTy(.alloc, mut_final_ptr_ty);
            const empty_trash_count = trash_block.instructions.items.len;

            for (placeholders, 0..) |bitcast_inst, i| {
                const sub_ptr_ty = sema.typeOf(Air.indexToRef(bitcast_inst));

                if (mut_final_ptr_ty.eql(sub_ptr_ty, sema.mod)) {
                    // New result location type is the same as the old one; nothing
                    // to do here.
                    continue;
                }

                var bitcast_block = block.makeSubBlock();
                defer bitcast_block.instructions.deinit(gpa);

                trash_block.instructions.shrinkRetainingCapacity(empty_trash_count);
                const sub_ptr = try sema.coerceResultPtr(&bitcast_block, src, ptr, dummy_ptr, peer_inst_list[i], &trash_block);

                assert(bitcast_block.instructions.items.len > 0);
                // If only one instruction is produced then we can replace the bitcast
                // placeholder instruction with this instruction; no need for an entire block.
                if (bitcast_block.instructions.items.len == 1) {
                    const only_inst = bitcast_block.instructions.items[0];
                    sema.air_instructions.set(bitcast_inst, sema.air_instructions.get(only_inst));
                    continue;
                }

                // Here we replace the placeholder bitcast instruction with a block
                // that does the coerce_result_ptr logic.
                _ = try bitcast_block.addBr(bitcast_inst, sub_ptr);
                const ty_inst = sema.air_instructions.items(.data)[bitcast_inst].ty_op.ty;
                try sema.air_extra.ensureUnusedCapacity(
                    gpa,
                    @typeInfo(Air.Block).Struct.fields.len + bitcast_block.instructions.items.len,
                );
                sema.air_instructions.set(bitcast_inst, .{
                    .tag = .block,
                    .data = .{ .ty_pl = .{
                        .ty = ty_inst,
                        .payload = sema.addExtraAssumeCapacity(Air.Block{
                            .body_len = @intCast(u32, bitcast_block.instructions.items.len),
                        }),
                    } },
                });
                sema.air_extra.appendSliceAssumeCapacity(bitcast_block.instructions.items);
            }
        },
        else => unreachable,
    }
}

fn zirArrayBasePtr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();

    const start_ptr = try sema.resolveInst(inst_data.operand);
    var base_ptr = start_ptr;
    while (true) switch (sema.typeOf(base_ptr).childType().zigTypeTag()) {
        .ErrorUnion => base_ptr = try sema.analyzeErrUnionPayloadPtr(block, src, base_ptr, false, true),
        .Optional => base_ptr = try sema.analyzeOptionalPayloadPtr(block, src, base_ptr, false, true),
        else => break,
    };

    const elem_ty = sema.typeOf(base_ptr).childType();
    switch (elem_ty.zigTypeTag()) {
        .Array, .Vector => return base_ptr,
        .Struct => if (elem_ty.isTuple()) {
            // TODO validate element count
            return base_ptr;
        },
        else => {},
    }
    return sema.failWithArrayInitNotSupported(block, src, sema.typeOf(start_ptr).childType());
}

fn zirFieldBasePtr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();

    const start_ptr = try sema.resolveInst(inst_data.operand);
    var base_ptr = start_ptr;
    while (true) switch (sema.typeOf(base_ptr).childType().zigTypeTag()) {
        .ErrorUnion => base_ptr = try sema.analyzeErrUnionPayloadPtr(block, src, base_ptr, false, true),
        .Optional => base_ptr = try sema.analyzeOptionalPayloadPtr(block, src, base_ptr, false, true),
        else => break,
    };

    const elem_ty = sema.typeOf(base_ptr).childType();
    switch (elem_ty.zigTypeTag()) {
        .Struct, .Union => return base_ptr,
        else => {},
    }
    return sema.failWithStructInitNotSupported(block, src, sema.typeOf(start_ptr).childType());
}

fn zirForLen(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const gpa = sema.gpa;
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.MultiOp, inst_data.payload_index);
    const args = sema.code.refSlice(extra.end, extra.data.operands_len);
    const src = inst_data.src();

    var len: Air.Inst.Ref = .none;
    var len_val: ?Value = null;
    var len_idx: u32 = undefined;
    var any_runtime = false;

    const runtime_arg_lens = try gpa.alloc(Air.Inst.Ref, args.len);
    defer gpa.free(runtime_arg_lens);

    // First pass to look for comptime values.
    for (args, 0..) |zir_arg, i_usize| {
        const i = @intCast(u32, i_usize);
        runtime_arg_lens[i] = .none;
        if (zir_arg == .none) continue;
        const object = try sema.resolveInst(zir_arg);
        const object_ty = sema.typeOf(object);
        // Each arg could be an indexable, or a range, in which case the length
        // is passed directly as an integer.
        const is_int = switch (object_ty.zigTypeTag()) {
            .Int, .ComptimeInt => true,
            else => false,
        };
        const arg_src: LazySrcLoc = .{ .for_input = .{
            .for_node_offset = inst_data.src_node,
            .input_index = i,
        } };
        const arg_len_uncoerced = if (is_int) object else l: {
            try checkIndexable(sema, block, arg_src, object_ty);
            if (!object_ty.indexableHasLen()) continue;

            break :l try sema.fieldVal(block, arg_src, object, "len", arg_src);
        };
        const arg_len = try sema.coerce(block, Type.usize, arg_len_uncoerced, arg_src);
        if (len == .none) {
            len = arg_len;
            len_idx = i;
        }
        if (try sema.resolveDefinedValue(block, src, arg_len)) |arg_val| {
            if (len_val) |v| {
                if (!(try sema.valuesEqual(arg_val, v, Type.usize))) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "non-matching for loop lengths", .{});
                        errdefer msg.destroy(gpa);
                        const a_src: LazySrcLoc = .{ .for_input = .{
                            .for_node_offset = inst_data.src_node,
                            .input_index = len_idx,
                        } };
                        try sema.errNote(block, a_src, msg, "length {} here", .{
                            v.fmtValue(Type.usize, sema.mod),
                        });
                        try sema.errNote(block, arg_src, msg, "length {} here", .{
                            arg_val.fmtValue(Type.usize, sema.mod),
                        });
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
            } else {
                len = arg_len;
                len_val = arg_val;
                len_idx = i;
            }
            continue;
        }
        runtime_arg_lens[i] = arg_len;
        any_runtime = true;
    }

    if (len == .none) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "unbounded for loop", .{});
            errdefer msg.destroy(gpa);
            for (args, 0..) |zir_arg, i_usize| {
                const i = @intCast(u32, i_usize);
                if (zir_arg == .none) continue;
                const object = try sema.resolveInst(zir_arg);
                const object_ty = sema.typeOf(object);
                // Each arg could be an indexable, or a range, in which case the length
                // is passed directly as an integer.
                switch (object_ty.zigTypeTag()) {
                    .Int, .ComptimeInt => continue,
                    else => {},
                }
                const arg_src: LazySrcLoc = .{ .for_input = .{
                    .for_node_offset = inst_data.src_node,
                    .input_index = i,
                } };
                try sema.errNote(block, arg_src, msg, "type '{}' has no upper bound", .{
                    object_ty.fmt(sema.mod),
                });
            }
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    // Now for the runtime checks.
    if (any_runtime and block.wantSafety()) {
        for (runtime_arg_lens, 0..) |arg_len, i| {
            if (arg_len == .none) continue;
            if (i == len_idx) continue;
            const ok = try block.addBinOp(.cmp_eq, len, arg_len);
            try sema.addSafetyCheck(block, ok, .for_len_mismatch);
        }
    }

    return len;
}

fn validateArrayInitTy(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const ty_src: LazySrcLoc = .{ .node_offset_init_ty = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.ArrayInit, inst_data.payload_index).data;
    const ty = try sema.resolveType(block, ty_src, extra.ty);

    switch (ty.zigTypeTag()) {
        .Array => {
            const array_len = ty.arrayLen();
            if (extra.init_count != array_len) {
                return sema.fail(block, src, "expected {d} array elements; found {d}", .{
                    array_len, extra.init_count,
                });
            }
            return;
        },
        .Vector => {
            const array_len = ty.arrayLen();
            if (extra.init_count != array_len) {
                return sema.fail(block, src, "expected {d} vector elements; found {d}", .{
                    array_len, extra.init_count,
                });
            }
            return;
        },
        .Struct => if (ty.isTuple()) {
            _ = try sema.resolveTypeFields(ty);
            const array_len = ty.arrayLen();
            if (extra.init_count > array_len) {
                return sema.fail(block, src, "expected at most {d} tuple fields; found {d}", .{
                    array_len, extra.init_count,
                });
            }
            return;
        },
        else => {},
    }
    return sema.failWithArrayInitNotSupported(block, ty_src, ty);
}

fn validateStructInitTy(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ty = try sema.resolveType(block, src, inst_data.operand);

    switch (ty.zigTypeTag()) {
        .Struct, .Union => return,
        else => {},
    }
    return sema.failWithStructInitNotSupported(block, src, ty);
}

fn zirValidateStructInit(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_comptime: bool,
) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const validate_inst = sema.code.instructions.items(.data)[inst].pl_node;
    const init_src = validate_inst.src();
    const validate_extra = sema.code.extraData(Zir.Inst.Block, validate_inst.payload_index);
    const instrs = sema.code.extra[validate_extra.end..][0..validate_extra.data.body_len];
    const field_ptr_data = sema.code.instructions.items(.data)[instrs[0]].pl_node;
    const field_ptr_extra = sema.code.extraData(Zir.Inst.Field, field_ptr_data.payload_index).data;
    const object_ptr = try sema.resolveInst(field_ptr_extra.lhs);
    const agg_ty = sema.typeOf(object_ptr).childType();
    switch (agg_ty.zigTypeTag()) {
        .Struct => return sema.validateStructInit(
            block,
            agg_ty,
            init_src,
            instrs,
            is_comptime,
        ),
        .Union => return sema.validateUnionInit(
            block,
            agg_ty,
            init_src,
            instrs,
            object_ptr,
            is_comptime,
        ),
        else => unreachable,
    }
}

fn validateUnionInit(
    sema: *Sema,
    block: *Block,
    union_ty: Type,
    init_src: LazySrcLoc,
    instrs: []const Zir.Inst.Index,
    union_ptr: Air.Inst.Ref,
    is_comptime: bool,
) CompileError!void {
    if (instrs.len != 1) {
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                init_src,
                "cannot initialize multiple union fields at once, unions can only have one active field",
                .{},
            );
            errdefer msg.destroy(sema.gpa);

            for (instrs[1..]) |inst| {
                const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
                const inst_src: LazySrcLoc = .{ .node_offset_initializer = inst_data.src_node };
                try sema.errNote(block, inst_src, msg, "additional initializer here", .{});
            }
            try sema.addDeclaredHereNote(msg, union_ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    if ((is_comptime or block.is_comptime) and
        (try sema.resolveDefinedValue(block, init_src, union_ptr)) != null)
    {
        // In this case, comptime machinery already did everything. No work to do here.
        return;
    }

    const field_ptr = instrs[0];
    const field_ptr_data = sema.code.instructions.items(.data)[field_ptr].pl_node;
    const field_src: LazySrcLoc = .{ .node_offset_initializer = field_ptr_data.src_node };
    const field_ptr_extra = sema.code.extraData(Zir.Inst.Field, field_ptr_data.payload_index).data;
    const field_name = sema.code.nullTerminatedString(field_ptr_extra.field_name_start);
    // Validate the field access but ignore the index since we want the tag enum field index.
    _ = try sema.unionFieldIndex(block, union_ty, field_name, field_src);
    const air_tags = sema.air_instructions.items(.tag);
    const air_datas = sema.air_instructions.items(.data);
    const field_ptr_air_ref = sema.inst_map.get(field_ptr).?;
    const field_ptr_air_inst = Air.refToIndex(field_ptr_air_ref).?;

    // Our task here is to determine if the union is comptime-known. In such case,
    // we erase the runtime AIR instructions for initializing the union, and replace
    // the mapping with the comptime value. Either way, we will need to populate the tag.

    // We expect to see something like this in the current block AIR:
    //   %a = alloc(*const U)
    //   %b = bitcast(*U, %a)
    //   %c = field_ptr(..., %b)
    //   %e!= store(%c!, %d!)
    // If %d is a comptime operand, the union is comptime.
    // If the union is comptime, we want `first_block_index`
    // to point at %c so that the bitcast becomes the last instruction in the block.
    //
    // In the case of a comptime-known pointer to a union, the
    // the field_ptr instruction is missing, so we have to pattern-match
    // based only on the store instructions.
    // `first_block_index` needs to point to the `field_ptr` if it exists;
    // the `store` otherwise.
    //
    // It's also possible for there to be no store instruction, in the case
    // of nested `coerce_result_ptr` instructions. If we see the `field_ptr`
    // but we have not found a `store`, treat as a runtime-known field.
    var first_block_index = block.instructions.items.len;
    var block_index = block.instructions.items.len - 1;
    var init_val: ?Value = null;
    var make_runtime = false;
    while (block_index > 0) : (block_index -= 1) {
        const store_inst = block.instructions.items[block_index];
        if (store_inst == field_ptr_air_inst) break;
        if (air_tags[store_inst] != .store) continue;
        const bin_op = air_datas[store_inst].bin_op;
        var lhs = bin_op.lhs;
        if (Air.refToIndex(lhs)) |lhs_index| {
            if (air_tags[lhs_index] == .bitcast) {
                lhs = air_datas[lhs_index].ty_op.operand;
                block_index -= 1;
            }
        }
        if (lhs != field_ptr_air_ref) continue;
        while (block_index > 0) : (block_index -= 1) {
            const block_inst = block.instructions.items[block_index - 1];
            if (air_tags[block_inst] != .dbg_stmt) break;
        }
        if (block_index > 0 and
            field_ptr_air_inst == block.instructions.items[block_index - 1])
        {
            first_block_index = @min(first_block_index, block_index - 1);
        } else {
            first_block_index = @min(first_block_index, block_index);
        }
        init_val = try sema.resolveMaybeUndefValAllowVariablesMaybeRuntime(bin_op.rhs, &make_runtime);
        break;
    }

    const tag_ty = union_ty.unionTagTypeHypothetical();
    const enum_field_index = @intCast(u32, tag_ty.enumFieldIndex(field_name).?);
    const tag_val = try Value.Tag.enum_field_index.create(sema.arena, enum_field_index);

    if (init_val) |val| {
        // Our task is to delete all the `field_ptr` and `store` instructions, and insert
        // instead a single `store` to the result ptr with a comptime union value.
        block.instructions.shrinkRetainingCapacity(first_block_index);

        var union_val = try Value.Tag.@"union".create(sema.arena, .{
            .tag = tag_val,
            .val = val,
        });
        if (make_runtime) union_val = try Value.Tag.runtime_value.create(sema.arena, union_val);
        const union_init = try sema.addConstant(union_ty, union_val);
        try sema.storePtr2(block, init_src, union_ptr, init_src, union_init, init_src, .store);
        return;
    } else if (try sema.typeRequiresComptime(union_ty)) {
        return sema.failWithNeededComptime(block, field_ptr_data.src(), "initializer of comptime only union must be comptime-known");
    }

    const new_tag = try sema.addConstant(tag_ty, tag_val);
    _ = try block.addBinOp(.set_union_tag, union_ptr, new_tag);
}

fn validateStructInit(
    sema: *Sema,
    block: *Block,
    struct_ty: Type,
    init_src: LazySrcLoc,
    instrs: []const Zir.Inst.Index,
    is_comptime: bool,
) CompileError!void {
    const gpa = sema.gpa;

    // Maps field index to field_ptr index of where it was already initialized.
    const found_fields = try gpa.alloc(Zir.Inst.Index, struct_ty.structFieldCount());
    defer gpa.free(found_fields);
    mem.set(Zir.Inst.Index, found_fields, 0);

    var struct_ptr_zir_ref: Zir.Inst.Ref = undefined;

    for (instrs) |field_ptr| {
        const field_ptr_data = sema.code.instructions.items(.data)[field_ptr].pl_node;
        const field_src: LazySrcLoc = .{ .node_offset_initializer = field_ptr_data.src_node };
        const field_ptr_extra = sema.code.extraData(Zir.Inst.Field, field_ptr_data.payload_index).data;
        struct_ptr_zir_ref = field_ptr_extra.lhs;
        const field_name = sema.code.nullTerminatedString(field_ptr_extra.field_name_start);
        const field_index = if (struct_ty.isTuple())
            try sema.tupleFieldIndex(block, struct_ty, field_name, field_src)
        else
            try sema.structFieldIndex(block, struct_ty, field_name, field_src);
        if (found_fields[field_index] != 0) {
            const other_field_ptr = found_fields[field_index];
            const other_field_ptr_data = sema.code.instructions.items(.data)[other_field_ptr].pl_node;
            const other_field_src: LazySrcLoc = .{ .node_offset_initializer = other_field_ptr_data.src_node };
            const msg = msg: {
                const msg = try sema.errMsg(block, field_src, "duplicate field", .{});
                errdefer msg.destroy(gpa);
                try sema.errNote(block, other_field_src, msg, "other field here", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        found_fields[field_index] = field_ptr;
    }

    var root_msg: ?*Module.ErrorMsg = null;
    errdefer if (root_msg) |msg| msg.destroy(sema.gpa);

    const struct_ptr = try sema.resolveInst(struct_ptr_zir_ref);
    if ((is_comptime or block.is_comptime) and
        (try sema.resolveDefinedValue(block, init_src, struct_ptr)) != null)
    {
        try sema.resolveStructLayout(struct_ty);
        // In this case the only thing we need to do is evaluate the implicit
        // store instructions for default field values, and report any missing fields.
        // Avoid the cost of the extra machinery for detecting a comptime struct init value.
        for (found_fields, 0..) |field_ptr, i| {
            if (field_ptr != 0) continue;

            const default_val = struct_ty.structFieldDefaultValue(i);
            if (default_val.tag() == .unreachable_value) {
                if (struct_ty.isTuple()) {
                    const template = "missing tuple field with index {d}";
                    if (root_msg) |msg| {
                        try sema.errNote(block, init_src, msg, template, .{i});
                    } else {
                        root_msg = try sema.errMsg(block, init_src, template, .{i});
                    }
                    continue;
                }
                const field_name = struct_ty.structFieldName(i);
                const template = "missing struct field: {s}";
                const args = .{field_name};
                if (root_msg) |msg| {
                    try sema.errNote(block, init_src, msg, template, args);
                } else {
                    root_msg = try sema.errMsg(block, init_src, template, args);
                }
                continue;
            }

            const field_src = init_src; // TODO better source location
            const default_field_ptr = if (struct_ty.isTuple())
                try sema.tupleFieldPtr(block, init_src, struct_ptr, field_src, @intCast(u32, i), true)
            else
                try sema.structFieldPtrByIndex(block, init_src, struct_ptr, @intCast(u32, i), field_src, struct_ty, true);
            const field_ty = sema.typeOf(default_field_ptr).childType();
            const init = try sema.addConstant(field_ty, default_val);
            try sema.storePtr2(block, init_src, default_field_ptr, init_src, init, field_src, .store);
        }

        if (root_msg) |msg| {
            if (struct_ty.castTag(.@"struct")) |struct_obj| {
                const mod = sema.mod;
                const fqn = try struct_obj.data.getFullyQualifiedName(mod);
                defer gpa.free(fqn);
                try mod.errNoteNonLazy(
                    struct_obj.data.srcLoc(mod),
                    msg,
                    "struct '{s}' declared here",
                    .{fqn},
                );
            }
            root_msg = null;
            return sema.failWithOwnedErrorMsg(msg);
        }

        return;
    }

    var struct_is_comptime = true;
    var first_block_index = block.instructions.items.len;
    var make_runtime = false;

    const require_comptime = try sema.typeRequiresComptime(struct_ty);
    const air_tags = sema.air_instructions.items(.tag);
    const air_datas = sema.air_instructions.items(.data);

    // We collect the comptime field values in case the struct initialization
    // ends up being comptime-known.
    const field_values = try sema.arena.alloc(Value, struct_ty.structFieldCount());

    field: for (found_fields, 0..) |field_ptr, i| {
        if (field_ptr != 0) {
            // Determine whether the value stored to this pointer is comptime-known.
            const field_ty = struct_ty.structFieldType(i);
            if (try sema.typeHasOnePossibleValue(field_ty)) |opv| {
                field_values[i] = opv;
                continue;
            }

            const field_ptr_air_ref = sema.inst_map.get(field_ptr).?;
            const field_ptr_air_inst = Air.refToIndex(field_ptr_air_ref).?;

            //std.debug.print("validateStructInit (field_ptr_air_inst=%{d}):\n", .{
            //    field_ptr_air_inst,
            //});
            //for (block.instructions.items) |item| {
            //    std.debug.print("  %{d} = {s}\n", .{item, @tagName(air_tags[item])});
            //}

            // We expect to see something like this in the current block AIR:
            //   %a = field_ptr(...)
            //   store(%a, %b)
            // With an optional bitcast between the store and the field_ptr.
            // If %b is a comptime operand, this field is comptime.
            //
            // However, in the case of a comptime-known pointer to a struct, the
            // the field_ptr instruction is missing, so we have to pattern-match
            // based only on the store instructions.
            // `first_block_index` needs to point to the `field_ptr` if it exists;
            // the `store` otherwise.
            //
            // It's also possible for there to be no store instruction, in the case
            // of nested `coerce_result_ptr` instructions. If we see the `field_ptr`
            // but we have not found a `store`, treat as a runtime-known field.

            // Possible performance enhancement: save the `block_index` between iterations
            // of the for loop.
            var block_index = block.instructions.items.len - 1;
            while (block_index > 0) : (block_index -= 1) {
                const store_inst = block.instructions.items[block_index];
                if (store_inst == field_ptr_air_inst) {
                    struct_is_comptime = false;
                    continue :field;
                }
                if (air_tags[store_inst] != .store) continue;
                const bin_op = air_datas[store_inst].bin_op;
                var lhs = bin_op.lhs;
                {
                    const lhs_index = Air.refToIndex(lhs) orelse continue;
                    if (air_tags[lhs_index] == .bitcast) {
                        lhs = air_datas[lhs_index].ty_op.operand;
                        block_index -= 1;
                    }
                }
                if (lhs != field_ptr_air_ref) continue;
                while (block_index > 0) : (block_index -= 1) {
                    const block_inst = block.instructions.items[block_index - 1];
                    if (air_tags[block_inst] != .dbg_stmt) break;
                }
                if (block_index > 0 and
                    field_ptr_air_inst == block.instructions.items[block_index - 1])
                {
                    first_block_index = @min(first_block_index, block_index - 1);
                } else {
                    first_block_index = @min(first_block_index, block_index);
                }
                if (try sema.resolveMaybeUndefValAllowVariablesMaybeRuntime(bin_op.rhs, &make_runtime)) |val| {
                    field_values[i] = val;
                } else if (require_comptime) {
                    const field_ptr_data = sema.code.instructions.items(.data)[field_ptr].pl_node;
                    return sema.failWithNeededComptime(block, field_ptr_data.src(), "initializer of comptime only struct must be comptime-known");
                } else {
                    struct_is_comptime = false;
                }
                continue :field;
            }
            struct_is_comptime = false;
            continue :field;
        }

        const default_val = struct_ty.structFieldDefaultValue(i);
        if (default_val.tag() == .unreachable_value) {
            if (struct_ty.isTuple()) {
                const template = "missing tuple field with index {d}";
                if (root_msg) |msg| {
                    try sema.errNote(block, init_src, msg, template, .{i});
                } else {
                    root_msg = try sema.errMsg(block, init_src, template, .{i});
                }
                continue;
            }
            const field_name = struct_ty.structFieldName(i);
            const template = "missing struct field: {s}";
            const args = .{field_name};
            if (root_msg) |msg| {
                try sema.errNote(block, init_src, msg, template, args);
            } else {
                root_msg = try sema.errMsg(block, init_src, template, args);
            }
            continue;
        }
        field_values[i] = default_val;
    }

    if (root_msg) |msg| {
        if (struct_ty.castTag(.@"struct")) |struct_obj| {
            const fqn = try struct_obj.data.getFullyQualifiedName(sema.mod);
            defer gpa.free(fqn);
            try sema.mod.errNoteNonLazy(
                struct_obj.data.srcLoc(sema.mod),
                msg,
                "struct '{s}' declared here",
                .{fqn},
            );
        }
        root_msg = null;
        return sema.failWithOwnedErrorMsg(msg);
    }

    if (struct_is_comptime) {
        // Our task is to delete all the `field_ptr` and `store` instructions, and insert
        // instead a single `store` to the struct_ptr with a comptime struct value.

        block.instructions.shrinkRetainingCapacity(first_block_index);
        var struct_val = try Value.Tag.aggregate.create(sema.arena, field_values);
        if (make_runtime) struct_val = try Value.Tag.runtime_value.create(sema.arena, struct_val);
        const struct_init = try sema.addConstant(struct_ty, struct_val);
        try sema.storePtr2(block, init_src, struct_ptr, init_src, struct_init, init_src, .store);
        return;
    }
    try sema.resolveStructLayout(struct_ty);

    // Our task is to insert `store` instructions for all the default field values.
    for (found_fields, 0..) |field_ptr, i| {
        if (field_ptr != 0) continue;

        const field_src = init_src; // TODO better source location
        const default_field_ptr = if (struct_ty.isTuple())
            try sema.tupleFieldPtr(block, init_src, struct_ptr, field_src, @intCast(u32, i), true)
        else
            try sema.structFieldPtrByIndex(block, init_src, struct_ptr, @intCast(u32, i), field_src, struct_ty, true);
        const field_ty = sema.typeOf(default_field_ptr).childType();
        const init = try sema.addConstant(field_ty, field_values[i]);
        try sema.storePtr2(block, init_src, default_field_ptr, init_src, init, field_src, .store);
    }
}

fn zirValidateArrayInit(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_comptime: bool,
) CompileError!void {
    const validate_inst = sema.code.instructions.items(.data)[inst].pl_node;
    const init_src = validate_inst.src();
    const validate_extra = sema.code.extraData(Zir.Inst.Block, validate_inst.payload_index);
    const instrs = sema.code.extra[validate_extra.end..][0..validate_extra.data.body_len];
    const first_elem_ptr_data = sema.code.instructions.items(.data)[instrs[0]].pl_node;
    const elem_ptr_extra = sema.code.extraData(Zir.Inst.ElemPtrImm, first_elem_ptr_data.payload_index).data;
    const array_ptr = try sema.resolveInst(elem_ptr_extra.ptr);
    const array_ty = sema.typeOf(array_ptr).childType();
    const array_len = array_ty.arrayLen();

    if (instrs.len != array_len) switch (array_ty.zigTypeTag()) {
        .Struct => {
            var root_msg: ?*Module.ErrorMsg = null;
            errdefer if (root_msg) |msg| msg.destroy(sema.gpa);

            var i = instrs.len;
            while (i < array_len) : (i += 1) {
                const default_val = array_ty.structFieldDefaultValue(i);
                if (default_val.tag() == .unreachable_value) {
                    const template = "missing tuple field with index {d}";
                    if (root_msg) |msg| {
                        try sema.errNote(block, init_src, msg, template, .{i});
                    } else {
                        root_msg = try sema.errMsg(block, init_src, template, .{i});
                    }
                }
            }

            if (root_msg) |msg| {
                root_msg = null;
                return sema.failWithOwnedErrorMsg(msg);
            }
        },
        .Array => {
            return sema.fail(block, init_src, "expected {d} array elements; found {d}", .{
                array_len, instrs.len,
            });
        },
        .Vector => {
            return sema.fail(block, init_src, "expected {d} vector elements; found {d}", .{
                array_len, instrs.len,
            });
        },
        else => unreachable,
    };

    if ((is_comptime or block.is_comptime) and
        (try sema.resolveDefinedValue(block, init_src, array_ptr)) != null)
    {
        // In this case the comptime machinery will have evaluated the store instructions
        // at comptime so we have almost nothing to do here. However, in case of a
        // sentinel-terminated array, the sentinel will not have been populated by
        // any ZIR instructions at comptime; we need to do that here.
        if (array_ty.sentinel()) |sentinel_val| {
            const array_len_ref = try sema.addIntUnsigned(Type.usize, array_len);
            const sentinel_ptr = try sema.elemPtrArray(block, init_src, init_src, array_ptr, init_src, array_len_ref, true, true);
            const sentinel = try sema.addConstant(array_ty.childType(), sentinel_val);
            try sema.storePtr2(block, init_src, sentinel_ptr, init_src, sentinel, init_src, .store);
        }
        return;
    }

    var array_is_comptime = true;
    var first_block_index = block.instructions.items.len;
    var make_runtime = false;

    // Collect the comptime element values in case the array literal ends up
    // being comptime-known.
    const array_len_s = try sema.usizeCast(block, init_src, array_ty.arrayLenIncludingSentinel());
    const element_vals = try sema.arena.alloc(Value, array_len_s);
    const opt_opv = try sema.typeHasOnePossibleValue(array_ty);
    const air_tags = sema.air_instructions.items(.tag);
    const air_datas = sema.air_instructions.items(.data);

    outer: for (instrs, 0..) |elem_ptr, i| {
        // Determine whether the value stored to this pointer is comptime-known.

        if (array_ty.isTuple()) {
            if (array_ty.structFieldValueComptime(i)) |opv| {
                element_vals[i] = opv;
                continue;
            }
        } else {
            // Array has one possible value, so value is always comptime-known
            if (opt_opv) |opv| {
                element_vals[i] = opv;
                continue;
            }
        }

        const elem_ptr_air_ref = sema.inst_map.get(elem_ptr).?;
        const elem_ptr_air_inst = Air.refToIndex(elem_ptr_air_ref).?;

        // We expect to see something like this in the current block AIR:
        //   %a = elem_ptr(...)
        //   store(%a, %b)
        // With an optional bitcast between the store and the elem_ptr.
        // If %b is a comptime operand, this element is comptime.
        //
        // However, in the case of a comptime-known pointer to an array, the
        // the elem_ptr instruction is missing, so we have to pattern-match
        // based only on the store instructions.
        // `first_block_index` needs to point to the `elem_ptr` if it exists;
        // the `store` otherwise.
        //
        // It's also possible for there to be no store instruction, in the case
        // of nested `coerce_result_ptr` instructions. If we see the `elem_ptr`
        // but we have not found a `store`, treat as a runtime-known element.
        //
        // This is nearly identical to similar logic in `validateStructInit`.

        // Possible performance enhancement: save the `block_index` between iterations
        // of the for loop.
        var block_index = block.instructions.items.len - 1;
        while (block_index > 0) : (block_index -= 1) {
            const store_inst = block.instructions.items[block_index];
            if (store_inst == elem_ptr_air_inst) {
                array_is_comptime = false;
                continue :outer;
            }
            if (air_tags[store_inst] != .store) continue;
            const bin_op = air_datas[store_inst].bin_op;
            var lhs = bin_op.lhs;
            {
                const lhs_index = Air.refToIndex(lhs) orelse continue;
                if (air_tags[lhs_index] == .bitcast) {
                    lhs = air_datas[lhs_index].ty_op.operand;
                    block_index -= 1;
                }
            }
            if (lhs != elem_ptr_air_ref) continue;
            while (block_index > 0) : (block_index -= 1) {
                const block_inst = block.instructions.items[block_index - 1];
                if (air_tags[block_inst] != .dbg_stmt) break;
            }
            if (block_index > 0 and
                elem_ptr_air_inst == block.instructions.items[block_index - 1])
            {
                first_block_index = @min(first_block_index, block_index - 1);
            } else {
                first_block_index = @min(first_block_index, block_index);
            }
            if (try sema.resolveMaybeUndefValAllowVariablesMaybeRuntime(bin_op.rhs, &make_runtime)) |val| {
                element_vals[i] = val;
            } else {
                array_is_comptime = false;
            }
            continue :outer;
        }
        array_is_comptime = false;
        continue :outer;
    }

    if (array_is_comptime) {
        if (try sema.resolveDefinedValue(block, init_src, array_ptr)) |ptr_val| {
            if (ptr_val.tag() == .comptime_field_ptr) {
                // This store was validated by the individual elem ptrs.
                return;
            }
        }

        // Our task is to delete all the `elem_ptr` and `store` instructions, and insert
        // instead a single `store` to the array_ptr with a comptime struct value.
        // Also to populate the sentinel value, if any.
        if (array_ty.sentinel()) |sentinel_val| {
            element_vals[instrs.len] = sentinel_val;
        }

        block.instructions.shrinkRetainingCapacity(first_block_index);

        var array_val = try Value.Tag.aggregate.create(sema.arena, element_vals);
        if (make_runtime) array_val = try Value.Tag.runtime_value.create(sema.arena, array_val);
        const array_init = try sema.addConstant(array_ty, array_val);
        try sema.storePtr2(block, init_src, array_ptr, init_src, array_init, init_src, .store);
    }
}

fn zirValidateDeref(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);

    if (operand_ty.zigTypeTag() != .Pointer) {
        return sema.fail(block, src, "cannot dereference non-pointer type '{}'", .{operand_ty.fmt(sema.mod)});
    } else switch (operand_ty.ptrSize()) {
        .One, .C => {},
        .Many => return sema.fail(block, src, "index syntax required for unknown-length pointer type '{}'", .{operand_ty.fmt(sema.mod)}),
        .Slice => return sema.fail(block, src, "index syntax required for slice type '{}'", .{operand_ty.fmt(sema.mod)}),
    }

    const elem_ty = operand_ty.elemType2();
    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        if (val.isUndef()) {
            return sema.fail(block, src, "cannot dereference undefined value", .{});
        }
    } else if (!(try sema.validateRunTimeType(elem_ty, false))) {
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                src,
                "values of type '{}' must be comptime-known, but operand value is runtime-known",
                .{elem_ty.fmt(sema.mod)},
            );
            errdefer msg.destroy(sema.gpa);

            const src_decl = sema.mod.declPtr(block.src_decl);
            try sema.explainWhyTypeIsComptime(block, src, msg, src.toSrcLoc(src_decl), elem_ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
}

fn failWithBadMemberAccess(
    sema: *Sema,
    block: *Block,
    agg_ty: Type,
    field_src: LazySrcLoc,
    field_name: []const u8,
) CompileError {
    const kw_name = switch (agg_ty.zigTypeTag()) {
        .Union => "union",
        .Struct => "struct",
        .Opaque => "opaque",
        .Enum => "enum",
        else => unreachable,
    };
    if (agg_ty.getOwnerDeclOrNull()) |some| if (sema.mod.declIsRoot(some)) {
        return sema.fail(block, field_src, "root struct of file '{}' has no member named '{s}'", .{
            agg_ty.fmt(sema.mod), field_name,
        });
    };
    const msg = msg: {
        const msg = try sema.errMsg(block, field_src, "{s} '{}' has no member named '{s}'", .{
            kw_name, agg_ty.fmt(sema.mod), field_name,
        });
        errdefer msg.destroy(sema.gpa);
        try sema.addDeclaredHereNote(msg, agg_ty);
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn failWithBadStructFieldAccess(
    sema: *Sema,
    block: *Block,
    struct_obj: *Module.Struct,
    field_src: LazySrcLoc,
    field_name: []const u8,
) CompileError {
    const gpa = sema.gpa;

    const fqn = try struct_obj.getFullyQualifiedName(sema.mod);
    defer gpa.free(fqn);

    const msg = msg: {
        const msg = try sema.errMsg(
            block,
            field_src,
            "no field named '{s}' in struct '{s}'",
            .{ field_name, fqn },
        );
        errdefer msg.destroy(gpa);
        try sema.mod.errNoteNonLazy(struct_obj.srcLoc(sema.mod), msg, "struct declared here", .{});
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn failWithBadUnionFieldAccess(
    sema: *Sema,
    block: *Block,
    union_obj: *Module.Union,
    field_src: LazySrcLoc,
    field_name: []const u8,
) CompileError {
    const gpa = sema.gpa;

    const fqn = try union_obj.getFullyQualifiedName(sema.mod);
    defer gpa.free(fqn);

    const msg = msg: {
        const msg = try sema.errMsg(
            block,
            field_src,
            "no field named '{s}' in union '{s}'",
            .{ field_name, fqn },
        );
        errdefer msg.destroy(gpa);
        try sema.mod.errNoteNonLazy(union_obj.srcLoc(sema.mod), msg, "union declared here", .{});
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn addDeclaredHereNote(sema: *Sema, parent: *Module.ErrorMsg, decl_ty: Type) !void {
    const src_loc = decl_ty.declSrcLocOrNull(sema.mod) orelse return;
    const category = switch (decl_ty.zigTypeTag()) {
        .Union => "union",
        .Struct => "struct",
        .Enum => "enum",
        .Opaque => "opaque",
        .ErrorSet => "error set",
        else => unreachable,
    };
    try sema.mod.errNoteNonLazy(src_loc, parent, "{s} declared here", .{category});
}

fn zirStoreToBlockPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const bin_inst = sema.code.instructions.items(.data)[inst].bin;
    const ptr = sema.inst_map.get(Zir.refToIndex(bin_inst.lhs).?) orelse {
        // This is an elided instruction, but AstGen was unable to omit it.
        return;
    };
    const operand = try sema.resolveInst(bin_inst.rhs);
    const src: LazySrcLoc = sema.src;
    blk: {
        const ptr_inst = Air.refToIndex(ptr) orelse break :blk;
        if (sema.air_instructions.items(.tag)[ptr_inst] != .constant) break :blk;
        const air_datas = sema.air_instructions.items(.data);
        const ptr_val = sema.air_values.items[air_datas[ptr_inst].ty_pl.payload];
        switch (ptr_val.tag()) {
            .inferred_alloc_comptime => {
                const iac = ptr_val.castTag(.inferred_alloc_comptime).?;
                return sema.storeToInferredAllocComptime(block, src, operand, iac);
            },
            .inferred_alloc => {
                const inferred_alloc = ptr_val.castTag(.inferred_alloc).?;
                return sema.storeToInferredAlloc(block, src, ptr, operand, inferred_alloc);
            },
            else => break :blk,
        }
    }

    return sema.storePtr(block, src, ptr, operand);
}

fn zirStoreToInferredPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const src: LazySrcLoc = sema.src;
    const bin_inst = sema.code.instructions.items(.data)[inst].bin;
    const ptr = try sema.resolveInst(bin_inst.lhs);
    const operand = try sema.resolveInst(bin_inst.rhs);
    const ptr_inst = Air.refToIndex(ptr).?;
    assert(sema.air_instructions.items(.tag)[ptr_inst] == .constant);
    const air_datas = sema.air_instructions.items(.data);
    const ptr_val = sema.air_values.items[air_datas[ptr_inst].ty_pl.payload];

    switch (ptr_val.tag()) {
        .inferred_alloc_comptime => {
            const iac = ptr_val.castTag(.inferred_alloc_comptime).?;
            return sema.storeToInferredAllocComptime(block, src, operand, iac);
        },
        .inferred_alloc => {
            const inferred_alloc = ptr_val.castTag(.inferred_alloc).?;
            return sema.storeToInferredAlloc(block, src, ptr, operand, inferred_alloc);
        },
        else => unreachable,
    }
}

fn storeToInferredAlloc(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    ptr: Air.Inst.Ref,
    operand: Air.Inst.Ref,
    inferred_alloc: *Value.Payload.InferredAlloc,
) CompileError!void {
    const operand_ty = sema.typeOf(operand);
    // Create a runtime bitcast instruction with exactly the type the pointer wants.
    const target = sema.mod.getTarget();
    const ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = operand_ty,
        .@"align" = inferred_alloc.data.alignment,
        .@"addrspace" = target_util.defaultAddressSpace(target, .local),
    });
    const bitcasted_ptr = try block.addBitCast(ptr_ty, ptr);
    // Add the stored instruction to the set we will use to resolve peer types
    // for the inferred allocation.
    try inferred_alloc.data.prongs.append(sema.arena, .{
        .stored_inst = operand,
        .placeholder = Air.refToIndex(bitcasted_ptr).?,
    });
    return sema.storePtr2(block, src, bitcasted_ptr, src, operand, src, .bitcast);
}

fn storeToInferredAllocComptime(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    operand: Air.Inst.Ref,
    iac: *Value.Payload.InferredAllocComptime,
) CompileError!void {
    const operand_ty = sema.typeOf(operand);
    // There will be only one store_to_inferred_ptr because we are running at comptime.
    // The alloc will turn into a Decl.
    if (try sema.resolveMaybeUndefValAllowVariables(operand)) |operand_val| store: {
        if (operand_val.tag() == .variable) break :store;
        var anon_decl = try block.startAnonDecl();
        defer anon_decl.deinit();
        iac.data.decl_index = try anon_decl.finish(
            try operand_ty.copy(anon_decl.arena()),
            try operand_val.copy(anon_decl.arena()),
            iac.data.alignment,
        );
        return;
    }

    return sema.failWithNeededComptime(block, src, "value being stored to a comptime variable must be comptime-known");
}

fn zirSetEvalBranchQuota(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const quota = @intCast(u32, try sema.resolveInt(block, src, inst_data.operand, Type.u32, "eval branch quota must be comptime-known"));
    sema.branch_quota = @max(sema.branch_quota, quota);
}

fn zirStore(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const bin_inst = sema.code.instructions.items(.data)[inst].bin;
    const ptr = try sema.resolveInst(bin_inst.lhs);
    const value = try sema.resolveInst(bin_inst.rhs);
    return sema.storePtr(block, sema.src, ptr, value);
}

fn zirStoreNode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const zir_tags = sema.code.instructions.items(.tag);
    const zir_datas = sema.code.instructions.items(.data);
    const inst_data = zir_datas[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const ptr = try sema.resolveInst(extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);

    const is_ret = if (Zir.refToIndex(extra.lhs)) |ptr_index|
        zir_tags[ptr_index] == .ret_ptr
    else
        false;

    // Check for the possibility of this pattern:
    //   %a = ret_ptr
    //   %b = store(%a, %c)
    // Where %c is an error union or error set. In such case we need to add
    // to the current function's inferred error set, if any.
    if (is_ret and (sema.typeOf(operand).zigTypeTag() == .ErrorUnion or
        sema.typeOf(operand).zigTypeTag() == .ErrorSet) and
        sema.fn_ret_ty.zigTypeTag() == .ErrorUnion)
    {
        try sema.addToInferredErrorSet(operand);
    }

    const ptr_src: LazySrcLoc = .{ .node_offset_store_ptr = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_store_operand = inst_data.src_node };
    const air_tag: Air.Inst.Tag = if (is_ret) .ret_ptr else .store;
    return sema.storePtr2(block, src, ptr, ptr_src, operand, operand_src, air_tag);
}

fn zirStr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const bytes = sema.code.instructions.items(.data)[inst].str.get(sema.code);
    return sema.addStrLit(block, bytes);
}

fn addStrLit(sema: *Sema, block: *Block, zir_bytes: []const u8) CompileError!Air.Inst.Ref {
    // `zir_bytes` references memory inside the ZIR module, which can get deallocated
    // after semantic analysis is complete, for example in the case of the initialization
    // expression of a variable declaration.
    const mod = sema.mod;
    const gpa = sema.gpa;
    const string_bytes = &mod.string_literal_bytes;
    const StringLiteralAdapter = Module.StringLiteralAdapter;
    const StringLiteralContext = Module.StringLiteralContext;
    try string_bytes.ensureUnusedCapacity(gpa, zir_bytes.len);
    const gop = try mod.string_literal_table.getOrPutContextAdapted(gpa, zir_bytes, StringLiteralAdapter{
        .bytes = string_bytes,
    }, StringLiteralContext{
        .bytes = string_bytes,
    });
    if (!gop.found_existing) {
        gop.key_ptr.* = .{
            .index = @intCast(u32, string_bytes.items.len),
            .len = @intCast(u32, zir_bytes.len),
        };
        string_bytes.appendSliceAssumeCapacity(zir_bytes);
        gop.value_ptr.* = .none;
    }
    const decl_index = gop.value_ptr.unwrap() orelse di: {
        var anon_decl = try block.startAnonDecl();
        defer anon_decl.deinit();

        const decl_index = try anon_decl.finish(
            try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), gop.key_ptr.len),
            try Value.Tag.str_lit.create(anon_decl.arena(), gop.key_ptr.*),
            0, // default alignment
        );

        // Needed so that `Decl.clearValues` will additionally set the corresponding
        // string literal table value back to `Decl.OptionalIndex.none`.
        mod.declPtr(decl_index).owns_tv = true;

        gop.value_ptr.* = decl_index.toOptional();
        break :di decl_index;
    };
    return sema.analyzeDeclRef(decl_index);
}

fn zirInt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const tracy = trace(@src());
    defer tracy.end();

    const int = sema.code.instructions.items(.data)[inst].int;
    return sema.addIntUnsigned(Type.initTag(.comptime_int), int);
}

fn zirIntBig(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const tracy = trace(@src());
    defer tracy.end();

    const arena = sema.arena;
    const int = sema.code.instructions.items(.data)[inst].str;
    const byte_count = int.len * @sizeOf(std.math.big.Limb);
    const limb_bytes = sema.code.string_bytes[int.start..][0..byte_count];
    const limbs = try arena.alloc(std.math.big.Limb, int.len);
    mem.copy(u8, mem.sliceAsBytes(limbs), limb_bytes);

    return sema.addConstant(
        Type.initTag(.comptime_int),
        try Value.Tag.int_big_positive.create(arena, limbs),
    );
}

fn zirFloat(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const arena = sema.arena;
    const number = sema.code.instructions.items(.data)[inst].float;
    return sema.addConstant(
        Type.initTag(.comptime_float),
        try Value.Tag.float_64.create(arena, number),
    );
}

fn zirFloat128(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const arena = sema.arena;
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Float128, inst_data.payload_index).data;
    const number = extra.get();
    return sema.addConstant(
        Type.initTag(.comptime_float),
        try Value.Tag.float_128.create(arena, number),
    );
}

fn zirCompileError(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const msg = try sema.resolveConstString(block, operand_src, inst_data.operand, "compile error string must be comptime-known");
    return sema.fail(block, src, "{s}", .{msg});
}

fn zirCompileLog(
    sema: *Sema,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    var managed = sema.mod.compile_log_text.toManaged(sema.gpa);
    defer sema.mod.compile_log_text = managed.moveToUnmanaged();
    const writer = managed.writer();

    const extra = sema.code.extraData(Zir.Inst.NodeMultiOp, extended.operand);
    const src_node = extra.data.src_node;
    const args = sema.code.refSlice(extra.end, extended.small);

    for (args, 0..) |arg_ref, i| {
        if (i != 0) try writer.print(", ", .{});

        const arg = try sema.resolveInst(arg_ref);
        const arg_ty = sema.typeOf(arg);
        if (try sema.resolveMaybeUndefVal(arg)) |val| {
            try sema.resolveLazyValue(val);
            try writer.print("@as({}, {})", .{
                arg_ty.fmt(sema.mod), val.fmtValue(arg_ty, sema.mod),
            });
        } else {
            try writer.print("@as({}, [runtime value])", .{arg_ty.fmt(sema.mod)});
        }
    }
    try writer.print("\n", .{});

    const decl_index = if (sema.func) |some| some.owner_decl else sema.owner_decl_index;
    const gop = try sema.mod.compile_log_decls.getOrPut(sema.gpa, decl_index);
    if (!gop.found_existing) {
        gop.value_ptr.* = src_node;
    }
    return Air.Inst.Ref.void_value;
}

fn zirPanic(sema: *Sema, block: *Block, inst: Zir.Inst.Index, force_comptime: bool) CompileError!Zir.Inst.Index {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const msg_inst = try sema.resolveInst(inst_data.operand);

    if (block.is_comptime or force_comptime) {
        return sema.fail(block, src, "encountered @panic at comptime", .{});
    }
    try sema.panicWithMsg(block, src, msg_inst);
    return always_noreturn;
}

fn zirTrap(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const src_node = sema.code.instructions.items(.data)[inst].node;
    const src = LazySrcLoc.nodeOffset(src_node);
    sema.src = src;
    _ = try block.addNoOp(.trap);
    return always_noreturn;
}

fn zirLoop(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Block, inst_data.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];
    const gpa = sema.gpa;

    // AIR expects a block outside the loop block too.
    // Reserve space for a Loop instruction so that generated Break instructions can
    // point to it, even if it doesn't end up getting used because the code ends up being
    // comptime evaluated.
    const block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
    const loop_inst = block_inst + 1;
    try sema.air_instructions.ensureUnusedCapacity(gpa, 2);
    sema.air_instructions.appendAssumeCapacity(.{
        .tag = .block,
        .data = undefined,
    });
    sema.air_instructions.appendAssumeCapacity(.{
        .tag = .loop,
        .data = .{ .ty_pl = .{
            .ty = .noreturn_type,
            .payload = undefined,
        } },
    });
    var label: Block.Label = .{
        .zir_block = inst,
        .merges = .{
            .results = .{},
            .br_list = .{},
            .block_inst = block_inst,
        },
    };
    var child_block = parent_block.makeSubBlock();
    child_block.label = &label;
    child_block.runtime_cond = null;
    child_block.runtime_loop = src;
    child_block.runtime_index.increment();
    const merges = &child_block.label.?.merges;

    defer child_block.instructions.deinit(gpa);
    defer merges.results.deinit(gpa);
    defer merges.br_list.deinit(gpa);

    var loop_block = child_block.makeSubBlock();
    defer loop_block.instructions.deinit(gpa);

    try sema.analyzeBody(&loop_block, body);

    try child_block.instructions.append(gpa, loop_inst);

    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.Block).Struct.fields.len +
        loop_block.instructions.items.len);
    sema.air_instructions.items(.data)[loop_inst].ty_pl.payload = sema.addExtraAssumeCapacity(
        Air.Block{ .body_len = @intCast(u32, loop_block.instructions.items.len) },
    );
    sema.air_extra.appendSliceAssumeCapacity(loop_block.instructions.items);
    return sema.analyzeBlockBody(parent_block, src, &child_block, merges);
}

fn zirCImport(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const pl_node = sema.code.instructions.items(.data)[inst].pl_node;
    const src = pl_node.src();
    const extra = sema.code.extraData(Zir.Inst.Block, pl_node.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];

    // we check this here to avoid undefined symbols
    if (!@import("build_options").have_llvm)
        return sema.fail(parent_block, src, "cannot do C import on Zig compiler not built with LLVM-extension", .{});

    var c_import_buf = std.ArrayList(u8).init(sema.gpa);
    defer c_import_buf.deinit();

    var comptime_reason: Block.ComptimeReason = .{ .c_import = .{
        .block = parent_block,
        .src = src,
    } };
    var child_block: Block = .{
        .parent = parent_block,
        .sema = sema,
        .src_decl = parent_block.src_decl,
        .namespace = parent_block.namespace,
        .wip_capture_scope = parent_block.wip_capture_scope,
        .instructions = .{},
        .inlining = parent_block.inlining,
        .is_comptime = true,
        .comptime_reason = &comptime_reason,
        .c_import_buf = &c_import_buf,
        .runtime_cond = parent_block.runtime_cond,
        .runtime_loop = parent_block.runtime_loop,
        .runtime_index = parent_block.runtime_index,
    };
    defer child_block.instructions.deinit(sema.gpa);

    // Ignore the result, all the relevant operations have written to c_import_buf already.
    _ = try sema.analyzeBodyBreak(&child_block, body);

    const mod = sema.mod;
    const c_import_res = mod.comp.cImport(c_import_buf.items) catch |err|
        return sema.fail(&child_block, src, "C import failed: {s}", .{@errorName(err)});

    if (c_import_res.errors.len != 0) {
        const msg = msg: {
            defer @import("clang.zig").ErrorMsg.delete(c_import_res.errors.ptr, c_import_res.errors.len);

            const msg = try sema.errMsg(&child_block, src, "C import failed", .{});
            errdefer msg.destroy(sema.gpa);

            if (!mod.comp.bin_file.options.link_libc)
                try sema.errNote(&child_block, src, msg, "libc headers not available; compilation does not link against libc", .{});

            const gop = try sema.mod.cimport_errors.getOrPut(sema.gpa, sema.owner_decl_index);
            if (!gop.found_existing) {
                var errs = try std.ArrayListUnmanaged(Module.CImportError).initCapacity(sema.gpa, c_import_res.errors.len);
                errdefer {
                    for (errs.items) |err| err.deinit(sema.gpa);
                    errs.deinit(sema.gpa);
                }

                for (c_import_res.errors) |c_error| {
                    const path = if (c_error.filename_ptr) |some|
                        try sema.gpa.dupeZ(u8, some[0..c_error.filename_len])
                    else
                        null;
                    errdefer if (path) |some| sema.gpa.free(some);

                    const c_msg = try sema.gpa.dupeZ(u8, c_error.msg_ptr[0..c_error.msg_len]);
                    errdefer sema.gpa.free(c_msg);

                    const line = line: {
                        const source = c_error.source orelse break :line null;
                        var start = c_error.offset;
                        while (start > 0) : (start -= 1) {
                            if (source[start - 1] == '\n') break;
                        }
                        var end = c_error.offset;
                        while (true) : (end += 1) {
                            if (source[end] == 0) break;
                            if (source[end] == '\n') break;
                        }
                        break :line try sema.gpa.dupeZ(u8, source[start..end]);
                    };
                    errdefer if (line) |some| sema.gpa.free(some);

                    errs.appendAssumeCapacity(.{
                        .path = path orelse null,
                        .source_line = line orelse null,
                        .line = c_error.line,
                        .column = c_error.column,
                        .offset = c_error.offset,
                        .msg = c_msg,
                    });
                }
                gop.value_ptr.* = errs.items;
            }
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    const c_import_pkg = Package.create(
        sema.gpa,
        null,
        c_import_res.out_zig_path,
    ) catch |err| switch (err) {
        error.OutOfMemory => return error.OutOfMemory,
        else => unreachable, // we pass null for root_src_dir_path
    };

    const result = mod.importPkg(c_import_pkg) catch |err|
        return sema.fail(&child_block, src, "C import failed: {s}", .{@errorName(err)});

    mod.astGenFile(result.file) catch |err|
        return sema.fail(&child_block, src, "C import failed: {s}", .{@errorName(err)});

    try mod.semaFile(result.file);
    const file_root_decl_index = result.file.root_decl.unwrap().?;
    const file_root_decl = mod.declPtr(file_root_decl_index);
    try mod.declareDeclDependency(sema.owner_decl_index, file_root_decl_index);
    return sema.addConstant(file_root_decl.ty, file_root_decl.val);
}

fn zirSuspendBlock(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    return sema.failWithUseOfAsync(parent_block, src);
}

fn zirBlock(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const pl_node = sema.code.instructions.items(.data)[inst].pl_node;
    const src = pl_node.src();
    const extra = sema.code.extraData(Zir.Inst.Block, pl_node.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];
    const gpa = sema.gpa;

    // Reserve space for a Block instruction so that generated Break instructions can
    // point to it, even if it doesn't end up getting used because the code ends up being
    // comptime evaluated or is an unlabeled block.
    const block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
    try sema.air_instructions.append(gpa, .{
        .tag = .block,
        .data = undefined,
    });

    var label: Block.Label = .{
        .zir_block = inst,
        .merges = .{
            .results = .{},
            .br_list = .{},
            .block_inst = block_inst,
        },
    };

    var child_block: Block = .{
        .parent = parent_block,
        .sema = sema,
        .src_decl = parent_block.src_decl,
        .namespace = parent_block.namespace,
        .wip_capture_scope = parent_block.wip_capture_scope,
        .instructions = .{},
        .label = &label,
        .inlining = parent_block.inlining,
        .is_comptime = parent_block.is_comptime,
        .comptime_reason = parent_block.comptime_reason,
        .is_typeof = parent_block.is_typeof,
        .want_safety = parent_block.want_safety,
        .float_mode = parent_block.float_mode,
        .c_import_buf = parent_block.c_import_buf,
        .runtime_cond = parent_block.runtime_cond,
        .runtime_loop = parent_block.runtime_loop,
        .runtime_index = parent_block.runtime_index,
        .error_return_trace_index = parent_block.error_return_trace_index,
    };

    defer child_block.instructions.deinit(gpa);
    defer label.merges.results.deinit(gpa);
    defer label.merges.br_list.deinit(gpa);

    return sema.resolveBlockBody(parent_block, src, &child_block, body, inst, &label.merges);
}

fn resolveBlockBody(
    sema: *Sema,
    parent_block: *Block,
    src: LazySrcLoc,
    child_block: *Block,
    body: []const Zir.Inst.Index,
    /// This is the instruction that a break instruction within `body` can
    /// use to return from the body.
    body_inst: Zir.Inst.Index,
    merges: *Block.Merges,
) CompileError!Air.Inst.Ref {
    if (child_block.is_comptime) {
        return sema.resolveBody(child_block, body, body_inst);
    } else {
        if (sema.analyzeBodyInner(child_block, body)) |_| {
            return sema.analyzeBlockBody(parent_block, src, child_block, merges);
        } else |err| switch (err) {
            error.ComptimeBreak => {
                // Comptime control flow is happening, however child_block may still contain
                // runtime instructions which need to be copied to the parent block.
                try parent_block.instructions.appendSlice(sema.gpa, child_block.instructions.items);

                const break_inst = sema.comptime_break_inst;
                const break_data = sema.code.instructions.items(.data)[break_inst].@"break";
                if (break_data.block_inst == body_inst) {
                    return try sema.resolveInst(break_data.operand);
                } else {
                    return error.ComptimeBreak;
                }
            },
            else => |e| return e,
        }
    }
}

fn analyzeBlockBody(
    sema: *Sema,
    parent_block: *Block,
    src: LazySrcLoc,
    child_block: *Block,
    merges: *Block.Merges,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const gpa = sema.gpa;
    const mod = sema.mod;

    // Blocks must terminate with noreturn instruction.
    assert(child_block.instructions.items.len != 0);
    assert(sema.typeOf(Air.indexToRef(child_block.instructions.items[child_block.instructions.items.len - 1])).isNoReturn());

    if (merges.results.items.len == 0) {
        // No need for a block instruction. We can put the new instructions
        // directly into the parent block.
        try parent_block.instructions.appendSlice(gpa, child_block.instructions.items);
        return Air.indexToRef(child_block.instructions.items[child_block.instructions.items.len - 1]);
    }
    if (merges.results.items.len == 1) {
        const last_inst_index = child_block.instructions.items.len - 1;
        const last_inst = child_block.instructions.items[last_inst_index];
        if (sema.getBreakBlock(last_inst)) |br_block| {
            if (br_block == merges.block_inst) {
                // No need for a block instruction. We can put the new instructions directly
                // into the parent block. Here we omit the break instruction.
                const without_break = child_block.instructions.items[0..last_inst_index];
                try parent_block.instructions.appendSlice(gpa, without_break);
                return merges.results.items[0];
            }
        }
    }
    // It is impossible to have the number of results be > 1 in a comptime scope.
    assert(!child_block.is_comptime); // Should already got a compile error in the condbr condition.

    // Need to set the type and emit the Block instruction. This allows machine code generation
    // to emit a jump instruction to after the block when it encounters the break.
    try parent_block.instructions.append(gpa, merges.block_inst);
    const resolved_ty = try sema.resolvePeerTypes(parent_block, src, merges.results.items, .none);
    // TODO add note "missing else causes void value"

    const type_src = src; // TODO: better source location
    const valid_rt = try sema.validateRunTimeType(resolved_ty, false);
    if (!valid_rt) {
        const msg = msg: {
            const msg = try sema.errMsg(child_block, type_src, "value with comptime-only type '{}' depends on runtime control flow", .{resolved_ty.fmt(mod)});
            errdefer msg.destroy(sema.gpa);

            const runtime_src = child_block.runtime_cond orelse child_block.runtime_loop.?;
            try sema.errNote(child_block, runtime_src, msg, "runtime control flow here", .{});

            const child_src_decl = mod.declPtr(child_block.src_decl);
            try sema.explainWhyTypeIsComptime(child_block, type_src, msg, type_src.toSrcLoc(child_src_decl), resolved_ty);

            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    const ty_inst = try sema.addType(resolved_ty);
    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.Block).Struct.fields.len +
        child_block.instructions.items.len);
    sema.air_instructions.items(.data)[merges.block_inst] = .{ .ty_pl = .{
        .ty = ty_inst,
        .payload = sema.addExtraAssumeCapacity(Air.Block{
            .body_len = @intCast(u32, child_block.instructions.items.len),
        }),
    } };
    sema.air_extra.appendSliceAssumeCapacity(child_block.instructions.items);
    // Now that the block has its type resolved, we need to go back into all the break
    // instructions, and insert type coercion on the operands.
    for (merges.br_list.items) |br| {
        const br_operand = sema.air_instructions.items(.data)[br].br.operand;
        const br_operand_src = src;
        const br_operand_ty = sema.typeOf(br_operand);
        if (br_operand_ty.eql(resolved_ty, mod)) {
            // No type coercion needed.
            continue;
        }
        var coerce_block = parent_block.makeSubBlock();
        defer coerce_block.instructions.deinit(gpa);
        const coerced_operand = try sema.coerce(&coerce_block, resolved_ty, br_operand, br_operand_src);
        // If no instructions were produced, such as in the case of a coercion of a
        // constant value to a new type, we can simply point the br operand to it.
        if (coerce_block.instructions.items.len == 0) {
            sema.air_instructions.items(.data)[br].br.operand = coerced_operand;
            continue;
        }
        assert(coerce_block.instructions.items[coerce_block.instructions.items.len - 1] ==
            Air.refToIndex(coerced_operand).?);

        // Convert the br instruction to a block instruction that has the coercion
        // and then a new br inside that returns the coerced instruction.
        const sub_block_len = @intCast(u32, coerce_block.instructions.items.len + 1);
        try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.Block).Struct.fields.len +
            sub_block_len);
        try sema.air_instructions.ensureUnusedCapacity(gpa, 1);
        const sub_br_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);

        sema.air_instructions.items(.tag)[br] = .block;
        sema.air_instructions.items(.data)[br] = .{ .ty_pl = .{
            .ty = Air.Inst.Ref.noreturn_type,
            .payload = sema.addExtraAssumeCapacity(Air.Block{
                .body_len = sub_block_len,
            }),
        } };
        sema.air_extra.appendSliceAssumeCapacity(coerce_block.instructions.items);
        sema.air_extra.appendAssumeCapacity(sub_br_inst);

        sema.air_instructions.appendAssumeCapacity(.{
            .tag = .br,
            .data = .{ .br = .{
                .block_inst = merges.block_inst,
                .operand = coerced_operand,
            } },
        });
    }
    return Air.indexToRef(merges.block_inst);
}

fn zirExport(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Export, inst_data.payload_index).data;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const options_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const decl_name = sema.code.nullTerminatedString(extra.decl_name);
    const decl_index = if (extra.namespace != .none) index_blk: {
        const container_ty = try sema.resolveType(block, operand_src, extra.namespace);
        const container_namespace = container_ty.getNamespace().?;

        const maybe_index = try sema.lookupInNamespace(block, operand_src, container_namespace, decl_name, false);
        break :index_blk maybe_index orelse
            return sema.failWithBadMemberAccess(block, container_ty, operand_src, decl_name);
    } else try sema.lookupIdentifier(block, operand_src, decl_name);
    const options = sema.resolveExportOptions(block, .unneeded, extra.options) catch |err| switch (err) {
        error.NeededSourceLocation => {
            _ = try sema.resolveExportOptions(block, options_src, extra.options);
            unreachable;
        },
        else => |e| return e,
    };
    {
        try sema.mod.ensureDeclAnalyzed(decl_index);
        const exported_decl = sema.mod.declPtr(decl_index);
        if (exported_decl.val.castTag(.function)) |some| {
            return sema.analyzeExport(block, src, options, some.data.owner_decl);
        }
    }
    try sema.analyzeExport(block, src, options, decl_index);
}

fn zirExportValue(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.ExportValue, inst_data.payload_index).data;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const options_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const operand = try sema.resolveInstConst(block, operand_src, extra.operand, "export target must be comptime-known");
    const options = sema.resolveExportOptions(block, .unneeded, extra.options) catch |err| switch (err) {
        error.NeededSourceLocation => {
            _ = try sema.resolveExportOptions(block, options_src, extra.options);
            unreachable;
        },
        else => |e| return e,
    };
    const decl_index = switch (operand.val.tag()) {
        .function => operand.val.castTag(.function).?.data.owner_decl,
        else => return sema.fail(block, operand_src, "TODO implement exporting arbitrary Value objects", .{}), // TODO put this Value into an anonymous Decl and then export it.
    };
    try sema.analyzeExport(block, src, options, decl_index);
}

pub fn analyzeExport(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    borrowed_options: std.builtin.ExportOptions,
    exported_decl_index: Decl.Index,
) !void {
    const Export = Module.Export;
    const mod = sema.mod;

    if (borrowed_options.linkage == .Internal) {
        return;
    }

    try mod.ensureDeclAnalyzed(exported_decl_index);
    const exported_decl = mod.declPtr(exported_decl_index);

    if (!try sema.validateExternType(exported_decl.ty, .other)) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "unable to export type '{}'", .{exported_decl.ty.fmt(sema.mod)});
            errdefer msg.destroy(sema.gpa);

            const src_decl = sema.mod.declPtr(block.src_decl);
            try sema.explainWhyTypeIsNotExtern(msg, src.toSrcLoc(src_decl), exported_decl.ty, .other);

            try sema.addDeclaredHereNote(msg, exported_decl.ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    const gpa = mod.gpa;

    try mod.decl_exports.ensureUnusedCapacity(gpa, 1);
    try mod.export_owners.ensureUnusedCapacity(gpa, 1);

    const new_export = try gpa.create(Export);
    errdefer gpa.destroy(new_export);

    const symbol_name = try gpa.dupe(u8, borrowed_options.name);
    errdefer gpa.free(symbol_name);

    const section: ?[]const u8 = if (borrowed_options.section) |s| try gpa.dupe(u8, s) else null;
    errdefer if (section) |s| gpa.free(s);

    new_export.* = .{
        .options = .{
            .name = symbol_name,
            .linkage = borrowed_options.linkage,
            .section = section,
            .visibility = borrowed_options.visibility,
        },
        .src = src,
        .owner_decl = sema.owner_decl_index,
        .src_decl = block.src_decl,
        .exported_decl = exported_decl_index,
        .status = .in_progress,
    };

    // Add to export_owners table.
    const eo_gop = mod.export_owners.getOrPutAssumeCapacity(sema.owner_decl_index);
    if (!eo_gop.found_existing) {
        eo_gop.value_ptr.* = .{};
    }
    try eo_gop.value_ptr.append(gpa, new_export);
    errdefer _ = eo_gop.value_ptr.pop();

    // Add to exported_decl table.
    const de_gop = mod.decl_exports.getOrPutAssumeCapacity(exported_decl_index);
    if (!de_gop.found_existing) {
        de_gop.value_ptr.* = .{};
    }
    try de_gop.value_ptr.append(gpa, new_export);
    errdefer _ = de_gop.value_ptr.pop();
}

fn zirSetAlignStack(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!void {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const src = LazySrcLoc.nodeOffset(extra.node);
    const alignment = try sema.resolveAlign(block, operand_src, extra.operand);
    if (alignment > 256) {
        return sema.fail(block, src, "attempt to @setAlignStack({d}); maximum is 256", .{
            alignment,
        });
    }
    const func = sema.func orelse
        return sema.fail(block, src, "@setAlignStack outside function body", .{});

    const fn_owner_decl = sema.mod.declPtr(func.owner_decl);
    switch (fn_owner_decl.ty.fnCallingConvention()) {
        .Naked => return sema.fail(block, src, "@setAlignStack in naked function", .{}),
        .Inline => return sema.fail(block, src, "@setAlignStack in inline function", .{}),
        else => if (block.inlining != null) {
            return sema.fail(block, src, "@setAlignStack in inline call", .{});
        },
    }

    const gop = try sema.mod.align_stack_fns.getOrPut(sema.mod.gpa, func);
    if (gop.found_existing) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "multiple @setAlignStack in the same function body", .{});
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, gop.value_ptr.src, msg, "other instance here", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    gop.value_ptr.* = .{ .alignment = alignment, .src = src };
}

fn zirSetCold(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!void {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const is_cold = try sema.resolveConstBool(block, operand_src, extra.operand, "operand to @setCold must be comptime-known");
    const func = sema.func orelse return; // does nothing outside a function
    func.is_cold = is_cold;
}

fn zirSetFloatMode(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!void {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    block.float_mode = try sema.resolveBuiltinEnum(block, src, extra.operand, "FloatMode", "operand to @setFloatMode must be comptime-known");
}

fn zirSetRuntimeSafety(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    block.want_safety = try sema.resolveConstBool(block, operand_src, inst_data.operand, "operand to @setRuntimeSafety must be comptime-known");
}

fn zirFence(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!void {
    if (block.is_comptime) return;

    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const order_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const order = try sema.resolveAtomicOrder(block, order_src, extra.operand, "atomic order of @fence must be comptime-known");

    if (@enumToInt(order) < @enumToInt(std.builtin.AtomicOrder.Acquire)) {
        return sema.fail(block, order_src, "atomic ordering must be Acquire or stricter", .{});
    }

    _ = try block.addInst(.{
        .tag = .fence,
        .data = .{ .fence = order },
    });
}

fn zirBreak(sema: *Sema, start_block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].@"break";
    const operand = try sema.resolveInst(inst_data.operand);
    const zir_block = inst_data.block_inst;

    var block = start_block;
    while (true) {
        if (block.label) |label| {
            if (label.zir_block == zir_block) {
                const br_ref = try start_block.addBr(label.merges.block_inst, operand);
                try label.merges.results.append(sema.gpa, operand);
                try label.merges.br_list.append(sema.gpa, Air.refToIndex(br_ref).?);
                block.runtime_index.increment();
                if (block.runtime_cond == null and block.runtime_loop == null) {
                    block.runtime_cond = start_block.runtime_cond orelse start_block.runtime_loop;
                    block.runtime_loop = start_block.runtime_loop;
                }
                return inst;
            }
        }
        block = block.parent.?;
    }
}

fn zirDbgStmt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    // We do not set sema.src here because dbg_stmt instructions are only emitted for
    // ZIR code that possibly will need to generate runtime code. So error messages
    // and other source locations must not rely on sema.src being set from dbg_stmt
    // instructions.
    if (block.is_comptime or sema.mod.comp.bin_file.options.strip) return;

    const inst_data = sema.code.instructions.items(.data)[inst].dbg_stmt;
    _ = try block.addInst(.{
        .tag = .dbg_stmt,
        .data = .{ .dbg_stmt = .{
            .line = inst_data.line,
            .column = inst_data.column,
        } },
    });
}

fn zirDbgBlockBegin(sema: *Sema, block: *Block) CompileError!void {
    if (block.is_comptime or sema.mod.comp.bin_file.options.strip) return;

    _ = try block.addInst(.{
        .tag = .dbg_block_begin,
        .data = undefined,
    });
}

fn zirDbgBlockEnd(sema: *Sema, block: *Block) CompileError!void {
    if (block.is_comptime or sema.mod.comp.bin_file.options.strip) return;

    _ = try block.addInst(.{
        .tag = .dbg_block_end,
        .data = undefined,
    });
}

fn zirDbgVar(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
) CompileError!void {
    if (block.is_comptime or sema.mod.comp.bin_file.options.strip) return;

    const str_op = sema.code.instructions.items(.data)[inst].str_op;
    const operand = try sema.resolveInst(str_op.operand);
    const name = str_op.getStr(sema.code);
    try sema.addDbgVar(block, operand, air_tag, name);
}

fn addDbgVar(
    sema: *Sema,
    block: *Block,
    operand: Air.Inst.Ref,
    air_tag: Air.Inst.Tag,
    name: []const u8,
) CompileError!void {
    const operand_ty = sema.typeOf(operand);
    switch (air_tag) {
        .dbg_var_ptr => {
            if (!(try sema.typeHasRuntimeBits(operand_ty.childType()))) return;
        },
        .dbg_var_val => {
            if (!(try sema.typeHasRuntimeBits(operand_ty))) return;
        },
        else => unreachable,
    }

    try sema.queueFullTypeResolution(operand_ty);

    // Add the name to the AIR.
    const name_extra_index = @intCast(u32, sema.air_extra.items.len);
    const elements_used = name.len / 4 + 1;
    try sema.air_extra.ensureUnusedCapacity(sema.gpa, elements_used);
    const buffer = mem.sliceAsBytes(sema.air_extra.unusedCapacitySlice());
    mem.copy(u8, buffer, name);
    buffer[name.len] = 0;
    sema.air_extra.items.len += elements_used;

    _ = try block.addInst(.{
        .tag = air_tag,
        .data = .{ .pl_op = .{
            .payload = name_extra_index,
            .operand = operand,
        } },
    });
}

fn zirDeclRef(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const src = inst_data.src();
    const decl_name = inst_data.get(sema.code);
    const decl_index = try sema.lookupIdentifier(block, src, decl_name);
    try sema.addReferencedBy(block, src, decl_index);
    return sema.analyzeDeclRef(decl_index);
}

fn zirDeclVal(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const src = inst_data.src();
    const decl_name = inst_data.get(sema.code);
    const decl = try sema.lookupIdentifier(block, src, decl_name);
    return sema.analyzeDeclVal(block, src, decl);
}

fn lookupIdentifier(sema: *Sema, block: *Block, src: LazySrcLoc, name: []const u8) !Decl.Index {
    var namespace = block.namespace;
    while (true) {
        if (try sema.lookupInNamespace(block, src, namespace, name, false)) |decl_index| {
            return decl_index;
        }
        namespace = namespace.parent orelse break;
    }
    unreachable; // AstGen detects use of undeclared identifier errors.
}

/// This looks up a member of a specific namespace. It is affected by `usingnamespace` but
/// only for ones in the specified namespace.
fn lookupInNamespace(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    namespace: *Namespace,
    ident_name: []const u8,
    observe_usingnamespace: bool,
) CompileError!?Decl.Index {
    const mod = sema.mod;

    const namespace_decl_index = namespace.getDeclIndex();
    const namespace_decl = sema.mod.declPtr(namespace_decl_index);
    if (namespace_decl.analysis == .file_failure) {
        try mod.declareDeclDependency(sema.owner_decl_index, namespace_decl_index);
        return error.AnalysisFail;
    }

    if (observe_usingnamespace and namespace.usingnamespace_set.count() != 0) {
        const src_file = block.namespace.file_scope;

        const gpa = sema.gpa;
        var checked_namespaces: std.AutoArrayHashMapUnmanaged(*Namespace, bool) = .{};
        defer checked_namespaces.deinit(gpa);

        // Keep track of name conflicts for error notes.
        var candidates: std.ArrayListUnmanaged(Decl.Index) = .{};
        defer candidates.deinit(gpa);

        try checked_namespaces.put(gpa, namespace, namespace.file_scope == src_file);
        var check_i: usize = 0;

        while (check_i < checked_namespaces.count()) : (check_i += 1) {
            const check_ns = checked_namespaces.keys()[check_i];
            if (check_ns.decls.getKeyAdapted(ident_name, Module.DeclAdapter{ .mod = mod })) |decl_index| {
                // Skip decls which are not marked pub, which are in a different
                // file than the `a.b`/`@hasDecl` syntax.
                const decl = mod.declPtr(decl_index);
                if (decl.is_pub or (src_file == decl.getFileScope() and checked_namespaces.values()[check_i])) {
                    try candidates.append(gpa, decl_index);
                }
            }
            var it = check_ns.usingnamespace_set.iterator();
            while (it.next()) |entry| {
                const sub_usingnamespace_decl_index = entry.key_ptr.*;
                // Skip the decl we're currently analysing.
                if (sub_usingnamespace_decl_index == sema.owner_decl_index) continue;
                const sub_usingnamespace_decl = mod.declPtr(sub_usingnamespace_decl_index);
                const sub_is_pub = entry.value_ptr.*;
                if (!sub_is_pub and src_file != sub_usingnamespace_decl.getFileScope()) {
                    // Skip usingnamespace decls which are not marked pub, which are in
                    // a different file than the `a.b`/`@hasDecl` syntax.
                    continue;
                }
                try sema.ensureDeclAnalyzed(sub_usingnamespace_decl_index);
                const ns_ty = sub_usingnamespace_decl.val.castTag(.ty).?.data;
                const sub_ns = ns_ty.getNamespace().?;
                try checked_namespaces.put(gpa, sub_ns, src_file == sub_usingnamespace_decl.getFileScope());
            }
        }

        {
            var i: usize = 0;
            while (i < candidates.items.len) {
                if (candidates.items[i] == sema.owner_decl_index) {
                    _ = candidates.orderedRemove(i);
                } else {
                    i += 1;
                }
            }
        }

        switch (candidates.items.len) {
            0 => {},
            1 => {
                const decl_index = candidates.items[0];
                try mod.declareDeclDependency(sema.owner_decl_index, decl_index);
                return decl_index;
            },
            else => {
                const msg = msg: {
                    const msg = try sema.errMsg(block, src, "ambiguous reference", .{});
                    errdefer msg.destroy(gpa);
                    for (candidates.items) |candidate_index| {
                        const candidate = mod.declPtr(candidate_index);
                        const src_loc = candidate.srcLoc();
                        try mod.errNoteNonLazy(src_loc, msg, "declared here", .{});
                    }
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            },
        }
    } else if (namespace.decls.getKeyAdapted(ident_name, Module.DeclAdapter{ .mod = mod })) |decl_index| {
        try mod.declareDeclDependency(sema.owner_decl_index, decl_index);
        return decl_index;
    }

    log.debug("{*} ({s}) depends on non-existence of '{s}' in {*} ({s})", .{
        sema.owner_decl, sema.owner_decl.name, ident_name, namespace_decl, namespace_decl.name,
    });
    // TODO This dependency is too strong. Really, it should only be a dependency
    // on the non-existence of `ident_name` in the namespace. We can lessen the number of
    // outdated declarations by making this dependency more sophisticated.
    try mod.declareDeclDependency(sema.owner_decl_index, namespace_decl_index);
    return null;
}

fn funcDeclSrc(sema: *Sema, func_inst: Air.Inst.Ref) !?*Decl {
    const func_val = (try sema.resolveMaybeUndefVal(func_inst)) orelse return null;
    if (func_val.isUndef()) return null;
    const owner_decl_index = switch (func_val.tag()) {
        .extern_fn => func_val.castTag(.extern_fn).?.data.owner_decl,
        .function => func_val.castTag(.function).?.data.owner_decl,
        .decl_ref => sema.mod.declPtr(func_val.castTag(.decl_ref).?.data).val.castTag(.function).?.data.owner_decl,
        else => return null,
    };
    return sema.mod.declPtr(owner_decl_index);
}

pub fn analyzeSaveErrRetIndex(sema: *Sema, block: *Block) SemaError!Air.Inst.Ref {
    const src = sema.src;

    if (!sema.mod.backendSupportsFeature(.error_return_trace)) return .none;
    if (!sema.mod.comp.bin_file.options.error_return_tracing) return .none;

    if (block.is_comptime)
        return .none;

    const unresolved_stack_trace_ty = sema.getBuiltinType("StackTrace") catch |err| switch (err) {
        error.NeededSourceLocation, error.GenericPoison, error.ComptimeReturn, error.ComptimeBreak => unreachable,
        else => |e| return e,
    };
    const stack_trace_ty = sema.resolveTypeFields(unresolved_stack_trace_ty) catch |err| switch (err) {
        error.NeededSourceLocation, error.GenericPoison, error.ComptimeReturn, error.ComptimeBreak => unreachable,
        else => |e| return e,
    };
    const field_index = sema.structFieldIndex(block, stack_trace_ty, "index", src) catch |err| switch (err) {
        error.NeededSourceLocation, error.GenericPoison, error.ComptimeReturn, error.ComptimeBreak => unreachable,
        else => |e| return e,
    };

    return try block.addInst(.{
        .tag = .save_err_return_trace_index,
        .data = .{ .ty_pl = .{
            .ty = try sema.addType(stack_trace_ty),
            .payload = @intCast(u32, field_index),
        } },
    });
}

/// Add instructions to block to "pop" the error return trace.
/// If `operand` is provided, only pops if operand is non-error.
fn popErrorReturnTrace(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    operand: Air.Inst.Ref,
    saved_error_trace_index: Air.Inst.Ref,
) CompileError!void {
    var is_non_error: ?bool = null;
    var is_non_error_inst: Air.Inst.Ref = undefined;
    if (operand != .none) {
        is_non_error_inst = try sema.analyzeIsNonErr(block, src, operand);
        if (try sema.resolveDefinedValue(block, src, is_non_error_inst)) |cond_val|
            is_non_error = cond_val.toBool();
    } else is_non_error = true; // no operand means pop unconditionally

    if (is_non_error == true) {
        // AstGen determined this result does not go to an error-handling expr (try/catch/return etc.), or
        // the result is comptime-known to be a non-error. Either way, pop unconditionally.

        const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
        const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
        const ptr_stack_trace_ty = try Type.Tag.single_mut_pointer.create(sema.arena, stack_trace_ty);
        const err_return_trace = try block.addTy(.err_return_trace, ptr_stack_trace_ty);
        const field_ptr = try sema.structFieldPtr(block, src, err_return_trace, "index", src, stack_trace_ty, true);
        try sema.storePtr2(block, src, field_ptr, src, saved_error_trace_index, src, .store);
    } else if (is_non_error == null) {
        // The result might be an error. If it is, we leave the error trace alone. If it isn't, we need
        // to pop any error trace that may have been propagated from our arguments.

        try sema.air_extra.ensureUnusedCapacity(sema.gpa, @typeInfo(Air.Block).Struct.fields.len);
        const cond_block_inst = try block.addInstAsIndex(.{
            .tag = .block,
            .data = .{
                .ty_pl = .{
                    .ty = Air.Inst.Ref.void_type,
                    .payload = undefined, // updated below
                },
            },
        });

        var then_block = block.makeSubBlock();
        defer then_block.instructions.deinit(sema.gpa);

        // If non-error, then pop the error return trace by restoring the index.
        const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
        const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
        const ptr_stack_trace_ty = try Type.Tag.single_mut_pointer.create(sema.arena, stack_trace_ty);
        const err_return_trace = try then_block.addTy(.err_return_trace, ptr_stack_trace_ty);
        const field_ptr = try sema.structFieldPtr(&then_block, src, err_return_trace, "index", src, stack_trace_ty, true);
        try sema.storePtr2(&then_block, src, field_ptr, src, saved_error_trace_index, src, .store);
        _ = try then_block.addBr(cond_block_inst, Air.Inst.Ref.void_value);

        // Otherwise, do nothing
        var else_block = block.makeSubBlock();
        defer else_block.instructions.deinit(sema.gpa);
        _ = try else_block.addBr(cond_block_inst, Air.Inst.Ref.void_value);

        try sema.air_extra.ensureUnusedCapacity(sema.gpa, @typeInfo(Air.CondBr).Struct.fields.len +
            then_block.instructions.items.len + else_block.instructions.items.len +
            @typeInfo(Air.Block).Struct.fields.len + 1); // +1 for the sole .cond_br instruction in the .block

        const cond_br_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
        try sema.air_instructions.append(sema.gpa, .{ .tag = .cond_br, .data = .{ .pl_op = .{
            .operand = is_non_error_inst,
            .payload = sema.addExtraAssumeCapacity(Air.CondBr{
                .then_body_len = @intCast(u32, then_block.instructions.items.len),
                .else_body_len = @intCast(u32, else_block.instructions.items.len),
            }),
        } } });
        sema.air_extra.appendSliceAssumeCapacity(then_block.instructions.items);
        sema.air_extra.appendSliceAssumeCapacity(else_block.instructions.items);

        sema.air_instructions.items(.data)[cond_block_inst].ty_pl.payload = sema.addExtraAssumeCapacity(Air.Block{ .body_len = 1 });
        sema.air_extra.appendAssumeCapacity(cond_br_inst);
    }
}

fn zirCall(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const func_src: LazySrcLoc = .{ .node_offset_call_func = inst_data.src_node };
    const call_src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Call, inst_data.payload_index);
    const args_len = extra.data.flags.args_len;

    const modifier = @intToEnum(std.builtin.CallModifier, extra.data.flags.packed_modifier);
    const ensure_result_used = extra.data.flags.ensure_result_used;
    const pop_error_return_trace = extra.data.flags.pop_error_return_trace;

    var func = try sema.resolveInst(extra.data.callee);
    var resolved_args: []Air.Inst.Ref = undefined;
    var arg_index: u32 = 0;

    const func_type = sema.typeOf(func);

    // Desugar bound functions here
    var bound_arg_src: ?LazySrcLoc = null;
    if (func_type.tag() == .bound_fn) {
        bound_arg_src = func_src;
        const bound_func = try sema.resolveValue(block, .unneeded, func, "");
        const bound_data = &bound_func.cast(Value.Payload.BoundFn).?.data;
        func = bound_data.func_inst;
        resolved_args = try sema.arena.alloc(Air.Inst.Ref, args_len + 1);
        resolved_args[arg_index] = bound_data.arg0_inst;
        arg_index += 1;
    } else {
        resolved_args = try sema.arena.alloc(Air.Inst.Ref, args_len);
    }
    const total_args = args_len + @boolToInt(bound_arg_src != null);

    const callee_ty = sema.typeOf(func);
    const func_ty = func_ty: {
        switch (callee_ty.zigTypeTag()) {
            .Fn => break :func_ty callee_ty,
            .Pointer => {
                const ptr_info = callee_ty.ptrInfo().data;
                if (ptr_info.size == .One and ptr_info.pointee_type.zigTypeTag() == .Fn) {
                    break :func_ty ptr_info.pointee_type;
                }
            },
            else => {},
        }
        return sema.fail(block, func_src, "type '{}' not a function", .{callee_ty.fmt(sema.mod)});
    };
    const func_ty_info = func_ty.fnInfo();

    const fn_params_len = func_ty_info.param_types.len;
    check_args: {
        if (func_ty_info.is_var_args) {
            assert(func_ty_info.cc == .C);
            if (total_args >= fn_params_len) break :check_args;
        } else if (fn_params_len == total_args) {
            break :check_args;
        }

        const maybe_decl = try sema.funcDeclSrc(func);
        const member_str = if (bound_arg_src != null) "member function " else "";
        const variadic_str = if (func_ty_info.is_var_args) "at least " else "";
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                func_src,
                "{s}expected {s}{d} argument(s), found {d}",
                .{
                    member_str,
                    variadic_str,
                    fn_params_len - @boolToInt(bound_arg_src != null),
                    args_len,
                },
            );
            errdefer msg.destroy(sema.gpa);

            if (maybe_decl) |fn_decl| try sema.mod.errNoteNonLazy(fn_decl.srcLoc(), msg, "function declared here", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    const args_body = sema.code.extra[extra.end..];

    var input_is_error = false;
    const block_index = @intCast(Air.Inst.Index, block.instructions.items.len);

    const parent_comptime = block.is_comptime;
    // `extra_index` and `arg_index` are separate since the bound function is passed as the first argument.
    var extra_index: usize = 0;
    var arg_start: u32 = args_len;
    while (extra_index < args_len) : ({
        extra_index += 1;
        arg_index += 1;
    }) {
        const arg_end = sema.code.extra[extra.end + extra_index];
        defer arg_start = arg_end;

        const param_ty = if (arg_index >= fn_params_len or
            func_ty_info.param_types[arg_index].tag() == .generic_poison)
            Type.initTag(.var_args_param)
        else
            func_ty_info.param_types[arg_index];

        // Generate args to comptime params in comptime block.
        defer block.is_comptime = parent_comptime;
        if (arg_index < fn_params_len and func_ty_info.comptime_params[arg_index]) {
            block.is_comptime = true;
            // TODO set comptime_reason
        }

        const param_ty_inst = try sema.addType(param_ty);
        sema.inst_map.putAssumeCapacity(inst, param_ty_inst);

        const resolved = try sema.resolveBody(block, args_body[arg_start..arg_end], inst);
        const resolved_ty = sema.typeOf(resolved);
        if (resolved_ty.zigTypeTag() == .NoReturn) {
            return resolved;
        }
        if (resolved_ty.isError()) {
            input_is_error = true;
        }
        resolved_args[arg_index] = resolved;
    }
    if (sema.owner_func == null or !sema.owner_func.?.calls_or_awaits_errorable_fn)
        input_is_error = false; // input was an error type, but no errorable fn's were actually called

    if (sema.mod.backendSupportsFeature(.error_return_trace) and sema.mod.comp.bin_file.options.error_return_tracing and
        !block.is_comptime and !block.is_typeof and (input_is_error or pop_error_return_trace))
    {
        const call_inst: Air.Inst.Ref = if (modifier == .always_tail) undefined else b: {
            break :b try sema.analyzeCall(block, func, func_src, call_src, modifier, ensure_result_used, resolved_args, bound_arg_src);
        };

        const return_ty = sema.typeOf(call_inst);
        if (modifier != .always_tail and return_ty.isNoReturn())
            return call_inst; // call to "fn(...) noreturn", don't pop

        // If any input is an error-type, we might need to pop any trace it generated. Otherwise, we only
        // need to clean-up our own trace if we were passed to a non-error-handling expression.
        if (input_is_error or (pop_error_return_trace and modifier != .always_tail and return_ty.isError())) {
            const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
            const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
            const field_index = try sema.structFieldIndex(block, stack_trace_ty, "index", call_src);

            // Insert a save instruction before the arg resolution + call instructions we just generated
            const save_inst = try block.insertInst(block_index, .{
                .tag = .save_err_return_trace_index,
                .data = .{ .ty_pl = .{
                    .ty = try sema.addType(stack_trace_ty),
                    .payload = @intCast(u32, field_index),
                } },
            });

            // Pop the error return trace, testing the result for non-error if necessary
            const operand = if (pop_error_return_trace or modifier == .always_tail) .none else call_inst;
            try sema.popErrorReturnTrace(block, call_src, operand, save_inst);
        }

        if (modifier == .always_tail) // Perform the call *after* the restore, so that a tail call is possible.
            return sema.analyzeCall(block, func, func_src, call_src, modifier, ensure_result_used, resolved_args, bound_arg_src);

        return call_inst;
    } else {
        return sema.analyzeCall(block, func, func_src, call_src, modifier, ensure_result_used, resolved_args, bound_arg_src);
    }
}

const GenericCallAdapter = struct {
    generic_fn: *Module.Fn,
    precomputed_hash: u64,
    func_ty_info: Type.Payload.Function.Data,
    args: []const Arg,
    module: *Module,

    const Arg = struct {
        ty: Type,
        val: Value,
        is_anytype: bool,
    };

    pub fn eql(ctx: @This(), adapted_key: void, other_key: *Module.Fn) bool {
        _ = adapted_key;
        // Checking for equality may happen on an item that has been inserted
        // into the map but is not yet fully initialized. In such case, the
        // two initialized fields are `hash` and `generic_owner_decl`.
        if (ctx.generic_fn.owner_decl != other_key.generic_owner_decl.unwrap().?) return false;

        const other_comptime_args = other_key.comptime_args.?;
        for (other_comptime_args[0..ctx.func_ty_info.param_types.len], 0..) |other_arg, i| {
            const this_arg = ctx.args[i];
            const this_is_comptime = this_arg.val.tag() != .generic_poison;
            const other_is_comptime = other_arg.val.tag() != .generic_poison;
            const this_is_anytype = this_arg.is_anytype;
            const other_is_anytype = other_key.isAnytypeParam(ctx.module, @intCast(u32, i));

            if (other_is_anytype != this_is_anytype) return false;
            if (other_is_comptime != this_is_comptime) return false;

            if (this_is_anytype) {
                // Both are anytype parameters.
                if (!this_arg.ty.eql(other_arg.ty, ctx.module)) {
                    return false;
                }
                if (this_is_comptime) {
                    // Both are comptime and anytype parameters with matching types.
                    if (!this_arg.val.eql(other_arg.val, other_arg.ty, ctx.module)) {
                        return false;
                    }
                }
            } else if (this_is_comptime) {
                // Both are comptime parameters but not anytype parameters.
                // We assert no error is possible here because any lazy values must be resolved
                // before inserting into the generic function hash map.
                const is_eql = Value.eqlAdvanced(
                    this_arg.val,
                    this_arg.ty,
                    other_arg.val,
                    other_arg.ty,
                    ctx.module,
                    null,
                ) catch unreachable;
                if (!is_eql) {
                    return false;
                }
            }
        }
        return true;
    }

    /// The implementation of the hash is in semantic analysis of function calls, so
    /// that any errors when computing the hash can be properly reported.
    pub fn hash(ctx: @This(), adapted_key: void) u64 {
        _ = adapted_key;
        return ctx.precomputed_hash;
    }
};

fn analyzeCall(
    sema: *Sema,
    block: *Block,
    func: Air.Inst.Ref,
    func_src: LazySrcLoc,
    call_src: LazySrcLoc,
    modifier: std.builtin.CallModifier,
    ensure_result_used: bool,
    uncasted_args: []const Air.Inst.Ref,
    bound_arg_src: ?LazySrcLoc,
) CompileError!Air.Inst.Ref {
    const mod = sema.mod;

    const callee_ty = sema.typeOf(func);
    const func_ty = func_ty: {
        switch (callee_ty.zigTypeTag()) {
            .Fn => break :func_ty callee_ty,
            .Pointer => {
                const ptr_info = callee_ty.ptrInfo().data;
                if (ptr_info.size == .One and ptr_info.pointee_type.zigTypeTag() == .Fn) {
                    break :func_ty ptr_info.pointee_type;
                }
            },
            else => {},
        }
        return sema.fail(block, func_src, "type '{}' is not a function", .{callee_ty.fmt(sema.mod)});
    };

    const func_ty_info = func_ty.fnInfo();
    const cc = func_ty_info.cc;
    if (cc == .Naked) {
        const maybe_decl = try sema.funcDeclSrc(func);
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                func_src,
                "unable to call function with naked calling convention",
                .{},
            );
            errdefer msg.destroy(sema.gpa);

            if (maybe_decl) |fn_decl| try sema.mod.errNoteNonLazy(fn_decl.srcLoc(), msg, "function declared here", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    const fn_params_len = func_ty_info.param_types.len;
    if (func_ty_info.is_var_args) {
        assert(cc == .C);
        if (uncasted_args.len < fn_params_len) {
            // TODO add error note: declared here
            return sema.fail(
                block,
                func_src,
                "expected at least {d} argument(s), found {d}",
                .{ fn_params_len, uncasted_args.len },
            );
        }
    } else if (fn_params_len != uncasted_args.len) {
        // TODO add error note: declared here
        return sema.fail(
            block,
            call_src,
            "expected {d} argument(s), found {d}",
            .{ fn_params_len, uncasted_args.len },
        );
    }

    const call_tag: Air.Inst.Tag = switch (modifier) {
        .auto,
        .always_inline,
        .compile_time,
        .no_async,
        => Air.Inst.Tag.call,

        .never_tail => Air.Inst.Tag.call_never_tail,
        .never_inline => Air.Inst.Tag.call_never_inline,
        .always_tail => Air.Inst.Tag.call_always_tail,

        .async_kw => return sema.failWithUseOfAsync(block, call_src),
    };

    if (modifier == .never_inline and func_ty_info.cc == .Inline) {
        return sema.fail(block, call_src, "no-inline call of inline function", .{});
    }

    const gpa = sema.gpa;

    var is_generic_call = func_ty_info.is_generic;
    var is_comptime_call = block.is_comptime or modifier == .compile_time;
    var comptime_reason_buf: Block.ComptimeReason = undefined;
    var comptime_reason: ?*const Block.ComptimeReason = null;
    if (!is_comptime_call) {
        if (sema.typeRequiresComptime(func_ty_info.return_type)) |ct| {
            is_comptime_call = ct;
            if (ct) {
                // stage1 can't handle doing this directly
                comptime_reason_buf = .{ .comptime_ret_ty = .{
                    .block = block,
                    .func = func,
                    .func_src = func_src,
                    .return_ty = func_ty_info.return_type,
                } };
                comptime_reason = &comptime_reason_buf;
            }
        } else |err| switch (err) {
            error.GenericPoison => is_generic_call = true,
            else => |e| return e,
        }
    }
    var is_inline_call = is_comptime_call or modifier == .always_inline or
        func_ty_info.cc == .Inline;

    if (!is_inline_call and is_generic_call) {
        if (sema.instantiateGenericCall(
            block,
            func,
            func_src,
            call_src,
            func_ty_info,
            ensure_result_used,
            uncasted_args,
            call_tag,
            bound_arg_src,
        )) |some| {
            return some;
        } else |err| switch (err) {
            error.GenericPoison => {
                is_inline_call = true;
            },
            error.ComptimeReturn => {
                is_inline_call = true;
                is_comptime_call = true;
                // stage1 can't handle doing this directly
                comptime_reason_buf = .{ .comptime_ret_ty = .{
                    .block = block,
                    .func = func,
                    .func_src = func_src,
                    .return_ty = func_ty_info.return_type,
                } };
                comptime_reason = &comptime_reason_buf;
            },
            else => |e| return e,
        }
    }

    if (is_comptime_call and modifier == .never_inline) {
        return sema.fail(block, call_src, "unable to perform 'never_inline' call at compile-time", .{});
    }

    const result: Air.Inst.Ref = if (is_inline_call) res: {
        const func_val = sema.resolveConstValue(block, func_src, func, "function being called at comptime must be comptime-known") catch |err| {
            if (err == error.AnalysisFail and comptime_reason != null) try comptime_reason.?.explain(sema, sema.err);
            return err;
        };
        const module_fn = switch (func_val.tag()) {
            .decl_ref => mod.declPtr(func_val.castTag(.decl_ref).?.data).val.castTag(.function).?.data,
            .function => func_val.castTag(.function).?.data,
            .extern_fn => return sema.fail(block, call_src, "{s} call of extern function", .{
                @as([]const u8, if (is_comptime_call) "comptime" else "inline"),
            }),
            else => {
                assert(callee_ty.isPtrAtRuntime());
                return sema.fail(block, call_src, "{s} call of function pointer", .{
                    @as([]const u8, if (is_comptime_call) "comptime" else "inline"),
                });
            },
        };
        if (func_ty_info.is_var_args) {
            return sema.fail(block, call_src, "{s} call of variadic function", .{
                @as([]const u8, if (is_comptime_call) "comptime" else "inline"),
            });
        }

        // Analyze the ZIR. The same ZIR gets analyzed into a runtime function
        // or an inlined call depending on what union tag the `label` field is
        // set to in the `Block`.
        // This block instruction will be used to capture the return value from the
        // inlined function.
        const block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
        try sema.air_instructions.append(gpa, .{
            .tag = .block,
            .data = undefined,
        });
        // This one is shared among sub-blocks within the same callee, but not
        // shared among the entire inline/comptime call stack.
        var inlining: Block.Inlining = .{
            .func = null,
            .comptime_result = undefined,
            .merges = .{
                .results = .{},
                .br_list = .{},
                .block_inst = block_inst,
            },
        };
        // In order to save a bit of stack space, directly modify Sema rather
        // than create a child one.
        const parent_zir = sema.code;
        const fn_owner_decl = mod.declPtr(module_fn.owner_decl);
        sema.code = fn_owner_decl.getFileScope().zir;
        defer sema.code = parent_zir;

        const parent_inst_map = sema.inst_map;
        sema.inst_map = .{};
        defer {
            sema.src = call_src;
            sema.inst_map.deinit(gpa);
            sema.inst_map = parent_inst_map;
        }

        const parent_func = sema.func;
        sema.func = module_fn;
        defer sema.func = parent_func;

        const parent_err_ret_index = sema.error_return_trace_index_on_fn_entry;
        sema.error_return_trace_index_on_fn_entry = block.error_return_trace_index;
        defer sema.error_return_trace_index_on_fn_entry = parent_err_ret_index;

        var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, fn_owner_decl.src_scope);
        defer wip_captures.deinit();

        var child_block: Block = .{
            .parent = null,
            .sema = sema,
            .src_decl = module_fn.owner_decl,
            .namespace = fn_owner_decl.src_namespace,
            .wip_capture_scope = wip_captures.scope,
            .instructions = .{},
            .label = null,
            .inlining = &inlining,
            .is_typeof = block.is_typeof,
            .is_comptime = is_comptime_call,
            .comptime_reason = comptime_reason,
            .error_return_trace_index = block.error_return_trace_index,
        };

        const merges = &child_block.inlining.?.merges;

        defer child_block.instructions.deinit(gpa);
        defer merges.results.deinit(gpa);
        defer merges.br_list.deinit(gpa);

        // If it's a comptime function call, we need to memoize it as long as no external
        // comptime memory is mutated.
        var memoized_call_key: Module.MemoizedCall.Key = undefined;
        var delete_memoized_call_key = false;
        defer if (delete_memoized_call_key) gpa.free(memoized_call_key.args);
        if (is_comptime_call) {
            memoized_call_key = .{
                .func = module_fn,
                .args = try gpa.alloc(TypedValue, func_ty_info.param_types.len),
            };
            delete_memoized_call_key = true;
        }

        try sema.emitBackwardBranch(block, call_src);

        // Whether this call should be memoized, set to false if the call can mutate
        // comptime state.
        var should_memoize = true;

        var new_fn_info = fn_owner_decl.ty.fnInfo();
        new_fn_info.param_types = try sema.arena.alloc(Type, new_fn_info.param_types.len);
        new_fn_info.comptime_params = (try sema.arena.alloc(bool, new_fn_info.param_types.len)).ptr;

        // This will have return instructions analyzed as break instructions to
        // the block_inst above. Here we are performing "comptime/inline semantic analysis"
        // for a function body, which means we must map the parameter ZIR instructions to
        // the AIR instructions of the callsite. The callee could be a generic function
        // which means its parameter type expressions must be resolved in order and used
        // to successively coerce the arguments.
        const fn_info = sema.code.getFnInfo(module_fn.zir_body_inst);
        try sema.inst_map.ensureSpaceForInstructions(sema.gpa, fn_info.param_body);

        var has_comptime_args = false;
        var arg_i: usize = 0;
        for (fn_info.param_body) |inst| {
            sema.analyzeInlineCallArg(
                block,
                &child_block,
                .unneeded,
                inst,
                new_fn_info,
                &arg_i,
                uncasted_args,
                is_comptime_call,
                &should_memoize,
                memoized_call_key,
                func_ty_info.param_types,
                func,
                &has_comptime_args,
            ) catch |err| switch (err) {
                error.NeededSourceLocation => {
                    _ = sema.inst_map.remove(inst);
                    const decl = sema.mod.declPtr(block.src_decl);
                    try sema.analyzeInlineCallArg(
                        block,
                        &child_block,
                        Module.argSrc(call_src.node_offset.x, sema.gpa, decl, arg_i, bound_arg_src),
                        inst,
                        new_fn_info,
                        &arg_i,
                        uncasted_args,
                        is_comptime_call,
                        &should_memoize,
                        memoized_call_key,
                        func_ty_info.param_types,
                        func,
                        &has_comptime_args,
                    );
                    unreachable;
                },
                else => |e| return e,
            };
        }

        if (!has_comptime_args and module_fn.state == .sema_failure) return error.AnalysisFail;

        const recursive_msg = "inline call is recursive";
        var head = if (!has_comptime_args) block else null;
        while (head) |some| {
            const parent_inlining = some.inlining orelse break;
            if (parent_inlining.func == module_fn) {
                return sema.fail(block, call_src, recursive_msg, .{});
            }
            head = some.parent;
        }
        if (!has_comptime_args) inlining.func = module_fn;

        // In case it is a generic function with an expression for the return type that depends
        // on parameters, we must now do the same for the return type as we just did with
        // each of the parameters, resolving the return type and providing it to the child
        // `Sema` so that it can be used for the `ret_ptr` instruction.
        const ret_ty_inst = if (fn_info.ret_ty_body.len != 0)
            try sema.resolveBody(&child_block, fn_info.ret_ty_body, module_fn.zir_body_inst)
        else
            try sema.resolveInst(fn_info.ret_ty_ref);
        const ret_ty_src: LazySrcLoc = .{ .node_offset_fn_type_ret_ty = 0 };
        const bare_return_type = try sema.analyzeAsType(&child_block, ret_ty_src, ret_ty_inst);
        // Create a fresh inferred error set type for inline/comptime calls.
        const fn_ret_ty = blk: {
            if (module_fn.hasInferredErrorSet(mod)) {
                const node = try sema.gpa.create(Module.Fn.InferredErrorSetListNode);
                node.data = .{ .func = module_fn };
                if (parent_func) |some| {
                    some.inferred_error_sets.prepend(node);
                }

                const error_set_ty = try Type.Tag.error_set_inferred.create(sema.arena, &node.data);
                break :blk try Type.Tag.error_union.create(sema.arena, .{
                    .error_set = error_set_ty,
                    .payload = bare_return_type,
                });
            }
            break :blk bare_return_type;
        };
        new_fn_info.return_type = fn_ret_ty;
        const parent_fn_ret_ty = sema.fn_ret_ty;
        sema.fn_ret_ty = fn_ret_ty;
        defer sema.fn_ret_ty = parent_fn_ret_ty;

        // This `res2` is here instead of directly breaking from `res` due to a stage1
        // bug generating invalid LLVM IR.
        const res2: Air.Inst.Ref = res2: {
            if (should_memoize and is_comptime_call) {
                if (mod.memoized_calls.getContext(memoized_call_key, .{ .module = mod })) |result| {
                    const ty_inst = try sema.addType(fn_ret_ty);
                    try sema.air_values.append(gpa, result.val);
                    sema.air_instructions.set(block_inst, .{
                        .tag = .constant,
                        .data = .{ .ty_pl = .{
                            .ty = ty_inst,
                            .payload = @intCast(u32, sema.air_values.items.len - 1),
                        } },
                    });
                    break :res2 Air.indexToRef(block_inst);
                }
            }

            const new_func_resolved_ty = try Type.Tag.function.create(sema.arena, new_fn_info);
            if (!is_comptime_call and !block.is_typeof) {
                try sema.emitDbgInline(block, parent_func.?, module_fn, new_func_resolved_ty, .dbg_inline_begin);

                const zir_tags = sema.code.instructions.items(.tag);
                for (fn_info.param_body) |param| switch (zir_tags[param]) {
                    .param, .param_comptime => {
                        const inst_data = sema.code.instructions.items(.data)[param].pl_tok;
                        const extra = sema.code.extraData(Zir.Inst.Param, inst_data.payload_index);
                        const param_name = sema.code.nullTerminatedString(extra.data.name);
                        const inst = sema.inst_map.get(param).?;

                        try sema.addDbgVar(&child_block, inst, .dbg_var_val, param_name);
                    },
                    .param_anytype, .param_anytype_comptime => {
                        const inst_data = sema.code.instructions.items(.data)[param].str_tok;
                        const param_name = inst_data.get(sema.code);
                        const inst = sema.inst_map.get(param).?;

                        try sema.addDbgVar(&child_block, inst, .dbg_var_val, param_name);
                    },
                    else => continue,
                };
            }

            if (is_comptime_call and ensure_result_used) {
                try sema.ensureResultUsed(block, fn_ret_ty, call_src);
            }

            const result = result: {
                sema.analyzeBody(&child_block, fn_info.body) catch |err| switch (err) {
                    error.ComptimeReturn => break :result inlining.comptime_result,
                    error.AnalysisFail => {
                        const err_msg = sema.err orelse return err;
                        if (std.mem.eql(u8, err_msg.msg, recursive_msg)) return err;
                        try sema.errNote(block, call_src, err_msg, "called from here", .{});
                        err_msg.clearTrace(sema.gpa);
                        return err;
                    },
                    else => |e| return e,
                };
                break :result try sema.analyzeBlockBody(block, call_src, &child_block, merges);
            };

            if (!is_comptime_call and !block.is_typeof and sema.typeOf(result).zigTypeTag() != .NoReturn) {
                try sema.emitDbgInline(
                    block,
                    module_fn,
                    parent_func.?,
                    mod.declPtr(parent_func.?.owner_decl).ty,
                    .dbg_inline_end,
                );
            }

            if (should_memoize and is_comptime_call) {
                const result_val = try sema.resolveConstMaybeUndefVal(block, .unneeded, result, "");

                // TODO: check whether any external comptime memory was mutated by the
                // comptime function call. If so, then do not memoize the call here.
                // TODO: re-evaluate whether memoized_calls needs its own arena. I think
                // it should be fine to use the Decl arena for the function.
                {
                    var arena_allocator = std.heap.ArenaAllocator.init(gpa);
                    errdefer arena_allocator.deinit();
                    const arena = arena_allocator.allocator();

                    for (memoized_call_key.args) |*arg| {
                        arg.* = try arg.*.copy(arena);
                    }

                    try mod.memoized_calls.putContext(gpa, memoized_call_key, .{
                        .val = try result_val.copy(arena),
                        .arena = arena_allocator.state,
                    }, .{ .module = mod });
                    delete_memoized_call_key = false;
                }
            }

            break :res2 result;
        };

        try wip_captures.finalize();

        break :res res2;
    } else res: {
        assert(!func_ty_info.is_generic);

        const args = try sema.arena.alloc(Air.Inst.Ref, uncasted_args.len);
        for (uncasted_args, 0..) |uncasted_arg, i| {
            if (i < fn_params_len) {
                const opts: CoerceOpts = .{ .param_src = .{
                    .func_inst = func,
                    .param_i = @intCast(u32, i),
                } };
                const param_ty = func_ty.fnParamType(i);
                args[i] = sema.analyzeCallArg(
                    block,
                    .unneeded,
                    param_ty,
                    uncasted_arg,
                    opts,
                ) catch |err| switch (err) {
                    error.NeededSourceLocation => {
                        const decl = sema.mod.declPtr(block.src_decl);
                        _ = try sema.analyzeCallArg(
                            block,
                            Module.argSrc(call_src.node_offset.x, sema.gpa, decl, i, bound_arg_src),
                            param_ty,
                            uncasted_arg,
                            opts,
                        );
                        unreachable;
                    },
                    else => |e| return e,
                };
            } else {
                args[i] = sema.coerceVarArgParam(block, uncasted_arg, .unneeded) catch |err| switch (err) {
                    error.NeededSourceLocation => {
                        const decl = sema.mod.declPtr(block.src_decl);
                        _ = try sema.coerceVarArgParam(
                            block,
                            uncasted_arg,
                            Module.argSrc(call_src.node_offset.x, sema.gpa, decl, i, bound_arg_src),
                        );
                        unreachable;
                    },
                    else => |e| return e,
                };
            }
        }

        try sema.queueFullTypeResolution(func_ty_info.return_type);
        if (sema.owner_func != null and func_ty_info.return_type.isError()) {
            sema.owner_func.?.calls_or_awaits_errorable_fn = true;
        }

        try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.Call).Struct.fields.len +
            args.len);
        const func_inst = try block.addInst(.{
            .tag = call_tag,
            .data = .{ .pl_op = .{
                .operand = func,
                .payload = sema.addExtraAssumeCapacity(Air.Call{
                    .args_len = @intCast(u32, args.len),
                }),
            } },
        });
        sema.appendRefsAssumeCapacity(args);
        break :res func_inst;
    };

    if (ensure_result_used) {
        try sema.ensureResultUsed(block, sema.typeOf(result), call_src);
    }
    if (call_tag == .call_always_tail) {
        return sema.handleTailCall(block, call_src, func_ty, result);
    }
    return result;
}

fn handleTailCall(sema: *Sema, block: *Block, call_src: LazySrcLoc, func_ty: Type, result: Air.Inst.Ref) !Air.Inst.Ref {
    const target = sema.mod.getTarget();
    const backend = sema.mod.comp.getZigBackend();
    if (!target_util.supportsTailCall(target, backend)) {
        return sema.fail(block, call_src, "unable to perform tail call: compiler backend '{s}' does not support tail calls on target architecture '{s}' with the selected CPU feature flags", .{
            @tagName(backend), @tagName(target.cpu.arch),
        });
    }
    const func_decl = sema.mod.declPtr(sema.owner_func.?.owner_decl);
    if (!func_ty.eql(func_decl.ty, sema.mod)) {
        return sema.fail(block, call_src, "unable to perform tail call: type of function being called '{}' does not match type of calling function '{}'", .{
            func_ty.fmt(sema.mod), func_decl.ty.fmt(sema.mod),
        });
    }
    _ = try block.addUnOp(.ret, result);
    return Air.Inst.Ref.unreachable_value;
}

fn analyzeInlineCallArg(
    sema: *Sema,
    arg_block: *Block,
    param_block: *Block,
    arg_src: LazySrcLoc,
    inst: Zir.Inst.Index,
    new_fn_info: Type.Payload.Function.Data,
    arg_i: *usize,
    uncasted_args: []const Air.Inst.Ref,
    is_comptime_call: bool,
    should_memoize: *bool,
    memoized_call_key: Module.MemoizedCall.Key,
    raw_param_types: []const Type,
    func_inst: Air.Inst.Ref,
    has_comptime_args: *bool,
) !void {
    const zir_tags = sema.code.instructions.items(.tag);
    switch (zir_tags[inst]) {
        .param_comptime, .param_anytype_comptime => has_comptime_args.* = true,
        else => {},
    }
    switch (zir_tags[inst]) {
        .param, .param_comptime => {
            // Evaluate the parameter type expression now that previous ones have
            // been mapped, and coerce the corresponding argument to it.
            const pl_tok = sema.code.instructions.items(.data)[inst].pl_tok;
            const param_src = pl_tok.src();
            const extra = sema.code.extraData(Zir.Inst.Param, pl_tok.payload_index);
            const param_body = sema.code.extra[extra.end..][0..extra.data.body_len];
            const param_ty = param_ty: {
                const raw_param_ty = raw_param_types[arg_i.*];
                if (raw_param_ty.tag() != .generic_poison) break :param_ty raw_param_ty;
                const param_ty_inst = try sema.resolveBody(param_block, param_body, inst);
                break :param_ty try sema.analyzeAsType(param_block, param_src, param_ty_inst);
            };
            new_fn_info.param_types[arg_i.*] = param_ty;
            const uncasted_arg = uncasted_args[arg_i.*];
            if (try sema.typeRequiresComptime(param_ty)) {
                _ = sema.resolveConstMaybeUndefVal(arg_block, arg_src, uncasted_arg, "argument to parameter with comptime-only type must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and param_block.comptime_reason != null) try param_block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
            } else if (!is_comptime_call and zir_tags[inst] == .param_comptime) {
                _ = try sema.resolveConstMaybeUndefVal(arg_block, arg_src, uncasted_arg, "parameter is comptime");
            }
            const casted_arg = sema.coerceExtra(arg_block, param_ty, uncasted_arg, arg_src, .{ .param_src = .{
                .func_inst = func_inst,
                .param_i = @intCast(u32, arg_i.*),
            } }) catch |err| switch (err) {
                error.NotCoercible => unreachable,
                else => |e| return e,
            };

            if (is_comptime_call) {
                sema.inst_map.putAssumeCapacityNoClobber(inst, casted_arg);
                const arg_val = sema.resolveConstMaybeUndefVal(arg_block, arg_src, casted_arg, "argument to function being called at comptime must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and param_block.comptime_reason != null) try param_block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                switch (arg_val.tag()) {
                    .generic_poison, .generic_poison_type => {
                        // This function is currently evaluated as part of an as-of-yet unresolvable
                        // parameter or return type.
                        return error.GenericPoison;
                    },
                    else => {
                        // Needed so that lazy values do not trigger
                        // assertion due to type not being resolved
                        // when the hash function is called.
                        try sema.resolveLazyValue(arg_val);
                    },
                }
                should_memoize.* = should_memoize.* and !arg_val.canMutateComptimeVarState();
                memoized_call_key.args[arg_i.*] = .{
                    .ty = param_ty,
                    .val = arg_val,
                };
            } else {
                sema.inst_map.putAssumeCapacityNoClobber(inst, casted_arg);
            }

            if (try sema.resolveMaybeUndefVal(casted_arg)) |_| {
                has_comptime_args.* = true;
            }

            arg_i.* += 1;
        },
        .param_anytype, .param_anytype_comptime => {
            // No coercion needed.
            const uncasted_arg = uncasted_args[arg_i.*];
            new_fn_info.param_types[arg_i.*] = sema.typeOf(uncasted_arg);

            if (is_comptime_call) {
                sema.inst_map.putAssumeCapacityNoClobber(inst, uncasted_arg);
                const arg_val = sema.resolveConstMaybeUndefVal(arg_block, arg_src, uncasted_arg, "argument to function being called at comptime must be comptime-known") catch |err| {
                    if (err == error.AnalysisFail and param_block.comptime_reason != null) try param_block.comptime_reason.?.explain(sema, sema.err);
                    return err;
                };
                switch (arg_val.tag()) {
                    .generic_poison, .generic_poison_type => {
                        // This function is currently evaluated as part of an as-of-yet unresolvable
                        // parameter or return type.
                        return error.GenericPoison;
                    },
                    else => {
                        // Needed so that lazy values do not trigger
                        // assertion due to type not being resolved
                        // when the hash function is called.
                        try sema.resolveLazyValue(arg_val);
                    },
                }
                should_memoize.* = should_memoize.* and !arg_val.canMutateComptimeVarState();
                memoized_call_key.args[arg_i.*] = .{
                    .ty = sema.typeOf(uncasted_arg),
                    .val = arg_val,
                };
            } else {
                if (zir_tags[inst] == .param_anytype_comptime) {
                    _ = try sema.resolveConstMaybeUndefVal(arg_block, arg_src, uncasted_arg, "parameter is comptime");
                }
                sema.inst_map.putAssumeCapacityNoClobber(inst, uncasted_arg);
            }

            if (try sema.resolveMaybeUndefVal(uncasted_arg)) |_| {
                has_comptime_args.* = true;
            }

            arg_i.* += 1;
        },
        else => {},
    }
}

fn analyzeCallArg(
    sema: *Sema,
    block: *Block,
    arg_src: LazySrcLoc,
    param_ty: Type,
    uncasted_arg: Air.Inst.Ref,
    opts: CoerceOpts,
) !Air.Inst.Ref {
    try sema.resolveTypeFully(param_ty);
    return sema.coerceExtra(block, param_ty, uncasted_arg, arg_src, opts) catch |err| switch (err) {
        error.NotCoercible => unreachable,
        else => |e| return e,
    };
}

fn analyzeGenericCallArg(
    sema: *Sema,
    block: *Block,
    arg_src: LazySrcLoc,
    uncasted_arg: Air.Inst.Ref,
    comptime_arg: TypedValue,
    runtime_args: []Air.Inst.Ref,
    new_fn_info: Type.Payload.Function.Data,
    runtime_i: *u32,
) !void {
    const is_runtime = comptime_arg.val.tag() == .generic_poison and
        comptime_arg.ty.hasRuntimeBits() and
        !(try sema.typeRequiresComptime(comptime_arg.ty));
    if (is_runtime) {
        const param_ty = new_fn_info.param_types[runtime_i.*];
        const casted_arg = try sema.coerce(block, param_ty, uncasted_arg, arg_src);
        try sema.queueFullTypeResolution(param_ty);
        runtime_args[runtime_i.*] = casted_arg;
        runtime_i.* += 1;
    } else if (try sema.typeHasOnePossibleValue(comptime_arg.ty)) |_| {
        _ = try sema.coerce(block, comptime_arg.ty, uncasted_arg, arg_src);
    }
}

fn analyzeGenericCallArgVal(sema: *Sema, block: *Block, arg_src: LazySrcLoc, uncasted_arg: Air.Inst.Ref) !Value {
    const arg_val = try sema.resolveValue(block, arg_src, uncasted_arg, "parameter is comptime");
    try sema.resolveLazyValue(arg_val);
    return arg_val;
}

fn instantiateGenericCall(
    sema: *Sema,
    block: *Block,
    func: Air.Inst.Ref,
    func_src: LazySrcLoc,
    call_src: LazySrcLoc,
    func_ty_info: Type.Payload.Function.Data,
    ensure_result_used: bool,
    uncasted_args: []const Air.Inst.Ref,
    call_tag: Air.Inst.Tag,
    bound_arg_src: ?LazySrcLoc,
) CompileError!Air.Inst.Ref {
    const mod = sema.mod;
    const gpa = sema.gpa;

    const func_val = try sema.resolveConstValue(block, func_src, func, "generic function being called must be comptime-known");
    const module_fn = switch (func_val.tag()) {
        .function => func_val.castTag(.function).?.data,
        .decl_ref => mod.declPtr(func_val.castTag(.decl_ref).?.data).val.castTag(.function).?.data,
        else => unreachable,
    };
    // Check the Module's generic function map with an adapted context, so that we
    // can match against `uncasted_args` rather than doing the work below to create a
    // generic Scope only to junk it if it matches an existing instantiation.
    const fn_owner_decl = mod.declPtr(module_fn.owner_decl);
    const namespace = fn_owner_decl.src_namespace;
    const fn_zir = namespace.file_scope.zir;
    const fn_info = fn_zir.getFnInfo(module_fn.zir_body_inst);
    const zir_tags = fn_zir.instructions.items(.tag);

    // This hash must match `Module.MonomorphedFuncsContext.hash`.
    // For parameters explicitly marked comptime and simple parameter type expressions,
    // we know whether a parameter is elided from a monomorphed function, and can
    // use it in the hash here. However, for parameter type expressions that are not
    // explicitly marked comptime and rely on previous parameter comptime values, we
    // don't find out until after generating a monomorphed function whether the parameter
    // type ended up being a "must-be-comptime-known" type.
    var hasher = std.hash.Wyhash.init(0);
    std.hash.autoHash(&hasher, module_fn.owner_decl);

    const generic_args = try sema.arena.alloc(GenericCallAdapter.Arg, func_ty_info.param_types.len);
    {
        var i: usize = 0;
        for (fn_info.param_body) |inst| {
            var is_comptime = false;
            var is_anytype = false;
            switch (zir_tags[inst]) {
                .param => {
                    is_comptime = func_ty_info.paramIsComptime(i);
                },
                .param_comptime => {
                    is_comptime = true;
                },
                .param_anytype => {
                    is_anytype = true;
                    is_comptime = func_ty_info.paramIsComptime(i);
                },
                .param_anytype_comptime => {
                    is_anytype = true;
                    is_comptime = true;
                },
                else => continue,
            }

            const arg_ty = sema.typeOf(uncasted_args[i]);
            if (is_comptime or is_anytype) {
                // Tuple default values are a part of the type and need to be
                // resolved to hash the type.
                try sema.resolveTupleLazyValues(block, call_src, arg_ty);
            }

            if (is_comptime) {
                const arg_val = sema.analyzeGenericCallArgVal(block, .unneeded, uncasted_args[i]) catch |err| switch (err) {
                    error.NeededSourceLocation => {
                        const decl = sema.mod.declPtr(block.src_decl);
                        const arg_src = Module.argSrc(call_src.node_offset.x, sema.gpa, decl, i, bound_arg_src);
                        _ = try sema.analyzeGenericCallArgVal(block, arg_src, uncasted_args[i]);
                        unreachable;
                    },
                    else => |e| return e,
                };
                arg_val.hashUncoerced(arg_ty, &hasher, mod);
                if (is_anytype) {
                    arg_ty.hashWithHasher(&hasher, mod);
                    generic_args[i] = .{
                        .ty = arg_ty,
                        .val = arg_val,
                        .is_anytype = true,
                    };
                } else {
                    generic_args[i] = .{
                        .ty = arg_ty,
                        .val = arg_val,
                        .is_anytype = false,
                    };
                }
            } else if (is_anytype) {
                arg_ty.hashWithHasher(&hasher, mod);
                generic_args[i] = .{
                    .ty = arg_ty,
                    .val = Value.initTag(.generic_poison),
                    .is_anytype = true,
                };
            } else {
                generic_args[i] = .{
                    .ty = arg_ty,
                    .val = Value.initTag(.generic_poison),
                    .is_anytype = false,
                };
            }

            i += 1;
        }
    }

    const precomputed_hash = hasher.final();

    const adapter: GenericCallAdapter = .{
        .generic_fn = module_fn,
        .precomputed_hash = precomputed_hash,
        .func_ty_info = func_ty_info,
        .args = generic_args,
        .module = mod,
    };
    const gop = try mod.monomorphed_funcs.getOrPutAdapted(gpa, {}, adapter);
    const callee = if (!gop.found_existing) callee: {
        const new_module_func = try gpa.create(Module.Fn);

        // This ensures that we can operate on the hash map before the Module.Fn
        // struct is fully initialized.
        new_module_func.hash = precomputed_hash;
        new_module_func.generic_owner_decl = module_fn.owner_decl.toOptional();
        new_module_func.comptime_args = null;
        gop.key_ptr.* = new_module_func;

        try namespace.anon_decls.ensureUnusedCapacity(gpa, 1);

        // Create a Decl for the new function.
        const src_decl_index = namespace.getDeclIndex();
        const src_decl = mod.declPtr(src_decl_index);
        const new_decl_index = try mod.allocateNewDecl(namespace, fn_owner_decl.src_node, src_decl.src_scope);
        const new_decl = mod.declPtr(new_decl_index);
        // TODO better names for generic function instantiations
        const decl_name = try std.fmt.allocPrintZ(gpa, "{s}__anon_{d}", .{
            fn_owner_decl.name, @enumToInt(new_decl_index),
        });
        new_decl.name = decl_name;
        new_decl.src_line = fn_owner_decl.src_line;
        new_decl.is_pub = fn_owner_decl.is_pub;
        new_decl.is_exported = fn_owner_decl.is_exported;
        new_decl.has_align = fn_owner_decl.has_align;
        new_decl.has_linksection_or_addrspace = fn_owner_decl.has_linksection_or_addrspace;
        new_decl.@"linksection" = fn_owner_decl.@"linksection";
        new_decl.@"addrspace" = fn_owner_decl.@"addrspace";
        new_decl.zir_decl_index = fn_owner_decl.zir_decl_index;
        new_decl.alive = true; // This Decl is called at runtime.
        new_decl.analysis = .in_progress;
        new_decl.generation = mod.generation;

        namespace.anon_decls.putAssumeCapacityNoClobber(new_decl_index, {});

        // The generic function Decl is guaranteed to be the first dependency
        // of each of its instantiations.
        assert(new_decl.dependencies.keys().len == 0);
        try mod.declareDeclDependency(new_decl_index, module_fn.owner_decl);

        var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
        const new_decl_arena_allocator = new_decl_arena.allocator();

        const new_func = sema.resolveGenericInstantiationType(
            block,
            new_decl_arena_allocator,
            fn_zir,
            new_decl,
            new_decl_index,
            uncasted_args,
            module_fn,
            new_module_func,
            namespace,
            func_ty_info,
            call_src,
            bound_arg_src,
        ) catch |err| switch (err) {
            error.GenericPoison, error.ComptimeReturn => {
                new_decl_arena.deinit();
                // Resolving the new function type below will possibly declare more decl dependencies
                // and so we remove them all here in case of error.
                for (new_decl.dependencies.keys()) |dep_index| {
                    const dep = mod.declPtr(dep_index);
                    dep.removeDependant(new_decl_index);
                }
                assert(namespace.anon_decls.orderedRemove(new_decl_index));
                mod.destroyDecl(new_decl_index);
                assert(mod.monomorphed_funcs.remove(new_module_func));
                gpa.destroy(new_module_func);
                return err;
            },
            else => {
                assert(mod.monomorphed_funcs.remove(new_module_func));
                {
                    errdefer new_decl_arena.deinit();
                    try new_decl.finalizeNewArena(&new_decl_arena);
                }
                // TODO look up the compile error that happened here and attach a note to it
                // pointing here, at the generic instantiation callsite.
                if (sema.owner_func) |owner_func| {
                    owner_func.state = .dependency_failure;
                } else {
                    sema.owner_decl.analysis = .dependency_failure;
                }
                return err;
            },
        };
        errdefer new_decl_arena.deinit();

        try new_decl.finalizeNewArena(&new_decl_arena);
        break :callee new_func;
    } else gop.key_ptr.*;

    callee.branch_quota = @max(callee.branch_quota, sema.branch_quota);

    const callee_inst = try sema.analyzeDeclVal(block, func_src, callee.owner_decl);

    // Make a runtime call to the new function, making sure to omit the comptime args.
    const comptime_args = callee.comptime_args.?;
    const func_ty = mod.declPtr(callee.owner_decl).ty;
    const new_fn_info = func_ty.fnInfo();
    const runtime_args_len = @intCast(u32, new_fn_info.param_types.len);
    const runtime_args = try sema.arena.alloc(Air.Inst.Ref, runtime_args_len);
    {
        var runtime_i: u32 = 0;
        var total_i: u32 = 0;
        for (fn_info.param_body) |inst| {
            switch (zir_tags[inst]) {
                .param_comptime, .param_anytype_comptime, .param, .param_anytype => {},
                else => continue,
            }
            sema.analyzeGenericCallArg(
                block,
                .unneeded,
                uncasted_args[total_i],
                comptime_args[total_i],
                runtime_args,
                new_fn_info,
                &runtime_i,
            ) catch |err| switch (err) {
                error.NeededSourceLocation => {
                    const decl = sema.mod.declPtr(block.src_decl);
                    _ = try sema.analyzeGenericCallArg(
                        block,
                        Module.argSrc(call_src.node_offset.x, sema.gpa, decl, total_i, bound_arg_src),
                        uncasted_args[total_i],
                        comptime_args[total_i],
                        runtime_args,
                        new_fn_info,
                        &runtime_i,
                    );
                    unreachable;
                },
                else => |e| return e,
            };
            total_i += 1;
        }

        try sema.queueFullTypeResolution(new_fn_info.return_type);
    }

    if (sema.owner_func != null and new_fn_info.return_type.isError()) {
        sema.owner_func.?.calls_or_awaits_errorable_fn = true;
    }

    try sema.air_extra.ensureUnusedCapacity(sema.gpa, @typeInfo(Air.Call).Struct.fields.len +
        runtime_args_len);
    const result = try block.addInst(.{
        .tag = call_tag,
        .data = .{ .pl_op = .{
            .operand = callee_inst,
            .payload = sema.addExtraAssumeCapacity(Air.Call{
                .args_len = runtime_args_len,
            }),
        } },
    });
    sema.appendRefsAssumeCapacity(runtime_args);

    if (ensure_result_used) {
        try sema.ensureResultUsed(block, sema.typeOf(result), call_src);
    }
    if (call_tag == .call_always_tail) {
        return sema.handleTailCall(block, call_src, func_ty, result);
    }
    return result;
}

fn resolveGenericInstantiationType(
    sema: *Sema,
    block: *Block,
    new_decl_arena_allocator: Allocator,
    fn_zir: Zir,
    new_decl: *Decl,
    new_decl_index: Decl.Index,
    uncasted_args: []const Air.Inst.Ref,
    module_fn: *Module.Fn,
    new_module_func: *Module.Fn,
    namespace: *Namespace,
    func_ty_info: Type.Payload.Function.Data,
    call_src: LazySrcLoc,
    bound_arg_src: ?LazySrcLoc,
) !*Module.Fn {
    const mod = sema.mod;
    const gpa = sema.gpa;

    const zir_tags = fn_zir.instructions.items(.tag);
    const fn_info = fn_zir.getFnInfo(module_fn.zir_body_inst);

    // Re-run the block that creates the function, with the comptime parameters
    // pre-populated inside `inst_map`. This causes `param_comptime` and
    // `param_anytype_comptime` ZIR instructions to be ignored, resulting in a
    // new, monomorphized function, with the comptime parameters elided.
    var child_sema: Sema = .{
        .mod = mod,
        .gpa = gpa,
        .arena = sema.arena,
        .perm_arena = new_decl_arena_allocator,
        .code = fn_zir,
        .owner_decl = new_decl,
        .owner_decl_index = new_decl_index,
        .func = null,
        .fn_ret_ty = Type.void,
        .owner_func = null,
        .comptime_args = try new_decl_arena_allocator.alloc(TypedValue, uncasted_args.len),
        .comptime_args_fn_inst = module_fn.zir_body_inst,
        .preallocated_new_func = new_module_func,
        .is_generic_instantiation = true,
        .branch_quota = sema.branch_quota,
        .branch_count = sema.branch_count,
    };
    defer child_sema.deinit();

    var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, new_decl.src_scope);
    defer wip_captures.deinit();

    var child_block: Block = .{
        .parent = null,
        .sema = &child_sema,
        .src_decl = new_decl_index,
        .namespace = namespace,
        .wip_capture_scope = wip_captures.scope,
        .instructions = .{},
        .inlining = null,
        .is_comptime = true,
    };
    defer {
        child_block.instructions.deinit(gpa);
        child_block.params.deinit(gpa);
    }

    try child_sema.inst_map.ensureSpaceForInstructions(gpa, fn_info.param_body);

    var arg_i: usize = 0;
    for (fn_info.param_body) |inst| {
        var is_comptime = false;
        var is_anytype = false;
        switch (zir_tags[inst]) {
            .param => {
                is_comptime = func_ty_info.paramIsComptime(arg_i);
            },
            .param_comptime => {
                is_comptime = true;
            },
            .param_anytype => {
                is_anytype = true;
                is_comptime = func_ty_info.paramIsComptime(arg_i);
            },
            .param_anytype_comptime => {
                is_anytype = true;
                is_comptime = true;
            },
            else => continue,
        }
        const arg = uncasted_args[arg_i];
        if (is_comptime) {
            const arg_val = (try sema.resolveMaybeUndefVal(arg)).?;
            const child_arg = try child_sema.addConstant(sema.typeOf(arg), arg_val);
            child_sema.inst_map.putAssumeCapacityNoClobber(inst, child_arg);
        } else if (is_anytype) {
            const arg_ty = sema.typeOf(arg);
            if (try sema.typeRequiresComptime(arg_ty)) {
                const arg_val = sema.resolveConstValue(block, .unneeded, arg, "") catch |err| switch (err) {
                    error.NeededSourceLocation => {
                        const decl = sema.mod.declPtr(block.src_decl);
                        const arg_src = Module.argSrc(call_src.node_offset.x, sema.gpa, decl, arg_i, bound_arg_src);
                        _ = try sema.resolveConstValue(block, arg_src, arg, "argument to parameter with comptime-only type must be comptime-known");
                        unreachable;
                    },
                    else => |e| return e,
                };
                const child_arg = try child_sema.addConstant(arg_ty, arg_val);
                child_sema.inst_map.putAssumeCapacityNoClobber(inst, child_arg);
            } else {
                // We insert into the map an instruction which is runtime-known
                // but has the type of the argument.
                const child_arg = try child_block.addInst(.{
                    .tag = .arg,
                    .data = .{ .arg = .{
                        .ty = try child_sema.addType(arg_ty),
                        .src_index = @intCast(u32, arg_i),
                    } },
                });
                child_sema.inst_map.putAssumeCapacityNoClobber(inst, child_arg);
            }
        }
        arg_i += 1;
    }

    // Save the error trace as our first action in the function.
    // If this is unnecessary after all, Liveness will clean it up for us.
    const error_return_trace_index = try sema.analyzeSaveErrRetIndex(&child_block);
    child_sema.error_return_trace_index_on_fn_entry = error_return_trace_index;
    child_block.error_return_trace_index = error_return_trace_index;

    const new_func_inst = try child_sema.resolveBody(&child_block, fn_info.param_body, fn_info.param_body_inst);
    const new_func_val = child_sema.resolveConstValue(&child_block, .unneeded, new_func_inst, undefined) catch unreachable;
    const new_func = new_func_val.castTag(.function).?.data;
    errdefer new_func.deinit(gpa);
    assert(new_func == new_module_func);

    arg_i = 0;
    for (fn_info.param_body) |inst| {
        var is_comptime = false;
        switch (zir_tags[inst]) {
            .param => {
                is_comptime = func_ty_info.paramIsComptime(arg_i);
            },
            .param_comptime => {
                is_comptime = true;
            },
            .param_anytype => {
                is_comptime = func_ty_info.paramIsComptime(arg_i);
            },
            .param_anytype_comptime => {
                is_comptime = true;
            },
            else => continue,
        }

        // We populate the Type here regardless because it is needed by
        // `GenericCallAdapter.eql` as well as function body analysis.
        // Whether it is anytype is communicated by `isAnytypeParam`.
        const arg = child_sema.inst_map.get(inst).?;
        const copied_arg_ty = try child_sema.typeOf(arg).copy(new_decl_arena_allocator);

        if (try sema.typeRequiresComptime(copied_arg_ty)) {
            is_comptime = true;
        }

        if (is_comptime) {
            const arg_val = (child_sema.resolveMaybeUndefValAllowVariables(arg) catch unreachable).?;
            child_sema.comptime_args[arg_i] = .{
                .ty = copied_arg_ty,
                .val = try arg_val.copy(new_decl_arena_allocator),
            };
        } else {
            child_sema.comptime_args[arg_i] = .{
                .ty = copied_arg_ty,
                .val = Value.initTag(.generic_poison),
            };
        }

        arg_i += 1;
    }

    try wip_captures.finalize();

    // Populate the Decl ty/val with the function and its type.
    new_decl.ty = try child_sema.typeOf(new_func_inst).copy(new_decl_arena_allocator);
    // If the call evaluated to a return type that requires comptime, never mind
    // our generic instantiation. Instead we need to perform a comptime call.
    const new_fn_info = new_decl.ty.fnInfo();
    if (try sema.typeRequiresComptime(new_fn_info.return_type)) {
        return error.ComptimeReturn;
    }
    // Similarly, if the call evaluated to a generic type we need to instead
    // call it inline.
    if (new_fn_info.is_generic or new_fn_info.cc == .Inline) {
        return error.GenericPoison;
    }

    new_decl.val = try Value.Tag.function.create(new_decl_arena_allocator, new_func);
    new_decl.@"align" = 0;
    new_decl.has_tv = true;
    new_decl.owns_tv = true;
    new_decl.analysis = .complete;

    log.debug("generic function '{s}' instantiated with type {}", .{
        new_decl.name, new_decl.ty.fmtDebug(),
    });

    // Queue up a `codegen_func` work item for the new Fn. The `comptime_args` field
    // will be populated, ensuring it will have `analyzeBody` called with the ZIR
    // parameters mapped appropriately.
    try mod.comp.work_queue.writeItem(.{ .codegen_func = new_func });
    return new_func;
}

fn resolveTupleLazyValues(sema: *Sema, block: *Block, src: LazySrcLoc, ty: Type) CompileError!void {
    if (!ty.isSimpleTupleOrAnonStruct()) return;
    const tuple = ty.tupleFields();
    for (tuple.values, 0..) |field_val, i| {
        try sema.resolveTupleLazyValues(block, src, tuple.types[i]);
        if (field_val.tag() == .unreachable_value) continue;
        try sema.resolveLazyValue(field_val);
    }
}

fn emitDbgInline(
    sema: *Sema,
    block: *Block,
    old_func: *Module.Fn,
    new_func: *Module.Fn,
    new_func_ty: Type,
    tag: Air.Inst.Tag,
) CompileError!void {
    if (sema.mod.comp.bin_file.options.strip) return;

    // Recursive inline call; no dbg_inline needed.
    if (old_func == new_func) return;

    try sema.air_values.append(sema.gpa, try Value.Tag.function.create(sema.arena, new_func));
    _ = try block.addInst(.{
        .tag = tag,
        .data = .{ .ty_pl = .{
            .ty = try sema.addType(new_func_ty),
            .payload = @intCast(u32, sema.air_values.items.len - 1),
        } },
    });
}

fn zirIntType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const tracy = trace(@src());
    defer tracy.end();

    const int_type = sema.code.instructions.items(.data)[inst].int_type;
    const ty = try Module.makeIntType(sema.arena, int_type.signedness, int_type.bit_count);

    return sema.addType(ty);
}

fn zirOptionalType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_un_op = inst_data.src_node };
    const child_type = try sema.resolveType(block, operand_src, inst_data.operand);
    if (child_type.zigTypeTag() == .Opaque) {
        return sema.fail(block, operand_src, "opaque type '{}' cannot be optional", .{child_type.fmt(sema.mod)});
    } else if (child_type.zigTypeTag() == .Null) {
        return sema.fail(block, operand_src, "type '{}' cannot be optional", .{child_type.fmt(sema.mod)});
    }
    const opt_type = try Type.optional(sema.arena, child_type);

    return sema.addType(opt_type);
}

fn zirElemTypeIndex(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const bin = sema.code.instructions.items(.data)[inst].bin;
    const indexable_ty = try sema.resolveType(block, .unneeded, bin.lhs);
    assert(indexable_ty.isIndexable()); // validated by a previous instruction
    if (indexable_ty.zigTypeTag() == .Struct) {
        const elem_type = indexable_ty.structFieldType(@enumToInt(bin.rhs));
        return sema.addType(elem_type);
    } else {
        const elem_type = indexable_ty.elemType2();
        return sema.addType(elem_type);
    }
}

fn zirVectorType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const elem_type_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const len_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const len = try sema.resolveInt(block, len_src, extra.lhs, Type.u32, "vector length must be comptime-known");
    const elem_type = try sema.resolveType(block, elem_type_src, extra.rhs);
    try sema.checkVectorElemType(block, elem_type_src, elem_type);
    const vector_type = try Type.Tag.vector.create(sema.arena, .{
        .len = @intCast(u32, len),
        .elem_type = elem_type,
    });
    return sema.addType(vector_type);
}

fn zirArrayType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const len_src: LazySrcLoc = .{ .node_offset_array_type_len = inst_data.src_node };
    const elem_src: LazySrcLoc = .{ .node_offset_array_type_elem = inst_data.src_node };
    const len = try sema.resolveInt(block, len_src, extra.lhs, Type.usize, "array length must be comptime-known");
    const elem_type = try sema.resolveType(block, elem_src, extra.rhs);
    const array_ty = try Type.array(sema.arena, len, null, elem_type, sema.mod);

    return sema.addType(array_ty);
}

fn zirArrayTypeSentinel(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.ArrayTypeSentinel, inst_data.payload_index).data;
    const len_src: LazySrcLoc = .{ .node_offset_array_type_len = inst_data.src_node };
    const sentinel_src: LazySrcLoc = .{ .node_offset_array_type_sentinel = inst_data.src_node };
    const elem_src: LazySrcLoc = .{ .node_offset_array_type_elem = inst_data.src_node };
    const len = try sema.resolveInt(block, len_src, extra.len, Type.usize, "array length must be comptime-known");
    const elem_type = try sema.resolveType(block, elem_src, extra.elem_type);
    const uncasted_sentinel = try sema.resolveInst(extra.sentinel);
    const sentinel = try sema.coerce(block, elem_type, uncasted_sentinel, sentinel_src);
    const sentinel_val = try sema.resolveConstValue(block, sentinel_src, sentinel, "array sentinel value must be comptime-known");
    const array_ty = try Type.array(sema.arena, len, sentinel_val, elem_type, sema.mod);

    return sema.addType(array_ty);
}

fn zirAnyframeType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    if (true) {
        return sema.failWithUseOfAsync(block, inst_data.src());
    }
    const operand_src: LazySrcLoc = .{ .node_offset_anyframe_type = inst_data.src_node };
    const return_type = try sema.resolveType(block, operand_src, inst_data.operand);
    const anyframe_type = try Type.Tag.anyframe_T.create(sema.arena, return_type);

    return sema.addType(anyframe_type);
}

fn zirErrorUnionType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const error_set = try sema.resolveType(block, lhs_src, extra.lhs);
    const payload = try sema.resolveType(block, rhs_src, extra.rhs);

    if (error_set.zigTypeTag() != .ErrorSet) {
        return sema.fail(block, lhs_src, "expected error set type, found '{}'", .{
            error_set.fmt(sema.mod),
        });
    }
    try sema.validateErrorUnionPayloadType(block, payload, rhs_src);
    const err_union_ty = try Type.errorUnion(sema.arena, error_set, payload, sema.mod);
    return sema.addType(err_union_ty);
}

fn validateErrorUnionPayloadType(sema: *Sema, block: *Block, payload_ty: Type, payload_src: LazySrcLoc) !void {
    if (payload_ty.zigTypeTag() == .Opaque) {
        return sema.fail(block, payload_src, "error union with payload of opaque type '{}' not allowed", .{
            payload_ty.fmt(sema.mod),
        });
    } else if (payload_ty.zigTypeTag() == .ErrorSet) {
        return sema.fail(block, payload_src, "error union with payload of error set type '{}' not allowed", .{
            payload_ty.fmt(sema.mod),
        });
    }
}

fn zirErrorValue(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;

    // Create an anonymous error set type with only this error value, and return the value.
    const kv = try sema.mod.getErrorValue(inst_data.get(sema.code));
    const result_type = try Type.Tag.error_set_single.create(sema.arena, kv.key);
    return sema.addConstant(
        result_type,
        try Value.Tag.@"error".create(sema.arena, .{
            .name = kv.key,
        }),
    );
}

fn zirErrorToInt(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const uncasted_operand = try sema.resolveInst(extra.operand);
    const operand = try sema.coerce(block, Type.anyerror, uncasted_operand, operand_src);

    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        if (val.isUndef()) {
            return sema.addConstUndef(Type.err_int);
        }
        switch (val.tag()) {
            .@"error" => {
                const payload = try sema.arena.create(Value.Payload.U64);
                payload.* = .{
                    .base = .{ .tag = .int_u64 },
                    .data = (try sema.mod.getErrorValue(val.castTag(.@"error").?.data.name)).value,
                };
                return sema.addConstant(Type.err_int, Value.initPayload(&payload.base));
            },

            // This is not a valid combination with the type `anyerror`.
            .the_only_possible_value => unreachable,

            // Assume it's already encoded as an integer.
            else => return sema.addConstant(Type.err_int, val),
        }
    }

    const op_ty = sema.typeOf(uncasted_operand);
    try sema.resolveInferredErrorSetTy(block, src, op_ty);
    if (!op_ty.isAnyError()) {
        const names = op_ty.errorSetNames();
        switch (names.len) {
            0 => return sema.addConstant(Type.err_int, Value.zero),
            1 => return sema.addIntUnsigned(Type.err_int, sema.mod.global_error_set.get(names[0]).?),
            else => {},
        }
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    return block.addBitCast(Type.err_int, operand);
}

fn zirIntToError(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const uncasted_operand = try sema.resolveInst(extra.operand);
    const operand = try sema.coerce(block, Type.err_int, uncasted_operand, operand_src);
    const target = sema.mod.getTarget();

    if (try sema.resolveDefinedValue(block, operand_src, operand)) |value| {
        const int = try sema.usizeCast(block, operand_src, value.toUnsignedInt(target));
        if (int > sema.mod.global_error_set.count() or int == 0)
            return sema.fail(block, operand_src, "integer value '{d}' represents no error", .{int});
        const payload = try sema.arena.create(Value.Payload.Error);
        payload.* = .{
            .base = .{ .tag = .@"error" },
            .data = .{ .name = sema.mod.error_name_list.items[int] },
        };
        return sema.addConstant(Type.anyerror, Value.initPayload(&payload.base));
    }
    try sema.requireRuntimeBlock(block, src, operand_src);
    if (block.wantSafety()) {
        const is_lt_len = try block.addUnOp(.cmp_lt_errors_len, operand);
        const zero_val = try sema.addConstant(Type.err_int, Value.zero);
        const is_non_zero = try block.addBinOp(.cmp_neq, operand, zero_val);
        const ok = try block.addBinOp(.bit_and, is_lt_len, is_non_zero);
        try sema.addSafetyCheck(block, ok, .invalid_error_code);
    }
    return block.addInst(.{
        .tag = .bitcast,
        .data = .{ .ty_op = .{
            .ty = Air.Inst.Ref.anyerror_type,
            .operand = operand,
        } },
    });
}

fn zirMergeErrorSets(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    if (sema.typeOf(lhs).zigTypeTag() == .Bool and sema.typeOf(rhs).zigTypeTag() == .Bool) {
        const msg = msg: {
            const msg = try sema.errMsg(block, lhs_src, "expected error set type, found 'bool'", .{});
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, src, msg, "'||' merges error sets; 'or' performs boolean OR", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    const lhs_ty = try sema.analyzeAsType(block, lhs_src, lhs);
    const rhs_ty = try sema.analyzeAsType(block, rhs_src, rhs);
    if (lhs_ty.zigTypeTag() != .ErrorSet)
        return sema.fail(block, lhs_src, "expected error set type, found '{}'", .{lhs_ty.fmt(sema.mod)});
    if (rhs_ty.zigTypeTag() != .ErrorSet)
        return sema.fail(block, rhs_src, "expected error set type, found '{}'", .{rhs_ty.fmt(sema.mod)});

    // Anything merged with anyerror is anyerror.
    if (lhs_ty.tag() == .anyerror or rhs_ty.tag() == .anyerror) {
        return Air.Inst.Ref.anyerror_type;
    }

    if (lhs_ty.castTag(.error_set_inferred)) |payload| {
        try sema.resolveInferredErrorSet(block, src, payload.data);
        // isAnyError might have changed from a false negative to a true positive after resolution.
        if (lhs_ty.isAnyError()) {
            return Air.Inst.Ref.anyerror_type;
        }
    }
    if (rhs_ty.castTag(.error_set_inferred)) |payload| {
        try sema.resolveInferredErrorSet(block, src, payload.data);
        // isAnyError might have changed from a false negative to a true positive after resolution.
        if (rhs_ty.isAnyError()) {
            return Air.Inst.Ref.anyerror_type;
        }
    }

    const err_set_ty = try lhs_ty.errorSetMerge(sema.arena, rhs_ty);
    return sema.addType(err_set_ty);
}

fn zirEnumLiteral(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const duped_name = try sema.arena.dupe(u8, inst_data.get(sema.code));
    return sema.addConstant(
        Type.initTag(.enum_literal),
        try Value.Tag.enum_literal.create(sema.arena, duped_name),
    );
}

fn zirEnumToInt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const arena = sema.arena;
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);

    const enum_tag: Air.Inst.Ref = switch (operand_ty.zigTypeTag()) {
        .Enum => operand,
        .Union => blk: {
            const union_ty = try sema.resolveTypeFields(operand_ty);
            const tag_ty = union_ty.unionTagType() orelse {
                return sema.fail(
                    block,
                    operand_src,
                    "untagged union '{}' cannot be converted to integer",
                    .{src},
                );
            };
            break :blk try sema.unionToTag(block, tag_ty, operand, operand_src);
        },
        else => {
            return sema.fail(block, operand_src, "expected enum or tagged union, found '{}'", .{
                operand_ty.fmt(sema.mod),
            });
        },
    };
    const enum_tag_ty = sema.typeOf(enum_tag);

    var int_tag_type_buffer: Type.Payload.Bits = undefined;
    const int_tag_ty = try enum_tag_ty.intTagType(&int_tag_type_buffer).copy(arena);

    if (try sema.typeHasOnePossibleValue(enum_tag_ty)) |opv| {
        return sema.addConstant(int_tag_ty, opv);
    }

    if (try sema.resolveMaybeUndefVal(enum_tag)) |enum_tag_val| {
        var buffer: Value.Payload.U64 = undefined;
        const val = enum_tag_val.enumToInt(enum_tag_ty, &buffer);
        return sema.addConstant(int_tag_ty, try val.copy(sema.arena));
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    return block.addBitCast(int_tag_ty, enum_tag);
}

fn zirIntToEnum(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const src = inst_data.src();
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);

    if (dest_ty.zigTypeTag() != .Enum) {
        return sema.fail(block, dest_ty_src, "expected enum, found '{}'", .{dest_ty.fmt(sema.mod)});
    }
    _ = try sema.checkIntType(block, operand_src, sema.typeOf(operand));

    if (try sema.resolveMaybeUndefVal(operand)) |int_val| {
        if (dest_ty.isNonexhaustiveEnum()) {
            var buffer: Type.Payload.Bits = undefined;
            const int_tag_ty = dest_ty.intTagType(&buffer);
            if (try sema.intFitsInType(int_val, int_tag_ty, null)) {
                return sema.addConstant(dest_ty, int_val);
            }
            const msg = msg: {
                const msg = try sema.errMsg(
                    block,
                    src,
                    "int value '{}' out of range of non-exhaustive enum '{}'",
                    .{ int_val.fmtValue(sema.typeOf(operand), sema.mod), dest_ty.fmt(sema.mod) },
                );
                errdefer msg.destroy(sema.gpa);
                try sema.addDeclaredHereNote(msg, dest_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        if (int_val.isUndef()) {
            return sema.failWithUseOfUndef(block, operand_src);
        }
        if (!(try sema.enumHasInt(dest_ty, int_val))) {
            const msg = msg: {
                const msg = try sema.errMsg(
                    block,
                    src,
                    "enum '{}' has no tag with value '{}'",
                    .{ dest_ty.fmt(sema.mod), int_val.fmtValue(sema.typeOf(operand), sema.mod) },
                );
                errdefer msg.destroy(sema.gpa);
                try sema.addDeclaredHereNote(msg, dest_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        return sema.addConstant(dest_ty, int_val);
    }

    if (try sema.typeHasOnePossibleValue(dest_ty)) |opv| {
        const result = try sema.addConstant(dest_ty, opv);
        // The operand is runtime-known but the result is comptime-known. In
        // this case we still need a safety check.
        // TODO add a safety check here. we can't use is_named_enum_value -
        // it needs to convert the enum back to int and make sure it equals the operand int.
        return result;
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    const result = try block.addTyOp(.intcast, dest_ty, operand);
    if (block.wantSafety() and !dest_ty.isNonexhaustiveEnum() and
        sema.mod.backendSupportsFeature(.is_named_enum_value))
    {
        const ok = try block.addUnOp(.is_named_enum_value, result);
        try sema.addSafetyCheck(block, ok, .invalid_enum_value);
    }
    return result;
}

/// Pointer in, pointer out.
fn zirOptionalPayloadPtr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    safety_check: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const optional_ptr = try sema.resolveInst(inst_data.operand);
    const src = inst_data.src();

    return sema.analyzeOptionalPayloadPtr(block, src, optional_ptr, safety_check, false);
}

fn analyzeOptionalPayloadPtr(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    optional_ptr: Air.Inst.Ref,
    safety_check: bool,
    initializing: bool,
) CompileError!Air.Inst.Ref {
    const optional_ptr_ty = sema.typeOf(optional_ptr);
    assert(optional_ptr_ty.zigTypeTag() == .Pointer);

    const opt_type = optional_ptr_ty.elemType();
    if (opt_type.zigTypeTag() != .Optional) {
        return sema.fail(block, src, "expected optional type, found '{}'", .{opt_type.fmt(sema.mod)});
    }

    const child_type = try opt_type.optionalChildAlloc(sema.arena);
    const child_pointer = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = child_type,
        .mutable = !optional_ptr_ty.isConstPtr(),
        .@"addrspace" = optional_ptr_ty.ptrAddressSpace(),
    });

    if (try sema.resolveDefinedValue(block, src, optional_ptr)) |ptr_val| {
        if (initializing) {
            if (!ptr_val.isComptimeMutablePtr()) {
                // If the pointer resulting from this function was stored at comptime,
                // the optional non-null bit would be set that way. But in this case,
                // we need to emit a runtime instruction to do it.
                _ = try block.addTyOp(.optional_payload_ptr_set, child_pointer, optional_ptr);
            }
            return sema.addConstant(
                child_pointer,
                try Value.Tag.opt_payload_ptr.create(sema.arena, .{
                    .container_ptr = ptr_val,
                    .container_ty = optional_ptr_ty.childType(),
                }),
            );
        }
        if (try sema.pointerDeref(block, src, ptr_val, optional_ptr_ty)) |val| {
            if (val.isNull()) {
                return sema.fail(block, src, "unable to unwrap null", .{});
            }
            // The same Value represents the pointer to the optional and the payload.
            return sema.addConstant(
                child_pointer,
                try Value.Tag.opt_payload_ptr.create(sema.arena, .{
                    .container_ptr = ptr_val,
                    .container_ty = optional_ptr_ty.childType(),
                }),
            );
        }
    }

    try sema.requireRuntimeBlock(block, src, null);
    if (safety_check and block.wantSafety()) {
        const is_non_null = try block.addUnOp(.is_non_null_ptr, optional_ptr);
        try sema.addSafetyCheck(block, is_non_null, .unwrap_null);
    }
    const air_tag: Air.Inst.Tag = if (initializing)
        .optional_payload_ptr_set
    else
        .optional_payload_ptr;
    return block.addTyOp(air_tag, child_pointer, optional_ptr);
}

/// Value in, value out.
fn zirOptionalPayload(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    safety_check: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    const result_ty = switch (operand_ty.zigTypeTag()) {
        .Optional => try operand_ty.optionalChildAlloc(sema.arena),
        .Pointer => t: {
            if (operand_ty.ptrSize() != .C) {
                return sema.failWithExpectedOptionalType(block, src, operand_ty);
            }
            // TODO https://github.com/ziglang/zig/issues/6597
            if (true) break :t operand_ty;
            const ptr_info = operand_ty.ptrInfo().data;
            break :t try Type.ptr(sema.arena, sema.mod, .{
                .pointee_type = try ptr_info.pointee_type.copy(sema.arena),
                .@"align" = ptr_info.@"align",
                .@"addrspace" = ptr_info.@"addrspace",
                .mutable = ptr_info.mutable,
                .@"allowzero" = ptr_info.@"allowzero",
                .@"volatile" = ptr_info.@"volatile",
                .size = .One,
            });
        },
        else => return sema.failWithExpectedOptionalType(block, src, operand_ty),
    };

    if (try sema.resolveDefinedValue(block, src, operand)) |val| {
        if (val.isNull()) {
            return sema.fail(block, src, "unable to unwrap null", .{});
        }
        if (val.castTag(.opt_payload)) |payload| {
            return sema.addConstant(result_ty, payload.data);
        }
        return sema.addConstant(result_ty, val);
    }

    try sema.requireRuntimeBlock(block, src, null);
    if (safety_check and block.wantSafety()) {
        const is_non_null = try block.addUnOp(.is_non_null, operand);
        try sema.addSafetyCheck(block, is_non_null, .unwrap_null);
    }
    return block.addTyOp(.optional_payload, result_ty, operand);
}

/// Value in, value out
fn zirErrUnionPayload(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_src = src;
    const err_union_ty = sema.typeOf(operand);
    if (err_union_ty.zigTypeTag() != .ErrorUnion) {
        return sema.fail(block, operand_src, "expected error union type, found '{}'", .{
            err_union_ty.fmt(sema.mod),
        });
    }
    return sema.analyzeErrUnionPayload(block, src, err_union_ty, operand, operand_src, false);
}

fn analyzeErrUnionPayload(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    err_union_ty: Type,
    operand: Zir.Inst.Ref,
    operand_src: LazySrcLoc,
    safety_check: bool,
) CompileError!Air.Inst.Ref {
    const payload_ty = err_union_ty.errorUnionPayload();
    if (try sema.resolveDefinedValue(block, operand_src, operand)) |val| {
        if (val.getError()) |name| {
            return sema.fail(block, src, "caught unexpected error '{s}'", .{name});
        }
        const data = val.castTag(.eu_payload).?.data;
        return sema.addConstant(payload_ty, data);
    }

    try sema.requireRuntimeBlock(block, src, null);

    // If the error set has no fields then no safety check is needed.
    if (safety_check and block.wantSafety() and
        !err_union_ty.errorUnionSet().errorSetIsEmpty())
    {
        try sema.panicUnwrapError(block, operand, .unwrap_errunion_err, .is_non_err);
    }

    return block.addTyOp(.unwrap_errunion_payload, payload_ty, operand);
}

/// Pointer in, pointer out.
fn zirErrUnionPayloadPtr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const src = inst_data.src();

    return sema.analyzeErrUnionPayloadPtr(block, src, operand, false, false);
}

fn analyzeErrUnionPayloadPtr(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    operand: Air.Inst.Ref,
    safety_check: bool,
    initializing: bool,
) CompileError!Air.Inst.Ref {
    const operand_ty = sema.typeOf(operand);
    assert(operand_ty.zigTypeTag() == .Pointer);

    if (operand_ty.elemType().zigTypeTag() != .ErrorUnion) {
        return sema.fail(block, src, "expected error union type, found '{}'", .{
            operand_ty.elemType().fmt(sema.mod),
        });
    }

    const err_union_ty = operand_ty.elemType();
    const payload_ty = err_union_ty.errorUnionPayload();
    const operand_pointer_ty = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = payload_ty,
        .mutable = !operand_ty.isConstPtr(),
        .@"addrspace" = operand_ty.ptrAddressSpace(),
    });

    if (try sema.resolveDefinedValue(block, src, operand)) |ptr_val| {
        if (initializing) {
            if (!ptr_val.isComptimeMutablePtr()) {
                // If the pointer resulting from this function was stored at comptime,
                // the error union error code would be set that way. But in this case,
                // we need to emit a runtime instruction to do it.
                try sema.requireRuntimeBlock(block, src, null);
                _ = try block.addTyOp(.errunion_payload_ptr_set, operand_pointer_ty, operand);
            }
            return sema.addConstant(
                operand_pointer_ty,
                try Value.Tag.eu_payload_ptr.create(sema.arena, .{
                    .container_ptr = ptr_val,
                    .container_ty = operand_ty.elemType(),
                }),
            );
        }
        if (try sema.pointerDeref(block, src, ptr_val, operand_ty)) |val| {
            if (val.getError()) |name| {
                return sema.fail(block, src, "caught unexpected error '{s}'", .{name});
            }

            return sema.addConstant(
                operand_pointer_ty,
                try Value.Tag.eu_payload_ptr.create(sema.arena, .{
                    .container_ptr = ptr_val,
                    .container_ty = operand_ty.elemType(),
                }),
            );
        }
    }

    try sema.requireRuntimeBlock(block, src, null);

    // If the error set has no fields then no safety check is needed.
    if (safety_check and block.wantSafety() and
        !err_union_ty.errorUnionSet().errorSetIsEmpty())
    {
        try sema.panicUnwrapError(block, operand, .unwrap_errunion_err_ptr, .is_non_err_ptr);
    }

    const air_tag: Air.Inst.Tag = if (initializing)
        .errunion_payload_ptr_set
    else
        .unwrap_errunion_payload_ptr;
    return block.addTyOp(air_tag, operand_pointer_ty, operand);
}

/// Value in, value out
fn zirErrUnionCode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    return sema.analyzeErrUnionCode(block, src, operand);
}

fn analyzeErrUnionCode(sema: *Sema, block: *Block, src: LazySrcLoc, operand: Air.Inst.Ref) CompileError!Air.Inst.Ref {
    const operand_ty = sema.typeOf(operand);
    if (operand_ty.zigTypeTag() != .ErrorUnion) {
        return sema.fail(block, src, "expected error union type, found '{}'", .{
            operand_ty.fmt(sema.mod),
        });
    }

    const result_ty = operand_ty.errorUnionSet();

    if (try sema.resolveDefinedValue(block, src, operand)) |val| {
        assert(val.getError() != null);
        return sema.addConstant(result_ty, val);
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.unwrap_errunion_err, result_ty, operand);
}

/// Pointer in, value out
fn zirErrUnionCodePtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    assert(operand_ty.zigTypeTag() == .Pointer);

    if (operand_ty.elemType().zigTypeTag() != .ErrorUnion) {
        return sema.fail(block, src, "expected error union type, found '{}'", .{
            operand_ty.elemType().fmt(sema.mod),
        });
    }

    const result_ty = operand_ty.elemType().errorUnionSet();

    if (try sema.resolveDefinedValue(block, src, operand)) |pointer_val| {
        if (try sema.pointerDeref(block, src, pointer_val, operand_ty)) |val| {
            assert(val.getError() != null);
            return sema.addConstant(result_ty, val);
        }
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.unwrap_errunion_err_ptr, result_ty, operand);
}

fn zirFunc(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    inferred_error_set: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Func, inst_data.payload_index);
    const target = sema.mod.getTarget();
    const ret_ty_src: LazySrcLoc = .{ .node_offset_fn_type_ret_ty = inst_data.src_node };

    var extra_index = extra.end;

    const ret_ty: Type = switch (extra.data.ret_body_len) {
        0 => Type.void,
        1 => blk: {
            const ret_ty_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
            extra_index += 1;
            if (sema.resolveType(block, ret_ty_src, ret_ty_ref)) |ret_ty| {
                break :blk ret_ty;
            } else |err| switch (err) {
                error.GenericPoison => {
                    break :blk Type.initTag(.generic_poison);
                },
                else => |e| return e,
            }
        },
        else => blk: {
            const ret_ty_body = sema.code.extra[extra_index..][0..extra.data.ret_body_len];
            extra_index += ret_ty_body.len;

            const ret_ty_val = try sema.resolveGenericBody(block, ret_ty_src, ret_ty_body, inst, Type.type, "return type must be comptime-known");
            var buffer: Value.ToTypeBuffer = undefined;
            break :blk try ret_ty_val.toType(&buffer).copy(sema.arena);
        },
    };

    var src_locs: Zir.Inst.Func.SrcLocs = undefined;
    const has_body = extra.data.body_len != 0;
    if (has_body) {
        extra_index += extra.data.body_len;
        src_locs = sema.code.extraData(Zir.Inst.Func.SrcLocs, extra_index).data;
    }

    // If this instruction has a body it means it's the type of the `owner_decl`
    // otherwise it's a function type without a `callconv` attribute and should
    // never be `.C`.
    // NOTE: revisit when doing #1717
    const cc: std.builtin.CallingConvention = if (sema.owner_decl.is_exported and has_body)
        .C
    else
        .Unspecified;

    return sema.funcCommon(
        block,
        inst_data.src_node,
        inst,
        0,
        target_util.defaultAddressSpace(target, .function),
        FuncLinkSection.default,
        cc,
        ret_ty,
        false,
        inferred_error_set,
        false,
        has_body,
        src_locs,
        null,
        0,
        false,
    );
}

fn resolveGenericBody(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    body: []const Zir.Inst.Index,
    func_inst: Zir.Inst.Index,
    dest_ty: Type,
    reason: []const u8,
) !Value {
    assert(body.len != 0);

    const err = err: {
        // Make sure any nested param instructions don't clobber our work.
        const prev_params = block.params;
        block.params = .{};
        defer {
            block.params.deinit(sema.gpa);
            block.params = prev_params;
        }

        const uncasted = sema.resolveBody(block, body, func_inst) catch |err| break :err err;
        const result = sema.coerce(block, dest_ty, uncasted, src) catch |err| break :err err;
        const val = sema.resolveConstValue(block, src, result, reason) catch |err| break :err err;
        return val;
    };
    switch (err) {
        error.GenericPoison => {
            if (dest_ty.tag() == .type) {
                return Value.initTag(.generic_poison_type);
            } else {
                return Value.initTag(.generic_poison);
            }
        },
        else => |e| return e,
    }
}

/// Given a library name, examines if the library name should end up in
/// `link.File.Options.system_libs` table (for example, libc is always
/// specified via dedicated flag `link.File.Options.link_libc` instead),
/// and puts it there if it doesn't exist.
/// It also dupes the library name which can then be saved as part of the
/// respective `Decl` (either `ExternFn` or `Var`).
/// The liveness of the duped library name is tied to liveness of `Module`.
/// To deallocate, call `deinit` on the respective `Decl` (`ExternFn` or `Var`).
fn handleExternLibName(
    sema: *Sema,
    block: *Block,
    src_loc: LazySrcLoc,
    lib_name: []const u8,
) CompileError![:0]u8 {
    blk: {
        const mod = sema.mod;
        const comp = mod.comp;
        const target = mod.getTarget();
        log.debug("extern fn symbol expected in lib '{s}'", .{lib_name});
        if (target_util.is_libc_lib_name(target, lib_name)) {
            if (!comp.bin_file.options.link_libc and !comp.bin_file.options.parent_compilation_link_libc) {
                return sema.fail(
                    block,
                    src_loc,
                    "dependency on libc must be explicitly specified in the build command",
                    .{},
                );
            }
            comp.bin_file.options.link_libc = true;
            break :blk;
        }
        if (target_util.is_libcpp_lib_name(target, lib_name)) {
            if (!comp.bin_file.options.link_libcpp) {
                return sema.fail(
                    block,
                    src_loc,
                    "dependency on libc++ must be explicitly specified in the build command",
                    .{},
                );
            }
            comp.bin_file.options.link_libcpp = true;
            break :blk;
        }
        if (mem.eql(u8, lib_name, "unwind")) {
            comp.bin_file.options.link_libunwind = true;
            break :blk;
        }
        if (!target.isWasm() and !comp.bin_file.options.pic) {
            return sema.fail(
                block,
                src_loc,
                "dependency on dynamic library '{s}' requires enabling Position Independent Code. Fixed by '-l{s}' or '-fPIC'.",
                .{ lib_name, lib_name },
            );
        }
        comp.addLinkLib(lib_name) catch |err| {
            return sema.fail(block, src_loc, "unable to add link lib '{s}': {s}", .{
                lib_name, @errorName(err),
            });
        };
    }
    return sema.gpa.dupeZ(u8, lib_name);
}

const FuncLinkSection = union(enum) {
    generic,
    default,
    explicit: []const u8,
};

fn funcCommon(
    sema: *Sema,
    block: *Block,
    src_node_offset: i32,
    func_inst: Zir.Inst.Index,
    /// null means generic poison
    alignment: ?u32,
    /// null means generic poison
    address_space: ?std.builtin.AddressSpace,
    /// outer null means generic poison; inner null means default link section
    section: FuncLinkSection,
    /// null means generic poison
    cc: ?std.builtin.CallingConvention,
    /// this might be Type.generic_poison
    bare_return_type: Type,
    var_args: bool,
    inferred_error_set: bool,
    is_extern: bool,
    has_body: bool,
    src_locs: Zir.Inst.Func.SrcLocs,
    opt_lib_name: ?[]const u8,
    noalias_bits: u32,
    is_noinline: bool,
) CompileError!Air.Inst.Ref {
    const ret_ty_src: LazySrcLoc = .{ .node_offset_fn_type_ret_ty = src_node_offset };
    const cc_src: LazySrcLoc = .{ .node_offset_fn_type_cc = src_node_offset };
    const func_src = LazySrcLoc.nodeOffset(src_node_offset);

    var is_generic = bare_return_type.tag() == .generic_poison or
        alignment == null or
        address_space == null or
        section == .generic or
        cc == null;

    if (var_args) {
        if (is_generic) {
            return sema.fail(block, func_src, "generic function cannot be variadic", .{});
        }
        if (cc.? != .C) {
            return sema.fail(block, cc_src, "variadic function must have 'C' calling convention", .{});
        }
    }

    var destroy_fn_on_error = false;
    const new_func: *Module.Fn = new_func: {
        if (!has_body) break :new_func undefined;
        if (sema.comptime_args_fn_inst == func_inst) {
            const new_func = sema.preallocated_new_func.?;
            sema.preallocated_new_func = null; // take ownership
            break :new_func new_func;
        }
        destroy_fn_on_error = true;
        const new_func = try sema.gpa.create(Module.Fn);
        // Set this here so that the inferred return type can be printed correctly if it appears in an error.
        new_func.owner_decl = sema.owner_decl_index;
        break :new_func new_func;
    };
    errdefer if (destroy_fn_on_error) sema.gpa.destroy(new_func);

    var maybe_inferred_error_set_node: ?*Module.Fn.InferredErrorSetListNode = null;
    errdefer if (maybe_inferred_error_set_node) |node| sema.gpa.destroy(node);
    // Note: no need to errdefer since this will still be in its default state at the end of the function.

    const target = sema.mod.getTarget();
    const fn_ty: Type = fn_ty: {
        // Hot path for some common function types.
        // TODO can we eliminate some of these Type tag values? seems unnecessarily complicated.
        if (!is_generic and block.params.items.len == 0 and !var_args and !inferred_error_set and
            alignment.? == 0 and
            address_space.? == target_util.defaultAddressSpace(target, .function) and
            section == .default)
        {
            if (bare_return_type.zigTypeTag() == .NoReturn and cc.? == .Unspecified) {
                break :fn_ty Type.initTag(.fn_noreturn_no_args);
            }

            if (bare_return_type.zigTypeTag() == .Void and cc.? == .Unspecified) {
                break :fn_ty Type.initTag(.fn_void_no_args);
            }

            if (bare_return_type.zigTypeTag() == .NoReturn and cc.? == .Naked) {
                break :fn_ty Type.initTag(.fn_naked_noreturn_no_args);
            }

            if (bare_return_type.zigTypeTag() == .Void and cc.? == .C) {
                break :fn_ty Type.initTag(.fn_ccc_void_no_args);
            }
        }

        // In the case of generic calling convention, or generic alignment, we use
        // default values which are only meaningful for the generic function, *not*
        // the instantiation, which can depend on comptime parameters.
        // Related proposal: https://github.com/ziglang/zig/issues/11834
        const cc_resolved = cc orelse .Unspecified;
        const param_types = try sema.arena.alloc(Type, block.params.items.len);
        const comptime_params = try sema.arena.alloc(bool, block.params.items.len);
        for (block.params.items, 0..) |param, i| {
            const is_noalias = blk: {
                const index = std.math.cast(u5, i) orelse break :blk false;
                break :blk @truncate(u1, noalias_bits >> index) != 0;
            };
            param_types[i] = param.ty;
            sema.analyzeParameter(
                block,
                .unneeded,
                param,
                comptime_params,
                i,
                &is_generic,
                cc_resolved,
                has_body,
                is_noalias,
            ) catch |err| switch (err) {
                error.NeededSourceLocation => {
                    const decl = sema.mod.declPtr(block.src_decl);
                    try sema.analyzeParameter(
                        block,
                        Module.paramSrc(src_node_offset, sema.gpa, decl, i),
                        param,
                        comptime_params,
                        i,
                        &is_generic,
                        cc_resolved,
                        has_body,
                        is_noalias,
                    );
                    unreachable;
                },
                else => |e| return e,
            };
        }

        var ret_ty_requires_comptime = false;
        const ret_poison = if (sema.typeRequiresComptime(bare_return_type)) |ret_comptime| rp: {
            ret_ty_requires_comptime = ret_comptime;
            break :rp bare_return_type.tag() == .generic_poison;
        } else |err| switch (err) {
            error.GenericPoison => rp: {
                is_generic = true;
                break :rp true;
            },
            else => |e| return e,
        };

        const return_type = if (!inferred_error_set or ret_poison)
            bare_return_type
        else blk: {
            try sema.validateErrorUnionPayloadType(block, bare_return_type, ret_ty_src);
            const node = try sema.gpa.create(Module.Fn.InferredErrorSetListNode);
            node.data = .{ .func = new_func };
            maybe_inferred_error_set_node = node;

            const error_set_ty = try Type.Tag.error_set_inferred.create(sema.arena, &node.data);
            break :blk try Type.Tag.error_union.create(sema.arena, .{
                .error_set = error_set_ty,
                .payload = bare_return_type,
            });
        };

        if (!return_type.isValidReturnType()) {
            const opaque_str = if (return_type.zigTypeTag() == .Opaque) "opaque " else "";
            const msg = msg: {
                const msg = try sema.errMsg(block, ret_ty_src, "{s}return type '{}' not allowed", .{
                    opaque_str, return_type.fmt(sema.mod),
                });
                errdefer msg.destroy(sema.gpa);

                try sema.addDeclaredHereNote(msg, return_type);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        if (!Type.fnCallingConventionAllowsZigTypes(cc_resolved) and !try sema.validateExternType(return_type, .ret_ty)) {
            const msg = msg: {
                const msg = try sema.errMsg(block, ret_ty_src, "return type '{}' not allowed in function with calling convention '{s}'", .{
                    return_type.fmt(sema.mod), @tagName(cc_resolved),
                });
                errdefer msg.destroy(sema.gpa);

                const src_decl = sema.mod.declPtr(block.src_decl);
                try sema.explainWhyTypeIsNotExtern(msg, ret_ty_src.toSrcLoc(src_decl), return_type, .ret_ty);

                try sema.addDeclaredHereNote(msg, return_type);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }

        // If the return type is comptime-only but not dependent on parameters then all parameter types also need to be comptime
        if (!sema.is_generic_instantiation and has_body and ret_ty_requires_comptime) comptime_check: {
            for (block.params.items) |param| {
                if (!param.is_comptime) break;
            } else break :comptime_check;

            const msg = try sema.errMsg(
                block,
                ret_ty_src,
                "function with comptime-only return type '{}' requires all parameters to be comptime",
                .{return_type.fmt(sema.mod)},
            );
            try sema.explainWhyTypeIsComptime(block, ret_ty_src, msg, ret_ty_src.toSrcLoc(sema.owner_decl), return_type);

            const tags = sema.code.instructions.items(.tag);
            const data = sema.code.instructions.items(.data);
            const param_body = sema.code.getParamBody(func_inst);
            for (block.params.items, 0..) |param, i| {
                if (!param.is_comptime) {
                    const param_index = param_body[i];
                    const param_src = switch (tags[param_index]) {
                        .param => data[param_index].pl_tok.src(),
                        .param_anytype => data[param_index].str_tok.src(),
                        else => unreachable,
                    };
                    if (param.name.len != 0) {
                        try sema.errNote(block, param_src, msg, "param '{s}' is required to be comptime", .{param.name});
                    } else {
                        try sema.errNote(block, param_src, msg, "param is required to be comptime", .{});
                    }
                }
            }
            return sema.failWithOwnedErrorMsg(msg);
        }

        const arch = sema.mod.getTarget().cpu.arch;
        if (switch (cc_resolved) {
            .Unspecified, .C, .Naked, .Async, .Inline => null,
            .Interrupt => switch (arch) {
                .x86, .x86_64, .avr, .msp430 => null,
                else => @as([]const u8, "x86, x86_64, AVR, and MSP430"),
            },
            .Signal => switch (arch) {
                .avr => null,
                else => @as([]const u8, "AVR"),
            },
            .Stdcall, .Fastcall, .Thiscall => switch (arch) {
                .x86 => null,
                else => @as([]const u8, "x86"),
            },
            .Vectorcall => switch (arch) {
                .x86, .aarch64, .aarch64_be, .aarch64_32 => null,
                else => @as([]const u8, "x86 and AArch64"),
            },
            .APCS, .AAPCS, .AAPCSVFP => switch (arch) {
                .arm, .armeb, .aarch64, .aarch64_be, .aarch64_32, .thumb, .thumbeb => null,
                else => @as([]const u8, "ARM"),
            },
            .SysV, .Win64 => switch (arch) {
                .x86_64 => null,
                else => @as([]const u8, "x86_64"),
            },
            .PtxKernel => switch (arch) {
                .nvptx, .nvptx64 => null,
                else => @as([]const u8, "nvptx and nvptx64"),
            },
            .AmdgpuKernel => switch (arch) {
                .amdgcn => null,
                else => @as([]const u8, "amdgcn"),
            },
        }) |allowed_platform| {
            return sema.fail(block, cc_src, "callconv '{s}' is only available on {s}, not {s}", .{
                @tagName(cc_resolved),
                allowed_platform,
                @tagName(arch),
            });
        }

        if (cc_resolved == .Inline and is_noinline) {
            return sema.fail(block, cc_src, "'noinline' function cannot have callconv 'Inline'", .{});
        }
        if (is_generic and sema.no_partial_func_ty) return error.GenericPoison;
        for (comptime_params) |ct| is_generic = is_generic or ct;
        is_generic = is_generic or ret_ty_requires_comptime;

        if (!is_generic and sema.wantErrorReturnTracing(return_type)) {
            // Make sure that StackTrace's fields are resolved so that the backend can
            // lower this fn type.
            const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
            _ = try sema.resolveTypeFields(unresolved_stack_trace_ty);
        }

        break :fn_ty try Type.Tag.function.create(sema.arena, .{
            .param_types = param_types,
            .comptime_params = comptime_params.ptr,
            .return_type = return_type,
            .cc = cc_resolved,
            .cc_is_generic = cc == null,
            .alignment = alignment orelse 0,
            .align_is_generic = alignment == null,
            .section_is_generic = section == .generic,
            .addrspace_is_generic = address_space == null,
            .is_var_args = var_args,
            .is_generic = is_generic,
            .noalias_bits = noalias_bits,
        });
    };

    sema.owner_decl.@"linksection" = switch (section) {
        .generic => undefined,
        .default => null,
        .explicit => |section_name| try sema.perm_arena.dupeZ(u8, section_name),
    };
    sema.owner_decl.@"align" = alignment orelse 0;
    sema.owner_decl.@"addrspace" = address_space orelse .generic;

    if (is_extern) {
        const new_extern_fn = try sema.gpa.create(Module.ExternFn);
        errdefer sema.gpa.destroy(new_extern_fn);

        new_extern_fn.* = Module.ExternFn{
            .owner_decl = sema.owner_decl_index,
            .lib_name = null,
        };

        if (opt_lib_name) |lib_name| {
            new_extern_fn.lib_name = try sema.handleExternLibName(block, .{
                .node_offset_lib_name = src_node_offset,
            }, lib_name);
        }

        const extern_fn_payload = try sema.arena.create(Value.Payload.ExternFn);
        extern_fn_payload.* = .{
            .base = .{ .tag = .extern_fn },
            .data = new_extern_fn,
        };
        return sema.addConstant(fn_ty, Value.initPayload(&extern_fn_payload.base));
    }

    if (!has_body) {
        return sema.addType(fn_ty);
    }

    const is_inline = fn_ty.fnCallingConvention() == .Inline;
    const anal_state: Module.Fn.Analysis = if (is_inline) .inline_only else .queued;

    const comptime_args: ?[*]TypedValue = if (sema.comptime_args_fn_inst == func_inst) blk: {
        break :blk if (sema.comptime_args.len == 0) null else sema.comptime_args.ptr;
    } else null;

    const hash = new_func.hash;
    const generic_owner_decl = if (comptime_args == null) .none else new_func.generic_owner_decl;
    const fn_payload = try sema.arena.create(Value.Payload.Function);
    new_func.* = .{
        .state = anal_state,
        .zir_body_inst = func_inst,
        .owner_decl = sema.owner_decl_index,
        .generic_owner_decl = generic_owner_decl,
        .comptime_args = comptime_args,
        .hash = hash,
        .lbrace_line = src_locs.lbrace_line,
        .rbrace_line = src_locs.rbrace_line,
        .lbrace_column = @truncate(u16, src_locs.columns),
        .rbrace_column = @truncate(u16, src_locs.columns >> 16),
        .branch_quota = default_branch_quota,
        .is_noinline = is_noinline,
    };
    if (maybe_inferred_error_set_node) |node| {
        new_func.inferred_error_sets.prepend(node);
    }
    maybe_inferred_error_set_node = null;
    fn_payload.* = .{
        .base = .{ .tag = .function },
        .data = new_func,
    };
    return sema.addConstant(fn_ty, Value.initPayload(&fn_payload.base));
}

fn analyzeParameter(
    sema: *Sema,
    block: *Block,
    param_src: LazySrcLoc,
    param: Block.Param,
    comptime_params: []bool,
    i: usize,
    is_generic: *bool,
    cc: std.builtin.CallingConvention,
    has_body: bool,
    is_noalias: bool,
) !void {
    const requires_comptime = try sema.typeRequiresComptime(param.ty);
    comptime_params[i] = param.is_comptime or requires_comptime;
    const this_generic = param.ty.tag() == .generic_poison;
    is_generic.* = is_generic.* or this_generic;
    if (param.is_comptime and !Type.fnCallingConventionAllowsZigTypes(cc)) {
        return sema.fail(block, param_src, "comptime parameters not allowed in function with calling convention '{s}'", .{@tagName(cc)});
    }
    if (this_generic and !sema.no_partial_func_ty and !Type.fnCallingConventionAllowsZigTypes(cc)) {
        return sema.fail(block, param_src, "generic parameters not allowed in function with calling convention '{s}'", .{@tagName(cc)});
    }
    if (!param.ty.isValidParamType()) {
        const opaque_str = if (param.ty.zigTypeTag() == .Opaque) "opaque " else "";
        const msg = msg: {
            const msg = try sema.errMsg(block, param_src, "parameter of {s}type '{}' not allowed", .{
                opaque_str, param.ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);

            try sema.addDeclaredHereNote(msg, param.ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    if (!this_generic and !Type.fnCallingConventionAllowsZigTypes(cc) and !try sema.validateExternType(param.ty, .param_ty)) {
        const msg = msg: {
            const msg = try sema.errMsg(block, param_src, "parameter of type '{}' not allowed in function with calling convention '{s}'", .{
                param.ty.fmt(sema.mod), @tagName(cc),
            });
            errdefer msg.destroy(sema.gpa);

            const src_decl = sema.mod.declPtr(block.src_decl);
            try sema.explainWhyTypeIsNotExtern(msg, param_src.toSrcLoc(src_decl), param.ty, .param_ty);

            try sema.addDeclaredHereNote(msg, param.ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    if (!sema.is_generic_instantiation and requires_comptime and !param.is_comptime and has_body) {
        const msg = msg: {
            const msg = try sema.errMsg(block, param_src, "parameter of type '{}' must be declared comptime", .{
                param.ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);

            const src_decl = sema.mod.declPtr(block.src_decl);
            try sema.explainWhyTypeIsComptime(block, param_src, msg, param_src.toSrcLoc(src_decl), param.ty);

            try sema.addDeclaredHereNote(msg, param.ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    if (!sema.is_generic_instantiation and !this_generic and is_noalias and
        !(param.ty.zigTypeTag() == .Pointer or param.ty.isPtrLikeOptional()))
    {
        return sema.fail(block, param_src, "non-pointer parameter declared noalias", .{});
    }
}

fn zirParam(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    comptime_syntax: bool,
) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_tok;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Param, inst_data.payload_index);
    const param_name = sema.code.nullTerminatedString(extra.data.name);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];

    // We could be in a generic function instantiation, or we could be evaluating a generic
    // function without any comptime args provided.
    const param_ty = param_ty: {
        const err = err: {
            // Make sure any nested param instructions don't clobber our work.
            const prev_params = block.params;
            const prev_preallocated_new_func = sema.preallocated_new_func;
            const prev_no_partial_func_type = sema.no_partial_func_ty;
            block.params = .{};
            sema.preallocated_new_func = null;
            sema.no_partial_func_ty = true;
            defer {
                block.params.deinit(sema.gpa);
                block.params = prev_params;
                sema.preallocated_new_func = prev_preallocated_new_func;
                sema.no_partial_func_ty = prev_no_partial_func_type;
            }

            if (sema.resolveBody(block, body, inst)) |param_ty_inst| {
                if (sema.analyzeAsType(block, src, param_ty_inst)) |param_ty| {
                    break :param_ty param_ty;
                } else |err| break :err err;
            } else |err| break :err err;
        };
        switch (err) {
            error.GenericPoison => {
                if (sema.inst_map.get(inst)) |_| {
                    // A generic function is about to evaluate to another generic function.
                    // Return an error instead.
                    return error.GenericPoison;
                }
                // The type is not available until the generic instantiation.
                // We result the param instruction with a poison value and
                // insert an anytype parameter.
                try block.params.append(sema.gpa, .{
                    .ty = Type.initTag(.generic_poison),
                    .is_comptime = comptime_syntax,
                    .name = param_name,
                });
                sema.inst_map.putAssumeCapacityNoClobber(inst, .generic_poison);
                return;
            },
            else => |e| return e,
        }
    };
    const is_comptime = sema.typeRequiresComptime(param_ty) catch |err| switch (err) {
        error.GenericPoison => {
            if (sema.inst_map.get(inst)) |_| {
                // A generic function is about to evaluate to another generic function.
                // Return an error instead.
                return error.GenericPoison;
            }
            // The type is not available until the generic instantiation.
            // We result the param instruction with a poison value and
            // insert an anytype parameter.
            try block.params.append(sema.gpa, .{
                .ty = Type.initTag(.generic_poison),
                .is_comptime = comptime_syntax,
                .name = param_name,
            });
            sema.inst_map.putAssumeCapacityNoClobber(inst, .generic_poison);
            return;
        },
        else => |e| return e,
    } or comptime_syntax;
    if (sema.inst_map.get(inst)) |arg| {
        if (is_comptime and sema.preallocated_new_func != null) {
            // We have a comptime value for this parameter so it should be elided from the
            // function type of the function instruction in this block.
            const coerced_arg = sema.coerce(block, param_ty, arg, .unneeded) catch |err| switch (err) {
                error.NeededSourceLocation => {
                    // We are instantiating a generic function and a comptime arg
                    // cannot be coerced to the param type, but since we don't
                    // have the callee source location return `GenericPoison`
                    // so that the instantiation is failed and the coercion
                    // is handled by comptime call logic instead.
                    assert(sema.is_generic_instantiation);
                    return error.GenericPoison;
                },
                else => return err,
            };
            sema.inst_map.putAssumeCapacity(inst, coerced_arg);
            return;
        }
        // Even though a comptime argument is provided, the generic function wants to treat
        // this as a runtime parameter.
        assert(sema.inst_map.remove(inst));
    }

    if (sema.preallocated_new_func != null) {
        if (try sema.typeHasOnePossibleValue(param_ty)) |opv| {
            // In this case we are instantiating a generic function call with a non-comptime
            // non-anytype parameter that ended up being a one-possible-type.
            // We don't want the parameter to be part of the instantiated function type.
            const result = try sema.addConstant(param_ty, opv);
            sema.inst_map.putAssumeCapacity(inst, result);
            return;
        }
    }

    try block.params.append(sema.gpa, .{
        .ty = param_ty,
        .is_comptime = comptime_syntax,
        .name = param_name,
    });

    if (is_comptime) {
        // If this is a comptime parameter we can add a constant generic_poison
        // since this is also a generic parameter.
        const result = try sema.addConstant(param_ty, Value.initTag(.generic_poison));
        sema.inst_map.putAssumeCapacityNoClobber(inst, result);
    } else {
        // Otherwise we need a dummy runtime instruction.
        const result_index = @intCast(Air.Inst.Index, sema.air_instructions.len);
        try sema.air_instructions.append(sema.gpa, .{
            .tag = .alloc,
            .data = .{ .ty = param_ty },
        });
        const result = Air.indexToRef(result_index);
        sema.inst_map.putAssumeCapacityNoClobber(inst, result);
    }
}

fn zirParamAnytype(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    comptime_syntax: bool,
) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const param_name = inst_data.get(sema.code);

    if (sema.inst_map.get(inst)) |air_ref| {
        const param_ty = sema.typeOf(air_ref);
        if (comptime_syntax or try sema.typeRequiresComptime(param_ty)) {
            // We have a comptime value for this parameter so it should be elided from the
            // function type of the function instruction in this block.
            return;
        }
        if (null != try sema.typeHasOnePossibleValue(param_ty)) {
            return;
        }
        // The map is already populated but we do need to add a runtime parameter.
        try block.params.append(sema.gpa, .{
            .ty = param_ty,
            .is_comptime = false,
            .name = param_name,
        });
        return;
    }

    // We are evaluating a generic function without any comptime args provided.

    try block.params.append(sema.gpa, .{
        .ty = Type.initTag(.generic_poison),
        .is_comptime = comptime_syntax,
        .name = param_name,
    });
    sema.inst_map.putAssumeCapacity(inst, .generic_poison);
}

fn zirAs(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const bin_inst = sema.code.instructions.items(.data)[inst].bin;
    return sema.analyzeAs(block, sema.src, bin_inst.lhs, bin_inst.rhs, false);
}

fn zirAsNode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.As, inst_data.payload_index).data;
    sema.src = src;
    return sema.analyzeAs(block, src, extra.dest_type, extra.operand, false);
}

fn zirAsShiftOperand(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.As, inst_data.payload_index).data;
    return sema.analyzeAs(block, src, extra.dest_type, extra.operand, true);
}

fn analyzeAs(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    zir_dest_type: Zir.Inst.Ref,
    zir_operand: Zir.Inst.Ref,
    no_cast_to_comptime_int: bool,
) CompileError!Air.Inst.Ref {
    const is_ret = if (Zir.refToIndex(zir_dest_type)) |ptr_index|
        sema.code.instructions.items(.tag)[ptr_index] == .ret_type
    else
        false;
    const dest_ty = try sema.resolveType(block, src, zir_dest_type);
    const operand = try sema.resolveInst(zir_operand);
    if (dest_ty.tag() == .var_args_param) return operand;
    if (dest_ty.zigTypeTag() == .NoReturn) {
        return sema.fail(block, src, "cannot cast to noreturn", .{});
    }
    return sema.coerceExtra(block, dest_ty, operand, src, .{ .is_ret = is_ret, .no_cast_to_comptime_int = no_cast_to_comptime_int }) catch |err| switch (err) {
        error.NotCoercible => unreachable,
        else => |e| return e,
    };
}

fn zirPtrToInt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const ptr_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ptr = try sema.resolveInst(inst_data.operand);
    const ptr_ty = sema.typeOf(ptr);
    if (!ptr_ty.isPtrAtRuntime()) {
        return sema.fail(block, ptr_src, "expected pointer, found '{}'", .{ptr_ty.fmt(sema.mod)});
    }
    if (try sema.resolveMaybeUndefValIntable(ptr)) |ptr_val| {
        return sema.addConstant(Type.usize, ptr_val);
    }
    try sema.requireRuntimeBlock(block, inst_data.src(), ptr_src);
    return block.addUnOp(.ptrtoint, ptr);
}

fn zirFieldVal(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_field_name = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Field, inst_data.payload_index).data;
    const field_name = sema.code.nullTerminatedString(extra.field_name_start);
    const object = try sema.resolveInst(extra.lhs);
    return sema.fieldVal(block, src, object, field_name, field_name_src);
}

fn zirFieldPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index, initializing: bool) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_field_name = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Field, inst_data.payload_index).data;
    const field_name = sema.code.nullTerminatedString(extra.field_name_start);
    const object_ptr = try sema.resolveInst(extra.lhs);
    return sema.fieldPtr(block, src, object_ptr, field_name, field_name_src, initializing);
}

fn zirFieldCallBind(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_field_name = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Field, inst_data.payload_index).data;
    const field_name = sema.code.nullTerminatedString(extra.field_name_start);
    const object_ptr = try sema.resolveInst(extra.lhs);
    return sema.fieldCallBind(block, src, object_ptr, field_name, field_name_src);
}

fn zirFieldValNamed(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.FieldNamed, inst_data.payload_index).data;
    const object = try sema.resolveInst(extra.lhs);
    const field_name = try sema.resolveConstString(block, field_name_src, extra.field_name, "field name must be comptime-known");
    return sema.fieldVal(block, src, object, field_name, field_name_src);
}

fn zirFieldPtrNamed(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.FieldNamed, inst_data.payload_index).data;
    const object_ptr = try sema.resolveInst(extra.lhs);
    const field_name = try sema.resolveConstString(block, field_name_src, extra.field_name, "field name must be comptime-known");
    return sema.fieldPtr(block, src, object_ptr, field_name, field_name_src, false);
}

fn zirFieldCallBindNamed(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.FieldNamedNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const field_name_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = extra.node };
    const object_ptr = try sema.resolveInst(extra.lhs);
    const field_name = try sema.resolveConstString(block, field_name_src, extra.field_name, "field name must be comptime-known");
    return sema.fieldCallBind(block, src, object_ptr, field_name, field_name_src);
}

fn zirIntCast(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;

    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);

    return sema.intCast(block, inst_data.src(), dest_ty, dest_ty_src, operand, operand_src, true);
}

fn intCast(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    dest_ty: Type,
    dest_ty_src: LazySrcLoc,
    operand: Air.Inst.Ref,
    operand_src: LazySrcLoc,
    runtime_safety: bool,
) CompileError!Air.Inst.Ref {
    const operand_ty = sema.typeOf(operand);
    const dest_scalar_ty = try sema.checkIntOrVectorAllowComptime(block, dest_ty, dest_ty_src);
    const operand_scalar_ty = try sema.checkIntOrVectorAllowComptime(block, operand_ty, operand_src);

    if (try sema.isComptimeKnown(operand)) {
        return sema.coerce(block, dest_ty, operand, operand_src);
    } else if (dest_scalar_ty.zigTypeTag() == .ComptimeInt) {
        return sema.fail(block, operand_src, "unable to cast runtime value to 'comptime_int'", .{});
    }

    try sema.checkVectorizableBinaryOperands(block, operand_src, dest_ty, operand_ty, dest_ty_src, operand_src);
    const is_vector = dest_ty.zigTypeTag() == .Vector;

    if ((try sema.typeHasOnePossibleValue(dest_ty))) |opv| {
        // requirement: intCast(u0, input) iff input == 0
        if (runtime_safety and block.wantSafety()) {
            try sema.requireRuntimeBlock(block, src, operand_src);
            const target = sema.mod.getTarget();
            const wanted_info = dest_scalar_ty.intInfo(target);
            const wanted_bits = wanted_info.bits;

            if (wanted_bits == 0) {
                const ok = if (is_vector) ok: {
                    const zeros = try Value.Tag.repeated.create(sema.arena, Value.zero);
                    const zero_inst = try sema.addConstant(sema.typeOf(operand), zeros);
                    const is_in_range = try block.addCmpVector(operand, zero_inst, .eq);
                    const all_in_range = try block.addInst(.{
                        .tag = .reduce,
                        .data = .{ .reduce = .{ .operand = is_in_range, .operation = .And } },
                    });
                    break :ok all_in_range;
                } else ok: {
                    const zero_inst = try sema.addConstant(sema.typeOf(operand), Value.zero);
                    const is_in_range = try block.addBinOp(.cmp_lte, operand, zero_inst);
                    break :ok is_in_range;
                };
                try sema.addSafetyCheck(block, ok, .cast_truncated_data);
            }
        }

        return sema.addConstant(dest_ty, opv);
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    if (runtime_safety and block.wantSafety()) {
        const target = sema.mod.getTarget();
        const actual_info = operand_scalar_ty.intInfo(target);
        const wanted_info = dest_scalar_ty.intInfo(target);
        const actual_bits = actual_info.bits;
        const wanted_bits = wanted_info.bits;
        const actual_value_bits = actual_bits - @boolToInt(actual_info.signedness == .signed);
        const wanted_value_bits = wanted_bits - @boolToInt(wanted_info.signedness == .signed);

        // range shrinkage
        // requirement: int value fits into target type
        if (wanted_value_bits < actual_value_bits) {
            const dest_max_val_scalar = try dest_scalar_ty.maxInt(sema.arena, target);
            const dest_max_val = if (is_vector)
                try Value.Tag.repeated.create(sema.arena, dest_max_val_scalar)
            else
                dest_max_val_scalar;
            const dest_max = try sema.addConstant(operand_ty, dest_max_val);
            const diff = try block.addBinOp(.subwrap, dest_max, operand);

            if (actual_info.signedness == .signed) {
                // Reinterpret the sign-bit as part of the value. This will make
                // negative differences (`operand` > `dest_max`) appear too big.
                const unsigned_operand_ty = try Type.Tag.int_unsigned.create(sema.arena, actual_bits);
                const diff_unsigned = try block.addBitCast(unsigned_operand_ty, diff);

                // If the destination type is signed, then we need to double its
                // range to account for negative values.
                const dest_range_val = if (wanted_info.signedness == .signed) range_val: {
                    const range_minus_one = try dest_max_val.shl(Value.one, unsigned_operand_ty, sema.arena, sema.mod);
                    break :range_val try sema.intAdd(range_minus_one, Value.one, unsigned_operand_ty);
                } else dest_max_val;
                const dest_range = try sema.addConstant(unsigned_operand_ty, dest_range_val);

                const ok = if (is_vector) ok: {
                    const is_in_range = try block.addCmpVector(diff_unsigned, dest_range, .lte);
                    const all_in_range = try block.addInst(.{
                        .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                        .data = .{ .reduce = .{
                            .operand = is_in_range,
                            .operation = .And,
                        } },
                    });
                    break :ok all_in_range;
                } else ok: {
                    const is_in_range = try block.addBinOp(.cmp_lte, diff_unsigned, dest_range);
                    break :ok is_in_range;
                };
                // TODO negative_to_unsigned?
                try sema.addSafetyCheck(block, ok, .cast_truncated_data);
            } else {
                const ok = if (is_vector) ok: {
                    const is_in_range = try block.addCmpVector(diff, dest_max, .lte);
                    const all_in_range = try block.addInst(.{
                        .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                        .data = .{ .reduce = .{
                            .operand = is_in_range,
                            .operation = .And,
                        } },
                    });
                    break :ok all_in_range;
                } else ok: {
                    const is_in_range = try block.addBinOp(.cmp_lte, diff, dest_max);
                    break :ok is_in_range;
                };
                try sema.addSafetyCheck(block, ok, .cast_truncated_data);
            }
        } else if (actual_info.signedness == .signed and wanted_info.signedness == .unsigned) {
            // no shrinkage, yes sign loss
            // requirement: signed to unsigned >= 0
            const ok = if (is_vector) ok: {
                const zero_val = try Value.Tag.repeated.create(sema.arena, Value.zero);
                const zero_inst = try sema.addConstant(operand_ty, zero_val);
                const is_in_range = try block.addCmpVector(operand, zero_inst, .gte);
                const all_in_range = try block.addInst(.{
                    .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                    .data = .{ .reduce = .{
                        .operand = is_in_range,
                        .operation = .And,
                    } },
                });
                break :ok all_in_range;
            } else ok: {
                const zero_inst = try sema.addConstant(operand_ty, Value.zero);
                const is_in_range = try block.addBinOp(.cmp_gte, operand, zero_inst);
                break :ok is_in_range;
            };
            try sema.addSafetyCheck(block, ok, .negative_to_unsigned);
        }
    }
    return block.addTyOp(.intcast, dest_ty, operand);
}

fn zirBitcast(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;

    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const operand_ty = sema.typeOf(operand);
    switch (dest_ty.zigTypeTag()) {
        .AnyFrame,
        .ComptimeFloat,
        .ComptimeInt,
        .EnumLiteral,
        .ErrorSet,
        .ErrorUnion,
        .Fn,
        .Frame,
        .NoReturn,
        .Null,
        .Opaque,
        .Optional,
        .Type,
        .Undefined,
        .Void,
        => return sema.fail(block, dest_ty_src, "cannot @bitCast to '{}'", .{dest_ty.fmt(sema.mod)}),

        .Enum => {
            const msg = msg: {
                const msg = try sema.errMsg(block, dest_ty_src, "cannot @bitCast to '{}'", .{dest_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);
                switch (operand_ty.zigTypeTag()) {
                    .Int, .ComptimeInt => try sema.errNote(block, dest_ty_src, msg, "use @intToEnum to cast from '{}'", .{operand_ty.fmt(sema.mod)}),
                    else => {},
                }

                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },

        .Pointer => {
            const msg = msg: {
                const msg = try sema.errMsg(block, dest_ty_src, "cannot @bitCast to '{}'", .{dest_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);
                switch (operand_ty.zigTypeTag()) {
                    .Int, .ComptimeInt => try sema.errNote(block, dest_ty_src, msg, "use @intToPtr to cast from '{}'", .{operand_ty.fmt(sema.mod)}),
                    .Pointer => try sema.errNote(block, dest_ty_src, msg, "use @ptrCast to cast from '{}'", .{operand_ty.fmt(sema.mod)}),
                    else => {},
                }

                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        .Struct, .Union => if (dest_ty.containerLayout() == .Auto) {
            const container = switch (dest_ty.zigTypeTag()) {
                .Struct => "struct",
                .Union => "union",
                else => unreachable,
            };
            return sema.fail(block, dest_ty_src, "cannot @bitCast to '{}', {s} does not have a guaranteed in-memory layout", .{
                dest_ty.fmt(sema.mod), container,
            });
        },

        .Array,
        .Bool,
        .Float,
        .Int,
        .Vector,
        => {},
    }
    switch (operand_ty.zigTypeTag()) {
        .AnyFrame,
        .ComptimeFloat,
        .ComptimeInt,
        .EnumLiteral,
        .ErrorSet,
        .ErrorUnion,
        .Fn,
        .Frame,
        .NoReturn,
        .Null,
        .Opaque,
        .Optional,
        .Type,
        .Undefined,
        .Void,
        => return sema.fail(block, operand_src, "cannot @bitCast from '{}'", .{operand_ty.fmt(sema.mod)}),

        .Enum => {
            const msg = msg: {
                const msg = try sema.errMsg(block, operand_src, "cannot @bitCast from '{}'", .{operand_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);
                switch (dest_ty.zigTypeTag()) {
                    .Int, .ComptimeInt => try sema.errNote(block, operand_src, msg, "use @enumToInt to cast to '{}'", .{dest_ty.fmt(sema.mod)}),
                    else => {},
                }

                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        .Pointer => {
            const msg = msg: {
                const msg = try sema.errMsg(block, operand_src, "cannot @bitCast from '{}'", .{operand_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);
                switch (dest_ty.zigTypeTag()) {
                    .Int, .ComptimeInt => try sema.errNote(block, operand_src, msg, "use @ptrToInt to cast to '{}'", .{dest_ty.fmt(sema.mod)}),
                    .Pointer => try sema.errNote(block, operand_src, msg, "use @ptrCast to cast to '{}'", .{dest_ty.fmt(sema.mod)}),
                    else => {},
                }

                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        .Struct, .Union => if (operand_ty.containerLayout() == .Auto) {
            const container = switch (operand_ty.zigTypeTag()) {
                .Struct => "struct",
                .Union => "union",
                else => unreachable,
            };
            return sema.fail(block, operand_src, "cannot @bitCast from '{}', {s} does not have a guaranteed in-memory layout", .{
                operand_ty.fmt(sema.mod), container,
            });
        },

        .Array,
        .Bool,
        .Float,
        .Int,
        .Vector,
        => {},
    }
    return sema.bitCast(block, dest_ty, operand, inst_data.src(), operand_src);
}

fn zirFloatCast(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;

    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);

    const target = sema.mod.getTarget();
    const dest_is_comptime_float = switch (dest_ty.zigTypeTag()) {
        .ComptimeFloat => true,
        .Float => false,
        else => return sema.fail(
            block,
            dest_ty_src,
            "expected float type, found '{}'",
            .{dest_ty.fmt(sema.mod)},
        ),
    };

    const operand_ty = sema.typeOf(operand);
    switch (operand_ty.zigTypeTag()) {
        .ComptimeFloat, .Float, .ComptimeInt => {},
        else => return sema.fail(
            block,
            operand_src,
            "expected float type, found '{}'",
            .{operand_ty.fmt(sema.mod)},
        ),
    }

    if (try sema.resolveMaybeUndefVal(operand)) |operand_val| {
        return sema.addConstant(dest_ty, try operand_val.floatCast(sema.arena, dest_ty, target));
    }
    if (dest_is_comptime_float) {
        return sema.fail(block, operand_src, "unable to cast runtime value to 'comptime_float'", .{});
    }
    const src_bits = operand_ty.floatBits(target);
    const dst_bits = dest_ty.floatBits(target);
    if (dst_bits >= src_bits) {
        return sema.coerce(block, dest_ty, operand, operand_src);
    }
    try sema.requireRuntimeBlock(block, inst_data.src(), operand_src);
    return block.addTyOp(.fptrunc, dest_ty, operand);
}

fn zirElemVal(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const array = try sema.resolveInst(extra.lhs);
    const elem_index = try sema.resolveInst(extra.rhs);
    return sema.elemVal(block, src, array, elem_index, src, false);
}

fn zirElemValNode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const elem_index_src: LazySrcLoc = .{ .node_offset_array_access_index = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const array = try sema.resolveInst(extra.lhs);
    const elem_index = try sema.resolveInst(extra.rhs);
    return sema.elemVal(block, src, array, elem_index, elem_index_src, true);
}

fn zirElemPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.lhs);
    const elem_index = try sema.resolveInst(extra.rhs);
    const indexable_ty = sema.typeOf(array_ptr);
    if (indexable_ty.zigTypeTag() != .Pointer) {
        const capture_src: LazySrcLoc = .{ .for_capture_from_input = inst_data.src_node };
        const msg = msg: {
            const msg = try sema.errMsg(block, capture_src, "pointer capture of non pointer type '{}'", .{
                indexable_ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);
            if (indexable_ty.zigTypeTag() == .Array) {
                try sema.errNote(block, src, msg, "consider using '&' here", .{});
            }
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    return sema.elemPtrOneLayerOnly(block, src, array_ptr, elem_index, src, false, false);
}

fn zirElemPtrNode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const elem_index_src: LazySrcLoc = .{ .node_offset_array_access_index = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.lhs);
    const elem_index = try sema.resolveInst(extra.rhs);
    return sema.elemPtr(block, src, array_ptr, elem_index, elem_index_src, false, true);
}

fn zirElemPtrImm(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.ElemPtrImm, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.ptr);
    const elem_index = try sema.addIntUnsigned(Type.usize, extra.index);
    return sema.elemPtr(block, src, array_ptr, elem_index, src, true, true);
}

fn zirSliceStart(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.SliceStart, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.lhs);
    const start = try sema.resolveInst(extra.start);

    return sema.analyzeSlice(block, src, array_ptr, start, .none, .none, .unneeded);
}

fn zirSliceEnd(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.SliceEnd, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.lhs);
    const start = try sema.resolveInst(extra.start);
    const end = try sema.resolveInst(extra.end);

    return sema.analyzeSlice(block, src, array_ptr, start, end, .none, .unneeded);
}

fn zirSliceSentinel(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const sentinel_src: LazySrcLoc = .{ .node_offset_slice_sentinel = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.SliceSentinel, inst_data.payload_index).data;
    const array_ptr = try sema.resolveInst(extra.lhs);
    const start = try sema.resolveInst(extra.start);
    const end = try sema.resolveInst(extra.end);
    const sentinel = try sema.resolveInst(extra.sentinel);

    return sema.analyzeSlice(block, src, array_ptr, start, end, sentinel, sentinel_src);
}

fn zirSwitchCapture(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_multi: bool,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const zir_datas = sema.code.instructions.items(.data);
    const capture_info = zir_datas[inst].switch_capture;
    const switch_info = zir_datas[capture_info.switch_inst].pl_node;
    const switch_extra = sema.code.extraData(Zir.Inst.SwitchBlock, switch_info.payload_index);
    const operand_src: LazySrcLoc = .{ .node_offset_switch_operand = switch_info.src_node };
    const cond_inst = Zir.refToIndex(switch_extra.data.operand).?;
    const cond_info = zir_datas[cond_inst].un_node;
    const cond_tag = sema.code.instructions.items(.tag)[cond_inst];
    const operand_is_ref = cond_tag == .switch_cond_ref;
    const operand_ptr = try sema.resolveInst(cond_info.operand);
    const operand_ptr_ty = sema.typeOf(operand_ptr);
    const operand_ty = if (operand_is_ref) operand_ptr_ty.childType() else operand_ptr_ty;

    if (block.inline_case_capture != .none) {
        const item_val = sema.resolveConstValue(block, .unneeded, block.inline_case_capture, undefined) catch unreachable;
        if (operand_ty.zigTypeTag() == .Union) {
            const field_index = @intCast(u32, operand_ty.unionTagFieldIndex(item_val, sema.mod).?);
            const union_obj = operand_ty.cast(Type.Payload.Union).?.data;
            const field_ty = union_obj.fields.values()[field_index].ty;
            if (is_ref) {
                const ptr_field_ty = try Type.ptr(sema.arena, sema.mod, .{
                    .pointee_type = field_ty,
                    .mutable = operand_ptr_ty.ptrIsMutable(),
                    .@"volatile" = operand_ptr_ty.isVolatilePtr(),
                    .@"addrspace" = operand_ptr_ty.ptrAddressSpace(),
                });
                return block.addStructFieldPtr(operand_ptr, field_index, ptr_field_ty);
            } else {
                return block.addStructFieldVal(operand_ptr, field_index, field_ty);
            }
        } else if (is_ref) {
            return sema.addConstantMaybeRef(block, operand_ty, item_val, true);
        } else {
            return block.inline_case_capture;
        }
    }

    const operand = if (operand_is_ref)
        try sema.analyzeLoad(block, operand_src, operand_ptr, operand_src)
    else
        operand_ptr;

    if (capture_info.prong_index == std.math.maxInt(@TypeOf(capture_info.prong_index))) {
        // It is the else/`_` prong.
        if (is_ref) {
            return operand_ptr;
        }

        switch (operand_ty.zigTypeTag()) {
            .ErrorSet => if (block.switch_else_err_ty) |some| {
                return sema.bitCast(block, some, operand, operand_src, null);
            } else {
                try block.addUnreachable(false);
                return Air.Inst.Ref.unreachable_value;
            },
            else => return operand,
        }
    }

    const items = if (is_multi)
        switch_extra.data.getMultiProng(sema.code, switch_extra.end, capture_info.prong_index).items
    else
        &[_]Zir.Inst.Ref{
            switch_extra.data.getScalarProng(sema.code, switch_extra.end, capture_info.prong_index).item,
        };

    switch (operand_ty.zigTypeTag()) {
        .Union => {
            const union_obj = operand_ty.cast(Type.Payload.Union).?.data;
            const first_item = try sema.resolveInst(items[0]);
            // Previous switch validation ensured this will succeed
            const first_item_val = sema.resolveConstValue(block, .unneeded, first_item, "") catch unreachable;

            const first_field_index = @intCast(u32, operand_ty.unionTagFieldIndex(first_item_val, sema.mod).?);
            const first_field = union_obj.fields.values()[first_field_index];

            for (items[1..], 0..) |item, i| {
                const item_ref = try sema.resolveInst(item);
                // Previous switch validation ensured this will succeed
                const item_val = sema.resolveConstValue(block, .unneeded, item_ref, "") catch unreachable;

                const field_index = operand_ty.unionTagFieldIndex(item_val, sema.mod).?;
                const field = union_obj.fields.values()[field_index];
                if (!field.ty.eql(first_field.ty, sema.mod)) {
                    const msg = msg: {
                        const raw_capture_src = Module.SwitchProngSrc{ .multi_capture = capture_info.prong_index };
                        const capture_src = raw_capture_src.resolve(sema.gpa, sema.mod.declPtr(block.src_decl), switch_info.src_node, .first);

                        const msg = try sema.errMsg(block, capture_src, "capture group with incompatible types", .{});
                        errdefer msg.destroy(sema.gpa);

                        const raw_first_item_src = Module.SwitchProngSrc{ .multi = .{ .prong = capture_info.prong_index, .item = 0 } };
                        const first_item_src = raw_first_item_src.resolve(sema.gpa, sema.mod.declPtr(block.src_decl), switch_info.src_node, .first);
                        const raw_item_src = Module.SwitchProngSrc{ .multi = .{ .prong = capture_info.prong_index, .item = 1 + @intCast(u32, i) } };
                        const item_src = raw_item_src.resolve(sema.gpa, sema.mod.declPtr(block.src_decl), switch_info.src_node, .first);
                        try sema.errNote(block, first_item_src, msg, "type '{}' here", .{first_field.ty.fmt(sema.mod)});
                        try sema.errNote(block, item_src, msg, "type '{}' here", .{field.ty.fmt(sema.mod)});
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
            }

            if (is_ref) {
                const field_ty_ptr = try Type.ptr(sema.arena, sema.mod, .{
                    .pointee_type = first_field.ty,
                    .@"addrspace" = .generic,
                    .mutable = operand_ptr_ty.ptrIsMutable(),
                });

                if (try sema.resolveDefinedValue(block, operand_src, operand_ptr)) |op_ptr_val| {
                    return sema.addConstant(
                        field_ty_ptr,
                        try Value.Tag.field_ptr.create(sema.arena, .{
                            .container_ptr = op_ptr_val,
                            .container_ty = operand_ty,
                            .field_index = first_field_index,
                        }),
                    );
                }
                try sema.requireRuntimeBlock(block, operand_src, null);
                return block.addStructFieldPtr(operand_ptr, first_field_index, field_ty_ptr);
            }

            if (try sema.resolveDefinedValue(block, operand_src, operand)) |operand_val| {
                return sema.addConstant(
                    first_field.ty,
                    operand_val.castTag(.@"union").?.data.val,
                );
            }
            try sema.requireRuntimeBlock(block, operand_src, null);
            return block.addStructFieldVal(operand, first_field_index, first_field.ty);
        },
        .ErrorSet => {
            if (is_multi) {
                var names: Module.ErrorSet.NameMap = .{};
                try names.ensureUnusedCapacity(sema.arena, items.len);
                for (items) |item| {
                    const item_ref = try sema.resolveInst(item);
                    // Previous switch validation ensured this will succeed
                    const item_val = sema.resolveConstValue(block, .unneeded, item_ref, "") catch unreachable;
                    names.putAssumeCapacityNoClobber(
                        item_val.getError().?,
                        {},
                    );
                }
                // names must be sorted
                Module.ErrorSet.sortNames(&names);
                const else_error_ty = try Type.Tag.error_set_merged.create(sema.arena, names);

                return sema.bitCast(block, else_error_ty, operand, operand_src, null);
            } else {
                const item_ref = try sema.resolveInst(items[0]);
                // Previous switch validation ensured this will succeed
                const item_val = sema.resolveConstValue(block, .unneeded, item_ref, "") catch unreachable;

                const item_ty = try Type.Tag.error_set_single.create(sema.arena, item_val.getError().?);
                return sema.bitCast(block, item_ty, operand, operand_src, null);
            }
        },
        else => {
            // In this case the capture value is just the passed-through value of the
            // switch condition.
            if (is_ref) {
                return operand_ptr;
            } else {
                return operand;
            }
        },
    }
}

fn zirSwitchCaptureTag(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const zir_datas = sema.code.instructions.items(.data);
    const inst_data = zir_datas[inst].un_tok;
    const src = inst_data.src();

    const switch_tag = sema.code.instructions.items(.tag)[Zir.refToIndex(inst_data.operand).?];
    const is_ref = switch_tag == .switch_cond_ref;
    const cond_data = zir_datas[Zir.refToIndex(inst_data.operand).?].un_node;
    const operand_ptr = try sema.resolveInst(cond_data.operand);
    const operand_ptr_ty = sema.typeOf(operand_ptr);
    const operand_ty = if (is_ref) operand_ptr_ty.childType() else operand_ptr_ty;

    if (operand_ty.zigTypeTag() != .Union) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "cannot capture tag of non-union type '{}'", .{
                operand_ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);
            try sema.addDeclaredHereNote(msg, operand_ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    return block.inline_case_capture;
}

fn zirSwitchCond(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_switch_operand = inst_data.src_node };
    const operand_ptr = try sema.resolveInst(inst_data.operand);
    const operand = if (is_ref)
        try sema.analyzeLoad(block, src, operand_ptr, operand_src)
    else
        operand_ptr;
    const operand_ty = sema.typeOf(operand);

    switch (operand_ty.zigTypeTag()) {
        .Type,
        .Void,
        .Bool,
        .Int,
        .Float,
        .ComptimeFloat,
        .ComptimeInt,
        .EnumLiteral,
        .Pointer,
        .Fn,
        .ErrorSet,
        .Enum,
        => {
            if (operand_ty.isSlice()) {
                return sema.fail(block, src, "switch on type '{}'", .{operand_ty.fmt(sema.mod)});
            }
            if ((try sema.typeHasOnePossibleValue(operand_ty))) |opv| {
                return sema.addConstant(operand_ty, opv);
            }
            return operand;
        },

        .Union => {
            const union_ty = try sema.resolveTypeFields(operand_ty);
            const enum_ty = union_ty.unionTagType() orelse {
                const msg = msg: {
                    const msg = try sema.errMsg(block, src, "switch on union with no attached enum", .{});
                    errdefer msg.destroy(sema.gpa);
                    if (union_ty.declSrcLocOrNull(sema.mod)) |union_src| {
                        try sema.mod.errNoteNonLazy(union_src, msg, "consider 'union(enum)' here", .{});
                    }
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            };
            return sema.unionToTag(block, enum_ty, operand, src);
        },

        .ErrorUnion,
        .NoReturn,
        .Array,
        .Struct,
        .Undefined,
        .Null,
        .Optional,
        .Opaque,
        .Vector,
        .Frame,
        .AnyFrame,
        => return sema.fail(block, src, "switch on type '{}'", .{operand_ty.fmt(sema.mod)}),
    }
}

const SwitchErrorSet = std.StringHashMap(Module.SwitchProngSrc);

fn zirSwitchBlock(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const gpa = sema.gpa;
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const src_node_offset = inst_data.src_node;
    const operand_src: LazySrcLoc = .{ .node_offset_switch_operand = src_node_offset };
    const special_prong_src: LazySrcLoc = .{ .node_offset_switch_special_prong = src_node_offset };
    const extra = sema.code.extraData(Zir.Inst.SwitchBlock, inst_data.payload_index);

    const operand = try sema.resolveInst(extra.data.operand);
    // AstGen guarantees that the instruction immediately following
    // switch_cond(_ref) is a dbg_stmt
    const cond_dbg_node_index = Zir.refToIndex(extra.data.operand).? + 1;

    var header_extra_index: usize = extra.end;

    const scalar_cases_len = extra.data.bits.scalar_cases_len;
    const multi_cases_len = if (extra.data.bits.has_multi_cases) blk: {
        const multi_cases_len = sema.code.extra[header_extra_index];
        header_extra_index += 1;
        break :blk multi_cases_len;
    } else 0;

    const special_prong = extra.data.bits.specialProng();
    const special: struct { body: []const Zir.Inst.Index, end: usize, is_inline: bool } = switch (special_prong) {
        .none => .{ .body = &.{}, .end = header_extra_index, .is_inline = false },
        .under, .@"else" => blk: {
            const body_len = @truncate(u31, sema.code.extra[header_extra_index]);
            const extra_body_start = header_extra_index + 1;
            break :blk .{
                .body = sema.code.extra[extra_body_start..][0..body_len],
                .end = extra_body_start + body_len,
                .is_inline = sema.code.extra[header_extra_index] >> 31 != 0,
            };
        },
    };

    const maybe_union_ty = blk: {
        const zir_tags = sema.code.instructions.items(.tag);
        const zir_data = sema.code.instructions.items(.data);
        const cond_index = Zir.refToIndex(extra.data.operand).?;
        const raw_operand = sema.resolveInst(zir_data[cond_index].un_node.operand) catch unreachable;
        const target_ty = sema.typeOf(raw_operand);
        break :blk if (zir_tags[cond_index] == .switch_cond_ref) target_ty.elemType() else target_ty;
    };
    const union_originally = maybe_union_ty.zigTypeTag() == .Union;

    // Duplicate checking variables later also used for `inline else`.
    var seen_enum_fields: []?Module.SwitchProngSrc = &.{};
    var seen_errors = SwitchErrorSet.init(gpa);
    var range_set = RangeSet.init(gpa, sema.mod);
    var true_count: u8 = 0;
    var false_count: u8 = 0;

    defer {
        range_set.deinit();
        gpa.free(seen_enum_fields);
        seen_errors.deinit();
    }

    var empty_enum = false;

    const operand_ty = sema.typeOf(operand);
    const err_set = operand_ty.zigTypeTag() == .ErrorSet;

    var else_error_ty: ?Type = null;

    // Validate usage of '_' prongs.
    if (special_prong == .under and (!operand_ty.isNonexhaustiveEnum() or union_originally)) {
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                src,
                "'_' prong only allowed when switching on non-exhaustive enums",
                .{},
            );
            errdefer msg.destroy(gpa);
            try sema.errNote(
                block,
                special_prong_src,
                msg,
                "'_' prong here",
                .{},
            );
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    const target = sema.mod.getTarget();

    // Validate for duplicate items, missing else prong, and invalid range.
    switch (operand_ty.zigTypeTag()) {
        .Union => unreachable, // handled in zirSwitchCond
        .Enum => {
            seen_enum_fields = try gpa.alloc(?Module.SwitchProngSrc, operand_ty.enumFieldCount());
            empty_enum = seen_enum_fields.len == 0 and !operand_ty.isNonexhaustiveEnum();
            mem.set(?Module.SwitchProngSrc, seen_enum_fields, null);
            // `range_set` is used for non-exhaustive enum values that do not correspond to any tags.

            var extra_index: usize = special.end;
            {
                var scalar_i: u32 = 0;
                while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                    const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    extra_index += body_len;

                    try sema.validateSwitchItemEnum(
                        block,
                        seen_enum_fields,
                        &range_set,
                        item_ref,
                        src_node_offset,
                        .{ .scalar = scalar_i },
                    );
                }
            }
            {
                var multi_i: u32 = 0;
                while (multi_i < multi_cases_len) : (multi_i += 1) {
                    const items_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const ranges_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const items = sema.code.refSlice(extra_index, items_len);
                    extra_index += items_len + body_len;

                    for (items, 0..) |item_ref, item_i| {
                        try sema.validateSwitchItemEnum(
                            block,
                            seen_enum_fields,
                            &range_set,
                            item_ref,
                            src_node_offset,
                            .{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } },
                        );
                    }

                    try sema.validateSwitchNoRange(block, ranges_len, operand_ty, src_node_offset);
                }
            }
            const all_tags_handled = for (seen_enum_fields) |seen_src| {
                if (seen_src == null) break false;
            } else true;

            if (special_prong == .@"else") {
                if (all_tags_handled and !operand_ty.isNonexhaustiveEnum()) return sema.fail(
                    block,
                    special_prong_src,
                    "unreachable else prong; all cases already handled",
                    .{},
                );
            } else if (!all_tags_handled) {
                const msg = msg: {
                    const msg = try sema.errMsg(
                        block,
                        src,
                        "switch must handle all possibilities",
                        .{},
                    );
                    errdefer msg.destroy(sema.gpa);
                    for (seen_enum_fields, 0..) |seen_src, i| {
                        if (seen_src != null) continue;

                        const field_name = operand_ty.enumFieldName(i);
                        try sema.addFieldErrNote(
                            operand_ty,
                            i,
                            msg,
                            "unhandled enumeration value: '{s}'",
                            .{field_name},
                        );
                    }
                    try sema.mod.errNoteNonLazy(
                        operand_ty.declSrcLoc(sema.mod),
                        msg,
                        "enum '{}' declared here",
                        .{operand_ty.fmt(sema.mod)},
                    );
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            } else if (special_prong == .none and operand_ty.isNonexhaustiveEnum() and !union_originally) {
                return sema.fail(
                    block,
                    src,
                    "switch on non-exhaustive enum must include 'else' or '_' prong",
                    .{},
                );
            }
        },
        .ErrorSet => {
            var extra_index: usize = special.end;
            {
                var scalar_i: u32 = 0;
                while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                    const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    extra_index += body_len;

                    try sema.validateSwitchItemError(
                        block,
                        &seen_errors,
                        item_ref,
                        src_node_offset,
                        .{ .scalar = scalar_i },
                    );
                }
            }
            {
                var multi_i: u32 = 0;
                while (multi_i < multi_cases_len) : (multi_i += 1) {
                    const items_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const ranges_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const items = sema.code.refSlice(extra_index, items_len);
                    extra_index += items_len + body_len;

                    for (items, 0..) |item_ref, item_i| {
                        try sema.validateSwitchItemError(
                            block,
                            &seen_errors,
                            item_ref,
                            src_node_offset,
                            .{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } },
                        );
                    }

                    try sema.validateSwitchNoRange(block, ranges_len, operand_ty, src_node_offset);
                }
            }

            try sema.resolveInferredErrorSetTy(block, src, operand_ty);

            if (operand_ty.isAnyError()) {
                if (special_prong != .@"else") {
                    return sema.fail(
                        block,
                        src,
                        "else prong required when switching on type 'anyerror'",
                        .{},
                    );
                }
                else_error_ty = Type.anyerror;
            } else else_validation: {
                var maybe_msg: ?*Module.ErrorMsg = null;
                errdefer if (maybe_msg) |msg| msg.destroy(sema.gpa);

                for (operand_ty.errorSetNames()) |error_name| {
                    if (!seen_errors.contains(error_name) and special_prong != .@"else") {
                        const msg = maybe_msg orelse blk: {
                            maybe_msg = try sema.errMsg(
                                block,
                                src,
                                "switch must handle all possibilities",
                                .{},
                            );
                            break :blk maybe_msg.?;
                        };

                        try sema.errNote(
                            block,
                            src,
                            msg,
                            "unhandled error value: 'error.{s}'",
                            .{error_name},
                        );
                    }
                }

                if (maybe_msg) |msg| {
                    maybe_msg = null;
                    try sema.addDeclaredHereNote(msg, operand_ty);
                    return sema.failWithOwnedErrorMsg(msg);
                }

                if (special_prong == .@"else" and seen_errors.count() == operand_ty.errorSetNames().len) {
                    // In order to enable common patterns for generic code allow simple else bodies
                    // else => unreachable,
                    // else => return,
                    // else => |e| return e,
                    // even if all the possible errors were already handled.
                    const tags = sema.code.instructions.items(.tag);
                    for (special.body) |else_inst| switch (tags[else_inst]) {
                        .dbg_block_begin,
                        .dbg_block_end,
                        .dbg_stmt,
                        .dbg_var_val,
                        .switch_capture,
                        .ret_type,
                        .as_node,
                        .ret_node,
                        .@"unreachable",
                        .@"defer",
                        .defer_err_code,
                        .err_union_code,
                        .ret_err_value_code,
                        .restore_err_ret_index,
                        .is_non_err,
                        .ret_is_non_err,
                        .condbr,
                        => {},
                        else => break,
                    } else break :else_validation;

                    return sema.fail(
                        block,
                        special_prong_src,
                        "unreachable else prong; all cases already handled",
                        .{},
                    );
                }

                const error_names = operand_ty.errorSetNames();
                var names: Module.ErrorSet.NameMap = .{};
                try names.ensureUnusedCapacity(sema.arena, error_names.len);
                for (error_names) |error_name| {
                    if (seen_errors.contains(error_name)) continue;

                    names.putAssumeCapacityNoClobber(error_name, {});
                }

                // names must be sorted
                Module.ErrorSet.sortNames(&names);
                else_error_ty = try Type.Tag.error_set_merged.create(sema.arena, names);
            }
        },
        .Int, .ComptimeInt => {
            var extra_index: usize = special.end;
            {
                var scalar_i: u32 = 0;
                while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                    const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    extra_index += body_len;

                    try sema.validateSwitchItem(
                        block,
                        &range_set,
                        item_ref,
                        operand_ty,
                        src_node_offset,
                        .{ .scalar = scalar_i },
                    );
                }
            }
            {
                var multi_i: u32 = 0;
                while (multi_i < multi_cases_len) : (multi_i += 1) {
                    const items_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const ranges_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const items = sema.code.refSlice(extra_index, items_len);
                    extra_index += items_len;

                    for (items, 0..) |item_ref, item_i| {
                        try sema.validateSwitchItem(
                            block,
                            &range_set,
                            item_ref,
                            operand_ty,
                            src_node_offset,
                            .{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } },
                        );
                    }

                    var range_i: u32 = 0;
                    while (range_i < ranges_len) : (range_i += 1) {
                        const item_first = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                        extra_index += 1;
                        const item_last = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                        extra_index += 1;

                        try sema.validateSwitchRange(
                            block,
                            &range_set,
                            item_first,
                            item_last,
                            operand_ty,
                            src_node_offset,
                            .{ .range = .{ .prong = multi_i, .item = range_i } },
                        );
                    }

                    extra_index += body_len;
                }
            }

            check_range: {
                if (operand_ty.zigTypeTag() == .Int) {
                    var arena = std.heap.ArenaAllocator.init(gpa);
                    defer arena.deinit();

                    const min_int = try operand_ty.minInt(arena.allocator(), target);
                    const max_int = try operand_ty.maxInt(arena.allocator(), target);
                    if (try range_set.spans(min_int, max_int, operand_ty)) {
                        if (special_prong == .@"else") {
                            return sema.fail(
                                block,
                                special_prong_src,
                                "unreachable else prong; all cases already handled",
                                .{},
                            );
                        }
                        break :check_range;
                    }
                }
                if (special_prong != .@"else") {
                    return sema.fail(
                        block,
                        src,
                        "switch must handle all possibilities",
                        .{},
                    );
                }
            }
        },
        .Bool => {
            var extra_index: usize = special.end;
            {
                var scalar_i: u32 = 0;
                while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                    const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    extra_index += body_len;

                    try sema.validateSwitchItemBool(
                        block,
                        &true_count,
                        &false_count,
                        item_ref,
                        src_node_offset,
                        .{ .scalar = scalar_i },
                    );
                }
            }
            {
                var multi_i: u32 = 0;
                while (multi_i < multi_cases_len) : (multi_i += 1) {
                    const items_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const ranges_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const items = sema.code.refSlice(extra_index, items_len);
                    extra_index += items_len + body_len;

                    for (items, 0..) |item_ref, item_i| {
                        try sema.validateSwitchItemBool(
                            block,
                            &true_count,
                            &false_count,
                            item_ref,
                            src_node_offset,
                            .{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } },
                        );
                    }

                    try sema.validateSwitchNoRange(block, ranges_len, operand_ty, src_node_offset);
                }
            }
            switch (special_prong) {
                .@"else" => {
                    if (true_count + false_count == 2) {
                        return sema.fail(
                            block,
                            special_prong_src,
                            "unreachable else prong; all cases already handled",
                            .{},
                        );
                    }
                },
                .under, .none => {
                    if (true_count + false_count < 2) {
                        return sema.fail(
                            block,
                            src,
                            "switch must handle all possibilities",
                            .{},
                        );
                    }
                },
            }
        },
        .EnumLiteral, .Void, .Fn, .Pointer, .Type => {
            if (special_prong != .@"else") {
                return sema.fail(
                    block,
                    src,
                    "else prong required when switching on type '{}'",
                    .{operand_ty.fmt(sema.mod)},
                );
            }

            var seen_values = ValueSrcMap.initContext(gpa, .{
                .ty = operand_ty,
                .mod = sema.mod,
            });
            defer seen_values.deinit();

            var extra_index: usize = special.end;
            {
                var scalar_i: u32 = 0;
                while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                    const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    extra_index += body_len;

                    try sema.validateSwitchItemSparse(
                        block,
                        &seen_values,
                        item_ref,
                        src_node_offset,
                        .{ .scalar = scalar_i },
                    );
                }
            }
            {
                var multi_i: u32 = 0;
                while (multi_i < multi_cases_len) : (multi_i += 1) {
                    const items_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const ranges_len = sema.code.extra[extra_index];
                    extra_index += 1;
                    const body_len = @truncate(u31, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const items = sema.code.refSlice(extra_index, items_len);
                    extra_index += items_len + body_len;

                    for (items, 0..) |item_ref, item_i| {
                        try sema.validateSwitchItemSparse(
                            block,
                            &seen_values,
                            item_ref,
                            src_node_offset,
                            .{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } },
                        );
                    }

                    try sema.validateSwitchNoRange(block, ranges_len, operand_ty, src_node_offset);
                }
            }
        },

        .ErrorUnion,
        .NoReturn,
        .Array,
        .Struct,
        .Undefined,
        .Null,
        .Optional,
        .Opaque,
        .Vector,
        .Frame,
        .AnyFrame,
        .ComptimeFloat,
        .Float,
        => return sema.fail(block, operand_src, "invalid switch operand type '{}'", .{
            operand_ty.fmt(sema.mod),
        }),
    }

    const block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
    try sema.air_instructions.append(gpa, .{
        .tag = .block,
        .data = undefined,
    });
    var label: Block.Label = .{
        .zir_block = inst,
        .merges = .{
            .results = .{},
            .br_list = .{},
            .block_inst = block_inst,
        },
    };

    var child_block: Block = .{
        .parent = block,
        .sema = sema,
        .src_decl = block.src_decl,
        .namespace = block.namespace,
        .wip_capture_scope = block.wip_capture_scope,
        .instructions = .{},
        .label = &label,
        .inlining = block.inlining,
        .is_comptime = block.is_comptime,
        .comptime_reason = block.comptime_reason,
        .is_typeof = block.is_typeof,
        .switch_else_err_ty = else_error_ty,
        .c_import_buf = block.c_import_buf,
        .runtime_cond = block.runtime_cond,
        .runtime_loop = block.runtime_loop,
        .runtime_index = block.runtime_index,
        .error_return_trace_index = block.error_return_trace_index,
    };
    const merges = &child_block.label.?.merges;
    defer child_block.instructions.deinit(gpa);
    defer merges.results.deinit(gpa);
    defer merges.br_list.deinit(gpa);

    if (try sema.resolveDefinedValue(&child_block, src, operand)) |operand_val| {
        var extra_index: usize = special.end;
        {
            var scalar_i: usize = 0;
            while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
                const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                extra_index += 1;
                const body_len = @truncate(u31, sema.code.extra[extra_index]);
                const is_inline = sema.code.extra[extra_index] >> 31 != 0;
                extra_index += 1;
                const body = sema.code.extra[extra_index..][0..body_len];
                extra_index += body_len;

                const item = try sema.resolveInst(item_ref);
                // Validation above ensured these will succeed.
                const item_val = sema.resolveConstValue(&child_block, .unneeded, item, "") catch unreachable;
                if (operand_val.eql(item_val, operand_ty, sema.mod)) {
                    if (is_inline) child_block.inline_case_capture = operand;

                    if (err_set) try sema.maybeErrorUnwrapComptime(&child_block, body, operand);
                    return sema.resolveBlockBody(block, src, &child_block, body, inst, merges);
                }
            }
        }
        {
            var multi_i: usize = 0;
            while (multi_i < multi_cases_len) : (multi_i += 1) {
                const items_len = sema.code.extra[extra_index];
                extra_index += 1;
                const ranges_len = sema.code.extra[extra_index];
                extra_index += 1;
                const body_len = @truncate(u31, sema.code.extra[extra_index]);
                const is_inline = sema.code.extra[extra_index] >> 31 != 0;
                extra_index += 1;
                const items = sema.code.refSlice(extra_index, items_len);
                extra_index += items_len;
                const body = sema.code.extra[extra_index + 2 * ranges_len ..][0..body_len];

                for (items) |item_ref| {
                    const item = try sema.resolveInst(item_ref);
                    // Validation above ensured these will succeed.
                    const item_val = sema.resolveConstValue(&child_block, .unneeded, item, "") catch unreachable;
                    if (operand_val.eql(item_val, operand_ty, sema.mod)) {
                        if (is_inline) child_block.inline_case_capture = operand;

                        if (err_set) try sema.maybeErrorUnwrapComptime(&child_block, body, operand);
                        return sema.resolveBlockBody(block, src, &child_block, body, inst, merges);
                    }
                }

                var range_i: usize = 0;
                while (range_i < ranges_len) : (range_i += 1) {
                    const item_first = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;
                    const item_last = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                    extra_index += 1;

                    // Validation above ensured these will succeed.
                    const first_tv = sema.resolveInstConst(&child_block, .unneeded, item_first, "") catch unreachable;
                    const last_tv = sema.resolveInstConst(&child_block, .unneeded, item_last, "") catch unreachable;
                    if ((try sema.compareAll(operand_val, .gte, first_tv.val, operand_ty)) and
                        (try sema.compareAll(operand_val, .lte, last_tv.val, operand_ty)))
                    {
                        if (is_inline) child_block.inline_case_capture = operand;
                        if (err_set) try sema.maybeErrorUnwrapComptime(&child_block, body, operand);
                        return sema.resolveBlockBody(block, src, &child_block, body, inst, merges);
                    }
                }

                extra_index += body_len;
            }
        }
        if (err_set) try sema.maybeErrorUnwrapComptime(&child_block, special.body, operand);
        if (special.is_inline) child_block.inline_case_capture = operand;
        if (empty_enum) {
            return Air.Inst.Ref.void_value;
        }
        return sema.resolveBlockBody(block, src, &child_block, special.body, inst, merges);
    }

    if (scalar_cases_len + multi_cases_len == 0 and !special.is_inline) {
        if (empty_enum) {
            return Air.Inst.Ref.void_value;
        }
        if (special_prong == .none) {
            return sema.fail(block, src, "switch must handle all possibilities", .{});
        }
        if (err_set and try sema.maybeErrorUnwrap(block, special.body, operand)) {
            return Air.Inst.Ref.unreachable_value;
        }
        if (sema.mod.backendSupportsFeature(.is_named_enum_value) and block.wantSafety() and operand_ty.zigTypeTag() == .Enum and
            (!operand_ty.isNonexhaustiveEnum() or union_originally))
        {
            try sema.zirDbgStmt(block, cond_dbg_node_index);
            const ok = try block.addUnOp(.is_named_enum_value, operand);
            try sema.addSafetyCheck(block, ok, .corrupt_switch);
        }
        return sema.resolveBlockBody(block, src, &child_block, special.body, inst, merges);
    }

    if (child_block.is_comptime) {
        _ = sema.resolveConstValue(&child_block, operand_src, operand, "condition in comptime switch must be comptime-known") catch |err| {
            if (err == error.AnalysisFail and child_block.comptime_reason != null) try child_block.comptime_reason.?.explain(sema, sema.err);
            return err;
        };
        unreachable;
    }

    const estimated_cases_extra = (scalar_cases_len + multi_cases_len) *
        @typeInfo(Air.SwitchBr.Case).Struct.fields.len + 2;
    var cases_extra = try std.ArrayListUnmanaged(u32).initCapacity(gpa, estimated_cases_extra);
    defer cases_extra.deinit(gpa);

    var case_block = child_block.makeSubBlock();
    case_block.runtime_loop = null;
    case_block.runtime_cond = operand_src;
    case_block.runtime_index.increment();
    defer case_block.instructions.deinit(gpa);

    var extra_index: usize = special.end;

    var scalar_i: usize = 0;
    while (scalar_i < scalar_cases_len) : (scalar_i += 1) {
        const item_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
        extra_index += 1;
        const body_len = @truncate(u31, sema.code.extra[extra_index]);
        const is_inline = sema.code.extra[extra_index] >> 31 != 0;
        extra_index += 1;
        const body = sema.code.extra[extra_index..][0..body_len];
        extra_index += body_len;

        var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, child_block.wip_capture_scope);
        defer wip_captures.deinit();

        case_block.instructions.shrinkRetainingCapacity(0);
        case_block.wip_capture_scope = wip_captures.scope;
        case_block.inline_case_capture = .none;

        const item = try sema.resolveInst(item_ref);
        if (is_inline) case_block.inline_case_capture = item;
        // `item` is already guaranteed to be constant known.

        const analyze_body = if (union_originally) blk: {
            const item_val = sema.resolveConstValue(block, .unneeded, item, "") catch unreachable;
            const field_ty = maybe_union_ty.unionFieldType(item_val, sema.mod);
            break :blk field_ty.zigTypeTag() != .NoReturn;
        } else true;

        if (err_set and try sema.maybeErrorUnwrap(&case_block, body, operand)) {
            // nothing to do here
        } else if (analyze_body) {
            try sema.analyzeBodyRuntimeBreak(&case_block, body);
        } else {
            _ = try case_block.addNoOp(.unreach);
        }

        try wip_captures.finalize();

        try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
        cases_extra.appendAssumeCapacity(1); // items_len
        cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
        cases_extra.appendAssumeCapacity(@enumToInt(item));
        cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
    }

    var is_first = true;
    var prev_cond_br: Air.Inst.Index = undefined;
    var first_else_body: []const Air.Inst.Index = &.{};
    defer gpa.free(first_else_body);
    var prev_then_body: []const Air.Inst.Index = &.{};
    defer gpa.free(prev_then_body);

    var cases_len = scalar_cases_len;
    var multi_i: u32 = 0;
    while (multi_i < multi_cases_len) : (multi_i += 1) {
        const items_len = sema.code.extra[extra_index];
        extra_index += 1;
        const ranges_len = sema.code.extra[extra_index];
        extra_index += 1;
        const body_len = @truncate(u31, sema.code.extra[extra_index]);
        const is_inline = sema.code.extra[extra_index] >> 31 != 0;
        extra_index += 1;
        const items = sema.code.refSlice(extra_index, items_len);
        extra_index += items_len;

        case_block.instructions.shrinkRetainingCapacity(0);
        case_block.wip_capture_scope = child_block.wip_capture_scope;
        case_block.inline_case_capture = .none;

        // Generate all possible cases as scalar prongs.
        if (is_inline) {
            const body_start = extra_index + 2 * ranges_len;
            const body = sema.code.extra[body_start..][0..body_len];
            var emit_bb = false;

            var range_i: u32 = 0;
            while (range_i < ranges_len) : (range_i += 1) {
                const first_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                extra_index += 1;
                const last_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                extra_index += 1;

                const item_first_ref = try sema.resolveInst(first_ref);
                var item = sema.resolveConstValue(block, .unneeded, item_first_ref, undefined) catch unreachable;
                const item_last_ref = try sema.resolveInst(last_ref);
                const item_last = sema.resolveConstValue(block, .unneeded, item_last_ref, undefined) catch unreachable;

                while (item.compareAll(.lte, item_last, operand_ty, sema.mod)) : ({
                    // Previous validation has resolved any possible lazy values.
                    item = try sema.intAddScalar(item, Value.one);
                }) {
                    cases_len += 1;

                    const item_ref = try sema.addConstant(operand_ty, item);
                    case_block.inline_case_capture = item_ref;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    if (emit_bb) sema.emitBackwardBranch(block, .unneeded) catch |err| switch (err) {
                        error.NeededSourceLocation => {
                            const case_src = Module.SwitchProngSrc{ .range = .{ .prong = multi_i, .item = range_i } };
                            const decl = sema.mod.declPtr(case_block.src_decl);
                            try sema.emitBackwardBranch(block, case_src.resolve(sema.gpa, decl, src_node_offset, .none));
                            unreachable;
                        },
                        else => return err,
                    };
                    emit_bb = true;

                    try sema.analyzeBodyRuntimeBreak(&case_block, body);

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
            }

            for (items, 0..) |item_ref, item_i| {
                cases_len += 1;

                const item = try sema.resolveInst(item_ref);
                case_block.inline_case_capture = item;

                case_block.instructions.shrinkRetainingCapacity(0);
                case_block.wip_capture_scope = child_block.wip_capture_scope;

                const analyze_body = if (union_originally) blk: {
                    const item_val = sema.resolveConstValue(block, .unneeded, item, undefined) catch unreachable;
                    const field_ty = maybe_union_ty.unionFieldType(item_val, sema.mod);
                    break :blk field_ty.zigTypeTag() != .NoReturn;
                } else true;

                if (emit_bb) sema.emitBackwardBranch(block, .unneeded) catch |err| switch (err) {
                    error.NeededSourceLocation => {
                        const case_src = Module.SwitchProngSrc{ .multi = .{ .prong = multi_i, .item = @intCast(u32, item_i) } };
                        const decl = sema.mod.declPtr(case_block.src_decl);
                        try sema.emitBackwardBranch(block, case_src.resolve(sema.gpa, decl, src_node_offset, .none));
                        unreachable;
                    },
                    else => return err,
                };
                emit_bb = true;

                if (analyze_body) {
                    try sema.analyzeBodyRuntimeBreak(&case_block, body);
                } else {
                    _ = try case_block.addNoOp(.unreach);
                }

                try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                cases_extra.appendAssumeCapacity(1); // items_len
                cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
            }

            extra_index += body_len;
            continue;
        }

        var any_ok: Air.Inst.Ref = .none;

        // If there are any ranges, we have to put all the items into the
        // else prong. Otherwise, we can take advantage of multiple items
        // mapping to the same body.
        if (ranges_len == 0) {
            cases_len += 1;

            const analyze_body = if (union_originally)
                for (items) |item_ref| {
                    const item = try sema.resolveInst(item_ref);
                    const item_val = sema.resolveConstValue(block, .unneeded, item, "") catch unreachable;
                    const field_ty = maybe_union_ty.unionFieldType(item_val, sema.mod);
                    if (field_ty.zigTypeTag() != .NoReturn) break true;
                } else false
            else
                true;

            const body = sema.code.extra[extra_index..][0..body_len];
            extra_index += body_len;
            if (err_set and try sema.maybeErrorUnwrap(&case_block, body, operand)) {
                // nothing to do here
            } else if (analyze_body) {
                try sema.analyzeBodyRuntimeBreak(&case_block, body);
            } else {
                _ = try case_block.addNoOp(.unreach);
            }

            try cases_extra.ensureUnusedCapacity(gpa, 2 + items.len +
                case_block.instructions.items.len);

            cases_extra.appendAssumeCapacity(@intCast(u32, items.len));
            cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));

            for (items) |item_ref| {
                const item = try sema.resolveInst(item_ref);
                cases_extra.appendAssumeCapacity(@enumToInt(item));
            }

            cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
        } else {
            for (items) |item_ref| {
                const item = try sema.resolveInst(item_ref);
                const cmp_ok = try case_block.addBinOp(if (case_block.float_mode == .Optimized) .cmp_eq_optimized else .cmp_eq, operand, item);
                if (any_ok != .none) {
                    any_ok = try case_block.addBinOp(.bool_or, any_ok, cmp_ok);
                } else {
                    any_ok = cmp_ok;
                }
            }

            var range_i: usize = 0;
            while (range_i < ranges_len) : (range_i += 1) {
                const first_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                extra_index += 1;
                const last_ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_index]);
                extra_index += 1;

                const item_first = try sema.resolveInst(first_ref);
                const item_last = try sema.resolveInst(last_ref);

                // operand >= first and operand <= last
                const range_first_ok = try case_block.addBinOp(
                    if (case_block.float_mode == .Optimized) .cmp_gte_optimized else .cmp_gte,
                    operand,
                    item_first,
                );
                const range_last_ok = try case_block.addBinOp(
                    if (case_block.float_mode == .Optimized) .cmp_lte_optimized else .cmp_lte,
                    operand,
                    item_last,
                );
                const range_ok = try case_block.addBinOp(
                    .bool_and,
                    range_first_ok,
                    range_last_ok,
                );
                if (any_ok != .none) {
                    any_ok = try case_block.addBinOp(.bool_or, any_ok, range_ok);
                } else {
                    any_ok = range_ok;
                }
            }

            const new_cond_br = try case_block.addInstAsIndex(.{ .tag = .cond_br, .data = .{
                .pl_op = .{
                    .operand = any_ok,
                    .payload = undefined,
                },
            } });
            var cond_body = try case_block.instructions.toOwnedSlice(gpa);
            defer gpa.free(cond_body);

            var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, child_block.wip_capture_scope);
            defer wip_captures.deinit();

            case_block.instructions.shrinkRetainingCapacity(0);
            case_block.wip_capture_scope = wip_captures.scope;

            const body = sema.code.extra[extra_index..][0..body_len];
            extra_index += body_len;
            if (err_set and try sema.maybeErrorUnwrap(&case_block, body, operand)) {
                // nothing to do here
            } else {
                try sema.analyzeBodyRuntimeBreak(&case_block, body);
            }

            try wip_captures.finalize();

            if (is_first) {
                is_first = false;
                first_else_body = cond_body;
                cond_body = &.{};
            } else {
                try sema.air_extra.ensureUnusedCapacity(
                    gpa,
                    @typeInfo(Air.CondBr).Struct.fields.len + prev_then_body.len + cond_body.len,
                );

                sema.air_instructions.items(.data)[prev_cond_br].pl_op.payload =
                    sema.addExtraAssumeCapacity(Air.CondBr{
                    .then_body_len = @intCast(u32, prev_then_body.len),
                    .else_body_len = @intCast(u32, cond_body.len),
                });
                sema.air_extra.appendSliceAssumeCapacity(prev_then_body);
                sema.air_extra.appendSliceAssumeCapacity(cond_body);
            }
            gpa.free(prev_then_body);
            prev_then_body = try case_block.instructions.toOwnedSlice(gpa);
            prev_cond_br = new_cond_br;
        }
    }

    var final_else_body: []const Air.Inst.Index = &.{};
    if (special.body.len != 0 or !is_first or case_block.wantSafety()) {
        var emit_bb = false;
        if (special.is_inline) switch (operand_ty.zigTypeTag()) {
            .Enum => {
                if (operand_ty.isNonexhaustiveEnum() and !union_originally) {
                    return sema.fail(block, special_prong_src, "cannot enumerate values of type '{}' for 'inline else'", .{
                        operand_ty.fmt(sema.mod),
                    });
                }
                for (seen_enum_fields, 0..) |f, i| {
                    if (f != null) continue;
                    cases_len += 1;

                    const item_val = try Value.Tag.enum_field_index.create(sema.arena, @intCast(u32, i));
                    const item_ref = try sema.addConstant(operand_ty, item_val);
                    case_block.inline_case_capture = item_ref;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    const analyze_body = if (union_originally) blk: {
                        const field_ty = maybe_union_ty.unionFieldType(item_val, sema.mod);
                        break :blk field_ty.zigTypeTag() != .NoReturn;
                    } else true;

                    if (emit_bb) try sema.emitBackwardBranch(block, special_prong_src);
                    emit_bb = true;

                    if (analyze_body) {
                        try sema.analyzeBodyRuntimeBreak(&case_block, special.body);
                    } else {
                        _ = try case_block.addNoOp(.unreach);
                    }

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
            },
            .ErrorSet => {
                if (operand_ty.isAnyError()) {
                    return sema.fail(block, special_prong_src, "cannot enumerate values of type '{}' for 'inline else'", .{
                        operand_ty.fmt(sema.mod),
                    });
                }
                for (operand_ty.errorSetNames()) |error_name| {
                    if (seen_errors.contains(error_name)) continue;
                    cases_len += 1;

                    const item_val = try Value.Tag.@"error".create(sema.arena, .{ .name = error_name });
                    const item_ref = try sema.addConstant(operand_ty, item_val);
                    case_block.inline_case_capture = item_ref;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    if (emit_bb) try sema.emitBackwardBranch(block, special_prong_src);
                    emit_bb = true;

                    try sema.analyzeBodyRuntimeBreak(&case_block, special.body);

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
            },
            .Int => {
                var it = try RangeSetUnhandledIterator.init(sema, operand_ty, range_set);
                while (try it.next()) |cur| {
                    cases_len += 1;

                    const item_ref = try sema.addConstant(operand_ty, cur);
                    case_block.inline_case_capture = item_ref;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    if (emit_bb) try sema.emitBackwardBranch(block, special_prong_src);
                    emit_bb = true;

                    try sema.analyzeBodyRuntimeBreak(&case_block, special.body);

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
            },
            .Bool => {
                if (true_count == 0) {
                    cases_len += 1;
                    case_block.inline_case_capture = Air.Inst.Ref.bool_true;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    if (emit_bb) try sema.emitBackwardBranch(block, special_prong_src);
                    emit_bb = true;

                    try sema.analyzeBodyRuntimeBreak(&case_block, special.body);

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
                if (false_count == 0) {
                    cases_len += 1;
                    case_block.inline_case_capture = Air.Inst.Ref.bool_false;

                    case_block.instructions.shrinkRetainingCapacity(0);
                    case_block.wip_capture_scope = child_block.wip_capture_scope;

                    if (emit_bb) try sema.emitBackwardBranch(block, special_prong_src);
                    emit_bb = true;

                    try sema.analyzeBodyRuntimeBreak(&case_block, special.body);

                    try cases_extra.ensureUnusedCapacity(gpa, 3 + case_block.instructions.items.len);
                    cases_extra.appendAssumeCapacity(1); // items_len
                    cases_extra.appendAssumeCapacity(@intCast(u32, case_block.instructions.items.len));
                    cases_extra.appendAssumeCapacity(@enumToInt(case_block.inline_case_capture));
                    cases_extra.appendSliceAssumeCapacity(case_block.instructions.items);
                }
            },
            else => return sema.fail(block, special_prong_src, "cannot enumerate values of type '{}' for 'inline else'", .{
                operand_ty.fmt(sema.mod),
            }),
        };

        var wip_captures = try WipCaptureScope.init(gpa, sema.perm_arena, child_block.wip_capture_scope);
        defer wip_captures.deinit();

        case_block.instructions.shrinkRetainingCapacity(0);
        case_block.wip_capture_scope = wip_captures.scope;
        case_block.inline_case_capture = .none;

        if (sema.mod.backendSupportsFeature(.is_named_enum_value) and special.body.len != 0 and block.wantSafety() and
            operand_ty.zigTypeTag() == .Enum and (!operand_ty.isNonexhaustiveEnum() or union_originally))
        {
            try sema.zirDbgStmt(&case_block, cond_dbg_node_index);
            const ok = try case_block.addUnOp(.is_named_enum_value, operand);
            try sema.addSafetyCheck(&case_block, ok, .corrupt_switch);
        }

        const analyze_body = if (union_originally and !special.is_inline)
            for (seen_enum_fields, 0..) |seen_field, index| {
                if (seen_field != null) continue;
                const union_obj = maybe_union_ty.cast(Type.Payload.Union).?.data;
                const field_ty = union_obj.fields.values()[index].ty;
                if (field_ty.zigTypeTag() != .NoReturn) break true;
            } else false
        else
            true;
        if (special.body.len != 0 and err_set and
            try sema.maybeErrorUnwrap(&case_block, special.body, operand))
        {
            // nothing to do here
        } else if (special.body.len != 0 and analyze_body and !special.is_inline) {
            try sema.analyzeBodyRuntimeBreak(&case_block, special.body);
        } else {
            // We still need a terminator in this block, but we have proven
            // that it is unreachable.
            if (case_block.wantSafety()) {
                try sema.zirDbgStmt(&case_block, cond_dbg_node_index);
                try sema.safetyPanic(&case_block, .corrupt_switch);
            } else {
                _ = try case_block.addNoOp(.unreach);
            }
        }

        try wip_captures.finalize();

        if (is_first) {
            final_else_body = case_block.instructions.items;
        } else {
            try sema.air_extra.ensureUnusedCapacity(gpa, prev_then_body.len +
                @typeInfo(Air.CondBr).Struct.fields.len + case_block.instructions.items.len);

            sema.air_instructions.items(.data)[prev_cond_br].pl_op.payload =
                sema.addExtraAssumeCapacity(Air.CondBr{
                .then_body_len = @intCast(u32, prev_then_body.len),
                .else_body_len = @intCast(u32, case_block.instructions.items.len),
            });
            sema.air_extra.appendSliceAssumeCapacity(prev_then_body);
            sema.air_extra.appendSliceAssumeCapacity(case_block.instructions.items);
            final_else_body = first_else_body;
        }
    }

    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.SwitchBr).Struct.fields.len +
        cases_extra.items.len + final_else_body.len);

    _ = try child_block.addInst(.{ .tag = .switch_br, .data = .{ .pl_op = .{
        .operand = operand,
        .payload = sema.addExtraAssumeCapacity(Air.SwitchBr{
            .cases_len = @intCast(u32, cases_len),
            .else_body_len = @intCast(u32, final_else_body.len),
        }),
    } } });
    sema.air_extra.appendSliceAssumeCapacity(cases_extra.items);
    sema.air_extra.appendSliceAssumeCapacity(final_else_body);

    return sema.analyzeBlockBody(block, src, &child_block, merges);
}

const RangeSetUnhandledIterator = struct {
    sema: *Sema,
    ty: Type,
    cur: Value,
    max: Value,
    ranges: []const RangeSet.Range,
    range_i: usize = 0,
    first: bool = true,

    fn init(sema: *Sema, ty: Type, range_set: RangeSet) !RangeSetUnhandledIterator {
        const target = sema.mod.getTarget();
        const min = try ty.minInt(sema.arena, target);
        const max = try ty.maxInt(sema.arena, target);

        return RangeSetUnhandledIterator{
            .sema = sema,
            .ty = ty,
            .cur = min,
            .max = max,
            .ranges = range_set.ranges.items,
        };
    }

    fn next(it: *RangeSetUnhandledIterator) !?Value {
        while (it.range_i < it.ranges.len) : (it.range_i += 1) {
            if (!it.first) {
                it.cur = try it.sema.intAdd(it.cur, Value.one, it.ty);
            }
            it.first = false;
            if (it.cur.compareAll(.lt, it.ranges[it.range_i].first, it.ty, it.sema.mod)) {
                return it.cur;
            }
            it.cur = it.ranges[it.range_i].last;
        }
        if (!it.first) {
            it.cur = try it.sema.intAdd(it.cur, Value.one, it.ty);
        }
        it.first = false;
        if (it.cur.compareAll(.lte, it.max, it.ty, it.sema.mod)) {
            return it.cur;
        }
        return null;
    }
};

fn resolveSwitchItemVal(
    sema: *Sema,
    block: *Block,
    item_ref: Zir.Inst.Ref,
    switch_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
    range_expand: Module.SwitchProngSrc.RangeExpand,
) CompileError!TypedValue {
    const item = try sema.resolveInst(item_ref);
    const item_ty = sema.typeOf(item);
    // Constructing a LazySrcLoc is costly because we only have the switch AST node.
    // Only if we know for sure we need to report a compile error do we resolve the
    // full source locations.
    if (sema.resolveConstValue(block, .unneeded, item, "")) |val| {
        try sema.resolveLazyValue(val);
        return TypedValue{ .ty = item_ty, .val = val };
    } else |err| switch (err) {
        error.NeededSourceLocation => {
            const src = switch_prong_src.resolve(sema.gpa, sema.mod.declPtr(block.src_decl), switch_node_offset, range_expand);
            _ = try sema.resolveConstValue(block, src, item, "switch prong values must be comptime-known");
            unreachable;
        },
        else => |e| return e,
    }
}

fn validateSwitchRange(
    sema: *Sema,
    block: *Block,
    range_set: *RangeSet,
    first_ref: Zir.Inst.Ref,
    last_ref: Zir.Inst.Ref,
    operand_ty: Type,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const first_val = (try sema.resolveSwitchItemVal(block, first_ref, src_node_offset, switch_prong_src, .first)).val;
    const last_val = (try sema.resolveSwitchItemVal(block, last_ref, src_node_offset, switch_prong_src, .last)).val;
    if (first_val.compareAll(.gt, last_val, operand_ty, sema.mod)) {
        const src = switch_prong_src.resolve(sema.gpa, sema.mod.declPtr(block.src_decl), src_node_offset, .first);
        return sema.fail(block, src, "range start value is greater than the end value", .{});
    }
    const maybe_prev_src = try range_set.add(first_val, last_val, operand_ty, switch_prong_src);
    return sema.validateSwitchDupe(block, maybe_prev_src, switch_prong_src, src_node_offset);
}

fn validateSwitchItem(
    sema: *Sema,
    block: *Block,
    range_set: *RangeSet,
    item_ref: Zir.Inst.Ref,
    operand_ty: Type,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const item_val = (try sema.resolveSwitchItemVal(block, item_ref, src_node_offset, switch_prong_src, .none)).val;
    const maybe_prev_src = try range_set.add(item_val, item_val, operand_ty, switch_prong_src);
    return sema.validateSwitchDupe(block, maybe_prev_src, switch_prong_src, src_node_offset);
}

fn validateSwitchItemEnum(
    sema: *Sema,
    block: *Block,
    seen_fields: []?Module.SwitchProngSrc,
    range_set: *RangeSet,
    item_ref: Zir.Inst.Ref,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const item_tv = try sema.resolveSwitchItemVal(block, item_ref, src_node_offset, switch_prong_src, .none);
    const field_index = item_tv.ty.enumTagFieldIndex(item_tv.val, sema.mod) orelse {
        const maybe_prev_src = try range_set.add(item_tv.val, item_tv.val, item_tv.ty, switch_prong_src);
        return sema.validateSwitchDupe(block, maybe_prev_src, switch_prong_src, src_node_offset);
    };
    const maybe_prev_src = seen_fields[field_index];
    seen_fields[field_index] = switch_prong_src;
    return sema.validateSwitchDupe(block, maybe_prev_src, switch_prong_src, src_node_offset);
}

fn validateSwitchItemError(
    sema: *Sema,
    block: *Block,
    seen_errors: *SwitchErrorSet,
    item_ref: Zir.Inst.Ref,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const item_tv = try sema.resolveSwitchItemVal(block, item_ref, src_node_offset, switch_prong_src, .none);
    // TODO: Do i need to typecheck here?
    const error_name = item_tv.val.castTag(.@"error").?.data.name;
    const maybe_prev_src = if (try seen_errors.fetchPut(error_name, switch_prong_src)) |prev|
        prev.value
    else
        null;
    return sema.validateSwitchDupe(block, maybe_prev_src, switch_prong_src, src_node_offset);
}

fn validateSwitchDupe(
    sema: *Sema,
    block: *Block,
    maybe_prev_src: ?Module.SwitchProngSrc,
    switch_prong_src: Module.SwitchProngSrc,
    src_node_offset: i32,
) CompileError!void {
    const prev_prong_src = maybe_prev_src orelse return;
    const gpa = sema.gpa;
    const block_src_decl = sema.mod.declPtr(block.src_decl);
    const src = switch_prong_src.resolve(gpa, block_src_decl, src_node_offset, .none);
    const prev_src = prev_prong_src.resolve(gpa, block_src_decl, src_node_offset, .none);
    const msg = msg: {
        const msg = try sema.errMsg(
            block,
            src,
            "duplicate switch value",
            .{},
        );
        errdefer msg.destroy(sema.gpa);
        try sema.errNote(
            block,
            prev_src,
            msg,
            "previous value here",
            .{},
        );
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn validateSwitchItemBool(
    sema: *Sema,
    block: *Block,
    true_count: *u8,
    false_count: *u8,
    item_ref: Zir.Inst.Ref,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const item_val = (try sema.resolveSwitchItemVal(block, item_ref, src_node_offset, switch_prong_src, .none)).val;
    if (item_val.toBool()) {
        true_count.* += 1;
    } else {
        false_count.* += 1;
    }
    if (true_count.* + false_count.* > 2) {
        const block_src_decl = sema.mod.declPtr(block.src_decl);
        const src = switch_prong_src.resolve(sema.gpa, block_src_decl, src_node_offset, .none);
        return sema.fail(block, src, "duplicate switch value", .{});
    }
}

const ValueSrcMap = std.HashMap(Value, Module.SwitchProngSrc, Value.HashContext, std.hash_map.default_max_load_percentage);

fn validateSwitchItemSparse(
    sema: *Sema,
    block: *Block,
    seen_values: *ValueSrcMap,
    item_ref: Zir.Inst.Ref,
    src_node_offset: i32,
    switch_prong_src: Module.SwitchProngSrc,
) CompileError!void {
    const item_val = (try sema.resolveSwitchItemVal(block, item_ref, src_node_offset, switch_prong_src, .none)).val;
    const kv = (try seen_values.fetchPut(item_val, switch_prong_src)) orelse return;
    return sema.validateSwitchDupe(block, kv.value, switch_prong_src, src_node_offset);
}

fn validateSwitchNoRange(
    sema: *Sema,
    block: *Block,
    ranges_len: u32,
    operand_ty: Type,
    src_node_offset: i32,
) CompileError!void {
    if (ranges_len == 0)
        return;

    const operand_src: LazySrcLoc = .{ .node_offset_switch_operand = src_node_offset };
    const range_src: LazySrcLoc = .{ .node_offset_switch_range = src_node_offset };

    const msg = msg: {
        const msg = try sema.errMsg(
            block,
            operand_src,
            "ranges not allowed when switching on type '{}'",
            .{operand_ty.fmt(sema.mod)},
        );
        errdefer msg.destroy(sema.gpa);
        try sema.errNote(
            block,
            range_src,
            msg,
            "range here",
            .{},
        );
        break :msg msg;
    };
    return sema.failWithOwnedErrorMsg(msg);
}

fn maybeErrorUnwrap(sema: *Sema, block: *Block, body: []const Zir.Inst.Index, operand: Air.Inst.Ref) !bool {
    if (!sema.mod.backendSupportsFeature(.panic_unwrap_error)) return false;

    const tags = sema.code.instructions.items(.tag);
    for (body) |inst| {
        switch (tags[inst]) {
            .save_err_ret_index,
            .dbg_block_begin,
            .dbg_block_end,
            .dbg_stmt,
            .@"unreachable",
            .str,
            .as_node,
            .panic,
            .field_val,
            => {},
            else => return false,
        }
    }

    for (body) |inst| {
        const air_inst = switch (tags[inst]) {
            .dbg_block_begin,
            .dbg_block_end,
            => continue,
            .dbg_stmt => {
                try sema.zirDbgStmt(block, inst);
                continue;
            },
            .save_err_ret_index => {
                try sema.zirSaveErrRetIndex(block, inst);
                continue;
            },
            .str => try sema.zirStr(block, inst),
            .as_node => try sema.zirAsNode(block, inst),
            .field_val => try sema.zirFieldVal(block, inst),
            .@"unreachable" => {
                const inst_data = sema.code.instructions.items(.data)[inst].@"unreachable";
                const src = inst_data.src();

                if (!sema.mod.comp.formatted_panics) {
                    try sema.safetyPanic(block, .unwrap_error);
                    return true;
                }

                const panic_fn = try sema.getBuiltin("panicUnwrapError");
                const err_return_trace = try sema.getErrorReturnTrace(block);
                const args: [2]Air.Inst.Ref = .{ err_return_trace, operand };
                _ = try sema.analyzeCall(block, panic_fn, src, src, .auto, false, &args, null);
                return true;
            },
            .panic => {
                const inst_data = sema.code.instructions.items(.data)[inst].un_node;
                const src = inst_data.src();
                const msg_inst = try sema.resolveInst(inst_data.operand);

                const panic_fn = try sema.getBuiltin("panic");
                const err_return_trace = try sema.getErrorReturnTrace(block);
                const args: [3]Air.Inst.Ref = .{ msg_inst, err_return_trace, .null_value };
                _ = try sema.analyzeCall(block, panic_fn, src, src, .auto, false, &args, null);
                return true;
            },
            else => unreachable,
        };
        if (sema.typeOf(air_inst).isNoReturn())
            return true;
        sema.inst_map.putAssumeCapacity(inst, air_inst);
    }
    unreachable;
}

fn maybeErrorUnwrapCondbr(sema: *Sema, block: *Block, body: []const Zir.Inst.Index, cond: Zir.Inst.Ref, cond_src: LazySrcLoc) !void {
    const index = Zir.refToIndex(cond) orelse return;
    if (sema.code.instructions.items(.tag)[index] != .is_non_err) return;

    const err_inst_data = sema.code.instructions.items(.data)[index].un_node;
    const err_operand = try sema.resolveInst(err_inst_data.operand);
    const operand_ty = sema.typeOf(err_operand);
    if (operand_ty.zigTypeTag() == .ErrorSet) {
        try sema.maybeErrorUnwrapComptime(block, body, err_operand);
        return;
    }
    if (try sema.resolveDefinedValue(block, cond_src, err_operand)) |val| {
        if (!operand_ty.isError()) return;
        if (val.getError() == null) return;
        try sema.maybeErrorUnwrapComptime(block, body, err_operand);
    }
}

fn maybeErrorUnwrapComptime(sema: *Sema, block: *Block, body: []const Zir.Inst.Index, operand: Air.Inst.Ref) !void {
    const tags = sema.code.instructions.items(.tag);
    const inst = for (body) |inst| {
        switch (tags[inst]) {
            .dbg_block_begin,
            .dbg_block_end,
            .dbg_stmt,
            .save_err_ret_index,
            => {},
            .@"unreachable" => break inst,
            else => return,
        }
    } else return;
    const inst_data = sema.code.instructions.items(.data)[inst].@"unreachable";
    const src = inst_data.src();

    if (try sema.resolveDefinedValue(block, src, operand)) |val| {
        if (val.getError()) |name| {
            return sema.fail(block, src, "caught unexpected error '{s}'", .{name});
        }
    }
}

fn zirHasField(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const name_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const unresolved_ty = try sema.resolveType(block, ty_src, extra.lhs);
    const field_name = try sema.resolveConstString(block, name_src, extra.rhs, "field name must be comptime-known");
    const ty = try sema.resolveTypeFields(unresolved_ty);

    const has_field = hf: {
        if (ty.isSlice()) {
            if (mem.eql(u8, field_name, "ptr")) break :hf true;
            if (mem.eql(u8, field_name, "len")) break :hf true;
            break :hf false;
        }
        if (ty.castTag(.anon_struct)) |pl| {
            break :hf for (pl.data.names) |name| {
                if (mem.eql(u8, name, field_name)) break true;
            } else false;
        }
        if (ty.isTuple()) {
            const field_index = std.fmt.parseUnsigned(u32, field_name, 10) catch break :hf false;
            break :hf field_index < ty.structFieldCount();
        }
        break :hf switch (ty.zigTypeTag()) {
            .Struct => ty.structFields().contains(field_name),
            .Union => ty.unionFields().contains(field_name),
            .Enum => ty.enumFields().contains(field_name),
            .Array => mem.eql(u8, field_name, "len"),
            else => return sema.fail(block, ty_src, "type '{}' does not support '@hasField'", .{
                ty.fmt(sema.mod),
            }),
        };
    };
    if (has_field) {
        return Air.Inst.Ref.bool_true;
    } else {
        return Air.Inst.Ref.bool_false;
    }
}

fn zirHasDecl(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const src = inst_data.src();
    const lhs_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const container_type = try sema.resolveType(block, lhs_src, extra.lhs);
    const decl_name = try sema.resolveConstString(block, rhs_src, extra.rhs, "decl name must be comptime-known");

    try sema.checkNamespaceType(block, lhs_src, container_type);

    const namespace = container_type.getNamespace() orelse return Air.Inst.Ref.bool_false;
    if (try sema.lookupInNamespace(block, src, namespace, decl_name, true)) |decl_index| {
        const decl = sema.mod.declPtr(decl_index);
        if (decl.is_pub or decl.getFileScope() == block.getFileScope()) {
            return Air.Inst.Ref.bool_true;
        }
    }
    return Air.Inst.Ref.bool_false;
}

fn zirImport(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const mod = sema.mod;
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const operand_src = inst_data.src();
    const operand = inst_data.get(sema.code);

    const result = mod.importFile(block.getFileScope(), operand) catch |err| switch (err) {
        error.ImportOutsidePkgPath => {
            return sema.fail(block, operand_src, "import of file outside package path: '{s}'", .{operand});
        },
        error.PackageNotFound => {
            const name = try block.getFileScope().pkg.getName(sema.gpa, mod.*);
            defer sema.gpa.free(name);
            return sema.fail(block, operand_src, "no package named '{s}' available within package '{s}'", .{ operand, name });
        },
        else => {
            // TODO: these errors are file system errors; make sure an update() will
            // retry this and not cache the file system error, which may be transient.
            return sema.fail(block, operand_src, "unable to open '{s}': {s}", .{ operand, @errorName(err) });
        },
    };
    try mod.semaFile(result.file);
    const file_root_decl_index = result.file.root_decl.unwrap().?;
    const file_root_decl = mod.declPtr(file_root_decl_index);
    try mod.declareDeclDependency(sema.owner_decl_index, file_root_decl_index);
    return sema.addConstant(file_root_decl.ty, file_root_decl.val);
}

fn zirEmbedFile(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const mod = sema.mod;
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const name = try sema.resolveConstString(block, operand_src, inst_data.operand, "file path name must be comptime-known");

    const embed_file = mod.embedFile(block.getFileScope(), name) catch |err| switch (err) {
        error.ImportOutsidePkgPath => {
            return sema.fail(block, operand_src, "embed of file outside package path: '{s}'", .{name});
        },
        else => {
            // TODO: these errors are file system errors; make sure an update() will
            // retry this and not cache the file system error, which may be transient.
            return sema.fail(block, operand_src, "unable to open '{s}': {s}", .{ name, @errorName(err) });
        },
    };

    var anon_decl = try block.startAnonDecl();
    defer anon_decl.deinit();

    const bytes_including_null = embed_file.bytes[0 .. embed_file.bytes.len + 1];

    // TODO instead of using `Value.Tag.bytes`, create a new value tag for pointing at
    // a `*Module.EmbedFile`. The purpose of this would be:
    // - If only the length is read and the bytes are not inspected by comptime code,
    //   there can be an optimization where the codegen backend does a copy_file_range
    //   into the final binary, and never loads the data into memory.
    // - When a Decl is destroyed, it can free the `*Module.EmbedFile`.
    embed_file.owner_decl = try anon_decl.finish(
        try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), embed_file.bytes.len),
        try Value.Tag.bytes.create(anon_decl.arena(), bytes_including_null),
        0, // default alignment
    );

    return sema.analyzeDeclRef(embed_file.owner_decl);
}

fn zirRetErrValueCode(sema: *Sema, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const err_name = inst_data.get(sema.code);

    // Return the error code from the function.
    const kv = try sema.mod.getErrorValue(err_name);
    const result_inst = try sema.addConstant(
        try Type.Tag.error_set_single.create(sema.arena, kv.key),
        try Value.Tag.@"error".create(sema.arena, .{ .name = kv.key }),
    );
    return result_inst;
}

fn zirShl(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const target = sema.mod.getTarget();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);

    const scalar_ty = lhs_ty.scalarType();
    const scalar_rhs_ty = rhs_ty.scalarType();

    // TODO coerce rhs if air_tag is not shl_sat
    const rhs_is_comptime_int = try sema.checkIntType(block, rhs_src, scalar_rhs_ty);

    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(rhs);

    if (maybe_rhs_val) |rhs_val| {
        if (rhs_val.isUndef()) {
            return sema.addConstUndef(sema.typeOf(lhs));
        }
        // If rhs is 0, return lhs without doing any calculations.
        if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
            return lhs;
        }
        if (scalar_ty.zigTypeTag() != .ComptimeInt and air_tag != .shl_sat) {
            var bits_payload = Value.Payload.U64{
                .base = .{ .tag = .int_u64 },
                .data = scalar_ty.intInfo(target).bits,
            };
            const bit_value = Value.initPayload(&bits_payload.base);
            if (rhs_ty.zigTypeTag() == .Vector) {
                var i: usize = 0;
                while (i < rhs_ty.vectorLen()) : (i += 1) {
                    var elem_value_buf: Value.ElemValueBuffer = undefined;
                    const rhs_elem = rhs_val.elemValueBuffer(sema.mod, i, &elem_value_buf);
                    if (rhs_elem.compareHetero(.gte, bit_value, target)) {
                        return sema.fail(block, rhs_src, "shift amount '{}' at index '{d}' is too large for operand type '{}'", .{
                            rhs_elem.fmtValue(scalar_ty, sema.mod),
                            i,
                            scalar_ty.fmt(sema.mod),
                        });
                    }
                }
            } else if (rhs_val.compareHetero(.gte, bit_value, target)) {
                return sema.fail(block, rhs_src, "shift amount '{}' is too large for operand type '{}'", .{
                    rhs_val.fmtValue(scalar_ty, sema.mod),
                    scalar_ty.fmt(sema.mod),
                });
            }
        }
        if (rhs_ty.zigTypeTag() == .Vector) {
            var i: usize = 0;
            while (i < rhs_ty.vectorLen()) : (i += 1) {
                var elem_value_buf: Value.ElemValueBuffer = undefined;
                const rhs_elem = rhs_val.elemValueBuffer(sema.mod, i, &elem_value_buf);
                if (rhs_elem.compareHetero(.lt, Value.zero, target)) {
                    return sema.fail(block, rhs_src, "shift by negative amount '{}' at index '{d}'", .{
                        rhs_elem.fmtValue(scalar_ty, sema.mod),
                        i,
                    });
                }
            }
        } else if (rhs_val.compareHetero(.lt, Value.zero, target)) {
            return sema.fail(block, rhs_src, "shift by negative amount '{}'", .{
                rhs_val.fmtValue(scalar_ty, sema.mod),
            });
        }
    }

    const runtime_src = if (maybe_lhs_val) |lhs_val| rs: {
        if (lhs_val.isUndef()) return sema.addConstUndef(lhs_ty);
        const rhs_val = maybe_rhs_val orelse {
            if (scalar_ty.zigTypeTag() == .ComptimeInt) {
                return sema.fail(block, src, "LHS of shift must be a fixed-width integer type, or RHS must be comptime-known", .{});
            }
            break :rs rhs_src;
        };

        const val = switch (air_tag) {
            .shl_exact => val: {
                const shifted = try lhs_val.shlWithOverflow(rhs_val, lhs_ty, sema.arena, sema.mod);
                if (scalar_ty.zigTypeTag() == .ComptimeInt) {
                    break :val shifted.wrapped_result;
                }
                if (shifted.overflow_bit.compareAllWithZero(.eq, sema.mod)) {
                    break :val shifted.wrapped_result;
                }
                return sema.fail(block, src, "operation caused overflow", .{});
            },

            .shl_sat => if (scalar_ty.zigTypeTag() == .ComptimeInt)
                try lhs_val.shl(rhs_val, lhs_ty, sema.arena, sema.mod)
            else
                try lhs_val.shlSat(rhs_val, lhs_ty, sema.arena, sema.mod),

            .shl => if (scalar_ty.zigTypeTag() == .ComptimeInt)
                try lhs_val.shl(rhs_val, lhs_ty, sema.arena, sema.mod)
            else
                try lhs_val.shlTrunc(rhs_val, lhs_ty, sema.arena, sema.mod),

            else => unreachable,
        };

        return sema.addConstant(lhs_ty, val);
    } else lhs_src;

    const new_rhs = if (air_tag == .shl_sat) rhs: {
        // Limit the RHS type for saturating shl to be an integer as small as the LHS.
        if (rhs_is_comptime_int or
            scalar_rhs_ty.intInfo(target).bits > scalar_ty.intInfo(target).bits)
        {
            const max_int = try sema.addConstant(
                lhs_ty,
                try lhs_ty.maxInt(sema.arena, target),
            );
            const rhs_limited = try sema.analyzeMinMax(block, rhs_src, rhs, max_int, .min, rhs_src, rhs_src);
            break :rhs try sema.intCast(block, src, lhs_ty, rhs_src, rhs_limited, rhs_src, false);
        } else {
            break :rhs rhs;
        }
    } else rhs;

    try sema.requireRuntimeBlock(block, src, runtime_src);
    if (block.wantSafety()) {
        const bit_count = scalar_ty.intInfo(target).bits;
        if (!std.math.isPowerOfTwo(bit_count)) {
            const bit_count_val = try Value.Tag.int_u64.create(sema.arena, bit_count);

            const ok = if (rhs_ty.zigTypeTag() == .Vector) ok: {
                const bit_count_inst = try sema.addConstant(rhs_ty, try Value.Tag.repeated.create(sema.arena, bit_count_val));
                const lt = try block.addCmpVector(rhs, bit_count_inst, .lt);
                break :ok try block.addInst(.{
                    .tag = .reduce,
                    .data = .{ .reduce = .{
                        .operand = lt,
                        .operation = .And,
                    } },
                });
            } else ok: {
                const bit_count_inst = try sema.addConstant(rhs_ty, bit_count_val);
                break :ok try block.addBinOp(.cmp_lt, rhs, bit_count_inst);
            };
            try sema.addSafetyCheck(block, ok, .shift_rhs_too_big);
        }

        if (air_tag == .shl_exact) {
            const op_ov_tuple_ty = try sema.overflowArithmeticTupleType(lhs_ty);
            const op_ov = try block.addInst(.{
                .tag = .shl_with_overflow,
                .data = .{ .ty_pl = .{
                    .ty = try sema.addType(op_ov_tuple_ty),
                    .payload = try sema.addExtra(Air.Bin{
                        .lhs = lhs,
                        .rhs = rhs,
                    }),
                } },
            });
            const ov_bit = try sema.tupleFieldValByIndex(block, src, op_ov, 1, op_ov_tuple_ty);
            const any_ov_bit = if (lhs_ty.zigTypeTag() == .Vector)
                try block.addInst(.{
                    .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                    .data = .{ .reduce = .{
                        .operand = ov_bit,
                        .operation = .Or,
                    } },
                })
            else
                ov_bit;
            const zero_ov = try sema.addConstant(Type.u1, Value.zero);
            const no_ov = try block.addBinOp(.cmp_eq, any_ov_bit, zero_ov);

            try sema.addSafetyCheck(block, no_ov, .shl_overflow);
            return sema.tupleFieldValByIndex(block, src, op_ov, 0, op_ov_tuple_ty);
        }
    }
    return block.addBinOp(air_tag, lhs, new_rhs);
}

fn zirShr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    const target = sema.mod.getTarget();
    const scalar_ty = lhs_ty.scalarType();

    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(rhs);

    const runtime_src = if (maybe_rhs_val) |rhs_val| rs: {
        if (rhs_val.isUndef()) {
            return sema.addConstUndef(lhs_ty);
        }
        // If rhs is 0, return lhs without doing any calculations.
        if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
            return lhs;
        }
        if (scalar_ty.zigTypeTag() != .ComptimeInt) {
            var bits_payload = Value.Payload.U64{
                .base = .{ .tag = .int_u64 },
                .data = scalar_ty.intInfo(target).bits,
            };
            const bit_value = Value.initPayload(&bits_payload.base);
            if (rhs_ty.zigTypeTag() == .Vector) {
                var i: usize = 0;
                while (i < rhs_ty.vectorLen()) : (i += 1) {
                    var elem_value_buf: Value.ElemValueBuffer = undefined;
                    const rhs_elem = rhs_val.elemValueBuffer(sema.mod, i, &elem_value_buf);
                    if (rhs_elem.compareHetero(.gte, bit_value, target)) {
                        return sema.fail(block, rhs_src, "shift amount '{}' at index '{d}' is too large for operand type '{}'", .{
                            rhs_elem.fmtValue(scalar_ty, sema.mod),
                            i,
                            scalar_ty.fmt(sema.mod),
                        });
                    }
                }
            } else if (rhs_val.compareHetero(.gte, bit_value, target)) {
                return sema.fail(block, rhs_src, "shift amount '{}' is too large for operand type '{}'", .{
                    rhs_val.fmtValue(scalar_ty, sema.mod),
                    scalar_ty.fmt(sema.mod),
                });
            }
        }
        if (rhs_ty.zigTypeTag() == .Vector) {
            var i: usize = 0;
            while (i < rhs_ty.vectorLen()) : (i += 1) {
                var elem_value_buf: Value.ElemValueBuffer = undefined;
                const rhs_elem = rhs_val.elemValueBuffer(sema.mod, i, &elem_value_buf);
                if (rhs_elem.compareHetero(.lt, Value.zero, target)) {
                    return sema.fail(block, rhs_src, "shift by negative amount '{}' at index '{d}'", .{
                        rhs_elem.fmtValue(scalar_ty, sema.mod),
                        i,
                    });
                }
            }
        } else if (rhs_val.compareHetero(.lt, Value.zero, target)) {
            return sema.fail(block, rhs_src, "shift by negative amount '{}'", .{
                rhs_val.fmtValue(scalar_ty, sema.mod),
            });
        }
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                return sema.addConstUndef(lhs_ty);
            }
            if (air_tag == .shr_exact) {
                // Detect if any ones would be shifted out.
                const truncated = try lhs_val.intTruncBitsAsValue(lhs_ty, sema.arena, .unsigned, rhs_val, sema.mod);
                if (!(try truncated.compareAllWithZeroAdvanced(.eq, sema))) {
                    return sema.fail(block, src, "exact shift shifted out 1 bits", .{});
                }
            }
            const val = try lhs_val.shr(rhs_val, lhs_ty, sema.arena, sema.mod);
            return sema.addConstant(lhs_ty, val);
        } else {
            break :rs lhs_src;
        }
    } else rhs_src;

    if (maybe_rhs_val == null and scalar_ty.zigTypeTag() == .ComptimeInt) {
        return sema.fail(block, src, "LHS of shift must be a fixed-width integer type, or RHS must be comptime-known", .{});
    }

    try sema.requireRuntimeBlock(block, src, runtime_src);
    const result = try block.addBinOp(air_tag, lhs, rhs);
    if (block.wantSafety()) {
        const bit_count = scalar_ty.intInfo(target).bits;
        if (!std.math.isPowerOfTwo(bit_count)) {
            const bit_count_val = try Value.Tag.int_u64.create(sema.arena, bit_count);

            const ok = if (rhs_ty.zigTypeTag() == .Vector) ok: {
                const bit_count_inst = try sema.addConstant(rhs_ty, try Value.Tag.repeated.create(sema.arena, bit_count_val));
                const lt = try block.addCmpVector(rhs, bit_count_inst, .lt);
                break :ok try block.addInst(.{
                    .tag = .reduce,
                    .data = .{ .reduce = .{
                        .operand = lt,
                        .operation = .And,
                    } },
                });
            } else ok: {
                const bit_count_inst = try sema.addConstant(rhs_ty, bit_count_val);
                break :ok try block.addBinOp(.cmp_lt, rhs, bit_count_inst);
            };
            try sema.addSafetyCheck(block, ok, .shift_rhs_too_big);
        }

        if (air_tag == .shr_exact) {
            const back = try block.addBinOp(.shl, result, rhs);

            const ok = if (rhs_ty.zigTypeTag() == .Vector) ok: {
                const eql = try block.addCmpVector(lhs, back, .eq);
                break :ok try block.addInst(.{
                    .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                    .data = .{ .reduce = .{
                        .operand = eql,
                        .operation = .And,
                    } },
                });
            } else try block.addBinOp(.cmp_eq, lhs, back);
            try sema.addSafetyCheck(block, ok, .shr_overflow);
        }
    }
    return result;
}

fn zirBitwise(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{ .override = &[_]LazySrcLoc{ lhs_src, rhs_src } });
    const scalar_type = resolved_type.scalarType();
    const scalar_tag = scalar_type.zigTypeTag();

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    if (!is_int) {
        return sema.fail(block, src, "invalid operands to binary bitwise expression: '{s}' and '{s}'", .{ @tagName(lhs_ty.zigTypeTag()), @tagName(rhs_ty.zigTypeTag()) });
    }

    const runtime_src = runtime: {
        // TODO: ask the linker what kind of relocations are available, and
        // in some cases emit a Value that means "this decl's address AND'd with this operand".
        if (try sema.resolveMaybeUndefValIntable(casted_lhs)) |lhs_val| {
            if (try sema.resolveMaybeUndefValIntable(casted_rhs)) |rhs_val| {
                const result_val = switch (air_tag) {
                    .bit_and => try lhs_val.bitwiseAnd(rhs_val, resolved_type, sema.arena, sema.mod),
                    .bit_or => try lhs_val.bitwiseOr(rhs_val, resolved_type, sema.arena, sema.mod),
                    .xor => try lhs_val.bitwiseXor(rhs_val, resolved_type, sema.arena, sema.mod),
                    else => unreachable,
                };
                return sema.addConstant(resolved_type, result_val);
            } else {
                break :runtime rhs_src;
            }
        } else {
            break :runtime lhs_src;
        }
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);
    return block.addBinOp(air_tag, casted_lhs, casted_rhs);
}

fn zirBitNot(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_un_op = inst_data.src_node };

    const operand = try sema.resolveInst(inst_data.operand);
    const operand_type = sema.typeOf(operand);
    const scalar_type = operand_type.scalarType();

    if (scalar_type.zigTypeTag() != .Int) {
        return sema.fail(block, src, "unable to perform binary not operation on type '{}'", .{
            operand_type.fmt(sema.mod),
        });
    }

    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        if (val.isUndef()) {
            return sema.addConstUndef(operand_type);
        } else if (operand_type.zigTypeTag() == .Vector) {
            const vec_len = try sema.usizeCast(block, operand_src, operand_type.vectorLen());
            var elem_val_buf: Value.ElemValueBuffer = undefined;
            const elems = try sema.arena.alloc(Value, vec_len);
            for (elems, 0..) |*elem, i| {
                const elem_val = val.elemValueBuffer(sema.mod, i, &elem_val_buf);
                elem.* = try elem_val.bitwiseNot(scalar_type, sema.arena, sema.mod);
            }
            return sema.addConstant(
                operand_type,
                try Value.Tag.aggregate.create(sema.arena, elems),
            );
        } else {
            const result_val = try val.bitwiseNot(operand_type, sema.arena, sema.mod);
            return sema.addConstant(operand_type, result_val);
        }
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.not, operand_type, operand);
}

fn analyzeTupleCat(
    sema: *Sema,
    block: *Block,
    src_node: i32,
    lhs: Air.Inst.Ref,
    rhs: Air.Inst.Ref,
) CompileError!Air.Inst.Ref {
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const src = LazySrcLoc.nodeOffset(src_node);
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = src_node };

    const lhs_len = lhs_ty.structFieldCount();
    const rhs_len = rhs_ty.structFieldCount();
    const dest_fields = lhs_len + rhs_len;

    if (dest_fields == 0) {
        return sema.addConstant(Type.initTag(.empty_struct_literal), Value.initTag(.empty_struct_value));
    }
    if (lhs_len == 0) {
        return rhs;
    }
    if (rhs_len == 0) {
        return lhs;
    }
    const final_len = try sema.usizeCast(block, rhs_src, dest_fields);

    const types = try sema.arena.alloc(Type, final_len);
    const values = try sema.arena.alloc(Value, final_len);

    const opt_runtime_src = rs: {
        var runtime_src: ?LazySrcLoc = null;
        var i: u32 = 0;
        while (i < lhs_len) : (i += 1) {
            types[i] = lhs_ty.structFieldType(i);
            const default_val = lhs_ty.structFieldDefaultValue(i);
            values[i] = default_val;
            const operand_src = lhs_src; // TODO better source location
            if (default_val.tag() == .unreachable_value) {
                runtime_src = operand_src;
            }
        }
        i = 0;
        while (i < rhs_len) : (i += 1) {
            types[i + lhs_len] = rhs_ty.structFieldType(i);
            const default_val = rhs_ty.structFieldDefaultValue(i);
            values[i + lhs_len] = default_val;
            const operand_src = rhs_src; // TODO better source location
            if (default_val.tag() == .unreachable_value) {
                runtime_src = operand_src;
            }
        }
        break :rs runtime_src;
    };

    const tuple_ty = try Type.Tag.tuple.create(sema.arena, .{
        .types = types,
        .values = values,
    });

    const runtime_src = opt_runtime_src orelse {
        const tuple_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstant(tuple_ty, tuple_val);
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, final_len);
    var i: u32 = 0;
    while (i < lhs_len) : (i += 1) {
        const operand_src = lhs_src; // TODO better source location
        element_refs[i] = try sema.tupleFieldValByIndex(block, operand_src, lhs, i, lhs_ty);
    }
    i = 0;
    while (i < rhs_len) : (i += 1) {
        const operand_src = rhs_src; // TODO better source location
        element_refs[i + lhs_len] =
            try sema.tupleFieldValByIndex(block, operand_src, rhs, i, rhs_ty);
    }

    return block.addAggregateInit(tuple_ty, element_refs);
}

fn zirArrayCat(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const src = inst_data.src();

    const lhs_is_tuple = lhs_ty.isTuple();
    const rhs_is_tuple = rhs_ty.isTuple();
    if (lhs_is_tuple and rhs_is_tuple) {
        return sema.analyzeTupleCat(block, inst_data.src_node, lhs, rhs);
    }

    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };

    const lhs_info = try sema.getArrayCatInfo(block, lhs_src, lhs, rhs_ty) orelse lhs_info: {
        if (lhs_is_tuple) break :lhs_info @as(Type.ArrayInfo, undefined);
        return sema.fail(block, lhs_src, "expected indexable; found '{}'", .{lhs_ty.fmt(sema.mod)});
    };
    const rhs_info = try sema.getArrayCatInfo(block, rhs_src, rhs, lhs_ty) orelse {
        assert(!rhs_is_tuple);
        return sema.fail(block, rhs_src, "expected indexable; found '{}'", .{rhs_ty.fmt(sema.mod)});
    };

    const resolved_elem_ty = t: {
        var trash_block = block.makeSubBlock();
        trash_block.is_comptime = false;
        defer trash_block.instructions.deinit(sema.gpa);

        const instructions = [_]Air.Inst.Ref{
            try trash_block.addBitCast(lhs_info.elem_type, .void_value),
            try trash_block.addBitCast(rhs_info.elem_type, .void_value),
        };
        break :t try sema.resolvePeerTypes(block, src, &instructions, .{
            .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
        });
    };

    // When there is a sentinel mismatch, no sentinel on the result.
    // Otherwise, use the sentinel value provided by either operand,
    // coercing it to the peer-resolved element type.
    const res_sent_val: ?Value = s: {
        if (lhs_info.sentinel) |lhs_sent_val| {
            const lhs_sent = try sema.addConstant(lhs_info.elem_type, lhs_sent_val);
            if (rhs_info.sentinel) |rhs_sent_val| {
                const rhs_sent = try sema.addConstant(rhs_info.elem_type, rhs_sent_val);
                const lhs_sent_casted = try sema.coerce(block, resolved_elem_ty, lhs_sent, lhs_src);
                const rhs_sent_casted = try sema.coerce(block, resolved_elem_ty, rhs_sent, rhs_src);
                const lhs_sent_casted_val = try sema.resolveConstValue(block, lhs_src, lhs_sent_casted, "array sentinel value must be comptime-known");
                const rhs_sent_casted_val = try sema.resolveConstValue(block, rhs_src, rhs_sent_casted, "array sentinel value must be comptime-known");
                if (try sema.valuesEqual(lhs_sent_casted_val, rhs_sent_casted_val, resolved_elem_ty)) {
                    break :s lhs_sent_casted_val;
                } else {
                    break :s null;
                }
            } else {
                const lhs_sent_casted = try sema.coerce(block, resolved_elem_ty, lhs_sent, lhs_src);
                const lhs_sent_casted_val = try sema.resolveConstValue(block, lhs_src, lhs_sent_casted, "array sentinel value must be comptime-known");
                break :s lhs_sent_casted_val;
            }
        } else {
            if (rhs_info.sentinel) |rhs_sent_val| {
                const rhs_sent = try sema.addConstant(rhs_info.elem_type, rhs_sent_val);
                const rhs_sent_casted = try sema.coerce(block, resolved_elem_ty, rhs_sent, rhs_src);
                const rhs_sent_casted_val = try sema.resolveConstValue(block, rhs_src, rhs_sent_casted, "array sentinel value must be comptime-known");
                break :s rhs_sent_casted_val;
            } else {
                break :s null;
            }
        }
    };

    const lhs_len = try sema.usizeCast(block, lhs_src, lhs_info.len);
    const rhs_len = try sema.usizeCast(block, lhs_src, rhs_info.len);
    const result_len = std.math.add(usize, lhs_len, rhs_len) catch |err| switch (err) {
        error.Overflow => return sema.fail(
            block,
            src,
            "concatenating arrays of length {d} and {d} produces an array too large for this compiler implementation to handle",
            .{ lhs_len, rhs_len },
        ),
    };

    const result_ty = try Type.array(sema.arena, result_len, res_sent_val, resolved_elem_ty, sema.mod);
    const ptr_addrspace = p: {
        if (lhs_ty.zigTypeTag() == .Pointer) break :p lhs_ty.ptrAddressSpace();
        if (rhs_ty.zigTypeTag() == .Pointer) break :p rhs_ty.ptrAddressSpace();
        break :p null;
    };

    const runtime_src = if (switch (lhs_ty.zigTypeTag()) {
        .Array, .Struct => try sema.resolveMaybeUndefVal(lhs),
        .Pointer => try sema.resolveDefinedValue(block, lhs_src, lhs),
        else => unreachable,
    }) |lhs_val| rs: {
        if (switch (rhs_ty.zigTypeTag()) {
            .Array, .Struct => try sema.resolveMaybeUndefVal(rhs),
            .Pointer => try sema.resolveDefinedValue(block, rhs_src, rhs),
            else => unreachable,
        }) |rhs_val| {
            const lhs_sub_val = if (lhs_ty.isSinglePointer())
                (try sema.pointerDeref(block, lhs_src, lhs_val, lhs_ty)).?
            else
                lhs_val;

            const rhs_sub_val = if (rhs_ty.isSinglePointer())
                (try sema.pointerDeref(block, rhs_src, rhs_val, rhs_ty)).?
            else
                rhs_val;

            const final_len_including_sent = result_len + @boolToInt(res_sent_val != null);
            const element_vals = try sema.arena.alloc(Value, final_len_including_sent);
            var elem_i: usize = 0;
            while (elem_i < lhs_len) : (elem_i += 1) {
                const lhs_elem_i = elem_i;
                const elem_ty = if (lhs_is_tuple) lhs_ty.structFieldType(lhs_elem_i) else lhs_info.elem_type;
                const elem_default_val = if (lhs_is_tuple) lhs_ty.structFieldDefaultValue(lhs_elem_i) else Value.initTag(.unreachable_value);
                const elem_val = if (elem_default_val.tag() == .unreachable_value) try lhs_sub_val.elemValue(sema.mod, sema.arena, lhs_elem_i) else elem_default_val;
                const elem_val_inst = try sema.addConstant(elem_ty, elem_val);
                const coerced_elem_val_inst = try sema.coerce(block, resolved_elem_ty, elem_val_inst, .unneeded);
                const coerced_elem_val = try sema.resolveConstMaybeUndefVal(block, .unneeded, coerced_elem_val_inst, "");
                element_vals[elem_i] = coerced_elem_val;
            }
            while (elem_i < result_len) : (elem_i += 1) {
                const rhs_elem_i = elem_i - lhs_len;
                const elem_ty = if (rhs_is_tuple) rhs_ty.structFieldType(rhs_elem_i) else rhs_info.elem_type;
                const elem_default_val = if (rhs_is_tuple) rhs_ty.structFieldDefaultValue(rhs_elem_i) else Value.initTag(.unreachable_value);
                const elem_val = if (elem_default_val.tag() == .unreachable_value) try rhs_sub_val.elemValue(sema.mod, sema.arena, rhs_elem_i) else elem_default_val;
                const elem_val_inst = try sema.addConstant(elem_ty, elem_val);
                const coerced_elem_val_inst = try sema.coerce(block, resolved_elem_ty, elem_val_inst, .unneeded);
                const coerced_elem_val = try sema.resolveConstMaybeUndefVal(block, .unneeded, coerced_elem_val_inst, "");
                element_vals[elem_i] = coerced_elem_val;
            }
            if (res_sent_val) |sent_val| {
                element_vals[result_len] = sent_val;
            }
            const val = try Value.Tag.aggregate.create(sema.arena, element_vals);
            return sema.addConstantMaybeRef(block, result_ty, val, ptr_addrspace != null);
        } else break :rs rhs_src;
    } else lhs_src;

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (ptr_addrspace) |ptr_as| {
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = result_ty,
            .@"addrspace" = ptr_as,
        });
        const alloc = try block.addTy(.alloc, alloc_ty);
        const elem_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = resolved_elem_ty,
            .@"addrspace" = ptr_as,
        });

        var elem_i: usize = 0;
        while (elem_i < lhs_len) : (elem_i += 1) {
            const elem_index = try sema.addIntUnsigned(Type.usize, elem_i);
            const elem_ptr = try block.addPtrElemPtr(alloc, elem_index, elem_ptr_ty);
            const init = try sema.elemVal(block, lhs_src, lhs, elem_index, src, true);
            try sema.storePtr2(block, src, elem_ptr, src, init, lhs_src, .store);
        }
        while (elem_i < result_len) : (elem_i += 1) {
            const elem_index = try sema.addIntUnsigned(Type.usize, elem_i);
            const rhs_index = try sema.addIntUnsigned(Type.usize, elem_i - lhs_len);
            const elem_ptr = try block.addPtrElemPtr(alloc, elem_index, elem_ptr_ty);
            const init = try sema.elemVal(block, rhs_src, rhs, rhs_index, src, true);
            try sema.storePtr2(block, src, elem_ptr, src, init, rhs_src, .store);
        }
        if (res_sent_val) |sent_val| {
            const elem_index = try sema.addIntUnsigned(Type.usize, result_len);
            const elem_ptr = try block.addPtrElemPtr(alloc, elem_index, elem_ptr_ty);
            const init = try sema.addConstant(lhs_info.elem_type, sent_val);
            try sema.storePtr2(block, src, elem_ptr, src, init, lhs_src, .store);
        }

        return alloc;
    }

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, result_len);
    {
        var elem_i: usize = 0;
        while (elem_i < lhs_len) : (elem_i += 1) {
            const index = try sema.addIntUnsigned(Type.usize, elem_i);
            const init = try sema.elemVal(block, lhs_src, lhs, index, src, true);
            element_refs[elem_i] = try sema.coerce(block, resolved_elem_ty, init, lhs_src);
        }
        while (elem_i < result_len) : (elem_i += 1) {
            const index = try sema.addIntUnsigned(Type.usize, elem_i - lhs_len);
            const init = try sema.elemVal(block, rhs_src, rhs, index, src, true);
            element_refs[elem_i] = try sema.coerce(block, resolved_elem_ty, init, rhs_src);
        }
    }

    return block.addAggregateInit(result_ty, element_refs);
}

fn getArrayCatInfo(sema: *Sema, block: *Block, src: LazySrcLoc, operand: Air.Inst.Ref, peer_ty: Type) !?Type.ArrayInfo {
    const operand_ty = sema.typeOf(operand);
    switch (operand_ty.zigTypeTag()) {
        .Array => return operand_ty.arrayInfo(),
        .Pointer => {
            const ptr_info = operand_ty.ptrInfo().data;
            switch (ptr_info.size) {
                // TODO: in the Many case here this should only work if the type
                // has a sentinel, and this code should compute the length based
                // on the sentinel value.
                .Slice, .Many => {
                    const val = try sema.resolveConstValue(block, src, operand, "slice value being concatenated must be comptime-known");
                    return Type.ArrayInfo{
                        .elem_type = ptr_info.pointee_type,
                        .sentinel = ptr_info.sentinel,
                        .len = val.sliceLen(sema.mod),
                    };
                },
                .One => {
                    if (ptr_info.pointee_type.zigTypeTag() == .Array) {
                        return ptr_info.pointee_type.arrayInfo();
                    }
                },
                .C => {},
            }
        },
        .Struct => {
            if (operand_ty.isTuple() and peer_ty.isIndexable()) {
                assert(!peer_ty.isTuple());
                return .{
                    .elem_type = peer_ty.elemType2(),
                    .sentinel = null,
                    .len = operand_ty.arrayLen(),
                };
            }
        },
        else => {},
    }
    return null;
}

fn analyzeTupleMul(
    sema: *Sema,
    block: *Block,
    src_node: i32,
    operand: Air.Inst.Ref,
    factor: u64,
) CompileError!Air.Inst.Ref {
    const operand_ty = sema.typeOf(operand);
    const src = LazySrcLoc.nodeOffset(src_node);
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = src_node };

    const tuple_len = operand_ty.structFieldCount();
    const final_len_u64 = std.math.mul(u64, tuple_len, factor) catch
        return sema.fail(block, rhs_src, "operation results in overflow", .{});

    if (final_len_u64 == 0) {
        return sema.addConstant(Type.initTag(.empty_struct_literal), Value.initTag(.empty_struct_value));
    }
    const final_len = try sema.usizeCast(block, rhs_src, final_len_u64);

    const types = try sema.arena.alloc(Type, final_len);
    const values = try sema.arena.alloc(Value, final_len);

    const opt_runtime_src = rs: {
        var runtime_src: ?LazySrcLoc = null;
        var i: u32 = 0;
        while (i < tuple_len) : (i += 1) {
            types[i] = operand_ty.structFieldType(i);
            values[i] = operand_ty.structFieldDefaultValue(i);
            const operand_src = lhs_src; // TODO better source location
            if (values[i].tag() == .unreachable_value) {
                runtime_src = operand_src;
            }
        }
        i = 0;
        while (i < factor) : (i += 1) {
            mem.copy(Type, types[tuple_len * i ..], types[0..tuple_len]);
            mem.copy(Value, values[tuple_len * i ..], values[0..tuple_len]);
        }
        break :rs runtime_src;
    };

    const tuple_ty = try Type.Tag.tuple.create(sema.arena, .{
        .types = types,
        .values = values,
    });

    const runtime_src = opt_runtime_src orelse {
        const tuple_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstant(tuple_ty, tuple_val);
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, final_len);
    var i: u32 = 0;
    while (i < tuple_len) : (i += 1) {
        const operand_src = lhs_src; // TODO better source location
        element_refs[i] = try sema.tupleFieldValByIndex(block, operand_src, operand, @intCast(u32, i), operand_ty);
    }
    i = 1;
    while (i < factor) : (i += 1) {
        mem.copy(Air.Inst.Ref, element_refs[tuple_len * i ..], element_refs[0..tuple_len]);
    }

    return block.addAggregateInit(tuple_ty, element_refs);
}

fn zirArrayMul(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const lhs_ty = sema.typeOf(lhs);
    const src: LazySrcLoc = inst_data.src();
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const operator_src: LazySrcLoc = .{ .node_offset_main_token = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };

    if (lhs_ty.isTuple()) {
        // In `**` rhs must be comptime-known, but lhs can be runtime-known
        const factor = try sema.resolveInt(block, rhs_src, extra.rhs, Type.usize, "array multiplication factor must be comptime-known");
        return sema.analyzeTupleMul(block, inst_data.src_node, lhs, factor);
    }

    // Analyze the lhs first, to catch the case that someone tried to do exponentiation
    const lhs_info = try sema.getArrayCatInfo(block, lhs_src, lhs, lhs_ty) orelse {
        const msg = msg: {
            const msg = try sema.errMsg(block, lhs_src, "expected indexable; found '{}'", .{lhs_ty.fmt(sema.mod)});
            errdefer msg.destroy(sema.gpa);
            switch (lhs_ty.zigTypeTag()) {
                .Int, .Float, .ComptimeFloat, .ComptimeInt, .Vector => {
                    try sema.errNote(block, operator_src, msg, "this operator multiplies arrays; use std.math.pow for exponentiation", .{});
                },
                else => {},
            }
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    };

    // In `**` rhs must be comptime-known, but lhs can be runtime-known
    const factor = try sema.resolveInt(block, rhs_src, extra.rhs, Type.usize, "array multiplication factor must be comptime-known");

    const result_len_u64 = std.math.mul(u64, lhs_info.len, factor) catch
        return sema.fail(block, rhs_src, "operation results in overflow", .{});
    const result_len = try sema.usizeCast(block, src, result_len_u64);

    const result_ty = try Type.array(sema.arena, result_len, lhs_info.sentinel, lhs_info.elem_type, sema.mod);

    const ptr_addrspace = if (lhs_ty.zigTypeTag() == .Pointer) lhs_ty.ptrAddressSpace() else null;
    const lhs_len = try sema.usizeCast(block, lhs_src, lhs_info.len);

    if (try sema.resolveDefinedValue(block, lhs_src, lhs)) |lhs_val| {
        const final_len_including_sent = result_len + @boolToInt(lhs_info.sentinel != null);

        const lhs_sub_val = if (lhs_ty.isSinglePointer())
            (try sema.pointerDeref(block, lhs_src, lhs_val, lhs_ty)).?
        else
            lhs_val;

        const val = v: {
            // Optimization for the common pattern of a single element repeated N times, such
            // as zero-filling a byte array.
            if (lhs_len == 1) {
                const elem_val = try lhs_sub_val.elemValue(sema.mod, sema.arena, 0);
                break :v try Value.Tag.repeated.create(sema.arena, elem_val);
            }

            const element_vals = try sema.arena.alloc(Value, final_len_including_sent);
            var elem_i: usize = 0;
            while (elem_i < result_len) {
                var lhs_i: usize = 0;
                while (lhs_i < lhs_len) : (lhs_i += 1) {
                    const elem_val = try lhs_sub_val.elemValue(sema.mod, sema.arena, lhs_i);
                    element_vals[elem_i] = elem_val;
                    elem_i += 1;
                }
            }
            if (lhs_info.sentinel) |sent_val| {
                element_vals[result_len] = sent_val;
            }
            break :v try Value.Tag.aggregate.create(sema.arena, element_vals);
        };
        return sema.addConstantMaybeRef(block, result_ty, val, ptr_addrspace != null);
    }

    try sema.requireRuntimeBlock(block, src, lhs_src);

    if (ptr_addrspace) |ptr_as| {
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = result_ty,
            .@"addrspace" = ptr_as,
        });
        const alloc = try block.addTy(.alloc, alloc_ty);
        const elem_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = lhs_info.elem_type,
            .@"addrspace" = ptr_as,
        });

        var elem_i: usize = 0;
        while (elem_i < result_len) {
            var lhs_i: usize = 0;
            while (lhs_i < lhs_len) : (lhs_i += 1) {
                const elem_index = try sema.addIntUnsigned(Type.usize, elem_i);
                elem_i += 1;
                const lhs_index = try sema.addIntUnsigned(Type.usize, lhs_i);
                const elem_ptr = try block.addPtrElemPtr(alloc, elem_index, elem_ptr_ty);
                const init = try sema.elemVal(block, lhs_src, lhs, lhs_index, src, true);
                try sema.storePtr2(block, src, elem_ptr, src, init, lhs_src, .store);
            }
        }
        if (lhs_info.sentinel) |sent_val| {
            const elem_index = try sema.addIntUnsigned(Type.usize, result_len);
            const elem_ptr = try block.addPtrElemPtr(alloc, elem_index, elem_ptr_ty);
            const init = try sema.addConstant(lhs_info.elem_type, sent_val);
            try sema.storePtr2(block, src, elem_ptr, src, init, lhs_src, .store);
        }

        return alloc;
    }

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, result_len);
    var elem_i: usize = 0;
    while (elem_i < result_len) {
        var lhs_i: usize = 0;
        while (lhs_i < lhs_len) : (lhs_i += 1) {
            const lhs_index = try sema.addIntUnsigned(Type.usize, lhs_i);
            const init = try sema.elemVal(block, lhs_src, lhs, lhs_index, src, true);
            element_refs[elem_i] = init;
            elem_i += 1;
        }
    }

    return block.addAggregateInit(result_ty, element_refs);
}

fn zirNegate(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const lhs_src = src;
    const rhs_src: LazySrcLoc = .{ .node_offset_un_op = inst_data.src_node };

    const rhs = try sema.resolveInst(inst_data.operand);
    const rhs_ty = sema.typeOf(rhs);
    const rhs_scalar_ty = rhs_ty.scalarType();

    if (rhs_scalar_ty.isUnsignedInt() or switch (rhs_scalar_ty.zigTypeTag()) {
        .Int, .ComptimeInt, .Float, .ComptimeFloat => false,
        else => true,
    }) {
        return sema.fail(block, src, "negation of type '{}'", .{rhs_ty.fmt(sema.mod)});
    }

    if (rhs_scalar_ty.isAnyFloat()) {
        // We handle float negation here to ensure negative zero is represented in the bits.
        if (try sema.resolveMaybeUndefVal(rhs)) |rhs_val| {
            if (rhs_val.isUndef()) return sema.addConstUndef(rhs_ty);
            return sema.addConstant(rhs_ty, try rhs_val.floatNeg(rhs_ty, sema.arena, sema.mod));
        }
        try sema.requireRuntimeBlock(block, src, null);
        return block.addUnOp(if (block.float_mode == .Optimized) .neg_optimized else .neg, rhs);
    }

    const lhs = if (rhs_ty.zigTypeTag() == .Vector)
        try sema.addConstant(rhs_ty, try Value.Tag.repeated.create(sema.arena, Value.zero))
    else
        try sema.resolveInst(.zero);

    return sema.analyzeArithmetic(block, .sub, lhs, rhs, src, lhs_src, rhs_src, true);
}

fn zirNegateWrap(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const lhs_src = src;
    const rhs_src: LazySrcLoc = .{ .node_offset_un_op = inst_data.src_node };

    const rhs = try sema.resolveInst(inst_data.operand);
    const rhs_ty = sema.typeOf(rhs);
    const rhs_scalar_ty = rhs_ty.scalarType();

    switch (rhs_scalar_ty.zigTypeTag()) {
        .Int, .ComptimeInt, .Float, .ComptimeFloat => {},
        else => return sema.fail(block, src, "negation of type '{}'", .{rhs_ty.fmt(sema.mod)}),
    }

    const lhs = if (rhs_ty.zigTypeTag() == .Vector)
        try sema.addConstant(rhs_ty, try Value.Tag.repeated.create(sema.arena, Value.zero))
    else
        try sema.resolveInst(.zero);

    return sema.analyzeArithmetic(block, .subwrap, lhs, rhs, src, lhs_src, rhs_src, true);
}

fn zirArithmetic(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    zir_tag: Zir.Inst.Tag,
    safety: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    sema.src = .{ .node_offset_bin_op = inst_data.src_node };
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);

    return sema.analyzeArithmetic(block, zir_tag, lhs, rhs, sema.src, lhs_src, rhs_src, safety);
}

fn zirDiv(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const lhs_scalar_ty = lhs_ty.scalarType();
    const rhs_scalar_ty = rhs_ty.scalarType();
    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .div);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    if ((lhs_ty.zigTypeTag() == .ComptimeFloat and rhs_ty.zigTypeTag() == .ComptimeInt) or
        (lhs_ty.zigTypeTag() == .ComptimeInt and rhs_ty.zigTypeTag() == .ComptimeFloat))
    {
        // If it makes a difference whether we coerce to ints or floats before doing the division, error.
        // If lhs % rhs is 0, it doesn't matter.
        const lhs_val = maybe_lhs_val orelse unreachable;
        const rhs_val = maybe_rhs_val orelse unreachable;
        const rem = lhs_val.floatRem(rhs_val, resolved_type, sema.arena, mod) catch unreachable;
        if (!rem.compareAllWithZero(.eq, mod)) {
            return sema.fail(block, src, "ambiguous coercion of division operands '{s}' and '{s}'; non-zero remainder '{}'", .{
                @tagName(lhs_ty.tag()), @tagName(rhs_ty.tag()), rem.fmtValue(resolved_type, sema.mod),
            });
        }
    }

    // TODO: emit compile error when .div is used on integers and there would be an
    // ambiguous result between div_floor and div_trunc.

    // For integers:
    // If the lhs is zero, then zero is returned regardless of rhs.
    // If the rhs is zero, compile error for division by zero.
    // If the rhs is undefined, compile error because there is a possible
    // value (zero) for which the division would be illegal behavior.
    // If the lhs is undefined:
    //   * if lhs type is signed:
    //     * if rhs is comptime-known and not -1, result is undefined
    //     * if rhs is -1 or runtime-known, compile error because there is a
    //        possible value (-min_int / -1)  for which division would be
    //        illegal behavior.
    //   * if lhs type is unsigned, undef is returned regardless of rhs.
    //
    // For floats:
    // If the rhs is zero:
    //  * comptime_float: compile error for division by zero.
    //  * other float type:
    //    * if the lhs is zero: QNaN
    //    * otherwise: +Inf or -Inf depending on lhs sign
    // If the rhs is undefined:
    //  * comptime_float: compile error because there is a possible
    //    value (zero) for which the division would be illegal behavior.
    //  * other float type: result is undefined
    // If the lhs is undefined, result is undefined.
    switch (scalar_tag) {
        .Int, .ComptimeInt, .ComptimeFloat => {
            if (maybe_lhs_val) |lhs_val| {
                if (!lhs_val.isUndef()) {
                    if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        const zero_val = if (is_vector) b: {
                            break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                        } else Value.zero;
                        return sema.addConstant(resolved_type, zero_val);
                    }
                }
            }
            if (maybe_rhs_val) |rhs_val| {
                if (rhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                    return sema.failWithDivideByZero(block, rhs_src);
                }
                // TODO: if the RHS is one, return the LHS directly
            }
        },
        else => {},
    }

    const runtime_src = rs: {
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                if (lhs_scalar_ty.isSignedInt() and rhs_scalar_ty.isSignedInt()) {
                    if (maybe_rhs_val) |rhs_val| {
                        if (try sema.compareAll(rhs_val, .neq, Value.negative_one, resolved_type)) {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                return sema.addConstUndef(resolved_type);
            }

            if (maybe_rhs_val) |rhs_val| {
                if (is_int) {
                    const res = try lhs_val.intDiv(rhs_val, resolved_type, sema.arena, mod);
                    var vector_index: usize = undefined;
                    if (!(try sema.intFitsInType(res, resolved_type, &vector_index))) {
                        return sema.failWithIntegerOverflow(block, src, resolved_type, res, vector_index);
                    }
                    return sema.addConstant(resolved_type, res);
                } else {
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.floatDiv(rhs_val, resolved_type, sema.arena, mod),
                    );
                }
            } else {
                break :rs rhs_src;
            }
        } else {
            break :rs lhs_src;
        }
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivIntOverflowSafety(block, resolved_type, lhs_scalar_ty, maybe_lhs_val, maybe_rhs_val, casted_lhs, casted_rhs, is_int);
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    const air_tag = if (is_int) blk: {
        if (lhs_ty.isSignedInt() or rhs_ty.isSignedInt()) {
            return sema.fail(block, src, "division with '{s}' and '{s}': signed integers must use @divTrunc, @divFloor, or @divExact", .{ @tagName(lhs_ty.tag()), @tagName(rhs_ty.tag()) });
        }
        break :blk Air.Inst.Tag.div_trunc;
    } else switch (block.float_mode) {
        .Optimized => Air.Inst.Tag.div_float_optimized,
        .Strict => Air.Inst.Tag.div_float,
    };
    return block.addBinOp(air_tag, casted_lhs, casted_rhs);
}

fn zirDivExact(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const lhs_scalar_ty = lhs_ty.scalarType();
    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .div_exact);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // If the lhs is zero, then zero is returned regardless of rhs.
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, compile error because there is a possible
        // value for which the division would result in a remainder.
        // TODO: emit runtime safety for if there is a remainder
        // TODO: emit runtime safety for division by zero
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, compile error because there is a possible
        // value for which the division would result in a remainder.
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            } else {
                if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                    const zero_val = if (is_vector) b: {
                        break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                    } else Value.zero;
                    return sema.addConstant(resolved_type, zero_val);
                }
            }
        }
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
            // TODO: if the RHS is one, return the LHS directly
        }
        if (maybe_lhs_val) |lhs_val| {
            if (maybe_rhs_val) |rhs_val| {
                if (is_int) {
                    const modulus_val = try lhs_val.intMod(rhs_val, resolved_type, sema.arena, mod);
                    if (!(modulus_val.compareAllWithZero(.eq, mod))) {
                        return sema.fail(block, src, "exact division produced remainder", .{});
                    }
                    const res = try lhs_val.intDiv(rhs_val, resolved_type, sema.arena, mod);
                    var vector_index: usize = undefined;
                    if (!(try sema.intFitsInType(res, resolved_type, &vector_index))) {
                        return sema.failWithIntegerOverflow(block, src, resolved_type, res, vector_index);
                    }
                    return sema.addConstant(resolved_type, res);
                } else {
                    const modulus_val = try lhs_val.floatMod(rhs_val, resolved_type, sema.arena, mod);
                    if (!(modulus_val.compareAllWithZero(.eq, mod))) {
                        return sema.fail(block, src, "exact division produced remainder", .{});
                    }
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.floatDiv(rhs_val, resolved_type, sema.arena, mod),
                    );
                }
            } else break :rs rhs_src;
        } else break :rs lhs_src;
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    // Depending on whether safety is enabled, we will have a slightly different strategy
    // here. The `div_exact` AIR instruction causes undefined behavior if a remainder
    // is produced, so in the safety check case, it cannot be used. Instead we do a
    // div_trunc and check for remainder.

    if (block.wantSafety()) {
        try sema.addDivIntOverflowSafety(block, resolved_type, lhs_scalar_ty, maybe_lhs_val, maybe_rhs_val, casted_lhs, casted_rhs, is_int);
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);

        const result = try block.addBinOp(.div_trunc, casted_lhs, casted_rhs);
        const ok = if (!is_int) ok: {
            const floored = try block.addUnOp(.floor, result);

            if (resolved_type.zigTypeTag() == .Vector) {
                const eql = try block.addCmpVector(result, floored, .eq);
                break :ok try block.addInst(.{
                    .tag = switch (block.float_mode) {
                        .Strict => .reduce,
                        .Optimized => .reduce_optimized,
                    },
                    .data = .{ .reduce = .{
                        .operand = eql,
                        .operation = .And,
                    } },
                });
            } else {
                const is_in_range = try block.addBinOp(switch (block.float_mode) {
                    .Strict => .cmp_eq,
                    .Optimized => .cmp_eq_optimized,
                }, result, floored);
                break :ok is_in_range;
            }
        } else ok: {
            const remainder = try block.addBinOp(.rem, casted_lhs, casted_rhs);

            if (resolved_type.zigTypeTag() == .Vector) {
                const zero_val = try Value.Tag.repeated.create(sema.arena, Value.zero);
                const zero = try sema.addConstant(resolved_type, zero_val);
                const eql = try block.addCmpVector(remainder, zero, .eq);
                break :ok try block.addInst(.{
                    .tag = .reduce,
                    .data = .{ .reduce = .{
                        .operand = eql,
                        .operation = .And,
                    } },
                });
            } else {
                const zero = try sema.addConstant(resolved_type, Value.zero);
                const is_in_range = try block.addBinOp(.cmp_eq, remainder, zero);
                break :ok is_in_range;
            }
        };
        try sema.addSafetyCheck(block, ok, .exact_division_remainder);
        return result;
    }

    return block.addBinOp(airTag(block, is_int, .div_exact, .div_exact_optimized), casted_lhs, casted_rhs);
}

fn zirDivFloor(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const lhs_scalar_ty = lhs_ty.scalarType();
    const rhs_scalar_ty = rhs_ty.scalarType();
    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .div_floor);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // If the lhs is zero, then zero is returned regardless of rhs.
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined:
        //   * if lhs type is signed:
        //     * if rhs is comptime-known and not -1, result is undefined
        //     * if rhs is -1 or runtime-known, compile error because there is a
        //        possible value (-min_int / -1)  for which division would be
        //        illegal behavior.
        //   * if lhs type is unsigned, undef is returned regardless of rhs.
        // TODO: emit runtime safety for division by zero
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, result is undefined.
        if (maybe_lhs_val) |lhs_val| {
            if (!lhs_val.isUndef()) {
                if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                    const zero_val = if (is_vector) b: {
                        break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                    } else Value.zero;
                    return sema.addConstant(resolved_type, zero_val);
                }
            }
        }
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
            // TODO: if the RHS is one, return the LHS directly
        }
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                if (lhs_scalar_ty.isSignedInt() and rhs_scalar_ty.isSignedInt()) {
                    if (maybe_rhs_val) |rhs_val| {
                        if (try sema.compareAll(rhs_val, .neq, Value.negative_one, resolved_type)) {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                return sema.addConstUndef(resolved_type);
            }

            if (maybe_rhs_val) |rhs_val| {
                if (is_int) {
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.intDivFloor(rhs_val, resolved_type, sema.arena, mod),
                    );
                } else {
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.floatDivFloor(rhs_val, resolved_type, sema.arena, mod),
                    );
                }
            } else break :rs rhs_src;
        } else break :rs lhs_src;
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivIntOverflowSafety(block, resolved_type, lhs_scalar_ty, maybe_lhs_val, maybe_rhs_val, casted_lhs, casted_rhs, is_int);
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    return block.addBinOp(airTag(block, is_int, .div_floor, .div_floor_optimized), casted_lhs, casted_rhs);
}

fn zirDivTrunc(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const lhs_scalar_ty = lhs_ty.scalarType();
    const rhs_scalar_ty = rhs_ty.scalarType();
    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .div_trunc);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // If the lhs is zero, then zero is returned regardless of rhs.
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined:
        //   * if lhs type is signed:
        //     * if rhs is comptime-known and not -1, result is undefined
        //     * if rhs is -1 or runtime-known, compile error because there is a
        //        possible value (-min_int / -1)  for which division would be
        //        illegal behavior.
        //   * if lhs type is unsigned, undef is returned regardless of rhs.
        // TODO: emit runtime safety for division by zero
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, result is undefined.
        if (maybe_lhs_val) |lhs_val| {
            if (!lhs_val.isUndef()) {
                if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                    const zero_val = if (is_vector) b: {
                        break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                    } else Value.zero;
                    return sema.addConstant(resolved_type, zero_val);
                }
            }
        }
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
        }
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                if (lhs_scalar_ty.isSignedInt() and rhs_scalar_ty.isSignedInt()) {
                    if (maybe_rhs_val) |rhs_val| {
                        if (try sema.compareAll(rhs_val, .neq, Value.negative_one, resolved_type)) {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                return sema.addConstUndef(resolved_type);
            }

            if (maybe_rhs_val) |rhs_val| {
                if (is_int) {
                    const res = try lhs_val.intDiv(rhs_val, resolved_type, sema.arena, mod);
                    var vector_index: usize = undefined;
                    if (!(try sema.intFitsInType(res, resolved_type, &vector_index))) {
                        return sema.failWithIntegerOverflow(block, src, resolved_type, res, vector_index);
                    }
                    return sema.addConstant(resolved_type, res);
                } else {
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.floatDivTrunc(rhs_val, resolved_type, sema.arena, mod),
                    );
                }
            } else break :rs rhs_src;
        } else break :rs lhs_src;
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivIntOverflowSafety(block, resolved_type, lhs_scalar_ty, maybe_lhs_val, maybe_rhs_val, casted_lhs, casted_rhs, is_int);
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    return block.addBinOp(airTag(block, is_int, .div_trunc, .div_trunc_optimized), casted_lhs, casted_rhs);
}

fn addDivIntOverflowSafety(
    sema: *Sema,
    block: *Block,
    resolved_type: Type,
    lhs_scalar_ty: Type,
    maybe_lhs_val: ?Value,
    maybe_rhs_val: ?Value,
    casted_lhs: Air.Inst.Ref,
    casted_rhs: Air.Inst.Ref,
    is_int: bool,
) CompileError!void {
    if (!is_int) return;

    // If the LHS is unsigned, it cannot cause overflow.
    if (!lhs_scalar_ty.isSignedInt()) return;

    const mod = sema.mod;
    const target = mod.getTarget();

    // If the LHS is widened to a larger integer type, no overflow is possible.
    if (lhs_scalar_ty.intInfo(target).bits < resolved_type.intInfo(target).bits) {
        return;
    }

    const min_int = try resolved_type.minInt(sema.arena, target);
    const neg_one_scalar = try Value.Tag.int_i64.create(sema.arena, -1);
    const neg_one = if (resolved_type.zigTypeTag() == .Vector)
        try Value.Tag.repeated.create(sema.arena, neg_one_scalar)
    else
        neg_one_scalar;

    // If the LHS is comptime-known to be not equal to the min int,
    // no overflow is possible.
    if (maybe_lhs_val) |lhs_val| {
        if (lhs_val.compareAll(.neq, min_int, resolved_type, mod)) return;
    }

    // If the RHS is comptime-known to not be equal to -1, no overflow is possible.
    if (maybe_rhs_val) |rhs_val| {
        if (rhs_val.compareAll(.neq, neg_one, resolved_type, mod)) return;
    }

    var ok: Air.Inst.Ref = .none;
    if (resolved_type.zigTypeTag() == .Vector) {
        if (maybe_lhs_val == null) {
            const min_int_ref = try sema.addConstant(resolved_type, min_int);
            ok = try block.addCmpVector(casted_lhs, min_int_ref, .neq);
        }
        if (maybe_rhs_val == null) {
            const neg_one_ref = try sema.addConstant(resolved_type, neg_one);
            const rhs_ok = try block.addCmpVector(casted_rhs, neg_one_ref, .neq);
            if (ok == .none) {
                ok = rhs_ok;
            } else {
                ok = try block.addBinOp(.bool_or, ok, rhs_ok);
            }
        }
        assert(ok != .none);
        ok = try block.addInst(.{
            .tag = .reduce,
            .data = .{ .reduce = .{
                .operand = ok,
                .operation = .And,
            } },
        });
    } else {
        if (maybe_lhs_val == null) {
            const min_int_ref = try sema.addConstant(resolved_type, min_int);
            ok = try block.addBinOp(.cmp_neq, casted_lhs, min_int_ref);
        }
        if (maybe_rhs_val == null) {
            const neg_one_ref = try sema.addConstant(resolved_type, neg_one);
            const rhs_ok = try block.addBinOp(.cmp_neq, casted_rhs, neg_one_ref);
            if (ok == .none) {
                ok = rhs_ok;
            } else {
                ok = try block.addBinOp(.bool_or, ok, rhs_ok);
            }
        }
        assert(ok != .none);
    }
    try sema.addSafetyCheck(block, ok, .integer_overflow);
}

fn addDivByZeroSafety(
    sema: *Sema,
    block: *Block,
    resolved_type: Type,
    maybe_rhs_val: ?Value,
    casted_rhs: Air.Inst.Ref,
    is_int: bool,
) CompileError!void {
    // Strict IEEE floats have well-defined division by zero.
    if (!is_int and block.float_mode == .Strict) return;

    // If rhs was comptime-known to be zero a compile error would have been
    // emitted above.
    if (maybe_rhs_val != null) return;

    const ok = if (resolved_type.zigTypeTag() == .Vector) ok: {
        const zero_val = try Value.Tag.repeated.create(sema.arena, Value.zero);
        const zero = try sema.addConstant(resolved_type, zero_val);
        const ok = try block.addCmpVector(casted_rhs, zero, .neq);
        break :ok try block.addInst(.{
            .tag = if (is_int) .reduce else .reduce_optimized,
            .data = .{ .reduce = .{
                .operand = ok,
                .operation = .And,
            } },
        });
    } else ok: {
        const zero = try sema.addConstant(resolved_type, Value.zero);
        break :ok try block.addBinOp(if (is_int) .cmp_neq else .cmp_neq_optimized, casted_rhs, zero);
    };
    try sema.addSafetyCheck(block, ok, .divide_by_zero);
}

fn airTag(block: *Block, is_int: bool, normal: Air.Inst.Tag, optimized: Air.Inst.Tag) Air.Inst.Tag {
    if (is_int) return normal;
    return switch (block.float_mode) {
        .Strict => normal,
        .Optimized => optimized,
    };
}

fn zirModRem(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const lhs_scalar_ty = lhs_ty.scalarType();
    const rhs_scalar_ty = rhs_ty.scalarType();
    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .mod_rem);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // Either operand being undef is a compile error because there exists
        // a possible value (TODO what is it?) that would invoke illegal behavior.
        // TODO: can lhs undef be handled better?
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, result is undefined.
        //
        // For either one: if the result would be different between @mod and @rem,
        // then emit a compile error saying you have to pick one.
        if (is_int) {
            if (maybe_lhs_val) |lhs_val| {
                if (lhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, lhs_src);
                }
                if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                    const zero_val = if (is_vector) b: {
                        break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                    } else Value.zero;
                    return sema.addConstant(resolved_type, zero_val);
                }
            } else if (lhs_scalar_ty.isSignedInt()) {
                return sema.failWithModRemNegative(block, lhs_src, lhs_ty, rhs_ty);
            }
            if (maybe_rhs_val) |rhs_val| {
                if (rhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                    return sema.failWithDivideByZero(block, rhs_src);
                }
                if (!(try rhs_val.compareAllWithZeroAdvanced(.gte, sema))) {
                    return sema.failWithModRemNegative(block, rhs_src, lhs_ty, rhs_ty);
                }
                if (maybe_lhs_val) |lhs_val| {
                    const rem_result = try sema.intRem(resolved_type, lhs_val, rhs_val);
                    // If this answer could possibly be different by doing `intMod`,
                    // we must emit a compile error. Otherwise, it's OK.
                    if (!(try lhs_val.compareAllWithZeroAdvanced(.gte, sema)) and
                        !(try rem_result.compareAllWithZeroAdvanced(.eq, sema)))
                    {
                        return sema.failWithModRemNegative(block, lhs_src, lhs_ty, rhs_ty);
                    }
                    return sema.addConstant(resolved_type, rem_result);
                }
                break :rs lhs_src;
            } else if (rhs_scalar_ty.isSignedInt()) {
                return sema.failWithModRemNegative(block, rhs_src, lhs_ty, rhs_ty);
            } else {
                break :rs rhs_src;
            }
        }
        // float operands
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.gte, sema))) {
                return sema.failWithModRemNegative(block, rhs_src, lhs_ty, rhs_ty);
            }
            if (maybe_lhs_val) |lhs_val| {
                if (lhs_val.isUndef() or !(try lhs_val.compareAllWithZeroAdvanced(.gte, sema))) {
                    return sema.failWithModRemNegative(block, lhs_src, lhs_ty, rhs_ty);
                }
                return sema.addConstant(
                    resolved_type,
                    try lhs_val.floatRem(rhs_val, resolved_type, sema.arena, mod),
                );
            } else {
                return sema.failWithModRemNegative(block, lhs_src, lhs_ty, rhs_ty);
            }
        } else {
            return sema.failWithModRemNegative(block, rhs_src, lhs_ty, rhs_ty);
        }
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    const air_tag = airTag(block, is_int, .rem, .rem_optimized);
    return block.addBinOp(air_tag, casted_lhs, casted_rhs);
}

fn intRem(
    sema: *Sema,
    ty: Type,
    lhs: Value,
    rhs: Value,
) CompileError!Value {
    if (ty.zigTypeTag() == .Vector) {
        const result_data = try sema.arena.alloc(Value, ty.vectorLen());
        for (result_data, 0..) |*scalar, i| {
            var lhs_buf: Value.ElemValueBuffer = undefined;
            var rhs_buf: Value.ElemValueBuffer = undefined;
            const lhs_elem = lhs.elemValueBuffer(sema.mod, i, &lhs_buf);
            const rhs_elem = rhs.elemValueBuffer(sema.mod, i, &rhs_buf);
            scalar.* = try sema.intRemScalar(lhs_elem, rhs_elem);
        }
        return Value.Tag.aggregate.create(sema.arena, result_data);
    }
    return sema.intRemScalar(lhs, rhs);
}

fn intRemScalar(
    sema: *Sema,
    lhs: Value,
    rhs: Value,
) CompileError!Value {
    const target = sema.mod.getTarget();
    // TODO is this a performance issue? maybe we should try the operation without
    // resorting to BigInt first.
    var lhs_space: Value.BigIntSpace = undefined;
    var rhs_space: Value.BigIntSpace = undefined;
    const lhs_bigint = try lhs.toBigIntAdvanced(&lhs_space, target, sema);
    const rhs_bigint = try rhs.toBigIntAdvanced(&rhs_space, target, sema);
    const limbs_q = try sema.arena.alloc(
        math.big.Limb,
        lhs_bigint.limbs.len,
    );
    const limbs_r = try sema.arena.alloc(
        math.big.Limb,
        // TODO: consider reworking Sema to re-use Values rather than
        // always producing new Value objects.
        rhs_bigint.limbs.len,
    );
    const limbs_buffer = try sema.arena.alloc(
        math.big.Limb,
        math.big.int.calcDivLimbsBufferLen(lhs_bigint.limbs.len, rhs_bigint.limbs.len),
    );
    var result_q = math.big.int.Mutable{ .limbs = limbs_q, .positive = undefined, .len = undefined };
    var result_r = math.big.int.Mutable{ .limbs = limbs_r, .positive = undefined, .len = undefined };
    result_q.divTrunc(&result_r, lhs_bigint, rhs_bigint, limbs_buffer);
    return Value.fromBigInt(sema.arena, result_r.toConst());
}

fn zirMod(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .mod);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // Either operand being undef is a compile error because there exists
        // a possible value (TODO what is it?) that would invoke illegal behavior.
        // TODO: can lhs zero be handled better?
        // TODO: can lhs undef be handled better?
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, result is undefined.
        if (is_int) {
            if (maybe_lhs_val) |lhs_val| {
                if (lhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, lhs_src);
                }
            }
            if (maybe_rhs_val) |rhs_val| {
                if (rhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                    return sema.failWithDivideByZero(block, rhs_src);
                }
                if (maybe_lhs_val) |lhs_val| {
                    return sema.addConstant(
                        resolved_type,
                        try lhs_val.intMod(rhs_val, resolved_type, sema.arena, mod),
                    );
                }
                break :rs lhs_src;
            } else {
                break :rs rhs_src;
            }
        }
        // float operands
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
        }
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                return sema.addConstUndef(resolved_type);
            }
            if (maybe_rhs_val) |rhs_val| {
                return sema.addConstant(
                    resolved_type,
                    try lhs_val.floatMod(rhs_val, resolved_type, sema.arena, mod),
                );
            } else break :rs rhs_src;
        } else break :rs lhs_src;
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    const air_tag = airTag(block, is_int, .mod, .mod_optimized);
    return block.addBinOp(air_tag, casted_lhs, casted_rhs);
}

fn zirRem(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src: LazySrcLoc = .{ .node_offset_bin_op = inst_data.src_node };
    sema.src = src;
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    try sema.checkInvalidPtrArithmetic(block, src, lhs_ty);

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, .rem);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);

    const runtime_src = rs: {
        // For integers:
        // Either operand being undef is a compile error because there exists
        // a possible value (TODO what is it?) that would invoke illegal behavior.
        // TODO: can lhs zero be handled better?
        // TODO: can lhs undef be handled better?
        //
        // For floats:
        // If the rhs is zero, compile error for division by zero.
        // If the rhs is undefined, compile error because there is a possible
        // value (zero) for which the division would be illegal behavior.
        // If the lhs is undefined, result is undefined.
        if (is_int) {
            if (maybe_lhs_val) |lhs_val| {
                if (lhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, lhs_src);
                }
            }
            if (maybe_rhs_val) |rhs_val| {
                if (rhs_val.isUndef()) {
                    return sema.failWithUseOfUndef(block, rhs_src);
                }
                if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                    return sema.failWithDivideByZero(block, rhs_src);
                }
                if (maybe_lhs_val) |lhs_val| {
                    return sema.addConstant(
                        resolved_type,
                        try sema.intRem(resolved_type, lhs_val, rhs_val),
                    );
                }
                break :rs lhs_src;
            } else {
                break :rs rhs_src;
            }
        }
        // float operands
        if (maybe_rhs_val) |rhs_val| {
            if (rhs_val.isUndef()) {
                return sema.failWithUseOfUndef(block, rhs_src);
            }
            if (!(try rhs_val.compareAllWithZeroAdvanced(.neq, sema))) {
                return sema.failWithDivideByZero(block, rhs_src);
            }
        }
        if (maybe_lhs_val) |lhs_val| {
            if (lhs_val.isUndef()) {
                return sema.addConstUndef(resolved_type);
            }
            if (maybe_rhs_val) |rhs_val| {
                return sema.addConstant(
                    resolved_type,
                    try lhs_val.floatRem(rhs_val, resolved_type, sema.arena, mod),
                );
            } else break :rs rhs_src;
        } else break :rs lhs_src;
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (block.wantSafety()) {
        try sema.addDivByZeroSafety(block, resolved_type, maybe_rhs_val, casted_rhs, is_int);
    }

    const air_tag = airTag(block, is_int, .rem, .rem_optimized);
    return block.addBinOp(air_tag, casted_lhs, casted_rhs);
}

fn zirOverflowArithmetic(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    zir_tag: Zir.Inst.Extended,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.BinNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);

    const lhs_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const rhs_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = extra.node };

    const uncasted_lhs = try sema.resolveInst(extra.lhs);
    const uncasted_rhs = try sema.resolveInst(extra.rhs);

    const lhs_ty = sema.typeOf(uncasted_lhs);
    const rhs_ty = sema.typeOf(uncasted_rhs);
    const mod = sema.mod;

    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);

    const instructions = &[_]Air.Inst.Ref{ uncasted_lhs, uncasted_rhs };
    const dest_ty = if (zir_tag == .shl_with_overflow)
        lhs_ty
    else
        try sema.resolvePeerTypes(block, src, instructions, .{
            .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
        });

    const rhs_dest_ty = if (zir_tag == .shl_with_overflow)
        try sema.log2IntType(block, lhs_ty, src)
    else
        dest_ty;

    const lhs = try sema.coerce(block, dest_ty, uncasted_lhs, lhs_src);
    const rhs = try sema.coerce(block, rhs_dest_ty, uncasted_rhs, rhs_src);

    if (dest_ty.scalarType().zigTypeTag() != .Int) {
        return sema.fail(block, src, "expected vector of integers or integer tag type, found '{}'", .{dest_ty.fmt(mod)});
    }

    const maybe_lhs_val = try sema.resolveMaybeUndefVal(lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefVal(rhs);

    const tuple_ty = try sema.overflowArithmeticTupleType(dest_ty);

    var result: struct {
        inst: Air.Inst.Ref = .none,
        wrapped: Value = Value.initTag(.unreachable_value),
        overflow_bit: Value,
    } = result: {
        switch (zir_tag) {
            .add_with_overflow => {
                // If either of the arguments is zero, `false` is returned and the other is stored
                // to the result, even if it is undefined..
                // Otherwise, if either of the argument is undefined, undefined is returned.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef() and (try lhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = rhs };
                    }
                }
                if (maybe_rhs_val) |rhs_val| {
                    if (!rhs_val.isUndef() and (try rhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                    }
                }
                if (maybe_lhs_val) |lhs_val| {
                    if (maybe_rhs_val) |rhs_val| {
                        if (lhs_val.isUndef() or rhs_val.isUndef()) {
                            break :result .{ .overflow_bit = Value.undef, .wrapped = Value.undef };
                        }

                        const result = try sema.intAddWithOverflow(lhs_val, rhs_val, dest_ty);
                        break :result .{ .overflow_bit = result.overflow_bit, .wrapped = result.wrapped_result };
                    }
                }
            },
            .sub_with_overflow => {
                // If the rhs is zero, then the result is lhs and no overflow occured.
                // Otherwise, if either result is undefined, both results are undefined.
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        break :result .{ .overflow_bit = Value.undef, .wrapped = Value.undef };
                    } else if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                    } else if (maybe_lhs_val) |lhs_val| {
                        if (lhs_val.isUndef()) {
                            break :result .{ .overflow_bit = Value.undef, .wrapped = Value.undef };
                        }

                        const result = try sema.intSubWithOverflow(lhs_val, rhs_val, dest_ty);
                        break :result .{ .overflow_bit = result.overflow_bit, .wrapped = result.wrapped_result };
                    }
                }
            },
            .mul_with_overflow => {
                // If either of the arguments is zero, the result is zero and no overflow occured.
                // If either of the arguments is one, the result is the other and no overflow occured.
                // Otherwise, if either of the arguments is undefined, both results are undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef()) {
                        if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                            break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                        } else if (try sema.compareAll(lhs_val, .eq, try maybeRepeated(sema, dest_ty, Value.one), dest_ty)) {
                            break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = rhs };
                        }
                    }
                }

                if (maybe_rhs_val) |rhs_val| {
                    if (!rhs_val.isUndef()) {
                        if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                            break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = rhs };
                        } else if (try sema.compareAll(rhs_val, .eq, try maybeRepeated(sema, dest_ty, Value.one), dest_ty)) {
                            break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                        }
                    }
                }

                if (maybe_lhs_val) |lhs_val| {
                    if (maybe_rhs_val) |rhs_val| {
                        if (lhs_val.isUndef() or rhs_val.isUndef()) {
                            break :result .{ .overflow_bit = Value.undef, .wrapped = Value.undef };
                        }

                        const result = try lhs_val.intMulWithOverflow(rhs_val, dest_ty, sema.arena, mod);
                        break :result .{ .overflow_bit = result.overflow_bit, .wrapped = result.wrapped_result };
                    }
                }
            },
            .shl_with_overflow => {
                // If lhs is zero, the result is zero and no overflow occurred.
                // If rhs is zero, the result is lhs (even if undefined) and no overflow occurred.
                // Oterhwise if either of the arguments is undefined, both results are undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef() and (try lhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                    }
                }
                if (maybe_rhs_val) |rhs_val| {
                    if (!rhs_val.isUndef() and (try rhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        break :result .{ .overflow_bit = try maybeRepeated(sema, dest_ty, Value.zero), .inst = lhs };
                    }
                }
                if (maybe_lhs_val) |lhs_val| {
                    if (maybe_rhs_val) |rhs_val| {
                        if (lhs_val.isUndef() or rhs_val.isUndef()) {
                            break :result .{ .overflow_bit = Value.undef, .wrapped = Value.undef };
                        }

                        const result = try lhs_val.shlWithOverflow(rhs_val, dest_ty, sema.arena, sema.mod);
                        break :result .{ .overflow_bit = result.overflow_bit, .wrapped = result.wrapped_result };
                    }
                }
            },
            else => unreachable,
        }

        const air_tag: Air.Inst.Tag = switch (zir_tag) {
            .add_with_overflow => .add_with_overflow,
            .mul_with_overflow => .mul_with_overflow,
            .sub_with_overflow => .sub_with_overflow,
            .shl_with_overflow => .shl_with_overflow,
            else => unreachable,
        };

        const runtime_src = if (maybe_lhs_val == null) lhs_src else rhs_src;
        try sema.requireRuntimeBlock(block, src, runtime_src);

        return block.addInst(.{
            .tag = air_tag,
            .data = .{ .ty_pl = .{
                .ty = try block.sema.addType(tuple_ty),
                .payload = try block.sema.addExtra(Air.Bin{
                    .lhs = lhs,
                    .rhs = rhs,
                }),
            } },
        });
    };

    if (result.inst != .none) {
        if (try sema.resolveMaybeUndefVal(result.inst)) |some| {
            result.wrapped = some;
            result.inst = .none;
        }
    }

    if (result.inst == .none) {
        const values = try sema.arena.alloc(Value, 2);
        values[0] = result.wrapped;
        values[1] = result.overflow_bit;
        const tuple_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstant(tuple_ty, tuple_val);
    }

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, 2);
    element_refs[0] = result.inst;
    element_refs[1] = try sema.addConstant(tuple_ty.structFieldType(1), result.overflow_bit);
    return block.addAggregateInit(tuple_ty, element_refs);
}

fn maybeRepeated(sema: *Sema, ty: Type, val: Value) !Value {
    if (ty.zigTypeTag() != .Vector) return val;
    return Value.Tag.repeated.create(sema.arena, val);
}

fn overflowArithmeticTupleType(sema: *Sema, ty: Type) !Type {
    const ov_ty = if (ty.zigTypeTag() == .Vector) try Type.vector(sema.arena, ty.vectorLen(), Type.u1) else Type.u1;

    const types = try sema.arena.alloc(Type, 2);
    const values = try sema.arena.alloc(Value, 2);
    const tuple_ty = try Type.Tag.tuple.create(sema.arena, .{
        .types = types,
        .values = values,
    });

    types[0] = ty;
    types[1] = ov_ty;
    values[0] = Value.initTag(.unreachable_value);
    values[1] = Value.initTag(.unreachable_value);

    return tuple_ty;
}

fn analyzeArithmetic(
    sema: *Sema,
    block: *Block,
    /// TODO performance investigation: make this comptime?
    zir_tag: Zir.Inst.Tag,
    lhs: Air.Inst.Ref,
    rhs: Air.Inst.Ref,
    src: LazySrcLoc,
    lhs_src: LazySrcLoc,
    rhs_src: LazySrcLoc,
    want_safety: bool,
) CompileError!Air.Inst.Ref {
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_zig_ty_tag = try lhs_ty.zigTypeTagOrPoison();
    const rhs_zig_ty_tag = try rhs_ty.zigTypeTagOrPoison();
    try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);

    if (lhs_zig_ty_tag == .Pointer) switch (lhs_ty.ptrSize()) {
        .One, .Slice => {},
        .Many, .C => {
            const air_tag: Air.Inst.Tag = switch (zir_tag) {
                .add => .ptr_add,
                .sub => .ptr_sub,
                else => return sema.fail(block, src, "invalid pointer arithmetic operator", .{}),
            };
            return sema.analyzePtrArithmetic(block, src, lhs, rhs, air_tag, lhs_src, rhs_src);
        },
    };

    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{
        .override = &[_]LazySrcLoc{ lhs_src, rhs_src },
    });

    const is_vector = resolved_type.zigTypeTag() == .Vector;

    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);

    const scalar_tag = resolved_type.scalarType().zigTypeTag();

    const is_int = scalar_tag == .Int or scalar_tag == .ComptimeInt;

    try sema.checkArithmeticOp(block, src, scalar_tag, lhs_zig_ty_tag, rhs_zig_ty_tag, zir_tag);

    const mod = sema.mod;
    const maybe_lhs_val = try sema.resolveMaybeUndefValIntable(casted_lhs);
    const maybe_rhs_val = try sema.resolveMaybeUndefValIntable(casted_rhs);
    const rs: struct { src: LazySrcLoc, air_tag: Air.Inst.Tag } = rs: {
        switch (zir_tag) {
            .add, .add_unsafe => {
                // For integers:intAddSat
                // If either of the operands are zero, then the other operand is
                // returned, even if it is undefined.
                // If either of the operands are undefined, it's a compile error
                // because there is a possible value for which the addition would
                // overflow (max_int), causing illegal behavior.
                // For floats: either operand being undef makes the result undef.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef() and (try lhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        return casted_rhs;
                    }
                }
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        if (is_int) {
                            return sema.failWithUseOfUndef(block, rhs_src);
                        } else {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .add_optimized else .add;
                if (maybe_lhs_val) |lhs_val| {
                    if (lhs_val.isUndef()) {
                        if (is_int) {
                            return sema.failWithUseOfUndef(block, lhs_src);
                        } else {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    if (maybe_rhs_val) |rhs_val| {
                        if (is_int) {
                            const sum = try sema.intAdd(lhs_val, rhs_val, resolved_type);
                            var vector_index: usize = undefined;
                            if (!(try sema.intFitsInType(sum, resolved_type, &vector_index))) {
                                return sema.failWithIntegerOverflow(block, src, resolved_type, sum, vector_index);
                            }
                            return sema.addConstant(resolved_type, sum);
                        } else {
                            return sema.addConstant(
                                resolved_type,
                                try sema.floatAdd(lhs_val, rhs_val, resolved_type),
                            );
                        }
                    } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
            },
            .addwrap => {
                // Integers only; floats are checked above.
                // If either of the operands are zero, the other operand is returned.
                // If either of the operands are undefined, the result is undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef() and (try lhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        return casted_rhs;
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .addwrap_optimized else .addwrap;
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                    if (maybe_lhs_val) |lhs_val| {
                        return sema.addConstant(
                            resolved_type,
                            try sema.numberAddWrapScalar(lhs_val, rhs_val, resolved_type),
                        );
                    } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
            },
            .add_sat => {
                // Integers only; floats are checked above.
                // If either of the operands are zero, then the other operand is returned.
                // If either of the operands are undefined, the result is undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef() and (try lhs_val.compareAllWithZeroAdvanced(.eq, sema))) {
                        return casted_rhs;
                    }
                }
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                    if (maybe_lhs_val) |lhs_val| {
                        const val = if (scalar_tag == .ComptimeInt)
                            try sema.intAdd(lhs_val, rhs_val, resolved_type)
                        else
                            try lhs_val.intAddSat(rhs_val, resolved_type, sema.arena, mod);

                        return sema.addConstant(resolved_type, val);
                    } else break :rs .{ .src = lhs_src, .air_tag = .add_sat };
                } else break :rs .{ .src = rhs_src, .air_tag = .add_sat };
            },
            .sub => {
                // For integers:
                // If the rhs is zero, then the other operand is
                // returned, even if it is undefined.
                // If either of the operands are undefined, it's a compile error
                // because there is a possible value for which the subtraction would
                // overflow, causing illegal behavior.
                // For floats: either operand being undef makes the result undef.
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        if (is_int) {
                            return sema.failWithUseOfUndef(block, rhs_src);
                        } else {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .sub_optimized else .sub;
                if (maybe_lhs_val) |lhs_val| {
                    if (lhs_val.isUndef()) {
                        if (is_int) {
                            return sema.failWithUseOfUndef(block, lhs_src);
                        } else {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    if (maybe_rhs_val) |rhs_val| {
                        if (is_int) {
                            const diff = try sema.intSub(lhs_val, rhs_val, resolved_type);
                            var vector_index: usize = undefined;
                            if (!(try sema.intFitsInType(diff, resolved_type, &vector_index))) {
                                return sema.failWithIntegerOverflow(block, src, resolved_type, diff, vector_index);
                            }
                            return sema.addConstant(resolved_type, diff);
                        } else {
                            return sema.addConstant(
                                resolved_type,
                                try sema.floatSub(lhs_val, rhs_val, resolved_type),
                            );
                        }
                    } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
            },
            .subwrap => {
                // Integers only; floats are checked above.
                // If the RHS is zero, then the other operand is returned, even if it is undefined.
                // If either of the operands are undefined, the result is undefined.
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .subwrap_optimized else .subwrap;
                if (maybe_lhs_val) |lhs_val| {
                    if (lhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (maybe_rhs_val) |rhs_val| {
                        return sema.addConstant(
                            resolved_type,
                            try sema.numberSubWrapScalar(lhs_val, rhs_val, resolved_type),
                        );
                    } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
            },
            .sub_sat => {
                // Integers only; floats are checked above.
                // If the RHS is zero, result is LHS.
                // If either of the operands are undefined, result is undefined.
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        return casted_lhs;
                    }
                }
                if (maybe_lhs_val) |lhs_val| {
                    if (lhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (maybe_rhs_val) |rhs_val| {
                        const val = if (scalar_tag == .ComptimeInt)
                            try sema.intSub(lhs_val, rhs_val, resolved_type)
                        else
                            try lhs_val.intSubSat(rhs_val, resolved_type, sema.arena, mod);

                        return sema.addConstant(resolved_type, val);
                    } else break :rs .{ .src = rhs_src, .air_tag = .sub_sat };
                } else break :rs .{ .src = lhs_src, .air_tag = .sub_sat };
            },
            .mul => {
                // For integers:
                // If either of the operands are zero, the result is zero.
                // If either of the operands are one, the result is the other
                // operand, even if it is undefined.
                // If either of the operands are undefined, it's a compile error
                // because there is a possible value for which the addition would
                // overflow (max_int), causing illegal behavior.
                // For floats: either operand being undef makes the result undef.
                // If either of the operands are inf, and the other operand is zero,
                // the result is nan.
                // If either of the operands are nan, the result is nan.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef()) {
                        if (lhs_val.isNan()) {
                            return sema.addConstant(resolved_type, lhs_val);
                        }
                        if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) lz: {
                            if (maybe_rhs_val) |rhs_val| {
                                if (rhs_val.isNan()) {
                                    return sema.addConstant(resolved_type, rhs_val);
                                }
                                if (rhs_val.isInf()) {
                                    return sema.addConstant(resolved_type, try Value.Tag.float_32.create(sema.arena, std.math.nan_f32));
                                }
                            } else if (resolved_type.isAnyFloat()) {
                                break :lz;
                            }
                            const zero_val = if (is_vector) b: {
                                break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                            } else Value.zero;
                            return sema.addConstant(resolved_type, zero_val);
                        }
                        if (try sema.compareAll(lhs_val, .eq, Value.one, resolved_type)) {
                            return casted_rhs;
                        }
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .mul_optimized else .mul;
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        if (is_int) {
                            return sema.failWithUseOfUndef(block, rhs_src);
                        } else {
                            return sema.addConstUndef(resolved_type);
                        }
                    }
                    if (rhs_val.isNan()) {
                        return sema.addConstant(resolved_type, rhs_val);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) rz: {
                        if (maybe_lhs_val) |lhs_val| {
                            if (lhs_val.isInf()) {
                                return sema.addConstant(resolved_type, try Value.Tag.float_32.create(sema.arena, std.math.nan_f32));
                            }
                        } else if (resolved_type.isAnyFloat()) {
                            break :rz;
                        }
                        const zero_val = if (is_vector) b: {
                            break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                        } else Value.zero;
                        return sema.addConstant(resolved_type, zero_val);
                    }
                    if (try sema.compareAll(rhs_val, .eq, Value.one, resolved_type)) {
                        return casted_lhs;
                    }
                    if (maybe_lhs_val) |lhs_val| {
                        if (lhs_val.isUndef()) {
                            if (is_int) {
                                return sema.failWithUseOfUndef(block, lhs_src);
                            } else {
                                return sema.addConstUndef(resolved_type);
                            }
                        }
                        if (is_int) {
                            const product = try lhs_val.intMul(rhs_val, resolved_type, sema.arena, sema.mod);
                            var vector_index: usize = undefined;
                            if (!(try sema.intFitsInType(product, resolved_type, &vector_index))) {
                                return sema.failWithIntegerOverflow(block, src, resolved_type, product, vector_index);
                            }
                            return sema.addConstant(resolved_type, product);
                        } else {
                            return sema.addConstant(
                                resolved_type,
                                try lhs_val.floatMul(rhs_val, resolved_type, sema.arena, sema.mod),
                            );
                        }
                    } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
            },
            .mulwrap => {
                // Integers only; floats are handled above.
                // If either of the operands are zero, result is zero.
                // If either of the operands are one, result is the other operand.
                // If either of the operands are undefined, result is undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef()) {
                        if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                            const zero_val = if (is_vector) b: {
                                break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                            } else Value.zero;
                            return sema.addConstant(resolved_type, zero_val);
                        }
                        if (try sema.compareAll(lhs_val, .eq, Value.one, resolved_type)) {
                            return casted_rhs;
                        }
                    }
                }
                const air_tag: Air.Inst.Tag = if (block.float_mode == .Optimized) .mulwrap_optimized else .mulwrap;
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        const zero_val = if (is_vector) b: {
                            break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                        } else Value.zero;
                        return sema.addConstant(resolved_type, zero_val);
                    }
                    if (try sema.compareAll(rhs_val, .eq, Value.one, resolved_type)) {
                        return casted_lhs;
                    }
                    if (maybe_lhs_val) |lhs_val| {
                        if (lhs_val.isUndef()) {
                            return sema.addConstUndef(resolved_type);
                        }
                        return sema.addConstant(
                            resolved_type,
                            try lhs_val.numberMulWrap(rhs_val, resolved_type, sema.arena, sema.mod),
                        );
                    } else break :rs .{ .src = lhs_src, .air_tag = air_tag };
                } else break :rs .{ .src = rhs_src, .air_tag = air_tag };
            },
            .mul_sat => {
                // Integers only; floats are checked above.
                // If either of the operands are zero, result is zero.
                // If either of the operands are one, result is the other operand.
                // If either of the operands are undefined, result is undefined.
                if (maybe_lhs_val) |lhs_val| {
                    if (!lhs_val.isUndef()) {
                        if (try lhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                            const zero_val = if (is_vector) b: {
                                break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                            } else Value.zero;
                            return sema.addConstant(resolved_type, zero_val);
                        }
                        if (try sema.compareAll(lhs_val, .eq, Value.one, resolved_type)) {
                            return casted_rhs;
                        }
                    }
                }
                if (maybe_rhs_val) |rhs_val| {
                    if (rhs_val.isUndef()) {
                        return sema.addConstUndef(resolved_type);
                    }
                    if (try rhs_val.compareAllWithZeroAdvanced(.eq, sema)) {
                        const zero_val = if (is_vector) b: {
                            break :b try Value.Tag.repeated.create(sema.arena, Value.zero);
                        } else Value.zero;
                        return sema.addConstant(resolved_type, zero_val);
                    }
                    if (try sema.compareAll(rhs_val, .eq, Value.one, resolved_type)) {
                        return casted_lhs;
                    }
                    if (maybe_lhs_val) |lhs_val| {
                        if (lhs_val.isUndef()) {
                            return sema.addConstUndef(resolved_type);
                        }

                        const val = if (scalar_tag == .ComptimeInt)
                            try lhs_val.intMul(rhs_val, resolved_type, sema.arena, sema.mod)
                        else
                            try lhs_val.intMulSat(rhs_val, resolved_type, sema.arena, sema.mod);

                        return sema.addConstant(resolved_type, val);
                    } else break :rs .{ .src = lhs_src, .air_tag = .mul_sat };
                } else break :rs .{ .src = rhs_src, .air_tag = .mul_sat };
            },
            else => unreachable,
        }
    };

    try sema.requireRuntimeBlock(block, src, rs.src);
    if (block.wantSafety() and want_safety) {
        if (scalar_tag == .Int) {
            const maybe_op_ov: ?Air.Inst.Tag = switch (rs.air_tag) {
                .add => .add_with_overflow,
                .sub => .sub_with_overflow,
                .mul => .mul_with_overflow,
                else => null,
            };
            if (maybe_op_ov) |op_ov_tag| {
                const op_ov_tuple_ty = try sema.overflowArithmeticTupleType(resolved_type);
                const op_ov = try block.addInst(.{
                    .tag = op_ov_tag,
                    .data = .{ .ty_pl = .{
                        .ty = try sema.addType(op_ov_tuple_ty),
                        .payload = try sema.addExtra(Air.Bin{
                            .lhs = casted_lhs,
                            .rhs = casted_rhs,
                        }),
                    } },
                });
                const ov_bit = try sema.tupleFieldValByIndex(block, src, op_ov, 1, op_ov_tuple_ty);
                const any_ov_bit = if (resolved_type.zigTypeTag() == .Vector)
                    try block.addInst(.{
                        .tag = if (block.float_mode == .Optimized) .reduce_optimized else .reduce,
                        .data = .{ .reduce = .{
                            .operand = ov_bit,
                            .operation = .Or,
                        } },
                    })
                else
                    ov_bit;
                const zero_ov = try sema.addConstant(Type.u1, Value.zero);
                const no_ov = try block.addBinOp(.cmp_eq, any_ov_bit, zero_ov);

                try sema.addSafetyCheck(block, no_ov, .integer_overflow);
                return sema.tupleFieldValByIndex(block, src, op_ov, 0, op_ov_tuple_ty);
            }
        }
    }
    return block.addBinOp(rs.air_tag, casted_lhs, casted_rhs);
}

fn analyzePtrArithmetic(
    sema: *Sema,
    block: *Block,
    op_src: LazySrcLoc,
    ptr: Air.Inst.Ref,
    uncasted_offset: Air.Inst.Ref,
    air_tag: Air.Inst.Tag,
    ptr_src: LazySrcLoc,
    offset_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    // TODO if the operand is comptime-known to be negative, or is a negative int,
    // coerce to isize instead of usize.
    const offset = try sema.coerce(block, Type.usize, uncasted_offset, offset_src);
    const target = sema.mod.getTarget();
    const opt_ptr_val = try sema.resolveMaybeUndefVal(ptr);
    const opt_off_val = try sema.resolveDefinedValue(block, offset_src, offset);
    const ptr_ty = sema.typeOf(ptr);
    const ptr_info = ptr_ty.ptrInfo().data;
    const elem_ty = if (ptr_info.size == .One and ptr_info.pointee_type.zigTypeTag() == .Array)
        ptr_info.pointee_type.childType()
    else
        ptr_info.pointee_type;

    const new_ptr_ty = t: {
        // Calculate the new pointer alignment.
        // This code is duplicated in `elemPtrType`.
        if (ptr_info.@"align" == 0) {
            // ABI-aligned pointer. Any pointer arithmetic maintains the same ABI-alignedness.
            break :t ptr_ty;
        }
        // If the addend is not a comptime-known value we can still count on
        // it being a multiple of the type size.
        const elem_size = elem_ty.abiSize(target);
        const addend = if (opt_off_val) |off_val| a: {
            const off_int = try sema.usizeCast(block, offset_src, off_val.toUnsignedInt(target));
            break :a elem_size * off_int;
        } else elem_size;

        // The resulting pointer is aligned to the lcd between the offset (an
        // arbitrary number) and the alignment factor (always a power of two,
        // non zero).
        const new_align = @as(u32, 1) << @intCast(u5, @ctz(addend | ptr_info.@"align"));

        break :t try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = ptr_info.pointee_type,
            .sentinel = ptr_info.sentinel,
            .@"align" = new_align,
            .@"addrspace" = ptr_info.@"addrspace",
            .mutable = ptr_info.mutable,
            .@"allowzero" = ptr_info.@"allowzero",
            .@"volatile" = ptr_info.@"volatile",
            .size = ptr_info.size,
        });
    };

    const runtime_src = rs: {
        if (opt_ptr_val) |ptr_val| {
            if (opt_off_val) |offset_val| {
                if (ptr_val.isUndef()) return sema.addConstUndef(new_ptr_ty);

                const offset_int = try sema.usizeCast(block, offset_src, offset_val.toUnsignedInt(target));
                if (offset_int == 0) return ptr;
                if (try ptr_val.getUnsignedIntAdvanced(target, sema)) |addr| {
                    const elem_size = elem_ty.abiSize(target);
                    const new_addr = switch (air_tag) {
                        .ptr_add => addr + elem_size * offset_int,
                        .ptr_sub => addr - elem_size * offset_int,
                        else => unreachable,
                    };
                    const new_ptr_val = try Value.Tag.int_u64.create(sema.arena, new_addr);
                    return sema.addConstant(new_ptr_ty, new_ptr_val);
                }
                if (air_tag == .ptr_sub) {
                    return sema.fail(block, op_src, "TODO implement Sema comptime pointer subtraction", .{});
                }
                const new_ptr_val = try ptr_val.elemPtr(ptr_ty, sema.arena, offset_int, sema.mod);
                return sema.addConstant(new_ptr_ty, new_ptr_val);
            } else break :rs offset_src;
        } else break :rs ptr_src;
    };

    try sema.requireRuntimeBlock(block, op_src, runtime_src);
    return block.addInst(.{
        .tag = air_tag,
        .data = .{ .ty_pl = .{
            .ty = try sema.addType(new_ptr_ty),
            .payload = try sema.addExtra(Air.Bin{
                .lhs = ptr,
                .rhs = offset,
            }),
        } },
    });
}

fn zirLoad(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ptr_src = src; // TODO better source location
    const ptr = try sema.resolveInst(inst_data.operand);
    return sema.analyzeLoad(block, src, ptr, ptr_src);
}

fn zirAsm(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
    tmpl_is_expr: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.Asm, extended.operand);
    const src = LazySrcLoc.nodeOffset(extra.data.src_node);
    const ret_ty_src: LazySrcLoc = .{ .node_offset_asm_ret_ty = extra.data.src_node };
    const outputs_len = @truncate(u5, extended.small);
    const inputs_len = @truncate(u5, extended.small >> 5);
    const clobbers_len = @truncate(u5, extended.small >> 10);
    const is_volatile = @truncate(u1, extended.small >> 15) != 0;
    const is_global_assembly = sema.func == null;

    const asm_source: []const u8 = if (tmpl_is_expr) blk: {
        const tmpl = @intToEnum(Zir.Inst.Ref, extra.data.asm_source);
        const s: []const u8 = try sema.resolveConstString(block, src, tmpl, "assembly code must be comptime-known");
        break :blk s;
    } else sema.code.nullTerminatedString(extra.data.asm_source);

    if (is_global_assembly) {
        if (outputs_len != 0) {
            return sema.fail(block, src, "module-level assembly does not support outputs", .{});
        }
        if (inputs_len != 0) {
            return sema.fail(block, src, "module-level assembly does not support inputs", .{});
        }
        if (clobbers_len != 0) {
            return sema.fail(block, src, "module-level assembly does not support clobbers", .{});
        }
        if (is_volatile) {
            return sema.fail(block, src, "volatile keyword is redundant on module-level assembly", .{});
        }
        try sema.mod.addGlobalAssembly(sema.owner_decl_index, asm_source);
        return Air.Inst.Ref.void_value;
    }

    if (block.is_comptime) {
        try sema.requireRuntimeBlock(block, src, null);
    }

    var extra_i = extra.end;
    var output_type_bits = extra.data.output_type_bits;
    var needed_capacity: usize = @typeInfo(Air.Asm).Struct.fields.len + outputs_len + inputs_len;

    const ConstraintName = struct { c: []const u8, n: []const u8 };
    const out_args = try sema.arena.alloc(Air.Inst.Ref, outputs_len);
    const outputs = try sema.arena.alloc(ConstraintName, outputs_len);
    var expr_ty = Air.Inst.Ref.void_type;

    for (out_args, 0..) |*arg, out_i| {
        const output = sema.code.extraData(Zir.Inst.Asm.Output, extra_i);
        extra_i = output.end;

        const is_type = @truncate(u1, output_type_bits) != 0;
        output_type_bits >>= 1;

        if (is_type) {
            // Indicate the output is the asm instruction return value.
            arg.* = .none;
            const out_ty = try sema.resolveType(block, ret_ty_src, output.data.operand);
            try sema.queueFullTypeResolution(out_ty);
            expr_ty = try sema.addType(out_ty);
        } else {
            arg.* = try sema.resolveInst(output.data.operand);
        }

        const constraint = sema.code.nullTerminatedString(output.data.constraint);
        const name = sema.code.nullTerminatedString(output.data.name);
        needed_capacity += (constraint.len + name.len + (2 + 3)) / 4;

        outputs[out_i] = .{ .c = constraint, .n = name };
    }

    const args = try sema.arena.alloc(Air.Inst.Ref, inputs_len);
    const inputs = try sema.arena.alloc(ConstraintName, inputs_len);

    for (args, 0..) |*arg, arg_i| {
        const input = sema.code.extraData(Zir.Inst.Asm.Input, extra_i);
        extra_i = input.end;

        const uncasted_arg = try sema.resolveInst(input.data.operand);
        const uncasted_arg_ty = sema.typeOf(uncasted_arg);
        switch (uncasted_arg_ty.zigTypeTag()) {
            .ComptimeInt => arg.* = try sema.coerce(block, Type.initTag(.usize), uncasted_arg, src),
            .ComptimeFloat => arg.* = try sema.coerce(block, Type.initTag(.f64), uncasted_arg, src),
            else => {
                arg.* = uncasted_arg;
                try sema.queueFullTypeResolution(uncasted_arg_ty);
            },
        }

        const constraint = sema.code.nullTerminatedString(input.data.constraint);
        const name = sema.code.nullTerminatedString(input.data.name);
        needed_capacity += (constraint.len + name.len + (2 + 3)) / 4;
        inputs[arg_i] = .{ .c = constraint, .n = name };
    }

    const clobbers = try sema.arena.alloc([]const u8, clobbers_len);
    for (clobbers) |*name| {
        name.* = sema.code.nullTerminatedString(sema.code.extra[extra_i]);
        extra_i += 1;

        needed_capacity += name.*.len / 4 + 1;
    }

    needed_capacity += (asm_source.len + 3) / 4;

    const gpa = sema.gpa;
    try sema.air_extra.ensureUnusedCapacity(gpa, needed_capacity);
    const asm_air = try block.addInst(.{
        .tag = .assembly,
        .data = .{ .ty_pl = .{
            .ty = expr_ty,
            .payload = sema.addExtraAssumeCapacity(Air.Asm{
                .source_len = @intCast(u32, asm_source.len),
                .outputs_len = outputs_len,
                .inputs_len = @intCast(u32, args.len),
                .flags = (@as(u32, @boolToInt(is_volatile)) << 31) | @intCast(u32, clobbers.len),
            }),
        } },
    });
    sema.appendRefsAssumeCapacity(out_args);
    sema.appendRefsAssumeCapacity(args);
    for (outputs) |o| {
        const buffer = mem.sliceAsBytes(sema.air_extra.unusedCapacitySlice());
        mem.copy(u8, buffer, o.c);
        buffer[o.c.len] = 0;
        mem.copy(u8, buffer[o.c.len + 1 ..], o.n);
        buffer[o.c.len + 1 + o.n.len] = 0;
        sema.air_extra.items.len += (o.c.len + o.n.len + (2 + 3)) / 4;
    }
    for (inputs) |input| {
        const buffer = mem.sliceAsBytes(sema.air_extra.unusedCapacitySlice());
        mem.copy(u8, buffer, input.c);
        buffer[input.c.len] = 0;
        mem.copy(u8, buffer[input.c.len + 1 ..], input.n);
        buffer[input.c.len + 1 + input.n.len] = 0;
        sema.air_extra.items.len += (input.c.len + input.n.len + (2 + 3)) / 4;
    }
    for (clobbers) |clobber| {
        const buffer = mem.sliceAsBytes(sema.air_extra.unusedCapacitySlice());
        mem.copy(u8, buffer, clobber);
        buffer[clobber.len] = 0;
        sema.air_extra.items.len += clobber.len / 4 + 1;
    }
    {
        const buffer = mem.sliceAsBytes(sema.air_extra.unusedCapacitySlice());
        mem.copy(u8, buffer, asm_source);
        sema.air_extra.items.len += (asm_source.len + 3) / 4;
    }
    return asm_air;
}

/// Only called for equality operators. See also `zirCmp`.
fn zirCmpEq(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    op: std.math.CompareOperator,
    air_tag: Air.Inst.Tag,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const src: LazySrcLoc = inst_data.src();
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);

    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    const lhs_ty_tag = lhs_ty.zigTypeTag();
    const rhs_ty_tag = rhs_ty.zigTypeTag();
    if (lhs_ty_tag == .Null and rhs_ty_tag == .Null) {
        // null == null, null != null
        if (op == .eq) {
            return Air.Inst.Ref.bool_true;
        } else {
            return Air.Inst.Ref.bool_false;
        }
    }

    // comparing null with optionals
    if (lhs_ty_tag == .Null and (rhs_ty_tag == .Optional or rhs_ty.isCPtr())) {
        return sema.analyzeIsNull(block, src, rhs, op == .neq);
    }
    if (rhs_ty_tag == .Null and (lhs_ty_tag == .Optional or lhs_ty.isCPtr())) {
        return sema.analyzeIsNull(block, src, lhs, op == .neq);
    }

    if (lhs_ty_tag == .Null or rhs_ty_tag == .Null) {
        const non_null_type = if (lhs_ty_tag == .Null) rhs_ty else lhs_ty;
        return sema.fail(block, src, "comparison of '{}' with null", .{non_null_type.fmt(sema.mod)});
    }

    if (lhs_ty_tag == .Union and (rhs_ty_tag == .EnumLiteral or rhs_ty_tag == .Enum)) {
        return sema.analyzeCmpUnionTag(block, src, lhs, lhs_src, rhs, rhs_src, op);
    }
    if (rhs_ty_tag == .Union and (lhs_ty_tag == .EnumLiteral or lhs_ty_tag == .Enum)) {
        return sema.analyzeCmpUnionTag(block, src, rhs, rhs_src, lhs, lhs_src, op);
    }

    if (lhs_ty_tag == .ErrorSet and rhs_ty_tag == .ErrorSet) {
        const runtime_src: LazySrcLoc = src: {
            if (try sema.resolveMaybeUndefVal(lhs)) |lval| {
                if (try sema.resolveMaybeUndefVal(rhs)) |rval| {
                    if (lval.isUndef() or rval.isUndef()) {
                        return sema.addConstUndef(Type.bool);
                    }
                    // TODO optimisation opportunity: evaluate if mem.eql is faster with the names,
                    // or calling to Module.getErrorValue to get the values and then compare them is
                    // faster.
                    const lhs_name = lval.castTag(.@"error").?.data.name;
                    const rhs_name = rval.castTag(.@"error").?.data.name;
                    if (mem.eql(u8, lhs_name, rhs_name) == (op == .eq)) {
                        return Air.Inst.Ref.bool_true;
                    } else {
                        return Air.Inst.Ref.bool_false;
                    }
                } else {
                    break :src rhs_src;
                }
            } else {
                break :src lhs_src;
            }
        };
        try sema.requireRuntimeBlock(block, src, runtime_src);
        return block.addBinOp(air_tag, lhs, rhs);
    }
    if (lhs_ty_tag == .Type and rhs_ty_tag == .Type) {
        const lhs_as_type = try sema.analyzeAsType(block, lhs_src, lhs);
        const rhs_as_type = try sema.analyzeAsType(block, rhs_src, rhs);
        if (lhs_as_type.eql(rhs_as_type, sema.mod) == (op == .eq)) {
            return Air.Inst.Ref.bool_true;
        } else {
            return Air.Inst.Ref.bool_false;
        }
    }
    return sema.analyzeCmp(block, src, lhs, rhs, op, lhs_src, rhs_src, true);
}

fn analyzeCmpUnionTag(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    un: Air.Inst.Ref,
    un_src: LazySrcLoc,
    tag: Air.Inst.Ref,
    tag_src: LazySrcLoc,
    op: std.math.CompareOperator,
) CompileError!Air.Inst.Ref {
    const union_ty = try sema.resolveTypeFields(sema.typeOf(un));
    const union_tag_ty = union_ty.unionTagType() orelse {
        const msg = msg: {
            const msg = try sema.errMsg(block, un_src, "comparison of union and enum literal is only valid for tagged union types", .{});
            errdefer msg.destroy(sema.gpa);
            try sema.mod.errNoteNonLazy(union_ty.declSrcLoc(sema.mod), msg, "union '{}' is not a tagged union", .{union_ty.fmt(sema.mod)});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    };
    // Coerce both the union and the tag to the union's tag type, and then execute the
    // enum comparison codepath.
    const coerced_tag = try sema.coerce(block, union_tag_ty, tag, tag_src);
    const coerced_union = try sema.coerce(block, union_tag_ty, un, un_src);

    if (try sema.resolveMaybeUndefVal(coerced_tag)) |enum_val| {
        if (enum_val.isUndef()) return sema.addConstUndef(Type.bool);
        const field_ty = union_ty.unionFieldType(enum_val, sema.mod);
        if (field_ty.zigTypeTag() == .NoReturn) {
            return Air.Inst.Ref.bool_false;
        }
    }

    return sema.cmpSelf(block, src, coerced_union, coerced_tag, op, un_src, tag_src);
}

/// Only called for non-equality operators. See also `zirCmpEq`.
fn zirCmp(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    op: std.math.CompareOperator,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const src: LazySrcLoc = inst_data.src();
    const lhs_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const rhs_src: LazySrcLoc = .{ .node_offset_bin_rhs = inst_data.src_node };
    const lhs = try sema.resolveInst(extra.lhs);
    const rhs = try sema.resolveInst(extra.rhs);
    return sema.analyzeCmp(block, src, lhs, rhs, op, lhs_src, rhs_src, false);
}

fn analyzeCmp(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    lhs: Air.Inst.Ref,
    rhs: Air.Inst.Ref,
    op: std.math.CompareOperator,
    lhs_src: LazySrcLoc,
    rhs_src: LazySrcLoc,
    is_equality_cmp: bool,
) CompileError!Air.Inst.Ref {
    const lhs_ty = sema.typeOf(lhs);
    const rhs_ty = sema.typeOf(rhs);
    if (lhs_ty.zigTypeTag() != .Optional and rhs_ty.zigTypeTag() != .Optional) {
        try sema.checkVectorizableBinaryOperands(block, src, lhs_ty, rhs_ty, lhs_src, rhs_src);
    }

    if (lhs_ty.zigTypeTag() == .Vector and rhs_ty.zigTypeTag() == .Vector) {
        return sema.cmpVector(block, src, lhs, rhs, op, lhs_src, rhs_src);
    }
    if (lhs_ty.isNumeric() and rhs_ty.isNumeric()) {
        // This operation allows any combination of integer and float types, regardless of the
        // signed-ness, comptime-ness, and bit-width. So peer type resolution is incorrect for
        // numeric types.
        return sema.cmpNumeric(block, src, lhs, rhs, op, lhs_src, rhs_src);
    }
    if (is_equality_cmp and lhs_ty.zigTypeTag() == .ErrorUnion and rhs_ty.zigTypeTag() == .ErrorSet) {
        const casted_lhs = try sema.analyzeErrUnionCode(block, lhs_src, lhs);
        return sema.cmpSelf(block, src, casted_lhs, rhs, op, lhs_src, rhs_src);
    }
    if (is_equality_cmp and lhs_ty.zigTypeTag() == .ErrorSet and rhs_ty.zigTypeTag() == .ErrorUnion) {
        const casted_rhs = try sema.analyzeErrUnionCode(block, rhs_src, rhs);
        return sema.cmpSelf(block, src, lhs, casted_rhs, op, lhs_src, rhs_src);
    }
    const instructions = &[_]Air.Inst.Ref{ lhs, rhs };
    const resolved_type = try sema.resolvePeerTypes(block, src, instructions, .{ .override = &[_]LazySrcLoc{ lhs_src, rhs_src } });
    if (!resolved_type.isSelfComparable(is_equality_cmp)) {
        return sema.fail(block, src, "operator {s} not allowed for type '{}'", .{
            compareOperatorName(op), resolved_type.fmt(sema.mod),
        });
    }
    const casted_lhs = try sema.coerce(block, resolved_type, lhs, lhs_src);
    const casted_rhs = try sema.coerce(block, resolved_type, rhs, rhs_src);
    return sema.cmpSelf(block, src, casted_lhs, casted_rhs, op, lhs_src, rhs_src);
}

fn compareOperatorName(comp: std.math.CompareOperator) []const u8 {
    return switch (comp) {
        .lt => "<",
        .lte => "<=",
        .eq => "==",
        .gte => ">=",
        .gt => ">",
        .neq => "!=",
    };
}

fn cmpSelf(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    casted_lhs: Air.Inst.Ref,
    casted_rhs: Air.Inst.Ref,
    op: std.math.CompareOperator,
    lhs_src: LazySrcLoc,
    rhs_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    const resolved_type = sema.typeOf(casted_lhs);
    const runtime_src: LazySrcLoc = src: {
        if (try sema.resolveMaybeUndefVal(casted_lhs)) |lhs_val| {
            if (lhs_val.isUndef()) return sema.addConstUndef(Type.bool);
            if (try sema.resolveMaybeUndefVal(casted_rhs)) |rhs_val| {
                if (rhs_val.isUndef()) return sema.addConstUndef(Type.bool);

                if (resolved_type.zigTypeTag() == .Vector) {
                    const result_ty = try Type.vector(sema.arena, resolved_type.vectorLen(), Type.bool);
                    const cmp_val = try sema.compareVector(lhs_val, op, rhs_val, resolved_type);
                    return sema.addConstant(result_ty, cmp_val);
                }

                if (try sema.compareAll(lhs_val, op, rhs_val, resolved_type)) {
                    return Air.Inst.Ref.bool_true;
                } else {
                    return Air.Inst.Ref.bool_false;
                }
            } else {
                if (resolved_type.zigTypeTag() == .Bool) {
                    // We can lower bool eq/neq more efficiently.
                    return sema.runtimeBoolCmp(block, src, op, casted_rhs, lhs_val.toBool(), rhs_src);
                }
                break :src rhs_src;
            }
        } else {
            // For bools, we still check the other operand, because we can lower
            // bool eq/neq more efficiently.
            if (resolved_type.zigTypeTag() == .Bool) {
                if (try sema.resolveMaybeUndefVal(casted_rhs)) |rhs_val| {
                    if (rhs_val.isUndef()) return sema.addConstUndef(Type.bool);
                    return sema.runtimeBoolCmp(block, src, op, casted_lhs, rhs_val.toBool(), lhs_src);
                }
            }
            break :src lhs_src;
        }
    };
    try sema.requireRuntimeBlock(block, src, runtime_src);
    if (resolved_type.zigTypeTag() == .Vector) {
        return block.addCmpVector(casted_lhs, casted_rhs, op);
    }
    const tag = Air.Inst.Tag.fromCmpOp(op, block.float_mode == .Optimized);
    return block.addBinOp(tag, casted_lhs, casted_rhs);
}

/// cmp_eq (x, false) => not(x)
/// cmp_eq (x, true ) => x
/// cmp_neq(x, false) => x
/// cmp_neq(x, true ) => not(x)
fn runtimeBoolCmp(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    op: std.math.CompareOperator,
    lhs: Air.Inst.Ref,
    rhs: bool,
    runtime_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    if ((op == .neq) == rhs) {
        try sema.requireRuntimeBlock(block, src, runtime_src);
        return block.addTyOp(.not, Type.bool, lhs);
    } else {
        return lhs;
    }
}

fn zirSizeOf(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ty = try sema.resolveType(block, operand_src, inst_data.operand);
    switch (ty.zigTypeTag()) {
        .Fn,
        .NoReturn,
        .Undefined,
        .Null,
        .Opaque,
        => return sema.fail(block, operand_src, "no size available for type '{}'", .{ty.fmt(sema.mod)}),

        .Type,
        .EnumLiteral,
        .ComptimeFloat,
        .ComptimeInt,
        .Void,
        => return sema.addIntUnsigned(Type.comptime_int, 0),

        .Bool,
        .Int,
        .Float,
        .Pointer,
        .Array,
        .Struct,
        .Optional,
        .ErrorUnion,
        .ErrorSet,
        .Enum,
        .Union,
        .Vector,
        .Frame,
        .AnyFrame,
        => {},
    }
    const target = sema.mod.getTarget();
    const val = try ty.lazyAbiSize(target, sema.arena);
    if (val.tag() == .lazy_size) {
        try sema.queueFullTypeResolution(ty);
    }
    return sema.addConstant(Type.comptime_int, val);
}

fn zirBitSizeOf(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_ty = try sema.resolveType(block, operand_src, inst_data.operand);
    switch (operand_ty.zigTypeTag()) {
        .Fn,
        .NoReturn,
        .Undefined,
        .Null,
        .Opaque,
        => return sema.fail(block, operand_src, "no size available for type '{}'", .{operand_ty.fmt(sema.mod)}),

        .Type,
        .EnumLiteral,
        .ComptimeFloat,
        .ComptimeInt,
        .Void,
        => return sema.addIntUnsigned(Type.comptime_int, 0),

        .Bool,
        .Int,
        .Float,
        .Pointer,
        .Array,
        .Struct,
        .Optional,
        .ErrorUnion,
        .ErrorSet,
        .Enum,
        .Union,
        .Vector,
        .Frame,
        .AnyFrame,
        => {},
    }
    const target = sema.mod.getTarget();
    const bit_size = try operand_ty.bitSizeAdvanced(target, sema);
    return sema.addIntUnsigned(Type.comptime_int, bit_size);
}

fn zirThis(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const this_decl_index = block.namespace.getDeclIndex();
    const src = LazySrcLoc.nodeOffset(@bitCast(i32, extended.operand));
    return sema.analyzeDeclVal(block, src, this_decl_index);
}

fn zirClosureCapture(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!void {
    // TODO: Compile error when closed over values are modified
    const inst_data = sema.code.instructions.items(.data)[inst].un_tok;
    // Closures are not necessarily constant values. For example, the
    // code might do something like this:
    // fn foo(x: anytype) void { const S = struct {field: @TypeOf(x)}; }
    // ...in which case the closure_capture instruction has access to a runtime
    // value only. In such case we preserve the type and use a dummy runtime value.
    const operand = try sema.resolveInst(inst_data.operand);
    const val = (try sema.resolveMaybeUndefValAllowVariables(operand)) orelse
        Value.initTag(.unreachable_value);

    try block.wip_capture_scope.captures.putNoClobber(sema.gpa, inst, .{
        .ty = try sema.typeOf(operand).copy(sema.perm_arena),
        .val = try val.copy(sema.perm_arena),
    });
}

fn zirClosureGet(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    // TODO CLOSURE: Test this with inline functions
    const inst_data = sema.code.instructions.items(.data)[inst].inst_node;
    var scope: *CaptureScope = sema.mod.declPtr(block.src_decl).src_scope.?;
    // Note: The target closure must be in this scope list.
    // If it's not here, the zir is invalid, or the list is broken.
    const tv = while (true) {
        // Note: We don't need to add a dependency here, because
        // decls always depend on their lexical parents.

        // Fail this decl if a scope it depended on failed.
        if (scope.failed()) {
            if (sema.owner_func) |owner_func| {
                owner_func.state = .dependency_failure;
            } else {
                sema.owner_decl.analysis = .dependency_failure;
            }
            return error.AnalysisFail;
        }
        if (scope.captures.getPtr(inst_data.inst)) |tv| {
            break tv;
        }
        scope = scope.parent.?;
    };

    if (tv.val.tag() == .unreachable_value and !block.is_typeof and sema.func == null) {
        const msg = msg: {
            const name = name: {
                const file = sema.owner_decl.getFileScope();
                const tree = file.getTree(sema.mod.gpa) catch |err| {
                    // In this case we emit a warning + a less precise source location.
                    log.warn("unable to load {s}: {s}", .{
                        file.sub_file_path, @errorName(err),
                    });
                    break :name null;
                };
                const node = sema.owner_decl.relativeToNodeIndex(inst_data.src_node);
                const token = tree.nodes.items(.main_token)[node];
                break :name tree.tokenSlice(token);
            };

            const msg = if (name) |some|
                try sema.errMsg(block, inst_data.src(), "'{s}' not accessible outside function scope", .{some})
            else
                try sema.errMsg(block, inst_data.src(), "variable not accessible outside function scope", .{});
            errdefer msg.destroy(sema.gpa);

            // TODO add "declared here" note
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    if (tv.val.tag() == .unreachable_value and !block.is_typeof and !block.is_comptime and sema.func != null) {
        const msg = msg: {
            const name = name: {
                const file = sema.owner_decl.getFileScope();
                const tree = file.getTree(sema.mod.gpa) catch |err| {
                    // In this case we emit a warning + a less precise source location.
                    log.warn("unable to load {s}: {s}", .{
                        file.sub_file_path, @errorName(err),
                    });
                    break :name null;
                };
                const node = sema.owner_decl.relativeToNodeIndex(inst_data.src_node);
                const token = tree.nodes.items(.main_token)[node];
                break :name tree.tokenSlice(token);
            };

            const msg = if (name) |some|
                try sema.errMsg(block, inst_data.src(), "'{s}' not accessible from inner function", .{some})
            else
                try sema.errMsg(block, inst_data.src(), "variable not accessible from inner function", .{});
            errdefer msg.destroy(sema.gpa);

            try sema.errNote(block, LazySrcLoc.nodeOffset(0), msg, "crossed function definition here", .{});

            // TODO add "declared here" note
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    if (tv.val.tag() == .unreachable_value) {
        assert(block.is_typeof);
        // We need a dummy runtime instruction with the correct type.
        return block.addTy(.alloc, tv.ty);
    }

    return sema.addConstant(tv.ty, tv.val);
}

fn zirRetAddr(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const src = LazySrcLoc.nodeOffset(@bitCast(i32, extended.operand));
    try sema.requireRuntimeBlock(block, src, null);
    return try block.addNoOp(.ret_addr);
}

fn zirFrameAddress(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const src = LazySrcLoc.nodeOffset(@bitCast(i32, extended.operand));
    try sema.requireRuntimeBlock(block, src, null);
    return try block.addNoOp(.frame_addr);
}

fn zirBuiltinSrc(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.Src, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const func = sema.func orelse return sema.fail(block, src, "@src outside function", .{});
    const fn_owner_decl = sema.mod.declPtr(func.owner_decl);

    const func_name_val = blk: {
        var anon_decl = try block.startAnonDecl();
        defer anon_decl.deinit();
        const name = std.mem.span(fn_owner_decl.name);
        const bytes = try anon_decl.arena().dupe(u8, name[0 .. name.len + 1]);
        const new_decl = try anon_decl.finish(
            try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len - 1),
            try Value.Tag.bytes.create(anon_decl.arena(), bytes),
            0, // default alignment
        );
        break :blk try Value.Tag.decl_ref.create(sema.arena, new_decl);
    };

    const file_name_val = blk: {
        var anon_decl = try block.startAnonDecl();
        defer anon_decl.deinit();
        // The compiler must not call realpath anywhere.
        const name = try fn_owner_decl.getFileScope().fullPathZ(anon_decl.arena());
        const new_decl = try anon_decl.finish(
            try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), name.len),
            try Value.Tag.bytes.create(anon_decl.arena(), name[0 .. name.len + 1]),
            0, // default alignment
        );
        break :blk try Value.Tag.decl_ref.create(sema.arena, new_decl);
    };

    const field_values = try sema.arena.alloc(Value, 4);
    // file: [:0]const u8,
    field_values[0] = file_name_val;
    // fn_name: [:0]const u8,
    field_values[1] = func_name_val;
    // line: u32
    field_values[2] = try Value.Tag.runtime_value.create(sema.arena, try Value.Tag.int_u64.create(sema.arena, extra.line + 1));
    // column: u32,
    field_values[3] = try Value.Tag.int_u64.create(sema.arena, extra.column + 1);

    return sema.addConstant(
        try sema.getBuiltinType("SourceLocation"),
        try Value.Tag.aggregate.create(sema.arena, field_values),
    );
}

fn zirTypeInfo(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ty = try sema.resolveType(block, src, inst_data.operand);
    const type_info_ty = try sema.getBuiltinType("Type");
    const target = sema.mod.getTarget();

    switch (ty.zigTypeTag()) {
        .Type => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Type)),
                .val = Value.void,
            }),
        ),
        .Void => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Void)),
                .val = Value.void,
            }),
        ),
        .Bool => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Bool)),
                .val = Value.void,
            }),
        ),
        .NoReturn => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.NoReturn)),
                .val = Value.void,
            }),
        ),
        .ComptimeFloat => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.ComptimeFloat)),
                .val = Value.void,
            }),
        ),
        .ComptimeInt => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.ComptimeInt)),
                .val = Value.void,
            }),
        ),
        .Undefined => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Undefined)),
                .val = Value.void,
            }),
        ),
        .Null => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Null)),
                .val = Value.void,
            }),
        ),
        .EnumLiteral => return sema.addConstant(
            type_info_ty,
            try Value.Tag.@"union".create(sema.arena, .{
                .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.EnumLiteral)),
                .val = Value.void,
            }),
        ),
        .Fn => {
            // TODO: look into memoizing this result.
            const info = ty.fnInfo();

            var params_anon_decl = try block.startAnonDecl();
            defer params_anon_decl.deinit();

            const param_vals = try params_anon_decl.arena().alloc(Value, info.param_types.len);
            for (param_vals, 0..) |*param_val, i| {
                const param_ty = info.param_types[i];
                const is_generic = param_ty.tag() == .generic_poison;
                const param_ty_val = if (is_generic)
                    Value.null
                else
                    try Value.Tag.opt_payload.create(
                        params_anon_decl.arena(),
                        try Value.Tag.ty.create(params_anon_decl.arena(), try param_ty.copy(params_anon_decl.arena())),
                    );

                const is_noalias = blk: {
                    const index = std.math.cast(u5, i) orelse break :blk false;
                    break :blk @truncate(u1, info.noalias_bits >> index) != 0;
                };

                const param_fields = try params_anon_decl.arena().create([3]Value);
                param_fields.* = .{
                    // is_generic: bool,
                    Value.makeBool(is_generic),
                    // is_noalias: bool,
                    Value.makeBool(is_noalias),
                    // type: ?type,
                    param_ty_val,
                };
                param_val.* = try Value.Tag.aggregate.create(params_anon_decl.arena(), param_fields);
            }

            const args_val = v: {
                const fn_info_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    type_info_ty.getNamespace().?,
                    "Fn",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, fn_info_decl_index);
                try sema.ensureDeclAnalyzed(fn_info_decl_index);
                const fn_info_decl = sema.mod.declPtr(fn_info_decl_index);
                var fn_ty_buffer: Value.ToTypeBuffer = undefined;
                const fn_ty = fn_info_decl.val.toType(&fn_ty_buffer);
                const param_info_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    fn_ty.getNamespace().?,
                    "Param",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, param_info_decl_index);
                try sema.ensureDeclAnalyzed(param_info_decl_index);
                const param_info_decl = sema.mod.declPtr(param_info_decl_index);
                var param_buffer: Value.ToTypeBuffer = undefined;
                const param_ty = param_info_decl.val.toType(&param_buffer);
                const new_decl = try params_anon_decl.finish(
                    try Type.Tag.array.create(params_anon_decl.arena(), .{
                        .len = param_vals.len,
                        .elem_type = try param_ty.copy(params_anon_decl.arena()),
                    }),
                    try Value.Tag.aggregate.create(
                        params_anon_decl.arena(),
                        param_vals,
                    ),
                    0, // default alignment
                );
                break :v try Value.Tag.slice.create(sema.arena, .{
                    .ptr = try Value.Tag.decl_ref.create(sema.arena, new_decl),
                    .len = try Value.Tag.int_u64.create(sema.arena, param_vals.len),
                });
            };

            const ret_ty_opt = if (info.return_type.tag() != .generic_poison)
                try Value.Tag.opt_payload.create(
                    sema.arena,
                    try Value.Tag.ty.create(sema.arena, info.return_type),
                )
            else
                Value.null;

            const field_values = try sema.arena.create([6]Value);
            field_values.* = .{
                // calling_convention: CallingConvention,
                try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(info.cc)),
                // alignment: comptime_int,
                try Value.Tag.int_u64.create(sema.arena, ty.abiAlignment(target)),
                // is_generic: bool,
                Value.makeBool(info.is_generic),
                // is_var_args: bool,
                Value.makeBool(info.is_var_args),
                // return_type: ?type,
                ret_ty_opt,
                // args: []const Fn.Param,
                args_val,
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Fn)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Int => {
            const info = ty.intInfo(target);
            const field_values = try sema.arena.alloc(Value, 2);
            // signedness: Signedness,
            field_values[0] = try Value.Tag.enum_field_index.create(
                sema.arena,
                @enumToInt(info.signedness),
            );
            // bits: comptime_int,
            field_values[1] = try Value.Tag.int_u64.create(sema.arena, info.bits);

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Int)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Float => {
            const field_values = try sema.arena.alloc(Value, 1);
            // bits: comptime_int,
            field_values[0] = try Value.Tag.int_u64.create(sema.arena, ty.bitSize(target));

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Float)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Pointer => {
            const info = ty.ptrInfo().data;
            const alignment = if (info.@"align" != 0)
                try Value.Tag.int_u64.create(sema.arena, info.@"align")
            else
                try info.pointee_type.lazyAbiAlignment(target, sema.arena);

            const field_values = try sema.arena.create([8]Value);
            field_values.* = .{
                // size: Size,
                try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(info.size)),
                // is_const: bool,
                Value.makeBool(!info.mutable),
                // is_volatile: bool,
                Value.makeBool(info.@"volatile"),
                // alignment: comptime_int,
                alignment,
                // address_space: AddressSpace
                try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(info.@"addrspace")),
                // child: type,
                try Value.Tag.ty.create(sema.arena, info.pointee_type),
                // is_allowzero: bool,
                Value.makeBool(info.@"allowzero"),
                // sentinel: ?*const anyopaque,
                try sema.optRefValue(block, info.pointee_type, info.sentinel),
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Pointer)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Array => {
            const info = ty.arrayInfo();
            const field_values = try sema.arena.alloc(Value, 3);
            // len: comptime_int,
            field_values[0] = try Value.Tag.int_u64.create(sema.arena, info.len);
            // child: type,
            field_values[1] = try Value.Tag.ty.create(sema.arena, info.elem_type);
            // sentinel: ?*const anyopaque,
            field_values[2] = try sema.optRefValue(block, info.elem_type, info.sentinel);

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Array)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Vector => {
            const info = ty.arrayInfo();
            const field_values = try sema.arena.alloc(Value, 2);
            // len: comptime_int,
            field_values[0] = try Value.Tag.int_u64.create(sema.arena, info.len);
            // child: type,
            field_values[1] = try Value.Tag.ty.create(sema.arena, info.elem_type);

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Vector)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Optional => {
            const field_values = try sema.arena.alloc(Value, 1);
            // child: type,
            field_values[0] = try Value.Tag.ty.create(sema.arena, try ty.optionalChildAlloc(sema.arena));

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Optional)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .ErrorSet => {
            var fields_anon_decl = try block.startAnonDecl();
            defer fields_anon_decl.deinit();

            // Get the Error type
            const error_field_ty = t: {
                const set_field_ty_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    type_info_ty.getNamespace().?,
                    "Error",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, set_field_ty_decl_index);
                try sema.ensureDeclAnalyzed(set_field_ty_decl_index);
                const set_field_ty_decl = sema.mod.declPtr(set_field_ty_decl_index);
                var buffer: Value.ToTypeBuffer = undefined;
                break :t try set_field_ty_decl.val.toType(&buffer).copy(fields_anon_decl.arena());
            };

            try sema.queueFullTypeResolution(try error_field_ty.copy(sema.arena));

            // If the error set is inferred it must be resolved at this point
            try sema.resolveInferredErrorSetTy(block, src, ty);

            // Build our list of Error values
            // Optional value is only null if anyerror
            // Value can be zero-length slice otherwise
            const error_field_vals: ?[]Value = if (ty.isAnyError()) null else blk: {
                const names = ty.errorSetNames();
                const vals = try fields_anon_decl.arena().alloc(Value, names.len);
                for (vals, 0..) |*field_val, i| {
                    const name = names[i];
                    const name_val = v: {
                        var anon_decl = try block.startAnonDecl();
                        defer anon_decl.deinit();
                        const bytes = try anon_decl.arena().dupeZ(u8, name);
                        const new_decl = try anon_decl.finish(
                            try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                            try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                            0, // default alignment
                        );
                        break :v try Value.Tag.decl_ref.create(fields_anon_decl.arena(), new_decl);
                    };

                    const error_field_fields = try fields_anon_decl.arena().create([1]Value);
                    error_field_fields.* = .{
                        // name: []const u8,
                        name_val,
                    };

                    field_val.* = try Value.Tag.aggregate.create(
                        fields_anon_decl.arena(),
                        error_field_fields,
                    );
                }

                break :blk vals;
            };

            // Build our ?[]const Error value
            const errors_val = if (error_field_vals) |vals| v: {
                const new_decl = try fields_anon_decl.finish(
                    try Type.Tag.array.create(fields_anon_decl.arena(), .{
                        .len = vals.len,
                        .elem_type = error_field_ty,
                    }),
                    try Value.Tag.aggregate.create(
                        fields_anon_decl.arena(),
                        vals,
                    ),
                    0, // default alignment
                );

                const new_decl_val = try Value.Tag.decl_ref.create(sema.arena, new_decl);
                const slice_val = try Value.Tag.slice.create(sema.arena, .{
                    .ptr = new_decl_val,
                    .len = try Value.Tag.int_u64.create(sema.arena, vals.len),
                });
                break :v try Value.Tag.opt_payload.create(sema.arena, slice_val);
            } else Value.null;

            // Construct Type{ .ErrorSet = errors_val }
            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.ErrorSet)),
                    .val = errors_val,
                }),
            );
        },
        .ErrorUnion => {
            const field_values = try sema.arena.alloc(Value, 2);
            // error_set: type,
            field_values[0] = try Value.Tag.ty.create(sema.arena, ty.errorUnionSet());
            // payload: type,
            field_values[1] = try Value.Tag.ty.create(sema.arena, ty.errorUnionPayload());

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.ErrorUnion)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Enum => {
            // TODO: look into memoizing this result.
            var int_tag_type_buffer: Type.Payload.Bits = undefined;
            const int_tag_ty = try ty.intTagType(&int_tag_type_buffer).copy(sema.arena);

            const is_exhaustive = Value.makeBool(!ty.isNonexhaustiveEnum());

            var fields_anon_decl = try block.startAnonDecl();
            defer fields_anon_decl.deinit();

            const enum_field_ty = t: {
                const enum_field_ty_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    type_info_ty.getNamespace().?,
                    "EnumField",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, enum_field_ty_decl_index);
                try sema.ensureDeclAnalyzed(enum_field_ty_decl_index);
                const enum_field_ty_decl = sema.mod.declPtr(enum_field_ty_decl_index);
                var buffer: Value.ToTypeBuffer = undefined;
                break :t try enum_field_ty_decl.val.toType(&buffer).copy(fields_anon_decl.arena());
            };

            const enum_fields = ty.enumFields();
            const enum_field_vals = try fields_anon_decl.arena().alloc(Value, enum_fields.count());

            for (enum_field_vals, 0..) |*field_val, i| {
                var tag_val_payload: Value.Payload.U32 = .{
                    .base = .{ .tag = .enum_field_index },
                    .data = @intCast(u32, i),
                };
                const tag_val = Value.initPayload(&tag_val_payload.base);

                var buffer: Value.Payload.U64 = undefined;
                const int_val = try tag_val.enumToInt(ty, &buffer).copy(fields_anon_decl.arena());

                const name = enum_fields.keys()[i];
                const name_val = v: {
                    var anon_decl = try block.startAnonDecl();
                    defer anon_decl.deinit();
                    const bytes = try anon_decl.arena().dupeZ(u8, name);
                    const new_decl = try anon_decl.finish(
                        try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                        try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                        0, // default alignment
                    );
                    break :v try Value.Tag.decl_ref.create(fields_anon_decl.arena(), new_decl);
                };

                const enum_field_fields = try fields_anon_decl.arena().create([2]Value);
                enum_field_fields.* = .{
                    // name: []const u8,
                    name_val,
                    // value: comptime_int,
                    int_val,
                };
                field_val.* = try Value.Tag.aggregate.create(fields_anon_decl.arena(), enum_field_fields);
            }

            const fields_val = v: {
                const new_decl = try fields_anon_decl.finish(
                    try Type.Tag.array.create(fields_anon_decl.arena(), .{
                        .len = enum_field_vals.len,
                        .elem_type = enum_field_ty,
                    }),
                    try Value.Tag.aggregate.create(
                        fields_anon_decl.arena(),
                        enum_field_vals,
                    ),
                    0, // default alignment
                );
                break :v try Value.Tag.decl_ref.create(sema.arena, new_decl);
            };

            const decls_val = try sema.typeInfoDecls(block, src, type_info_ty, ty.getNamespace());

            const field_values = try sema.arena.create([4]Value);
            field_values.* = .{
                // tag_type: type,
                try Value.Tag.ty.create(sema.arena, int_tag_ty),
                // fields: []const EnumField,
                fields_val,
                // decls: []const Declaration,
                decls_val,
                // is_exhaustive: bool,
                is_exhaustive,
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Enum)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Union => {
            // TODO: look into memoizing this result.

            var fields_anon_decl = try block.startAnonDecl();
            defer fields_anon_decl.deinit();

            const union_field_ty = t: {
                const union_field_ty_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    type_info_ty.getNamespace().?,
                    "UnionField",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, union_field_ty_decl_index);
                try sema.ensureDeclAnalyzed(union_field_ty_decl_index);
                const union_field_ty_decl = sema.mod.declPtr(union_field_ty_decl_index);
                var buffer: Value.ToTypeBuffer = undefined;
                break :t try union_field_ty_decl.val.toType(&buffer).copy(fields_anon_decl.arena());
            };

            const union_ty = try sema.resolveTypeFields(ty);
            try sema.resolveTypeLayout(ty); // Getting alignment requires type layout
            const layout = union_ty.containerLayout();

            const union_fields = union_ty.unionFields();
            const union_field_vals = try fields_anon_decl.arena().alloc(Value, union_fields.count());

            for (union_field_vals, 0..) |*field_val, i| {
                const field = union_fields.values()[i];
                const name = union_fields.keys()[i];
                const name_val = v: {
                    var anon_decl = try block.startAnonDecl();
                    defer anon_decl.deinit();
                    const bytes = try anon_decl.arena().dupeZ(u8, name);
                    const new_decl = try anon_decl.finish(
                        try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                        try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                        0, // default alignment
                    );
                    break :v try Value.Tag.decl_ref.create(fields_anon_decl.arena(), new_decl);
                };

                const union_field_fields = try fields_anon_decl.arena().create([3]Value);
                const alignment = switch (layout) {
                    .Auto, .Extern => try sema.unionFieldAlignment(field),
                    .Packed => 0,
                };

                union_field_fields.* = .{
                    // name: []const u8,
                    name_val,
                    // type: type,
                    try Value.Tag.ty.create(fields_anon_decl.arena(), field.ty),
                    // alignment: comptime_int,
                    try Value.Tag.int_u64.create(fields_anon_decl.arena(), alignment),
                };
                field_val.* = try Value.Tag.aggregate.create(fields_anon_decl.arena(), union_field_fields);
            }

            const fields_val = v: {
                const new_decl = try fields_anon_decl.finish(
                    try Type.Tag.array.create(fields_anon_decl.arena(), .{
                        .len = union_field_vals.len,
                        .elem_type = union_field_ty,
                    }),
                    try Value.Tag.aggregate.create(
                        fields_anon_decl.arena(),
                        try fields_anon_decl.arena().dupe(Value, union_field_vals),
                    ),
                    0, // default alignment
                );
                break :v try Value.Tag.slice.create(sema.arena, .{
                    .ptr = try Value.Tag.decl_ref.create(sema.arena, new_decl),
                    .len = try Value.Tag.int_u64.create(sema.arena, union_field_vals.len),
                });
            };

            const decls_val = try sema.typeInfoDecls(block, src, type_info_ty, union_ty.getNamespace());

            const enum_tag_ty_val = if (union_ty.unionTagType()) |tag_ty| v: {
                const ty_val = try Value.Tag.ty.create(sema.arena, tag_ty);
                break :v try Value.Tag.opt_payload.create(sema.arena, ty_val);
            } else Value.null;

            const field_values = try sema.arena.create([4]Value);
            field_values.* = .{
                // layout: ContainerLayout,
                try Value.Tag.enum_field_index.create(
                    sema.arena,
                    @enumToInt(layout),
                ),

                // tag_type: ?type,
                enum_tag_ty_val,
                // fields: []const UnionField,
                fields_val,
                // decls: []const Declaration,
                decls_val,
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Union)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Struct => {
            // TODO: look into memoizing this result.

            var fields_anon_decl = try block.startAnonDecl();
            defer fields_anon_decl.deinit();

            const struct_field_ty = t: {
                const struct_field_ty_decl_index = (try sema.namespaceLookup(
                    block,
                    src,
                    type_info_ty.getNamespace().?,
                    "StructField",
                )).?;
                try sema.mod.declareDeclDependency(sema.owner_decl_index, struct_field_ty_decl_index);
                try sema.ensureDeclAnalyzed(struct_field_ty_decl_index);
                const struct_field_ty_decl = sema.mod.declPtr(struct_field_ty_decl_index);
                var buffer: Value.ToTypeBuffer = undefined;
                break :t try struct_field_ty_decl.val.toType(&buffer).copy(fields_anon_decl.arena());
            };
            const struct_ty = try sema.resolveTypeFields(ty);
            try sema.resolveTypeLayout(ty); // Getting alignment requires type layout
            const layout = struct_ty.containerLayout();

            const struct_field_vals = fv: {
                if (struct_ty.isSimpleTupleOrAnonStruct()) {
                    const tuple = struct_ty.tupleFields();
                    const field_types = tuple.types;
                    const struct_field_vals = try fields_anon_decl.arena().alloc(Value, field_types.len);
                    for (struct_field_vals, 0..) |*struct_field_val, i| {
                        const field_ty = field_types[i];
                        const name_val = v: {
                            var anon_decl = try block.startAnonDecl();
                            defer anon_decl.deinit();
                            const bytes = if (struct_ty.castTag(.anon_struct)) |payload|
                                try anon_decl.arena().dupeZ(u8, payload.data.names[i])
                            else
                                try std.fmt.allocPrintZ(anon_decl.arena(), "{d}", .{i});
                            const new_decl = try anon_decl.finish(
                                try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                                try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                                0, // default alignment
                            );
                            break :v try Value.Tag.slice.create(fields_anon_decl.arena(), .{
                                .ptr = try Value.Tag.decl_ref.create(fields_anon_decl.arena(), new_decl),
                                .len = try Value.Tag.int_u64.create(fields_anon_decl.arena(), bytes.len),
                            });
                        };

                        const struct_field_fields = try fields_anon_decl.arena().create([5]Value);
                        const field_val = tuple.values[i];
                        const is_comptime = field_val.tag() != .unreachable_value;
                        const opt_default_val = if (is_comptime) field_val else null;
                        const default_val_ptr = try sema.optRefValue(block, field_ty, opt_default_val);
                        struct_field_fields.* = .{
                            // name: []const u8,
                            name_val,
                            // type: type,
                            try Value.Tag.ty.create(fields_anon_decl.arena(), field_ty),
                            // default_value: ?*const anyopaque,
                            try default_val_ptr.copy(fields_anon_decl.arena()),
                            // is_comptime: bool,
                            Value.makeBool(is_comptime),
                            // alignment: comptime_int,
                            try field_ty.lazyAbiAlignment(target, fields_anon_decl.arena()),
                        };
                        struct_field_val.* = try Value.Tag.aggregate.create(fields_anon_decl.arena(), struct_field_fields);
                    }
                    break :fv struct_field_vals;
                }
                const struct_fields = struct_ty.structFields();
                const struct_field_vals = try fields_anon_decl.arena().alloc(Value, struct_fields.count());

                for (struct_field_vals, 0..) |*field_val, i| {
                    const field = struct_fields.values()[i];
                    const name = struct_fields.keys()[i];
                    const name_val = v: {
                        var anon_decl = try block.startAnonDecl();
                        defer anon_decl.deinit();
                        const bytes = try anon_decl.arena().dupeZ(u8, name);
                        const new_decl = try anon_decl.finish(
                            try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                            try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                            0, // default alignment
                        );
                        break :v try Value.Tag.slice.create(fields_anon_decl.arena(), .{
                            .ptr = try Value.Tag.decl_ref.create(fields_anon_decl.arena(), new_decl),
                            .len = try Value.Tag.int_u64.create(fields_anon_decl.arena(), bytes.len),
                        });
                    };

                    const struct_field_fields = try fields_anon_decl.arena().create([5]Value);
                    const opt_default_val = if (field.default_val.tag() == .unreachable_value)
                        null
                    else
                        field.default_val;
                    const default_val_ptr = try sema.optRefValue(block, field.ty, opt_default_val);
                    const alignment = field.alignment(target, layout);

                    struct_field_fields.* = .{
                        // name: []const u8,
                        name_val,
                        // type: type,
                        try Value.Tag.ty.create(fields_anon_decl.arena(), field.ty),
                        // default_value: ?*const anyopaque,
                        try default_val_ptr.copy(fields_anon_decl.arena()),
                        // is_comptime: bool,
                        Value.makeBool(field.is_comptime),
                        // alignment: comptime_int,
                        try Value.Tag.int_u64.create(fields_anon_decl.arena(), alignment),
                    };
                    field_val.* = try Value.Tag.aggregate.create(fields_anon_decl.arena(), struct_field_fields);
                }
                break :fv struct_field_vals;
            };

            const fields_val = v: {
                const new_decl = try fields_anon_decl.finish(
                    try Type.Tag.array.create(fields_anon_decl.arena(), .{
                        .len = struct_field_vals.len,
                        .elem_type = struct_field_ty,
                    }),
                    try Value.Tag.aggregate.create(
                        fields_anon_decl.arena(),
                        try fields_anon_decl.arena().dupe(Value, struct_field_vals),
                    ),
                    0, // default alignment
                );
                break :v try Value.Tag.slice.create(sema.arena, .{
                    .ptr = try Value.Tag.decl_ref.create(sema.arena, new_decl),
                    .len = try Value.Tag.int_u64.create(sema.arena, struct_field_vals.len),
                });
            };

            const decls_val = try sema.typeInfoDecls(block, src, type_info_ty, struct_ty.getNamespace());

            const backing_integer_val = blk: {
                if (layout == .Packed) {
                    const struct_obj = struct_ty.castTag(.@"struct").?.data;
                    assert(struct_obj.haveLayout());
                    assert(struct_obj.backing_int_ty.isInt());
                    const backing_int_ty_val = try Value.Tag.ty.create(sema.arena, struct_obj.backing_int_ty);
                    break :blk try Value.Tag.opt_payload.create(sema.arena, backing_int_ty_val);
                } else {
                    break :blk Value.initTag(.null_value);
                }
            };

            const field_values = try sema.arena.create([5]Value);
            field_values.* = .{
                // layout: ContainerLayout,
                try Value.Tag.enum_field_index.create(
                    sema.arena,
                    @enumToInt(layout),
                ),
                // backing_integer: ?type,
                backing_integer_val,
                // fields: []const StructField,
                fields_val,
                // decls: []const Declaration,
                decls_val,
                // is_tuple: bool,
                Value.makeBool(struct_ty.isTuple()),
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Struct)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Opaque => {
            // TODO: look into memoizing this result.

            const opaque_ty = try sema.resolveTypeFields(ty);
            const decls_val = try sema.typeInfoDecls(block, src, type_info_ty, opaque_ty.getNamespace());

            const field_values = try sema.arena.create([1]Value);
            field_values.* = .{
                // decls: []const Declaration,
                decls_val,
            };

            return sema.addConstant(
                type_info_ty,
                try Value.Tag.@"union".create(sema.arena, .{
                    .tag = try Value.Tag.enum_field_index.create(sema.arena, @enumToInt(std.builtin.TypeId.Opaque)),
                    .val = try Value.Tag.aggregate.create(sema.arena, field_values),
                }),
            );
        },
        .Frame => return sema.failWithUseOfAsync(block, src),
        .AnyFrame => return sema.failWithUseOfAsync(block, src),
    }
}

fn typeInfoDecls(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    type_info_ty: Type,
    opt_namespace: ?*Module.Namespace,
) CompileError!Value {
    var decls_anon_decl = try block.startAnonDecl();
    defer decls_anon_decl.deinit();

    const declaration_ty = t: {
        const declaration_ty_decl_index = (try sema.namespaceLookup(
            block,
            src,
            type_info_ty.getNamespace().?,
            "Declaration",
        )).?;
        try sema.mod.declareDeclDependency(sema.owner_decl_index, declaration_ty_decl_index);
        try sema.ensureDeclAnalyzed(declaration_ty_decl_index);
        const declaration_ty_decl = sema.mod.declPtr(declaration_ty_decl_index);
        var buffer: Value.ToTypeBuffer = undefined;
        break :t try declaration_ty_decl.val.toType(&buffer).copy(decls_anon_decl.arena());
    };
    try sema.queueFullTypeResolution(try declaration_ty.copy(sema.arena));

    var decl_vals = std.ArrayList(Value).init(sema.gpa);
    defer decl_vals.deinit();

    var seen_namespaces = std.AutoHashMap(*Namespace, void).init(sema.gpa);
    defer seen_namespaces.deinit();

    if (opt_namespace) |some| {
        try sema.typeInfoNamespaceDecls(block, decls_anon_decl.arena(), some, &decl_vals, &seen_namespaces);
    }

    const new_decl = try decls_anon_decl.finish(
        try Type.Tag.array.create(decls_anon_decl.arena(), .{
            .len = decl_vals.items.len,
            .elem_type = declaration_ty,
        }),
        try Value.Tag.aggregate.create(
            decls_anon_decl.arena(),
            try decls_anon_decl.arena().dupe(Value, decl_vals.items),
        ),
        0, // default alignment
    );
    return try Value.Tag.slice.create(sema.arena, .{
        .ptr = try Value.Tag.decl_ref.create(sema.arena, new_decl),
        .len = try Value.Tag.int_u64.create(sema.arena, decl_vals.items.len),
    });
}

fn typeInfoNamespaceDecls(
    sema: *Sema,
    block: *Block,
    decls_anon_decl: Allocator,
    namespace: *Namespace,
    decl_vals: *std.ArrayList(Value),
    seen_namespaces: *std.AutoHashMap(*Namespace, void),
) !void {
    const gop = try seen_namespaces.getOrPut(namespace);
    if (gop.found_existing) return;
    const decls = namespace.decls.keys();
    for (decls) |decl_index| {
        const decl = sema.mod.declPtr(decl_index);
        if (decl.kind == .@"usingnamespace") {
            if (decl.analysis == .in_progress) continue;
            try sema.mod.ensureDeclAnalyzed(decl_index);
            var buf: Value.ToTypeBuffer = undefined;
            const new_ns = decl.val.toType(&buf).getNamespace().?;
            try sema.typeInfoNamespaceDecls(block, decls_anon_decl, new_ns, decl_vals, seen_namespaces);
            continue;
        }
        if (decl.kind != .named) continue;
        const name_val = v: {
            var anon_decl = try block.startAnonDecl();
            defer anon_decl.deinit();
            const bytes = try anon_decl.arena().dupeZ(u8, mem.sliceTo(decl.name, 0));
            const new_decl = try anon_decl.finish(
                try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
                try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
                0, // default alignment
            );
            break :v try Value.Tag.slice.create(decls_anon_decl, .{
                .ptr = try Value.Tag.decl_ref.create(decls_anon_decl, new_decl),
                .len = try Value.Tag.int_u64.create(decls_anon_decl, bytes.len),
            });
        };

        const fields = try decls_anon_decl.create([2]Value);
        fields.* = .{
            //name: []const u8,
            name_val,
            //is_pub: bool,
            Value.makeBool(decl.is_pub),
        };
        try decl_vals.append(try Value.Tag.aggregate.create(decls_anon_decl, fields));
    }
}

fn zirTypeof(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    _ = block;
    const zir_datas = sema.code.instructions.items(.data);
    const inst_data = zir_datas[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    return sema.addType(operand_ty);
}

fn zirTypeofBuiltin(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const pl_node = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Block, pl_node.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];

    var child_block: Block = .{
        .parent = block,
        .sema = sema,
        .src_decl = block.src_decl,
        .namespace = block.namespace,
        .wip_capture_scope = block.wip_capture_scope,
        .instructions = .{},
        .inlining = block.inlining,
        .is_comptime = false,
        .is_typeof = true,
        .want_safety = false,
        .error_return_trace_index = block.error_return_trace_index,
    };
    defer child_block.instructions.deinit(sema.gpa);

    const operand = try sema.resolveBody(&child_block, body, inst);
    const operand_ty = sema.typeOf(operand);
    if (operand_ty.tag() == .generic_poison) return error.GenericPoison;
    return sema.addType(operand_ty);
}

fn zirTypeofLog2IntType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    const res_ty = try sema.log2IntType(block, operand_ty, src);
    return sema.addType(res_ty);
}

fn log2IntType(sema: *Sema, block: *Block, operand: Type, src: LazySrcLoc) CompileError!Type {
    switch (operand.zigTypeTag()) {
        .ComptimeInt => return Type.comptime_int,
        .Int => {
            const bits = operand.bitSize(sema.mod.getTarget());
            const count = if (bits == 0)
                0
            else blk: {
                var count: u16 = 0;
                var s = bits - 1;
                while (s != 0) : (s >>= 1) {
                    count += 1;
                }
                break :blk count;
            };
            return Module.makeIntType(sema.arena, .unsigned, count);
        },
        .Vector => {
            const elem_ty = operand.elemType2();
            const log2_elem_ty = try sema.log2IntType(block, elem_ty, src);
            return Type.Tag.vector.create(sema.arena, .{
                .len = operand.vectorLen(),
                .elem_type = log2_elem_ty,
            });
        },
        else => {},
    }
    return sema.fail(
        block,
        src,
        "bit shifting operation expected integer type, found '{}'",
        .{operand.fmt(sema.mod)},
    );
}

fn zirTypeofPeer(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const extra = sema.code.extraData(Zir.Inst.TypeOfPeer, extended.operand);
    const src = LazySrcLoc.nodeOffset(extra.data.src_node);
    const body = sema.code.extra[extra.data.body_index..][0..extra.data.body_len];

    var child_block: Block = .{
        .parent = block,
        .sema = sema,
        .src_decl = block.src_decl,
        .namespace = block.namespace,
        .wip_capture_scope = block.wip_capture_scope,
        .instructions = .{},
        .inlining = block.inlining,
        .is_comptime = false,
        .is_typeof = true,
        .runtime_cond = block.runtime_cond,
        .runtime_loop = block.runtime_loop,
        .runtime_index = block.runtime_index,
    };
    defer child_block.instructions.deinit(sema.gpa);
    // Ignore the result, we only care about the instructions in `args`.
    _ = try sema.analyzeBodyBreak(&child_block, body);

    const args = sema.code.refSlice(extra.end, extended.small);

    const inst_list = try sema.gpa.alloc(Air.Inst.Ref, args.len);
    defer sema.gpa.free(inst_list);

    for (args, 0..) |arg_ref, i| {
        inst_list[i] = try sema.resolveInst(arg_ref);
    }

    const result_type = try sema.resolvePeerTypes(block, src, inst_list, .{ .typeof_builtin_call_node_offset = extra.data.src_node });
    return sema.addType(result_type);
}

fn zirBoolNot(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_un_op = inst_data.src_node };
    const uncasted_operand = try sema.resolveInst(inst_data.operand);

    const operand = try sema.coerce(block, Type.bool, uncasted_operand, operand_src);
    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        return if (val.isUndef())
            sema.addConstUndef(Type.bool)
        else if (val.toBool())
            Air.Inst.Ref.bool_false
        else
            Air.Inst.Ref.bool_true;
    }
    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.not, Type.bool, operand);
}

fn zirBoolBr(
    sema: *Sema,
    parent_block: *Block,
    inst: Zir.Inst.Index,
    is_bool_or: bool,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const datas = sema.code.instructions.items(.data);
    const inst_data = datas[inst].bool_br;
    const lhs = try sema.resolveInst(inst_data.lhs);
    const lhs_src = sema.src;
    const extra = sema.code.extraData(Zir.Inst.Block, inst_data.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];
    const gpa = sema.gpa;

    if (try sema.resolveDefinedValue(parent_block, lhs_src, lhs)) |lhs_val| {
        if (is_bool_or and lhs_val.toBool()) {
            return Air.Inst.Ref.bool_true;
        } else if (!is_bool_or and !lhs_val.toBool()) {
            return Air.Inst.Ref.bool_false;
        }
        // comptime-known left-hand side. No need for a block here; the result
        // is simply the rhs expression. Here we rely on there only being 1
        // break instruction (`break_inline`).
        return sema.resolveBody(parent_block, body, inst);
    }

    const block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
    try sema.air_instructions.append(gpa, .{
        .tag = .block,
        .data = .{ .ty_pl = .{
            .ty = .bool_type,
            .payload = undefined,
        } },
    });

    var child_block = parent_block.makeSubBlock();
    child_block.runtime_loop = null;
    child_block.runtime_cond = lhs_src;
    child_block.runtime_index.increment();
    defer child_block.instructions.deinit(gpa);

    var then_block = child_block.makeSubBlock();
    defer then_block.instructions.deinit(gpa);

    var else_block = child_block.makeSubBlock();
    defer else_block.instructions.deinit(gpa);

    const lhs_block = if (is_bool_or) &then_block else &else_block;
    const rhs_block = if (is_bool_or) &else_block else &then_block;

    const lhs_result: Air.Inst.Ref = if (is_bool_or) .bool_true else .bool_false;
    _ = try lhs_block.addBr(block_inst, lhs_result);

    const rhs_result = try sema.resolveBody(rhs_block, body, inst);
    if (!sema.typeOf(rhs_result).isNoReturn()) {
        _ = try rhs_block.addBr(block_inst, rhs_result);
    }

    const result = sema.finishCondBr(parent_block, &child_block, &then_block, &else_block, lhs, block_inst);
    if (!sema.typeOf(rhs_result).isNoReturn()) {
        if (try sema.resolveDefinedValue(rhs_block, sema.src, rhs_result)) |rhs_val| {
            if (is_bool_or and rhs_val.toBool()) {
                return Air.Inst.Ref.bool_true;
            } else if (!is_bool_or and !rhs_val.toBool()) {
                return Air.Inst.Ref.bool_false;
            }
        }
    }

    return result;
}

fn finishCondBr(
    sema: *Sema,
    parent_block: *Block,
    child_block: *Block,
    then_block: *Block,
    else_block: *Block,
    cond: Air.Inst.Ref,
    block_inst: Air.Inst.Index,
) !Air.Inst.Ref {
    const gpa = sema.gpa;

    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.CondBr).Struct.fields.len +
        then_block.instructions.items.len + else_block.instructions.items.len +
        @typeInfo(Air.Block).Struct.fields.len + child_block.instructions.items.len + 1);

    const cond_br_payload = sema.addExtraAssumeCapacity(Air.CondBr{
        .then_body_len = @intCast(u32, then_block.instructions.items.len),
        .else_body_len = @intCast(u32, else_block.instructions.items.len),
    });
    sema.air_extra.appendSliceAssumeCapacity(then_block.instructions.items);
    sema.air_extra.appendSliceAssumeCapacity(else_block.instructions.items);

    _ = try child_block.addInst(.{ .tag = .cond_br, .data = .{ .pl_op = .{
        .operand = cond,
        .payload = cond_br_payload,
    } } });

    sema.air_instructions.items(.data)[block_inst].ty_pl.payload = sema.addExtraAssumeCapacity(
        Air.Block{ .body_len = @intCast(u32, child_block.instructions.items.len) },
    );
    sema.air_extra.appendSliceAssumeCapacity(child_block.instructions.items);

    try parent_block.instructions.append(gpa, block_inst);
    return Air.indexToRef(block_inst);
}

fn checkNullableType(sema: *Sema, block: *Block, src: LazySrcLoc, ty: Type) !void {
    switch (ty.zigTypeTag()) {
        .Optional, .Null, .Undefined => return,
        .Pointer => if (ty.isPtrLikeOptional()) return,
        else => {},
    }
    return sema.failWithExpectedOptionalType(block, src, ty);
}

fn zirIsNonNull(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    try sema.checkNullableType(block, src, sema.typeOf(operand));
    return sema.analyzeIsNull(block, src, operand, true);
}

fn zirIsNonNullPtr(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ptr = try sema.resolveInst(inst_data.operand);
    try sema.checkNullableType(block, src, sema.typeOf(ptr).elemType2());
    if ((try sema.resolveMaybeUndefVal(ptr)) == null) {
        return block.addUnOp(.is_non_null_ptr, ptr);
    }
    const loaded = try sema.analyzeLoad(block, src, ptr, src);
    return sema.analyzeIsNull(block, src, loaded, true);
}

fn checkErrorType(sema: *Sema, block: *Block, src: LazySrcLoc, ty: Type) !void {
    switch (ty.zigTypeTag()) {
        .ErrorSet, .ErrorUnion, .Undefined => return,
        else => return sema.fail(block, src, "expected error union type, found '{}'", .{
            ty.fmt(sema.mod),
        }),
    }
}

fn zirIsNonErr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    try sema.checkErrorType(block, src, sema.typeOf(operand));
    return sema.analyzeIsNonErr(block, src, operand);
}

fn zirIsNonErrPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ptr = try sema.resolveInst(inst_data.operand);
    try sema.checkErrorType(block, src, sema.typeOf(ptr).elemType2());
    const loaded = try sema.analyzeLoad(block, src, ptr, src);
    return sema.analyzeIsNonErr(block, src, loaded);
}

fn zirRetIsNonErr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    return sema.analyzeIsNonErr(block, src, operand);
}

fn zirCondbr(
    sema: *Sema,
    parent_block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const cond_src: LazySrcLoc = .{ .node_offset_if_cond = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.CondBr, inst_data.payload_index);

    const then_body = sema.code.extra[extra.end..][0..extra.data.then_body_len];
    const else_body = sema.code.extra[extra.end + then_body.len ..][0..extra.data.else_body_len];

    const uncasted_cond = try sema.resolveInst(extra.data.condition);
    const cond = try sema.coerce(parent_block, Type.bool, uncasted_cond, cond_src);

    if (try sema.resolveDefinedValue(parent_block, cond_src, cond)) |cond_val| {
        const body = if (cond_val.toBool()) then_body else else_body;

        try sema.maybeErrorUnwrapCondbr(parent_block, body, extra.data.condition, cond_src);
        // We use `analyzeBodyInner` since we want to propagate any possible
        // `error.ComptimeBreak` to the caller.
        return sema.analyzeBodyInner(parent_block, body);
    }

    const gpa = sema.gpa;

    // We'll re-use the sub block to save on memory bandwidth, and yank out the
    // instructions array in between using it for the then block and else block.
    var sub_block = parent_block.makeSubBlock();
    sub_block.runtime_loop = null;
    sub_block.runtime_cond = cond_src;
    sub_block.runtime_index.increment();
    defer sub_block.instructions.deinit(gpa);

    try sema.analyzeBodyRuntimeBreak(&sub_block, then_body);
    const true_instructions = try sub_block.instructions.toOwnedSlice(gpa);
    defer gpa.free(true_instructions);

    const err_cond = blk: {
        const index = Zir.refToIndex(extra.data.condition) orelse break :blk null;
        if (sema.code.instructions.items(.tag)[index] != .is_non_err) break :blk null;

        const err_inst_data = sema.code.instructions.items(.data)[index].un_node;
        const err_operand = try sema.resolveInst(err_inst_data.operand);
        const operand_ty = sema.typeOf(err_operand);
        assert(operand_ty.zigTypeTag() == .ErrorUnion);
        const result_ty = operand_ty.errorUnionSet();
        break :blk try sub_block.addTyOp(.unwrap_errunion_err, result_ty, err_operand);
    };

    if (err_cond != null and try sema.maybeErrorUnwrap(&sub_block, else_body, err_cond.?)) {
        // nothing to do
    } else {
        try sema.analyzeBodyRuntimeBreak(&sub_block, else_body);
    }
    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.CondBr).Struct.fields.len +
        true_instructions.len + sub_block.instructions.items.len);
    _ = try parent_block.addInst(.{
        .tag = .cond_br,
        .data = .{ .pl_op = .{
            .operand = cond,
            .payload = sema.addExtraAssumeCapacity(Air.CondBr{
                .then_body_len = @intCast(u32, true_instructions.len),
                .else_body_len = @intCast(u32, sub_block.instructions.items.len),
            }),
        } },
    });
    sema.air_extra.appendSliceAssumeCapacity(true_instructions);
    sema.air_extra.appendSliceAssumeCapacity(sub_block.instructions.items);
    return always_noreturn;
}

fn zirTry(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];
    const err_union = try sema.resolveInst(extra.data.operand);
    const err_union_ty = sema.typeOf(err_union);
    if (err_union_ty.zigTypeTag() != .ErrorUnion) {
        return sema.fail(parent_block, operand_src, "expected error union type, found '{}'", .{
            err_union_ty.fmt(sema.mod),
        });
    }
    const is_non_err = try sema.analyzeIsNonErrComptimeOnly(parent_block, operand_src, err_union);
    if (is_non_err != .none) {
        const is_non_err_val = (try sema.resolveDefinedValue(parent_block, operand_src, is_non_err)).?;
        if (is_non_err_val.toBool()) {
            return sema.analyzeErrUnionPayload(parent_block, src, err_union_ty, err_union, operand_src, false);
        }
        // We can analyze the body directly in the parent block because we know there are
        // no breaks from the body possible, and that the body is noreturn.
        return sema.resolveBody(parent_block, body, inst);
    }

    var sub_block = parent_block.makeSubBlock();
    defer sub_block.instructions.deinit(sema.gpa);

    // This body is guaranteed to end with noreturn and has no breaks.
    _ = try sema.analyzeBodyInner(&sub_block, body);

    try sema.air_extra.ensureUnusedCapacity(sema.gpa, @typeInfo(Air.Try).Struct.fields.len +
        sub_block.instructions.items.len);
    const try_inst = try parent_block.addInst(.{
        .tag = .@"try",
        .data = .{ .pl_op = .{
            .operand = err_union,
            .payload = sema.addExtraAssumeCapacity(Air.Try{
                .body_len = @intCast(u32, sub_block.instructions.items.len),
            }),
        } },
    });
    sema.air_extra.appendSliceAssumeCapacity(sub_block.instructions.items);
    return try_inst;
}

fn zirTryPtr(sema: *Sema, parent_block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_bin_lhs = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Try, inst_data.payload_index);
    const body = sema.code.extra[extra.end..][0..extra.data.body_len];
    const operand = try sema.resolveInst(extra.data.operand);
    const err_union = try sema.analyzeLoad(parent_block, src, operand, operand_src);
    const err_union_ty = sema.typeOf(err_union);
    if (err_union_ty.zigTypeTag() != .ErrorUnion) {
        return sema.fail(parent_block, operand_src, "expected error union type, found '{}'", .{
            err_union_ty.fmt(sema.mod),
        });
    }
    const is_non_err = try sema.analyzeIsNonErrComptimeOnly(parent_block, operand_src, err_union);
    if (is_non_err != .none) {
        const is_non_err_val = (try sema.resolveDefinedValue(parent_block, operand_src, is_non_err)).?;
        if (is_non_err_val.toBool()) {
            return sema.analyzeErrUnionPayloadPtr(parent_block, src, operand, false, false);
        }
        // We can analyze the body directly in the parent block because we know there are
        // no breaks from the body possible, and that the body is noreturn.
        return sema.resolveBody(parent_block, body, inst);
    }

    var sub_block = parent_block.makeSubBlock();
    defer sub_block.instructions.deinit(sema.gpa);

    // This body is guaranteed to end with noreturn and has no breaks.
    _ = try sema.analyzeBodyInner(&sub_block, body);

    const operand_ty = sema.typeOf(operand);
    const ptr_info = operand_ty.ptrInfo().data;
    const res_ty = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = err_union_ty.errorUnionPayload(),
        .@"addrspace" = ptr_info.@"addrspace",
        .mutable = ptr_info.mutable,
        .@"allowzero" = ptr_info.@"allowzero",
        .@"volatile" = ptr_info.@"volatile",
    });
    const res_ty_ref = try sema.addType(res_ty);
    try sema.air_extra.ensureUnusedCapacity(sema.gpa, @typeInfo(Air.TryPtr).Struct.fields.len +
        sub_block.instructions.items.len);
    const try_inst = try parent_block.addInst(.{
        .tag = .try_ptr,
        .data = .{ .ty_pl = .{
            .ty = res_ty_ref,
            .payload = sema.addExtraAssumeCapacity(Air.TryPtr{
                .ptr = operand,
                .body_len = @intCast(u32, sub_block.instructions.items.len),
            }),
        } },
    });
    sema.air_extra.appendSliceAssumeCapacity(sub_block.instructions.items);
    return try_inst;
}

// A `break` statement is inside a runtime condition, but trying to
// break from an inline loop. In such case we must convert it to
// a runtime break.
fn addRuntimeBreak(sema: *Sema, child_block: *Block, break_data: BreakData) !void {
    const gop = sema.inst_map.getOrPutAssumeCapacity(break_data.block_inst);
    const labeled_block = if (!gop.found_existing) blk: {
        try sema.post_hoc_blocks.ensureUnusedCapacity(sema.gpa, 1);

        const new_block_inst = @intCast(Air.Inst.Index, sema.air_instructions.len);
        gop.value_ptr.* = Air.indexToRef(new_block_inst);
        try sema.air_instructions.append(sema.gpa, .{
            .tag = .block,
            .data = undefined,
        });
        const labeled_block = try sema.gpa.create(LabeledBlock);
        labeled_block.* = .{
            .label = .{
                .zir_block = break_data.block_inst,
                .merges = .{
                    .results = .{},
                    .br_list = .{},
                    .block_inst = new_block_inst,
                },
            },
            .block = .{
                .parent = child_block,
                .sema = sema,
                .src_decl = child_block.src_decl,
                .namespace = child_block.namespace,
                .wip_capture_scope = child_block.wip_capture_scope,
                .instructions = .{},
                .label = &labeled_block.label,
                .inlining = child_block.inlining,
                .is_comptime = child_block.is_comptime,
            },
        };
        sema.post_hoc_blocks.putAssumeCapacityNoClobber(new_block_inst, labeled_block);
        break :blk labeled_block;
    } else blk: {
        const new_block_inst = Air.refToIndex(gop.value_ptr.*).?;
        const labeled_block = sema.post_hoc_blocks.get(new_block_inst).?;
        break :blk labeled_block;
    };

    const operand = try sema.resolveInst(break_data.operand);
    const br_ref = try child_block.addBr(labeled_block.label.merges.block_inst, operand);
    try labeled_block.label.merges.results.append(sema.gpa, operand);
    try labeled_block.label.merges.br_list.append(sema.gpa, Air.refToIndex(br_ref).?);
    labeled_block.block.runtime_index.increment();
    if (labeled_block.block.runtime_cond == null and labeled_block.block.runtime_loop == null) {
        labeled_block.block.runtime_cond = child_block.runtime_cond orelse child_block.runtime_loop;
        labeled_block.block.runtime_loop = child_block.runtime_loop;
    }
}

fn zirUnreachable(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const inst_data = sema.code.instructions.items(.data)[inst].@"unreachable";
    const src = inst_data.src();

    if (block.is_comptime or inst_data.force_comptime) {
        return sema.fail(block, src, "reached unreachable code", .{});
    }
    // TODO Add compile error for @optimizeFor occurring too late in a scope.
    try block.addUnreachable(true);
    return always_noreturn;
}

fn zirRetErrValue(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Zir.Inst.Index {
    const inst_data = sema.code.instructions.items(.data)[inst].str_tok;
    const err_name = inst_data.get(sema.code);
    const src = inst_data.src();

    // Return the error code from the function.
    const kv = try sema.mod.getErrorValue(err_name);
    const result_inst = try sema.addConstant(
        try Type.Tag.error_set_single.create(sema.arena, kv.key),
        try Value.Tag.@"error".create(sema.arena, .{ .name = kv.key }),
    );
    return sema.analyzeRet(block, result_inst, src);
}

fn zirRetImplicit(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_tok;
    const operand = try sema.resolveInst(inst_data.operand);

    const r_brace_src = inst_data.src();
    const ret_ty_src: LazySrcLoc = .{ .node_offset_fn_type_ret_ty = 0 };
    const base_tag = sema.fn_ret_ty.baseZigTypeTag();
    if (base_tag == .NoReturn) {
        const msg = msg: {
            const msg = try sema.errMsg(block, ret_ty_src, "function declared '{}' implicitly returns", .{
                sema.fn_ret_ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, r_brace_src, msg, "control flow reaches end of body here", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    } else if (base_tag != .Void) {
        const msg = msg: {
            const msg = try sema.errMsg(block, ret_ty_src, "function with non-void return type '{}' implicitly returns", .{
                sema.fn_ret_ty.fmt(sema.mod),
            });
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, r_brace_src, msg, "control flow reaches end of body here", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    return sema.analyzeRet(block, operand, .unneeded);
}

fn zirRetNode(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const src = inst_data.src();

    return sema.analyzeRet(block, operand, src);
}

fn zirRetLoad(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Zir.Inst.Index {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const ret_ptr = try sema.resolveInst(inst_data.operand);

    if (block.is_comptime or block.inlining != null) {
        const operand = try sema.analyzeLoad(block, src, ret_ptr, src);
        return sema.analyzeRet(block, operand, src);
    }

    if (sema.wantErrorReturnTracing(sema.fn_ret_ty)) {
        const is_non_err = try sema.analyzePtrIsNonErr(block, src, ret_ptr);
        return sema.retWithErrTracing(block, src, is_non_err, .ret_load, ret_ptr);
    }

    _ = try block.addUnOp(.ret_load, ret_ptr);
    return always_noreturn;
}

fn retWithErrTracing(
    sema: *Sema,
    block: *Block,
    src: LazySrcLoc,
    is_non_err: Air.Inst.Ref,
    ret_tag: Air.Inst.Tag,
    operand: Air.Inst.Ref,
) CompileError!Zir.Inst.Index {
    const need_check = switch (is_non_err) {
        .bool_true => {
            _ = try block.addUnOp(ret_tag, operand);
            return always_noreturn;
        },
        .bool_false => false,
        else => true,
    };
    const gpa = sema.gpa;
    const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
    const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
    const ptr_stack_trace_ty = try Type.Tag.single_mut_pointer.create(sema.arena, stack_trace_ty);
    const err_return_trace = try block.addTy(.err_return_trace, ptr_stack_trace_ty);
    const return_err_fn = try sema.getBuiltin("returnError");
    const args: [1]Air.Inst.Ref = .{err_return_trace};

    if (!need_check) {
        _ = try sema.analyzeCall(block, return_err_fn, src, src, .never_inline, false, &args, null);
        _ = try block.addUnOp(ret_tag, operand);
        return always_noreturn;
    }

    var then_block = block.makeSubBlock();
    defer then_block.instructions.deinit(gpa);
    _ = try then_block.addUnOp(ret_tag, operand);

    var else_block = block.makeSubBlock();
    defer else_block.instructions.deinit(gpa);
    _ = try sema.analyzeCall(&else_block, return_err_fn, src, src, .never_inline, false, &args, null);
    _ = try else_block.addUnOp(ret_tag, operand);

    try sema.air_extra.ensureUnusedCapacity(gpa, @typeInfo(Air.CondBr).Struct.fields.len +
        then_block.instructions.items.len + else_block.instructions.items.len +
        @typeInfo(Air.Block).Struct.fields.len + 1);

    const cond_br_payload = sema.addExtraAssumeCapacity(Air.CondBr{
        .then_body_len = @intCast(u32, then_block.instructions.items.len),
        .else_body_len = @intCast(u32, else_block.instructions.items.len),
    });
    sema.air_extra.appendSliceAssumeCapacity(then_block.instructions.items);
    sema.air_extra.appendSliceAssumeCapacity(else_block.instructions.items);

    _ = try block.addInst(.{ .tag = .cond_br, .data = .{ .pl_op = .{
        .operand = is_non_err,
        .payload = cond_br_payload,
    } } });

    return always_noreturn;
}

fn wantErrorReturnTracing(sema: *Sema, fn_ret_ty: Type) bool {
    if (!sema.mod.backendSupportsFeature(.error_return_trace)) return false;

    return fn_ret_ty.isError() and
        sema.mod.comp.bin_file.options.error_return_tracing;
}

fn zirSaveErrRetIndex(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].save_err_ret_index;

    if (!sema.mod.backendSupportsFeature(.error_return_trace)) return;
    if (!sema.mod.comp.bin_file.options.error_return_tracing) return;

    // This is only relevant at runtime.
    if (block.is_comptime or block.is_typeof) return;

    const save_index = inst_data.operand == .none or b: {
        const operand = try sema.resolveInst(inst_data.operand);
        const operand_ty = sema.typeOf(operand);
        break :b operand_ty.isError();
    };

    if (save_index)
        block.error_return_trace_index = try sema.analyzeSaveErrRetIndex(block);
}

fn zirRestoreErrRetIndex(sema: *Sema, start_block: *Block, inst: Zir.Inst.Index) CompileError!void {
    const inst_data = sema.code.instructions.items(.data)[inst].restore_err_ret_index;
    const src = sema.src; // TODO

    // This is only relevant at runtime.
    if (start_block.is_comptime or start_block.is_typeof) return;

    if (!sema.mod.backendSupportsFeature(.error_return_trace)) return;
    if (!sema.owner_func.?.calls_or_awaits_errorable_fn) return;
    if (!sema.mod.comp.bin_file.options.error_return_tracing) return;

    const tracy = trace(@src());
    defer tracy.end();

    const saved_index = if (Zir.refToIndex(inst_data.block)) |zir_block| b: {
        var block = start_block;
        while (true) {
            if (block.label) |label| {
                if (label.zir_block == zir_block) {
                    const target_trace_index = if (block.parent) |parent_block| tgt: {
                        break :tgt parent_block.error_return_trace_index;
                    } else sema.error_return_trace_index_on_fn_entry;

                    if (start_block.error_return_trace_index != target_trace_index)
                        break :b target_trace_index;

                    return; // No need to restore
                }
            }
            block = block.parent.?;
        }
    } else b: {
        if (start_block.error_return_trace_index != sema.error_return_trace_index_on_fn_entry)
            break :b sema.error_return_trace_index_on_fn_entry;

        return; // No need to restore
    };

    assert(saved_index != .none); // The .error_return_trace_index field was dropped somewhere

    const operand = try sema.resolveInst(inst_data.operand);
    return sema.popErrorReturnTrace(start_block, src, operand, saved_index);
}

fn addToInferredErrorSet(sema: *Sema, uncasted_operand: Air.Inst.Ref) !void {
    assert(sema.fn_ret_ty.zigTypeTag() == .ErrorUnion);

    if (sema.fn_ret_ty.errorUnionSet().castTag(.error_set_inferred)) |payload| {
        const op_ty = sema.typeOf(uncasted_operand);
        switch (op_ty.zigTypeTag()) {
            .ErrorSet => {
                try payload.data.addErrorSet(sema.gpa, op_ty);
            },
            .ErrorUnion => {
                try payload.data.addErrorSet(sema.gpa, op_ty.errorUnionSet());
            },
            else => {},
        }
    }
}

fn analyzeRet(
    sema: *Sema,
    block: *Block,
    uncasted_operand: Air.Inst.Ref,
    src: LazySrcLoc,
) CompileError!Zir.Inst.Index {
    // Special case for returning an error to an inferred error set; we need to
    // add the error tag to the inferred error set of the in-scope function, so
    // that the coercion below works correctly.
    if (sema.fn_ret_ty.zigTypeTag() == .ErrorUnion) {
        try sema.addToInferredErrorSet(uncasted_operand);
    }
    const operand = sema.coerceExtra(block, sema.fn_ret_ty, uncasted_operand, src, .{ .is_ret = true }) catch |err| switch (err) {
        error.NotCoercible => unreachable,
        else => |e| return e,
    };

    if (block.inlining) |inlining| {
        if (block.is_comptime) {
            _ = try sema.resolveConstMaybeUndefVal(block, src, operand, "value being returned at comptime must be comptime-known");
            inlining.comptime_result = operand;
            return error.ComptimeReturn;
        }
        // We are inlining a function call; rewrite the `ret` as a `break`.
        try inlining.merges.results.append(sema.gpa, operand);
        _ = try block.addBr(inlining.merges.block_inst, operand);
        return always_noreturn;
    }

    try sema.resolveTypeLayout(sema.fn_ret_ty);

    if (sema.wantErrorReturnTracing(sema.fn_ret_ty)) {
        // Avoid adding a frame to the error return trace in case the value is comptime-known
        // to be not an error.
        const is_non_err = try sema.analyzeIsNonErr(block, src, operand);
        return sema.retWithErrTracing(block, src, is_non_err, .ret, operand);
    }

    _ = try block.addUnOp(.ret, operand);
    return always_noreturn;
}

fn floatOpAllowed(tag: Zir.Inst.Tag) bool {
    // extend this swich as additional operators are implemented
    return switch (tag) {
        .add, .sub, .mul, .div, .div_exact, .div_trunc, .div_floor, .mod, .rem, .mod_rem => true,
        else => false,
    };
}

fn zirPtrType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].ptr_type;
    const extra = sema.code.extraData(Zir.Inst.PtrType, inst_data.payload_index);
    const elem_ty_src: LazySrcLoc = .{ .node_offset_ptr_elem = extra.data.src_node };
    const sentinel_src: LazySrcLoc = .{ .node_offset_ptr_sentinel = extra.data.src_node };
    const align_src: LazySrcLoc = .{ .node_offset_ptr_align = extra.data.src_node };
    const addrspace_src: LazySrcLoc = .{ .node_offset_ptr_addrspace = extra.data.src_node };
    const bitoffset_src: LazySrcLoc = .{ .node_offset_ptr_bitoffset = extra.data.src_node };
    const hostsize_src: LazySrcLoc = .{ .node_offset_ptr_hostsize = extra.data.src_node };

    const elem_ty = blk: {
        const air_inst = try sema.resolveInst(extra.data.elem_type);
        const ty = sema.analyzeAsType(block, elem_ty_src, air_inst) catch |err| {
            if (err == error.AnalysisFail and sema.err != null and sema.typeOf(air_inst).isSinglePointer()) {
                try sema.errNote(block, elem_ty_src, sema.err.?, "use '.*' to dereference pointer", .{});
            }
            return err;
        };
        if (ty.tag() == .generic_poison) return error.GenericPoison;
        break :blk ty;
    };
    const target = sema.mod.getTarget();

    var extra_i = extra.end;

    const sentinel = if (inst_data.flags.has_sentinel) blk: {
        const ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_i]);
        extra_i += 1;
        break :blk (try sema.resolveInstConst(block, sentinel_src, ref, "pointer sentinel value must be comptime-known")).val;
    } else null;

    const abi_align: u32 = if (inst_data.flags.has_align) blk: {
        const ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_i]);
        extra_i += 1;
        const coerced = try sema.coerce(block, Type.u32, try sema.resolveInst(ref), align_src);
        const val = try sema.resolveConstValue(block, align_src, coerced, "pointer alignment must be comptime-known");
        // Check if this happens to be the lazy alignment of our element type, in
        // which case we can make this 0 without resolving it.
        if (val.castTag(.lazy_align)) |payload| {
            if (payload.data.eql(elem_ty, sema.mod)) {
                break :blk 0;
            }
        }
        const abi_align = @intCast(u32, (try val.getUnsignedIntAdvanced(target, sema)).?);
        try sema.validateAlign(block, align_src, abi_align);
        break :blk abi_align;
    } else 0;

    const address_space: std.builtin.AddressSpace = if (inst_data.flags.has_addrspace) blk: {
        const ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_i]);
        extra_i += 1;
        break :blk try sema.analyzeAddressSpace(block, addrspace_src, ref, .pointer);
    } else if (elem_ty.zigTypeTag() == .Fn and target.cpu.arch == .avr) .flash else .generic;

    const bit_offset = if (inst_data.flags.has_bit_range) blk: {
        const ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_i]);
        extra_i += 1;
        const bit_offset = try sema.resolveInt(block, bitoffset_src, ref, Type.u16, "pointer bit-offset must be comptime-known");
        break :blk @intCast(u16, bit_offset);
    } else 0;

    const host_size: u16 = if (inst_data.flags.has_bit_range) blk: {
        const ref = @intToEnum(Zir.Inst.Ref, sema.code.extra[extra_i]);
        extra_i += 1;
        const host_size = try sema.resolveInt(block, hostsize_src, ref, Type.u16, "pointer host size must be comptime-known");
        break :blk @intCast(u16, host_size);
    } else 0;

    if (host_size != 0 and bit_offset >= host_size * 8) {
        return sema.fail(block, bitoffset_src, "bit offset starts after end of host integer", .{});
    }

    if (elem_ty.zigTypeTag() == .NoReturn) {
        return sema.fail(block, elem_ty_src, "pointer to noreturn not allowed", .{});
    } else if (elem_ty.zigTypeTag() == .Fn) {
        if (inst_data.size != .One) {
            return sema.fail(block, elem_ty_src, "function pointers must be single pointers", .{});
        }
        const fn_align = elem_ty.fnInfo().alignment;
        if (inst_data.flags.has_align and abi_align != 0 and fn_align != 0 and
            abi_align != fn_align)
        {
            return sema.fail(block, align_src, "function pointer alignment disagrees with function alignment", .{});
        }
    } else if (inst_data.size == .Many and elem_ty.zigTypeTag() == .Opaque) {
        return sema.fail(block, elem_ty_src, "unknown-length pointer to opaque not allowed", .{});
    } else if (inst_data.size == .C) {
        if (!try sema.validateExternType(elem_ty, .other)) {
            const msg = msg: {
                const msg = try sema.errMsg(block, elem_ty_src, "C pointers cannot point to non-C-ABI-compatible type '{}'", .{elem_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);

                const src_decl = sema.mod.declPtr(block.src_decl);
                try sema.explainWhyTypeIsNotExtern(msg, elem_ty_src.toSrcLoc(src_decl), elem_ty, .other);

                try sema.addDeclaredHereNote(msg, elem_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        if (elem_ty.zigTypeTag() == .Opaque) {
            return sema.fail(block, elem_ty_src, "C pointers cannot point to opaque types", .{});
        }
    }

    const ty = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = elem_ty,
        .sentinel = sentinel,
        .@"align" = abi_align,
        .@"addrspace" = address_space,
        .bit_offset = bit_offset,
        .host_size = host_size,
        .mutable = inst_data.flags.is_mutable,
        .@"allowzero" = inst_data.flags.is_allowzero,
        .@"volatile" = inst_data.flags.is_volatile,
        .size = inst_data.size,
    });
    return sema.addType(ty);
}

fn zirStructInitEmpty(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const obj_ty = try sema.resolveType(block, src, inst_data.operand);

    switch (obj_ty.zigTypeTag()) {
        .Struct => return sema.structInitEmpty(block, obj_ty, src, src),
        .Array, .Vector => return sema.arrayInitEmpty(block, src, obj_ty),
        .Void => return sema.addConstant(obj_ty, Value.void),
        .Union => return sema.fail(block, src, "union initializer must initialize one field", .{}),
        else => return sema.failWithArrayInitNotSupported(block, src, obj_ty),
    }
}

fn structInitEmpty(
    sema: *Sema,
    block: *Block,
    obj_ty: Type,
    dest_src: LazySrcLoc,
    init_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    const gpa = sema.gpa;
    // This logic must be synchronized with that in `zirStructInit`.
    const struct_ty = try sema.resolveTypeFields(obj_ty);

    // The init values to use for the struct instance.
    const field_inits = try gpa.alloc(Air.Inst.Ref, struct_ty.structFieldCount());
    defer gpa.free(field_inits);
    mem.set(Air.Inst.Ref, field_inits, .none);

    return sema.finishStructInit(block, init_src, dest_src, field_inits, struct_ty, false);
}

fn arrayInitEmpty(sema: *Sema, block: *Block, src: LazySrcLoc, obj_ty: Type) CompileError!Air.Inst.Ref {
    const arr_len = obj_ty.arrayLen();
    if (arr_len != 0) {
        if (obj_ty.zigTypeTag() == .Array) {
            return sema.fail(block, src, "expected {d} array elements; found 0", .{arr_len});
        } else {
            return sema.fail(block, src, "expected {d} vector elements; found 0", .{arr_len});
        }
    }
    if (obj_ty.sentinel()) |sentinel| {
        const val = try Value.Tag.empty_array_sentinel.create(sema.arena, sentinel);
        return sema.addConstant(obj_ty, val);
    } else {
        return sema.addConstant(obj_ty, Value.initTag(.empty_array));
    }
}

fn zirUnionInit(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const field_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const init_src: LazySrcLoc = .{ .node_offset_builtin_call_arg2 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.UnionInit, inst_data.payload_index).data;
    const union_ty = try sema.resolveType(block, ty_src, extra.union_type);
    const field_name = try sema.resolveConstString(block, field_src, extra.field_name, "name of field being initialized must be comptime-known");
    const init = try sema.resolveInst(extra.init);
    return sema.unionInit(block, init, init_src, union_ty, ty_src, field_name, field_src);
}

fn unionInit(
    sema: *Sema,
    block: *Block,
    uncasted_init: Air.Inst.Ref,
    init_src: LazySrcLoc,
    union_ty: Type,
    union_ty_src: LazySrcLoc,
    field_name: []const u8,
    field_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    const field_index = try sema.unionFieldIndex(block, union_ty, field_name, field_src);
    const field = union_ty.unionFields().values()[field_index];
    const init = try sema.coerce(block, field.ty, uncasted_init, init_src);

    if (try sema.resolveMaybeUndefVal(init)) |init_val| {
        const tag_ty = union_ty.unionTagTypeHypothetical();
        const enum_field_index = @intCast(u32, tag_ty.enumFieldIndex(field_name).?);
        const tag_val = try Value.Tag.enum_field_index.create(sema.arena, enum_field_index);
        return sema.addConstant(union_ty, try Value.Tag.@"union".create(sema.arena, .{
            .tag = tag_val,
            .val = init_val,
        }));
    }

    try sema.requireRuntimeBlock(block, init_src, null);
    _ = union_ty_src;
    try sema.queueFullTypeResolution(union_ty);
    return block.addUnionInit(union_ty, field_index, init);
}

fn zirStructInit(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const gpa = sema.gpa;
    const zir_datas = sema.code.instructions.items(.data);
    const inst_data = zir_datas[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.StructInit, inst_data.payload_index);
    const src = inst_data.src();

    const first_item = sema.code.extraData(Zir.Inst.StructInit.Item, extra.end).data;
    const first_field_type_data = zir_datas[first_item.field_type].pl_node;
    const first_field_type_extra = sema.code.extraData(Zir.Inst.FieldType, first_field_type_data.payload_index).data;
    const resolved_ty = try sema.resolveType(block, src, first_field_type_extra.container_type);
    try sema.resolveTypeLayout(resolved_ty);

    if (resolved_ty.zigTypeTag() == .Struct) {
        // This logic must be synchronized with that in `zirStructInitEmpty`.

        // Maps field index to field_type index of where it was already initialized.
        // For making sure all fields are accounted for and no fields are duplicated.
        const found_fields = try gpa.alloc(Zir.Inst.Index, resolved_ty.structFieldCount());
        defer gpa.free(found_fields);

        // The init values to use for the struct instance.
        const field_inits = try gpa.alloc(Air.Inst.Ref, resolved_ty.structFieldCount());
        defer gpa.free(field_inits);
        mem.set(Air.Inst.Ref, field_inits, .none);

        var field_i: u32 = 0;
        var extra_index = extra.end;

        const is_packed = resolved_ty.containerLayout() == .Packed;
        while (field_i < extra.data.fields_len) : (field_i += 1) {
            const item = sema.code.extraData(Zir.Inst.StructInit.Item, extra_index);
            extra_index = item.end;

            const field_type_data = zir_datas[item.data.field_type].pl_node;
            const field_src: LazySrcLoc = .{ .node_offset_initializer = field_type_data.src_node };
            const field_type_extra = sema.code.extraData(Zir.Inst.FieldType, field_type_data.payload_index).data;
            const field_name = sema.code.nullTerminatedString(field_type_extra.name_start);
            const field_index = if (resolved_ty.isTuple())
                try sema.tupleFieldIndex(block, resolved_ty, field_name, field_src)
            else
                try sema.structFieldIndex(block, resolved_ty, field_name, field_src);
            if (field_inits[field_index] != .none) {
                const other_field_type = found_fields[field_index];
                const other_field_type_data = zir_datas[other_field_type].pl_node;
                const other_field_src: LazySrcLoc = .{ .node_offset_initializer = other_field_type_data.src_node };
                const msg = msg: {
                    const msg = try sema.errMsg(block, field_src, "duplicate field", .{});
                    errdefer msg.destroy(gpa);
                    try sema.errNote(block, other_field_src, msg, "other field here", .{});
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
            found_fields[field_index] = item.data.field_type;
            field_inits[field_index] = try sema.resolveInst(item.data.init);
            if (!is_packed) if (resolved_ty.structFieldValueComptime(field_index)) |default_value| {
                const init_val = (try sema.resolveMaybeUndefVal(field_inits[field_index])) orelse {
                    return sema.failWithNeededComptime(block, field_src, "value stored in comptime field must be comptime-known");
                };

                if (!init_val.eql(default_value, resolved_ty.structFieldType(field_index), sema.mod)) {
                    return sema.failWithInvalidComptimeFieldStore(block, field_src, resolved_ty, field_index);
                }
            };
        }

        return sema.finishStructInit(block, src, src, field_inits, resolved_ty, is_ref);
    } else if (resolved_ty.zigTypeTag() == .Union) {
        if (extra.data.fields_len != 1) {
            return sema.fail(block, src, "union initialization expects exactly one field", .{});
        }

        const item = sema.code.extraData(Zir.Inst.StructInit.Item, extra.end);

        const field_type_data = zir_datas[item.data.field_type].pl_node;
        const field_src: LazySrcLoc = .{ .node_offset_initializer = field_type_data.src_node };
        const field_type_extra = sema.code.extraData(Zir.Inst.FieldType, field_type_data.payload_index).data;
        const field_name = sema.code.nullTerminatedString(field_type_extra.name_start);
        const field_index = try sema.unionFieldIndex(block, resolved_ty, field_name, field_src);
        const tag_ty = resolved_ty.unionTagTypeHypothetical();
        const enum_field_index = @intCast(u32, tag_ty.enumFieldIndex(field_name).?);
        const tag_val = try Value.Tag.enum_field_index.create(sema.arena, enum_field_index);

        const init_inst = try sema.resolveInst(item.data.init);
        if (try sema.resolveMaybeUndefVal(init_inst)) |val| {
            return sema.addConstantMaybeRef(
                block,
                resolved_ty,
                try Value.Tag.@"union".create(sema.arena, .{ .tag = tag_val, .val = val }),
                is_ref,
            );
        }

        if (is_ref) {
            const target = sema.mod.getTarget();
            const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
                .pointee_type = resolved_ty,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
            });
            const alloc = try block.addTy(.alloc, alloc_ty);
            const field_ptr = try sema.unionFieldPtr(block, field_src, alloc, field_name, field_src, resolved_ty, true);
            try sema.storePtr(block, src, field_ptr, init_inst);
            const new_tag = try sema.addConstant(resolved_ty.unionTagTypeHypothetical(), tag_val);
            _ = try block.addBinOp(.set_union_tag, alloc, new_tag);
            return alloc;
        }

        try sema.requireRuntimeBlock(block, src, null);
        try sema.queueFullTypeResolution(resolved_ty);
        return block.addUnionInit(resolved_ty, field_index, init_inst);
    } else if (resolved_ty.isAnonStruct()) {
        return sema.fail(block, src, "TODO anon struct init validation", .{});
    }
    unreachable;
}

fn finishStructInit(
    sema: *Sema,
    block: *Block,
    init_src: LazySrcLoc,
    dest_src: LazySrcLoc,
    field_inits: []Air.Inst.Ref,
    struct_ty: Type,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const gpa = sema.gpa;

    var root_msg: ?*Module.ErrorMsg = null;
    errdefer if (root_msg) |msg| msg.destroy(sema.gpa);

    if (struct_ty.isAnonStruct()) {
        const struct_obj = struct_ty.castTag(.anon_struct).?.data;
        for (struct_obj.values, 0..) |default_val, i| {
            if (field_inits[i] != .none) continue;

            if (default_val.tag() == .unreachable_value) {
                const field_name = struct_obj.names[i];
                const template = "missing struct field: {s}";
                const args = .{field_name};
                if (root_msg) |msg| {
                    try sema.errNote(block, init_src, msg, template, args);
                } else {
                    root_msg = try sema.errMsg(block, init_src, template, args);
                }
            } else {
                field_inits[i] = try sema.addConstant(struct_obj.types[i], default_val);
            }
        }
    } else if (struct_ty.isTuple()) {
        var i: u32 = 0;
        const len = struct_ty.structFieldCount();
        while (i < len) : (i += 1) {
            if (field_inits[i] != .none) continue;

            const default_val = struct_ty.structFieldDefaultValue(i);
            if (default_val.tag() == .unreachable_value) {
                const template = "missing tuple field with index {d}";
                if (root_msg) |msg| {
                    try sema.errNote(block, init_src, msg, template, .{i});
                } else {
                    root_msg = try sema.errMsg(block, init_src, template, .{i});
                }
            } else {
                field_inits[i] = try sema.addConstant(struct_ty.structFieldType(i), default_val);
            }
        }
    } else {
        const struct_obj = struct_ty.castTag(.@"struct").?.data;
        for (struct_obj.fields.values(), 0..) |field, i| {
            if (field_inits[i] != .none) continue;

            if (field.default_val.tag() == .unreachable_value) {
                const field_name = struct_obj.fields.keys()[i];
                const template = "missing struct field: {s}";
                const args = .{field_name};
                if (root_msg) |msg| {
                    try sema.errNote(block, init_src, msg, template, args);
                } else {
                    root_msg = try sema.errMsg(block, init_src, template, args);
                }
            } else {
                field_inits[i] = try sema.addConstant(field.ty, field.default_val);
            }
        }
    }

    if (root_msg) |msg| {
        if (struct_ty.castTag(.@"struct")) |struct_obj| {
            const fqn = try struct_obj.data.getFullyQualifiedName(sema.mod);
            defer gpa.free(fqn);
            try sema.mod.errNoteNonLazy(
                struct_obj.data.srcLoc(sema.mod),
                msg,
                "struct '{s}' declared here",
                .{fqn},
            );
        }
        root_msg = null;
        return sema.failWithOwnedErrorMsg(msg);
    }

    const is_comptime = for (field_inits) |field_init| {
        if (!(try sema.isComptimeKnown(field_init))) {
            break false;
        }
    } else true;

    if (is_comptime) {
        const values = try sema.arena.alloc(Value, field_inits.len);
        for (field_inits, 0..) |field_init, i| {
            values[i] = (sema.resolveMaybeUndefVal(field_init) catch unreachable).?;
        }
        const struct_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstantMaybeRef(block, struct_ty, struct_val, is_ref);
    }

    if (is_ref) {
        try sema.resolveStructLayout(struct_ty);
        const target = sema.mod.getTarget();
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = struct_ty,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
        });
        const alloc = try block.addTy(.alloc, alloc_ty);
        for (field_inits, 0..) |field_init, i_usize| {
            const i = @intCast(u32, i_usize);
            const field_src = dest_src;
            const field_ptr = try sema.structFieldPtrByIndex(block, dest_src, alloc, i, field_src, struct_ty, true);
            try sema.storePtr(block, dest_src, field_ptr, field_init);
        }

        return alloc;
    }

    try sema.requireRuntimeBlock(block, dest_src, null);
    try sema.queueFullTypeResolution(struct_ty);
    return block.addAggregateInit(struct_ty, field_inits);
}

fn zirStructInitAnon(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.StructInitAnon, inst_data.payload_index);
    const types = try sema.arena.alloc(Type, extra.data.fields_len);
    const values = try sema.arena.alloc(Value, types.len);
    var fields = std.StringArrayHashMapUnmanaged(u32){};
    defer fields.deinit(sema.gpa);
    try fields.ensureUnusedCapacity(sema.gpa, types.len);

    const opt_runtime_index = rs: {
        var runtime_index: ?usize = null;
        var extra_index = extra.end;
        for (types, 0..) |*field_ty, i| {
            const item = sema.code.extraData(Zir.Inst.StructInitAnon.Item, extra_index);
            extra_index = item.end;

            const name = sema.code.nullTerminatedString(item.data.field_name);
            const gop = fields.getOrPutAssumeCapacity(name);
            if (gop.found_existing) {
                const msg = msg: {
                    const decl = sema.mod.declPtr(block.src_decl);
                    const field_src = Module.initSrc(src.node_offset.x, sema.gpa, decl, i);
                    const msg = try sema.errMsg(block, field_src, "duplicate field", .{});
                    errdefer msg.destroy(sema.gpa);

                    const prev_source = Module.initSrc(src.node_offset.x, sema.gpa, decl, gop.value_ptr.*);
                    try sema.errNote(block, prev_source, msg, "other field here", .{});
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
            gop.value_ptr.* = @intCast(u32, i);

            const init = try sema.resolveInst(item.data.init);
            field_ty.* = sema.typeOf(init);
            if (types[i].zigTypeTag() == .Opaque) {
                const msg = msg: {
                    const decl = sema.mod.declPtr(block.src_decl);
                    const field_src = Module.initSrc(src.node_offset.x, sema.gpa, decl, i);
                    const msg = try sema.errMsg(block, field_src, "opaque types have unknown size and therefore cannot be directly embedded in structs", .{});
                    errdefer msg.destroy(sema.gpa);

                    try sema.addDeclaredHereNote(msg, types[i]);
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
            if (try sema.resolveMaybeUndefVal(init)) |init_val| {
                values[i] = init_val;
            } else {
                values[i] = Value.initTag(.unreachable_value);
                runtime_index = i;
            }
        }
        break :rs runtime_index;
    };

    const tuple_ty = try Type.Tag.anon_struct.create(sema.arena, .{
        .names = try sema.arena.dupe([]const u8, fields.keys()),
        .types = types,
        .values = values,
    });

    const runtime_index = opt_runtime_index orelse {
        const tuple_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstantMaybeRef(block, tuple_ty, tuple_val, is_ref);
    };

    sema.requireRuntimeBlock(block, src, .unneeded) catch |err| switch (err) {
        error.NeededSourceLocation => {
            const decl = sema.mod.declPtr(block.src_decl);
            const field_src = Module.initSrc(src.node_offset.x, sema.gpa, decl, runtime_index);
            try sema.requireRuntimeBlock(block, src, field_src);
            unreachable;
        },
        else => |e| return e,
    };

    if (is_ref) {
        const target = sema.mod.getTarget();
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = tuple_ty,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
        });
        const alloc = try block.addTy(.alloc, alloc_ty);
        var extra_index = extra.end;
        for (types, 0..) |field_ty, i_usize| {
            const i = @intCast(u32, i_usize);
            const item = sema.code.extraData(Zir.Inst.StructInitAnon.Item, extra_index);
            extra_index = item.end;

            const field_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                .mutable = true,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
                .pointee_type = field_ty,
            });
            if (values[i].tag() == .unreachable_value) {
                const init = try sema.resolveInst(item.data.init);
                const field_ptr = try block.addStructFieldPtr(alloc, i, field_ptr_ty);
                _ = try block.addBinOp(.store, field_ptr, init);
            }
        }

        return alloc;
    }

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, types.len);
    var extra_index = extra.end;
    for (types, 0..) |_, i| {
        const item = sema.code.extraData(Zir.Inst.StructInitAnon.Item, extra_index);
        extra_index = item.end;
        element_refs[i] = try sema.resolveInst(item.data.init);
    }

    return block.addAggregateInit(tuple_ty, element_refs);
}

fn zirArrayInit(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const gpa = sema.gpa;
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();

    const extra = sema.code.extraData(Zir.Inst.MultiOp, inst_data.payload_index);
    const args = sema.code.refSlice(extra.end, extra.data.operands_len);
    assert(args.len >= 2); // array_ty + at least one element

    const array_ty = try sema.resolveType(block, src, args[0]);
    const sentinel_val = array_ty.sentinel();

    const resolved_args = try gpa.alloc(Air.Inst.Ref, args.len - 1 + @boolToInt(sentinel_val != null));
    defer gpa.free(resolved_args);
    for (args[1..], 0..) |arg, i| {
        const resolved_arg = try sema.resolveInst(arg);
        const elem_ty = if (array_ty.zigTypeTag() == .Struct)
            array_ty.structFieldType(i)
        else
            array_ty.elemType2();
        resolved_args[i] = sema.coerce(block, elem_ty, resolved_arg, .unneeded) catch |err| switch (err) {
            error.NeededSourceLocation => {
                const decl = sema.mod.declPtr(block.src_decl);
                const elem_src = Module.initSrc(src.node_offset.x, sema.gpa, decl, i);
                _ = try sema.coerce(block, elem_ty, resolved_arg, elem_src);
                unreachable;
            },
            else => return err,
        };
    }

    if (sentinel_val) |some| {
        resolved_args[resolved_args.len - 1] = try sema.addConstant(array_ty.elemType2(), some);
    }

    const opt_runtime_index: ?u32 = for (resolved_args, 0..) |arg, i| {
        const comptime_known = try sema.isComptimeKnown(arg);
        if (!comptime_known) break @intCast(u32, i);
    } else null;

    const runtime_index = opt_runtime_index orelse {
        const elem_vals = try sema.arena.alloc(Value, resolved_args.len);

        for (resolved_args, 0..) |arg, i| {
            // We checked that all args are comptime above.
            elem_vals[i] = (sema.resolveMaybeUndefVal(arg) catch unreachable).?;
        }

        const array_val = try Value.Tag.aggregate.create(sema.arena, elem_vals);
        return sema.addConstantMaybeRef(block, array_ty, array_val, is_ref);
    };

    sema.requireRuntimeBlock(block, src, .unneeded) catch |err| switch (err) {
        error.NeededSourceLocation => {
            const decl = sema.mod.declPtr(block.src_decl);
            const elem_src = Module.initSrc(src.node_offset.x, sema.gpa, decl, runtime_index);
            try sema.requireRuntimeBlock(block, src, elem_src);
            unreachable;
        },
        else => return err,
    };
    try sema.queueFullTypeResolution(array_ty);

    if (is_ref) {
        const target = sema.mod.getTarget();
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = array_ty,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
        });
        const alloc = try block.addTy(.alloc, alloc_ty);

        if (array_ty.isTuple()) {
            for (resolved_args, 0..) |arg, i| {
                const elem_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                    .mutable = true,
                    .@"addrspace" = target_util.defaultAddressSpace(target, .local),
                    .pointee_type = array_ty.structFieldType(i),
                });
                const elem_ptr_ty_ref = try sema.addType(elem_ptr_ty);

                const index = try sema.addIntUnsigned(Type.usize, i);
                const elem_ptr = try block.addPtrElemPtrTypeRef(alloc, index, elem_ptr_ty_ref);
                _ = try block.addBinOp(.store, elem_ptr, arg);
            }
            return alloc;
        }

        const elem_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
            .mutable = true,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
            .pointee_type = array_ty.elemType2(),
        });
        const elem_ptr_ty_ref = try sema.addType(elem_ptr_ty);

        for (resolved_args, 0..) |arg, i| {
            const index = try sema.addIntUnsigned(Type.usize, i);
            const elem_ptr = try block.addPtrElemPtrTypeRef(alloc, index, elem_ptr_ty_ref);
            _ = try block.addBinOp(.store, elem_ptr, arg);
        }
        return alloc;
    }

    return block.addAggregateInit(array_ty, resolved_args);
}

fn zirArrayInitAnon(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    is_ref: bool,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const extra = sema.code.extraData(Zir.Inst.MultiOp, inst_data.payload_index);
    const operands = sema.code.refSlice(extra.end, extra.data.operands_len);

    const types = try sema.arena.alloc(Type, operands.len);
    const values = try sema.arena.alloc(Value, operands.len);

    const opt_runtime_src = rs: {
        var runtime_src: ?LazySrcLoc = null;
        for (operands, 0..) |operand, i| {
            const operand_src = src; // TODO better source location
            const elem = try sema.resolveInst(operand);
            types[i] = sema.typeOf(elem);
            if (types[i].zigTypeTag() == .Opaque) {
                const msg = msg: {
                    const msg = try sema.errMsg(block, operand_src, "opaque types have unknown size and therefore cannot be directly embedded in structs", .{});
                    errdefer msg.destroy(sema.gpa);

                    try sema.addDeclaredHereNote(msg, types[i]);
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
            if (try sema.resolveMaybeUndefVal(elem)) |val| {
                values[i] = val;
            } else {
                values[i] = Value.initTag(.unreachable_value);
                runtime_src = operand_src;
            }
        }
        break :rs runtime_src;
    };

    const tuple_ty = try Type.Tag.tuple.create(sema.arena, .{
        .types = types,
        .values = values,
    });

    const runtime_src = opt_runtime_src orelse {
        const tuple_val = try Value.Tag.aggregate.create(sema.arena, values);
        return sema.addConstantMaybeRef(block, tuple_ty, tuple_val, is_ref);
    };

    try sema.requireRuntimeBlock(block, src, runtime_src);

    if (is_ref) {
        const target = sema.mod.getTarget();
        const alloc_ty = try Type.ptr(sema.arena, sema.mod, .{
            .pointee_type = tuple_ty,
            .@"addrspace" = target_util.defaultAddressSpace(target, .local),
        });
        const alloc = try block.addTy(.alloc, alloc_ty);
        for (operands, 0..) |operand, i_usize| {
            const i = @intCast(u32, i_usize);
            const field_ptr_ty = try Type.ptr(sema.arena, sema.mod, .{
                .mutable = true,
                .@"addrspace" = target_util.defaultAddressSpace(target, .local),
                .pointee_type = types[i],
            });
            if (values[i].tag() == .unreachable_value) {
                const field_ptr = try block.addStructFieldPtr(alloc, i, field_ptr_ty);
                _ = try block.addBinOp(.store, field_ptr, try sema.resolveInst(operand));
            }
        }

        return alloc;
    }

    const element_refs = try sema.arena.alloc(Air.Inst.Ref, operands.len);
    for (operands, 0..) |operand, i| {
        element_refs[i] = try sema.resolveInst(operand);
    }

    return block.addAggregateInit(tuple_ty, element_refs);
}

fn addConstantMaybeRef(
    sema: *Sema,
    block: *Block,
    ty: Type,
    val: Value,
    is_ref: bool,
) !Air.Inst.Ref {
    if (!is_ref) return sema.addConstant(ty, val);

    var anon_decl = try block.startAnonDecl();
    defer anon_decl.deinit();
    const decl = try anon_decl.finish(
        try ty.copy(anon_decl.arena()),
        try val.copy(anon_decl.arena()),
        0, // default alignment
    );
    return sema.analyzeDeclRef(decl);
}

fn zirFieldTypeRef(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.FieldTypeRef, inst_data.payload_index).data;
    const ty_src = inst_data.src();
    const field_src = inst_data.src();
    const aggregate_ty = try sema.resolveType(block, ty_src, extra.container_type);
    const field_name = try sema.resolveConstString(block, field_src, extra.field_name, "field name must be comptime-known");
    return sema.fieldType(block, aggregate_ty, field_name, field_src, ty_src);
}

fn zirFieldType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.FieldType, inst_data.payload_index).data;
    const ty_src = inst_data.src();
    const field_name_src: LazySrcLoc = .{ .node_offset_field_name = inst_data.src_node };
    const aggregate_ty = try sema.resolveType(block, ty_src, extra.container_type);
    if (aggregate_ty.tag() == .var_args_param) return sema.addType(aggregate_ty);
    const field_name = sema.code.nullTerminatedString(extra.name_start);
    return sema.fieldType(block, aggregate_ty, field_name, field_name_src, ty_src);
}

fn fieldType(
    sema: *Sema,
    block: *Block,
    aggregate_ty: Type,
    field_name: []const u8,
    field_src: LazySrcLoc,
    ty_src: LazySrcLoc,
) CompileError!Air.Inst.Ref {
    var cur_ty = aggregate_ty;
    while (true) {
        const resolved_ty = try sema.resolveTypeFields(cur_ty);
        cur_ty = resolved_ty;
        switch (cur_ty.zigTypeTag()) {
            .Struct => {
                if (cur_ty.isAnonStruct()) {
                    const field_index = try sema.anonStructFieldIndex(block, cur_ty, field_name, field_src);
                    return sema.addType(cur_ty.tupleFields().types[field_index]);
                }
                const struct_obj = cur_ty.castTag(.@"struct").?.data;
                const field = struct_obj.fields.get(field_name) orelse
                    return sema.failWithBadStructFieldAccess(block, struct_obj, field_src, field_name);
                return sema.addType(field.ty);
            },
            .Union => {
                const union_obj = cur_ty.cast(Type.Payload.Union).?.data;
                const field = union_obj.fields.get(field_name) orelse
                    return sema.failWithBadUnionFieldAccess(block, union_obj, field_src, field_name);
                return sema.addType(field.ty);
            },
            .Optional => {
                if (cur_ty.castTag(.optional)) |some| {
                    // Struct/array init through optional requires the child type to not be a pointer.
                    // If the child of .optional is a pointer it'll error on the next loop.
                    cur_ty = some.data;
                    continue;
                }
            },
            .ErrorUnion => {
                cur_ty = cur_ty.errorUnionPayload();
                continue;
            },
            else => {},
        }
        return sema.fail(block, ty_src, "expected struct or union; found '{}'", .{
            resolved_ty.fmt(sema.mod),
        });
    }
}

fn zirErrorReturnTrace(sema: *Sema, block: *Block) CompileError!Air.Inst.Ref {
    return sema.getErrorReturnTrace(block);
}

fn getErrorReturnTrace(sema: *Sema, block: *Block) CompileError!Air.Inst.Ref {
    const unresolved_stack_trace_ty = try sema.getBuiltinType("StackTrace");
    const stack_trace_ty = try sema.resolveTypeFields(unresolved_stack_trace_ty);
    const opt_ptr_stack_trace_ty = try Type.Tag.optional_single_mut_pointer.create(sema.arena, stack_trace_ty);

    if (sema.owner_func != null and
        sema.owner_func.?.calls_or_awaits_errorable_fn and
        sema.mod.comp.bin_file.options.error_return_tracing and
        sema.mod.backendSupportsFeature(.error_return_trace))
    {
        return block.addTy(.err_return_trace, opt_ptr_stack_trace_ty);
    }
    return sema.addConstant(opt_ptr_stack_trace_ty, Value.null);
}

fn zirFrame(
    sema: *Sema,
    block: *Block,
    extended: Zir.Inst.Extended.InstData,
) CompileError!Air.Inst.Ref {
    const src = LazySrcLoc.nodeOffset(@bitCast(i32, extended.operand));
    return sema.failWithUseOfAsync(block, src);
}

fn zirAlignOf(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ty = try sema.resolveType(block, operand_src, inst_data.operand);
    if (ty.isNoReturn()) {
        return sema.fail(block, operand_src, "no align available for type '{}'", .{ty.fmt(sema.mod)});
    }
    const target = sema.mod.getTarget();
    const val = try ty.lazyAbiAlignment(target, sema.arena);
    if (val.tag() == .lazy_align) {
        try sema.queueFullTypeResolution(ty);
    }
    return sema.addConstant(Type.comptime_int, val);
}

fn zirBoolToInt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        if (val.isUndef()) return sema.addConstUndef(Type.initTag(.u1));
        const bool_ints = [2]Air.Inst.Ref{ .zero, .one };
        return bool_ints[@boolToInt(val.toBool())];
    }
    return block.addUnOp(.bool_to_int, operand);
}

fn zirErrorName(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };

    if (try sema.resolveDefinedValue(block, operand_src, operand)) |val| {
        const bytes = val.castTag(.@"error").?.data.name;
        return sema.addStrLit(block, bytes);
    }

    // Similar to zirTagName, we have special AIR instruction for the error name in case an optimimzation pass
    // might be able to resolve the result at compile time.
    return block.addUnOp(.error_name, operand);
}

fn zirUnaryMath(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
    comptime eval: fn (Value, Type, Allocator, *Module) Allocator.Error!Value,
) CompileError!Air.Inst.Ref {
    const tracy = trace(@src());
    defer tracy.end();

    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_ty = sema.typeOf(operand);

    switch (operand_ty.zigTypeTag()) {
        .ComptimeFloat, .Float => {},
        .Vector => {
            const scalar_ty = operand_ty.scalarType();
            switch (scalar_ty.zigTypeTag()) {
                .ComptimeFloat, .Float => {},
                else => return sema.fail(block, operand_src, "expected vector of floats or float type, found '{}'", .{scalar_ty.fmt(sema.mod)}),
            }
        },
        else => return sema.fail(block, operand_src, "expected vector of floats or float type, found '{}'", .{operand_ty.fmt(sema.mod)}),
    }

    switch (operand_ty.zigTypeTag()) {
        .Vector => {
            const scalar_ty = operand_ty.scalarType();
            const vec_len = operand_ty.vectorLen();
            const result_ty = try Type.vector(sema.arena, vec_len, scalar_ty);
            if (try sema.resolveMaybeUndefVal(operand)) |val| {
                if (val.isUndef())
                    return sema.addConstUndef(result_ty);

                var elem_buf: Value.ElemValueBuffer = undefined;
                const elems = try sema.arena.alloc(Value, vec_len);
                for (elems, 0..) |*elem, i| {
                    const elem_val = val.elemValueBuffer(sema.mod, i, &elem_buf);
                    elem.* = try eval(elem_val, scalar_ty, sema.arena, sema.mod);
                }
                return sema.addConstant(
                    result_ty,
                    try Value.Tag.aggregate.create(sema.arena, elems),
                );
            }

            try sema.requireRuntimeBlock(block, operand_src, null);
            return block.addUnOp(air_tag, operand);
        },
        .ComptimeFloat, .Float => {
            if (try sema.resolveMaybeUndefVal(operand)) |operand_val| {
                if (operand_val.isUndef())
                    return sema.addConstUndef(operand_ty);
                const result_val = try eval(operand_val, operand_ty, sema.arena, sema.mod);
                return sema.addConstant(operand_ty, result_val);
            }

            try sema.requireRuntimeBlock(block, operand_src, null);
            return block.addUnOp(air_tag, operand);
        },
        else => unreachable,
    }
}

fn zirTagName(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const src = inst_data.src();
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    const mod = sema.mod;

    try sema.resolveTypeLayout(operand_ty);
    const enum_ty = switch (operand_ty.zigTypeTag()) {
        .EnumLiteral => {
            const val = try sema.resolveConstValue(block, .unneeded, operand, "");
            const bytes = val.castTag(.enum_literal).?.data;
            return sema.addStrLit(block, bytes);
        },
        .Enum => operand_ty,
        .Union => operand_ty.unionTagType() orelse {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "union '{}' is untagged", .{
                    operand_ty.fmt(sema.mod),
                });
                errdefer msg.destroy(sema.gpa);
                try sema.addDeclaredHereNote(msg, operand_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        },
        else => return sema.fail(block, operand_src, "expected enum or union; found '{}'", .{
            operand_ty.fmt(mod),
        }),
    };
    if (enum_ty.enumFieldCount() == 0) {
        // TODO I don't think this is the correct way to handle this but
        // it prevents a crash.
        return sema.fail(block, operand_src, "cannot get @tagName of empty enum '{}'", .{
            enum_ty.fmt(mod),
        });
    }
    const enum_decl_index = enum_ty.getOwnerDecl();
    const casted_operand = try sema.coerce(block, enum_ty, operand, operand_src);
    if (try sema.resolveDefinedValue(block, operand_src, casted_operand)) |val| {
        const field_index = enum_ty.enumTagFieldIndex(val, mod) orelse {
            const enum_decl = mod.declPtr(enum_decl_index);
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "no field with value '{}' in enum '{s}'", .{
                    val.fmtValue(enum_ty, sema.mod), enum_decl.name,
                });
                errdefer msg.destroy(sema.gpa);
                try mod.errNoteNonLazy(enum_decl.srcLoc(), msg, "declared here", .{});
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        };
        const field_name = enum_ty.enumFieldName(field_index);
        return sema.addStrLit(block, field_name);
    }
    try sema.requireRuntimeBlock(block, src, operand_src);
    if (block.wantSafety() and sema.mod.backendSupportsFeature(.is_named_enum_value)) {
        const ok = try block.addUnOp(.is_named_enum_value, casted_operand);
        try sema.addSafetyCheck(block, ok, .invalid_enum_value);
    }
    // In case the value is runtime-known, we have an AIR instruction for this instead
    // of trying to lower it in Sema because an optimization pass may result in the operand
    // being comptime-known, which would let us elide the `tag_name` AIR instruction.
    return block.addUnOp(.tag_name, casted_operand);
}

fn zirReify(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const mod = sema.mod;
    const name_strategy = @intToEnum(Zir.Inst.NameStrategy, extended.small);
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const type_info_ty = try sema.getBuiltinType("Type");
    const uncasted_operand = try sema.resolveInst(extra.operand);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const type_info = try sema.coerce(block, type_info_ty, uncasted_operand, operand_src);
    const val = try sema.resolveConstValue(block, operand_src, type_info, "operand to @Type must be comptime-known");
    const union_val = val.cast(Value.Payload.Union).?.data;
    const target = mod.getTarget();
    const tag_index = type_info_ty.unionTagFieldIndex(union_val.tag, mod).?;
    if (union_val.val.anyUndef()) return sema.failWithUseOfUndef(block, src);
    switch (@intToEnum(std.builtin.TypeId, tag_index)) {
        .Type => return Air.Inst.Ref.type_type,
        .Void => return Air.Inst.Ref.void_type,
        .Bool => return Air.Inst.Ref.bool_type,
        .NoReturn => return Air.Inst.Ref.noreturn_type,
        .ComptimeFloat => return Air.Inst.Ref.comptime_float_type,
        .ComptimeInt => return Air.Inst.Ref.comptime_int_type,
        .Undefined => return Air.Inst.Ref.undefined_type,
        .Null => return Air.Inst.Ref.null_type,
        .AnyFrame => return sema.failWithUseOfAsync(block, src),
        .EnumLiteral => return Air.Inst.Ref.enum_literal_type,
        .Int => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            const signedness_val = struct_val[0];
            const bits_val = struct_val[1];

            const signedness = signedness_val.toEnum(std.builtin.Signedness);
            const bits = @intCast(u16, bits_val.toUnsignedInt(target));
            const ty = switch (signedness) {
                .signed => try Type.Tag.int_signed.create(sema.arena, bits),
                .unsigned => try Type.Tag.int_unsigned.create(sema.arena, bits),
            };
            return sema.addType(ty);
        },
        .Vector => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            const len_val = struct_val[0];
            const child_val = struct_val[1];

            const len = len_val.toUnsignedInt(target);
            var buffer: Value.ToTypeBuffer = undefined;
            const child_ty = child_val.toType(&buffer);

            try sema.checkVectorElemType(block, src, child_ty);

            const ty = try Type.vector(sema.arena, len, try child_ty.copy(sema.arena));
            return sema.addType(ty);
        },
        .Float => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // bits: comptime_int,
            const bits_val = struct_val[0];

            const bits = @intCast(u16, bits_val.toUnsignedInt(target));
            const ty = switch (bits) {
                16 => Type.f16,
                32 => Type.f32,
                64 => Type.f64,
                80 => Type.f80,
                128 => Type.f128,
                else => return sema.fail(block, src, "{}-bit float unsupported", .{bits}),
            };
            return sema.addType(ty);
        },
        .Pointer => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            const size_val = struct_val[0];
            const is_const_val = struct_val[1];
            const is_volatile_val = struct_val[2];
            const alignment_val = struct_val[3];
            const address_space_val = struct_val[4];
            const child_val = struct_val[5];
            const is_allowzero_val = struct_val[6];
            const sentinel_val = struct_val[7];

            if (!try sema.intFitsInType(alignment_val, Type.u32, null)) {
                return sema.fail(block, src, "alignment must fit in 'u32'", .{});
            }
            const abi_align = @intCast(u29, (try alignment_val.getUnsignedIntAdvanced(target, sema)).?);

            var buffer: Value.ToTypeBuffer = undefined;
            const unresolved_elem_ty = child_val.toType(&buffer);
            const elem_ty = if (abi_align == 0)
                unresolved_elem_ty
            else t: {
                const elem_ty = try sema.resolveTypeFields(unresolved_elem_ty);
                try sema.resolveTypeLayout(elem_ty);
                break :t elem_ty;
            };

            const ptr_size = size_val.toEnum(std.builtin.Type.Pointer.Size);

            var actual_sentinel: ?Value = null;
            if (!sentinel_val.isNull()) {
                if (ptr_size == .One or ptr_size == .C) {
                    return sema.fail(block, src, "sentinels are only allowed on slices and unknown-length pointers", .{});
                }
                const sentinel_ptr_val = sentinel_val.castTag(.opt_payload).?.data;
                const ptr_ty = try Type.ptr(sema.arena, mod, .{
                    .@"addrspace" = .generic,
                    .pointee_type = try elem_ty.copy(sema.arena),
                });
                actual_sentinel = (try sema.pointerDeref(block, src, sentinel_ptr_val, ptr_ty)).?;
            }

            if (elem_ty.zigTypeTag() == .NoReturn) {
                return sema.fail(block, src, "pointer to noreturn not allowed", .{});
            } else if (elem_ty.zigTypeTag() == .Fn) {
                if (ptr_size != .One) {
                    return sema.fail(block, src, "function pointers must be single pointers", .{});
                }
                const fn_align = elem_ty.fnInfo().alignment;
                if (abi_align != 0 and fn_align != 0 and
                    abi_align != fn_align)
                {
                    return sema.fail(block, src, "function pointer alignment disagrees with function alignment", .{});
                }
            } else if (ptr_size == .Many and elem_ty.zigTypeTag() == .Opaque) {
                return sema.fail(block, src, "unknown-length pointer to opaque not allowed", .{});
            } else if (ptr_size == .C) {
                if (!try sema.validateExternType(elem_ty, .other)) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "C pointers cannot point to non-C-ABI-compatible type '{}'", .{elem_ty.fmt(sema.mod)});
                        errdefer msg.destroy(sema.gpa);

                        const src_decl = sema.mod.declPtr(block.src_decl);
                        try sema.explainWhyTypeIsNotExtern(msg, src.toSrcLoc(src_decl), elem_ty, .other);

                        try sema.addDeclaredHereNote(msg, elem_ty);
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
                if (elem_ty.zigTypeTag() == .Opaque) {
                    return sema.fail(block, src, "C pointers cannot point to opaque types", .{});
                }
            }

            const ty = try Type.ptr(sema.arena, mod, .{
                .size = ptr_size,
                .mutable = !is_const_val.toBool(),
                .@"volatile" = is_volatile_val.toBool(),
                .@"align" = abi_align,
                .@"addrspace" = address_space_val.toEnum(std.builtin.AddressSpace),
                .pointee_type = try elem_ty.copy(sema.arena),
                .@"allowzero" = is_allowzero_val.toBool(),
                .sentinel = actual_sentinel,
            });
            return sema.addType(ty);
        },
        .Array => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // len: comptime_int,
            const len_val = struct_val[0];
            // child: type,
            const child_val = struct_val[1];
            // sentinel: ?*const anyopaque,
            const sentinel_val = struct_val[2];

            const len = len_val.toUnsignedInt(target);
            var buffer: Value.ToTypeBuffer = undefined;
            const child_ty = try child_val.toType(&buffer).copy(sema.arena);
            const sentinel = if (sentinel_val.castTag(.opt_payload)) |p| blk: {
                const ptr_ty = try Type.ptr(sema.arena, mod, .{
                    .@"addrspace" = .generic,
                    .pointee_type = child_ty,
                });
                break :blk (try sema.pointerDeref(block, src, p.data, ptr_ty)).?;
            } else null;

            const ty = try Type.array(sema.arena, len, sentinel, child_ty, sema.mod);
            return sema.addType(ty);
        },
        .Optional => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // child: type,
            const child_val = struct_val[0];

            var buffer: Value.ToTypeBuffer = undefined;
            const child_ty = try child_val.toType(&buffer).copy(sema.arena);

            const ty = try Type.optional(sema.arena, child_ty);
            return sema.addType(ty);
        },
        .ErrorUnion => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // error_set: type,
            const error_set_val = struct_val[0];
            // payload: type,
            const payload_val = struct_val[1];

            var buffer: Value.ToTypeBuffer = undefined;
            const error_set_ty = try error_set_val.toType(&buffer).copy(sema.arena);
            const payload_ty = try payload_val.toType(&buffer).copy(sema.arena);

            if (error_set_ty.zigTypeTag() != .ErrorSet) {
                return sema.fail(block, src, "Type.ErrorUnion.error_set must be an error set type", .{});
            }

            const ty = try Type.Tag.error_union.create(sema.arena, .{
                .error_set = error_set_ty,
                .payload = payload_ty,
            });
            return sema.addType(ty);
        },
        .ErrorSet => {
            const payload_val = union_val.val.optionalValue() orelse
                return sema.addType(Type.initTag(.anyerror));
            const slice_val = payload_val.castTag(.slice).?.data;

            const len = try sema.usizeCast(block, src, slice_val.len.toUnsignedInt(mod.getTarget()));
            var names: Module.ErrorSet.NameMap = .{};
            try names.ensureUnusedCapacity(sema.arena, len);
            var i: usize = 0;
            while (i < len) : (i += 1) {
                var buf: Value.ElemValueBuffer = undefined;
                const elem_val = slice_val.ptr.elemValueBuffer(mod, i, &buf);
                const struct_val = elem_val.castTag(.aggregate).?.data;
                // TODO use reflection instead of magic numbers here
                // error_set: type,
                const name_val = struct_val[0];
                const name_str = try name_val.toAllocatedBytes(Type.initTag(.const_slice_u8), sema.arena, sema.mod);

                const kv = try mod.getErrorValue(name_str);
                const gop = names.getOrPutAssumeCapacity(kv.key);
                if (gop.found_existing) {
                    return sema.fail(block, src, "duplicate error '{s}'", .{name_str});
                }
            }

            // names must be sorted
            Module.ErrorSet.sortNames(&names);
            const ty = try Type.Tag.error_set_merged.create(sema.arena, names);
            return sema.addType(ty);
        },
        .Struct => {
            // TODO use reflection instead of magic numbers here
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // layout: containerlayout,
            const layout_val = struct_val[0];
            // backing_int: ?type,
            const backing_int_val = struct_val[1];
            // fields: []const enumfield,
            const fields_val = struct_val[2];
            // decls: []const declaration,
            const decls_val = struct_val[3];
            // is_tuple: bool,
            const is_tuple_val = struct_val[4];
            assert(struct_val.len == 5);

            const layout = layout_val.toEnum(std.builtin.Type.ContainerLayout);

            // Decls
            if (decls_val.sliceLen(mod) > 0) {
                return sema.fail(block, src, "reified structs must have no decls", .{});
            }

            if (layout != .Packed and !backing_int_val.isNull()) {
                return sema.fail(block, src, "non-packed struct does not support backing integer type", .{});
            }

            return try sema.reifyStruct(block, inst, src, layout, backing_int_val, fields_val, name_strategy, is_tuple_val.toBool());
        },
        .Enum => {
            const struct_val: []const Value = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // tag_type: type,
            const tag_type_val = struct_val[0];
            // fields: []const EnumField,
            const fields_val = struct_val[1];
            // decls: []const Declaration,
            const decls_val = struct_val[2];
            // is_exhaustive: bool,
            const is_exhaustive_val = struct_val[3];

            // Decls
            if (decls_val.sliceLen(mod) > 0) {
                return sema.fail(block, src, "reified enums must have no decls", .{});
            }

            const gpa = sema.gpa;
            var new_decl_arena = std.heap.ArenaAllocator.init(gpa);
            errdefer new_decl_arena.deinit();
            const new_decl_arena_allocator = new_decl_arena.allocator();

            // Define our empty enum decl
            const enum_obj = try new_decl_arena_allocator.create(Module.EnumFull);
            const enum_ty_payload = try new_decl_arena_allocator.create(Type.Payload.EnumFull);
            enum_ty_payload.* = .{
                .base = .{
                    .tag = if (!is_exhaustive_val.toBool())
                        .enum_nonexhaustive
                    else
                        .enum_full,
                },
                .data = enum_obj,
            };
            const enum_ty = Type.initPayload(&enum_ty_payload.base);
            const enum_val = try Value.Tag.ty.create(new_decl_arena_allocator, enum_ty);
            const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
                .ty = Type.type,
                .val = enum_val,
            }, name_strategy, "enum", inst);
            const new_decl = mod.declPtr(new_decl_index);
            new_decl.owns_tv = true;
            errdefer mod.abortAnonDecl(new_decl_index);

            enum_obj.* = .{
                .owner_decl = new_decl_index,
                .tag_ty = Type.null,
                .tag_ty_inferred = false,
                .fields = .{},
                .values = .{},
                .namespace = .{
                    .parent = block.namespace,
                    .ty = enum_ty,
                    .file_scope = block.getFileScope(),
                },
            };

            // Enum tag type
            var buffer: Value.ToTypeBuffer = undefined;
            const int_tag_ty = try tag_type_val.toType(&buffer).copy(new_decl_arena_allocator);

            if (int_tag_ty.zigTypeTag() != .Int) {
                return sema.fail(block, src, "Type.Enum.tag_type must be an integer type", .{});
            }
            enum_obj.tag_ty = int_tag_ty;

            // Fields
            const fields_len = try sema.usizeCast(block, src, fields_val.sliceLen(mod));
            try enum_obj.fields.ensureTotalCapacity(new_decl_arena_allocator, fields_len);
            try enum_obj.values.ensureTotalCapacityContext(new_decl_arena_allocator, fields_len, .{
                .ty = enum_obj.tag_ty,
                .mod = mod,
            });

            var field_i: usize = 0;
            while (field_i < fields_len) : (field_i += 1) {
                const elem_val = try fields_val.elemValue(sema.mod, sema.arena, field_i);
                const field_struct_val: []const Value = elem_val.castTag(.aggregate).?.data;
                // TODO use reflection instead of magic numbers here
                // name: []const u8
                const name_val = field_struct_val[0];
                // value: comptime_int
                const value_val = field_struct_val[1];

                const field_name = try name_val.toAllocatedBytes(
                    Type.initTag(.const_slice_u8),
                    new_decl_arena_allocator,
                    sema.mod,
                );

                if (!try sema.intFitsInType(value_val, enum_obj.tag_ty, null)) {
                    // TODO: better source location
                    return sema.fail(block, src, "field '{s}' with enumeration value '{}' is too large for backing int type '{}'", .{
                        field_name,
                        value_val.fmtValue(Type.comptime_int, mod),
                        enum_obj.tag_ty.fmt(mod),
                    });
                }

                const gop_field = enum_obj.fields.getOrPutAssumeCapacity(field_name);
                if (gop_field.found_existing) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "duplicate enum field '{s}'", .{field_name});
                        errdefer msg.destroy(gpa);
                        try sema.errNote(block, src, msg, "other field here", .{});
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }

                const copied_tag_val = try value_val.copy(new_decl_arena_allocator);
                const gop_val = enum_obj.values.getOrPutAssumeCapacityContext(copied_tag_val, .{
                    .ty = enum_obj.tag_ty,
                    .mod = mod,
                });
                if (gop_val.found_existing) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "enum tag value {} already taken", .{value_val.fmtValue(Type.comptime_int, mod)});
                        errdefer msg.destroy(gpa);
                        try sema.errNote(block, src, msg, "other enum tag value here", .{});
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
            }

            try new_decl.finalizeNewArena(&new_decl_arena);
            return sema.analyzeDeclVal(block, src, new_decl_index);
        },
        .Opaque => {
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // decls: []const Declaration,
            const decls_val = struct_val[0];

            // Decls
            if (decls_val.sliceLen(mod) > 0) {
                return sema.fail(block, src, "reified opaque must have no decls", .{});
            }

            var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
            errdefer new_decl_arena.deinit();
            const new_decl_arena_allocator = new_decl_arena.allocator();

            const opaque_obj = try new_decl_arena_allocator.create(Module.Opaque);
            const opaque_ty_payload = try new_decl_arena_allocator.create(Type.Payload.Opaque);
            opaque_ty_payload.* = .{
                .base = .{ .tag = .@"opaque" },
                .data = opaque_obj,
            };
            const opaque_ty = Type.initPayload(&opaque_ty_payload.base);
            const opaque_val = try Value.Tag.ty.create(new_decl_arena_allocator, opaque_ty);
            const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
                .ty = Type.type,
                .val = opaque_val,
            }, name_strategy, "opaque", inst);
            const new_decl = mod.declPtr(new_decl_index);
            new_decl.owns_tv = true;
            errdefer mod.abortAnonDecl(new_decl_index);

            opaque_obj.* = .{
                .owner_decl = new_decl_index,
                .namespace = .{
                    .parent = block.namespace,
                    .ty = opaque_ty,
                    .file_scope = block.getFileScope(),
                },
            };

            try new_decl.finalizeNewArena(&new_decl_arena);
            return sema.analyzeDeclVal(block, src, new_decl_index);
        },
        .Union => {
            // TODO use reflection instead of magic numbers here
            const struct_val = union_val.val.castTag(.aggregate).?.data;
            // layout: containerlayout,
            const layout_val = struct_val[0];
            // tag_type: ?type,
            const tag_type_val = struct_val[1];
            // fields: []const enumfield,
            const fields_val = struct_val[2];
            // decls: []const declaration,
            const decls_val = struct_val[3];

            // Decls
            if (decls_val.sliceLen(mod) > 0) {
                return sema.fail(block, src, "reified unions must have no decls", .{});
            }
            const layout = layout_val.toEnum(std.builtin.Type.ContainerLayout);

            var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
            errdefer new_decl_arena.deinit();
            const new_decl_arena_allocator = new_decl_arena.allocator();

            const union_obj = try new_decl_arena_allocator.create(Module.Union);
            const type_tag = if (!tag_type_val.isNull())
                Type.Tag.union_tagged
            else if (layout != .Auto)
                Type.Tag.@"union"
            else switch (block.sema.mod.optimizeMode()) {
                .Debug, .ReleaseSafe => Type.Tag.union_safety_tagged,
                .ReleaseFast, .ReleaseSmall => Type.Tag.@"union",
            };
            const union_payload = try new_decl_arena_allocator.create(Type.Payload.Union);
            union_payload.* = .{
                .base = .{ .tag = type_tag },
                .data = union_obj,
            };
            const union_ty = Type.initPayload(&union_payload.base);
            const new_union_val = try Value.Tag.ty.create(new_decl_arena_allocator, union_ty);
            const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
                .ty = Type.type,
                .val = new_union_val,
            }, name_strategy, "union", inst);
            const new_decl = mod.declPtr(new_decl_index);
            new_decl.owns_tv = true;
            errdefer mod.abortAnonDecl(new_decl_index);
            union_obj.* = .{
                .owner_decl = new_decl_index,
                .tag_ty = Type.initTag(.null),
                .fields = .{},
                .zir_index = inst,
                .layout = layout,
                .status = .have_field_types,
                .namespace = .{
                    .parent = block.namespace,
                    .ty = union_ty,
                    .file_scope = block.getFileScope(),
                },
            };

            // Tag type
            var tag_ty_field_names: ?Module.EnumFull.NameMap = null;
            var enum_field_names: ?*Module.EnumNumbered.NameMap = null;
            const fields_len = try sema.usizeCast(block, src, fields_val.sliceLen(mod));
            if (tag_type_val.optionalValue()) |payload_val| {
                var buffer: Value.ToTypeBuffer = undefined;
                union_obj.tag_ty = try payload_val.toType(&buffer).copy(new_decl_arena_allocator);

                if (union_obj.tag_ty.zigTypeTag() != .Enum) {
                    return sema.fail(block, src, "Type.Union.tag_type must be an enum type", .{});
                }
                tag_ty_field_names = try union_obj.tag_ty.enumFields().clone(sema.arena);
            } else {
                union_obj.tag_ty = try sema.generateUnionTagTypeSimple(block, fields_len, null);
                enum_field_names = &union_obj.tag_ty.castTag(.enum_simple).?.data.fields;
            }

            // Fields
            try union_obj.fields.ensureTotalCapacity(new_decl_arena_allocator, fields_len);

            var i: usize = 0;
            while (i < fields_len) : (i += 1) {
                const elem_val = try fields_val.elemValue(sema.mod, sema.arena, i);
                const field_struct_val = elem_val.castTag(.aggregate).?.data;
                // TODO use reflection instead of magic numbers here
                // name: []const u8
                const name_val = field_struct_val[0];
                // type: type,
                const type_val = field_struct_val[1];
                // alignment: comptime_int,
                const alignment_val = field_struct_val[2];

                const field_name = try name_val.toAllocatedBytes(
                    Type.initTag(.const_slice_u8),
                    new_decl_arena_allocator,
                    sema.mod,
                );

                if (enum_field_names) |set| {
                    set.putAssumeCapacity(field_name, {});
                }

                if (tag_ty_field_names) |*names| {
                    const enum_has_field = names.orderedRemove(field_name);
                    if (!enum_has_field) {
                        const msg = msg: {
                            const msg = try sema.errMsg(block, src, "no field named '{s}' in enum '{}'", .{ field_name, union_obj.tag_ty.fmt(sema.mod) });
                            errdefer msg.destroy(sema.gpa);
                            try sema.addDeclaredHereNote(msg, union_obj.tag_ty);
                            break :msg msg;
                        };
                        return sema.failWithOwnedErrorMsg(msg);
                    }
                }

                const gop = union_obj.fields.getOrPutAssumeCapacity(field_name);
                if (gop.found_existing) {
                    // TODO: better source location
                    return sema.fail(block, src, "duplicate union field {s}", .{field_name});
                }

                var buffer: Value.ToTypeBuffer = undefined;
                const field_ty = try type_val.toType(&buffer).copy(new_decl_arena_allocator);
                gop.value_ptr.* = .{
                    .ty = field_ty,
                    .abi_align = @intCast(u32, (try alignment_val.getUnsignedIntAdvanced(target, sema)).?),
                };

                if (field_ty.zigTypeTag() == .Opaque) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "opaque types have unknown size and therefore cannot be directly embedded in unions", .{});
                        errdefer msg.destroy(sema.gpa);

                        try sema.addDeclaredHereNote(msg, field_ty);
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
                if (union_obj.layout == .Extern and !try sema.validateExternType(field_ty, .union_field)) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "extern unions cannot contain fields of type '{}'", .{field_ty.fmt(sema.mod)});
                        errdefer msg.destroy(sema.gpa);

                        const src_decl = sema.mod.declPtr(block.src_decl);
                        try sema.explainWhyTypeIsNotExtern(msg, src.toSrcLoc(src_decl), field_ty, .union_field);

                        try sema.addDeclaredHereNote(msg, field_ty);
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                } else if (union_obj.layout == .Packed and !(validatePackedType(field_ty))) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "packed unions cannot contain fields of type '{}'", .{field_ty.fmt(sema.mod)});
                        errdefer msg.destroy(sema.gpa);

                        const src_decl = sema.mod.declPtr(block.src_decl);
                        try sema.explainWhyTypeIsNotPacked(msg, src.toSrcLoc(src_decl), field_ty);

                        try sema.addDeclaredHereNote(msg, field_ty);
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
            }

            if (tag_ty_field_names) |names| {
                if (names.count() > 0) {
                    const msg = msg: {
                        const msg = try sema.errMsg(block, src, "enum field(s) missing in union", .{});
                        errdefer msg.destroy(sema.gpa);

                        const enum_ty = union_obj.tag_ty;
                        for (names.keys()) |field_name| {
                            const field_index = enum_ty.enumFieldIndex(field_name).?;
                            try sema.addFieldErrNote(enum_ty, field_index, msg, "field '{s}' missing, declared here", .{field_name});
                        }
                        try sema.addDeclaredHereNote(msg, union_obj.tag_ty);
                        break :msg msg;
                    };
                    return sema.failWithOwnedErrorMsg(msg);
                }
            }

            try new_decl.finalizeNewArena(&new_decl_arena);
            return sema.analyzeDeclVal(block, src, new_decl_index);
        },
        .Fn => {
            const struct_val: []const Value = union_val.val.castTag(.aggregate).?.data;
            // TODO use reflection instead of magic numbers here
            // calling_convention: CallingConvention,
            const cc = struct_val[0].toEnum(std.builtin.CallingConvention);
            // alignment: comptime_int,
            const alignment_val = struct_val[1];
            // is_generic: bool,
            const is_generic = struct_val[2].toBool();
            // is_var_args: bool,
            const is_var_args = struct_val[3].toBool();
            // return_type: ?type,
            const return_type_val = struct_val[4];
            // args: []const Param,
            const args_val = struct_val[5];

            if (is_generic) {
                return sema.fail(block, src, "Type.Fn.is_generic must be false for @Type", .{});
            }

            if (is_var_args and cc != .C) {
                return sema.fail(block, src, "varargs functions must have C calling convention", .{});
            }

            const alignment = alignment: {
                if (!try sema.intFitsInType(alignment_val, Type.u32, null)) {
                    return sema.fail(block, src, "alignment must fit in 'u32'", .{});
                }
                const alignment = @intCast(u29, alignment_val.toUnsignedInt(target));
                if (alignment == target_util.defaultFunctionAlignment(target)) {
                    break :alignment 0;
                } else {
                    break :alignment alignment;
                }
            };
            const return_type = return_type_val.optionalValue() orelse
                return sema.fail(block, src, "Type.Fn.return_type must be non-null for @Type", .{});

            var buf: Value.ToTypeBuffer = undefined;

            const args_slice_val = args_val.castTag(.slice).?.data;
            const args_len = try sema.usizeCast(block, src, args_slice_val.len.toUnsignedInt(mod.getTarget()));

            const param_types = try sema.arena.alloc(Type, args_len);
            const comptime_params = try sema.arena.alloc(bool, args_len);

            var noalias_bits: u32 = 0;
            var i: usize = 0;
            while (i < args_len) : (i += 1) {
                var arg_buf: Value.ElemValueBuffer = undefined;
                const arg = args_slice_val.ptr.elemValueBuffer(mod, i, &arg_buf);
                const arg_val = arg.castTag(.aggregate).?.data;
                // TODO use reflection instead of magic numbers here
                // is_generic: bool,
                const arg_is_generic = arg_val[0].toBool();
                // is_noalias: bool,
                const arg_is_noalias = arg_val[1].toBool();
                // type: ?type,
                const param_type_opt_val = arg_val[2];

                if (arg_is_generic) {
                    return sema.fail(block, src, "Type.Fn.Param.is_generic must be false for @Type", .{});
                }

                const param_type_val = param_type_opt_val.optionalValue() orelse
                    return sema.fail(block, src, "Type.Fn.Param.arg_type must be non-null for @Type", .{});
                const param_type = try param_type_val.toType(&buf).copy(sema.arena);

                if (arg_is_noalias) {
                    if (!param_type.isPtrAtRuntime()) {
                        return sema.fail(block, src, "non-pointer parameter declared noalias", .{});
                    }
                    noalias_bits |= @as(u32, 1) << (std.math.cast(u5, i) orelse
                        return sema.fail(block, src, "this compiler implementation only supports 'noalias' on the first 32 parameters", .{}));
                }

                param_types[i] = param_type;
                comptime_params[i] = false;
            }

            var fn_info = Type.Payload.Function.Data{
                .param_types = param_types,
                .comptime_params = comptime_params.ptr,
                .noalias_bits = noalias_bits,
                .return_type = try return_type.toType(&buf).copy(sema.arena),
                .alignment = alignment,
                .cc = cc,
                .is_var_args = is_var_args,
                .is_generic = false,
                .align_is_generic = false,
                .cc_is_generic = false,
                .section_is_generic = false,
                .addrspace_is_generic = false,
            };

            const ty = try Type.Tag.function.create(sema.arena, fn_info);
            return sema.addType(ty);
        },
        .Frame => return sema.failWithUseOfAsync(block, src),
    }
}

fn reifyStruct(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    src: LazySrcLoc,
    layout: std.builtin.Type.ContainerLayout,
    backing_int_val: Value,
    fields_val: Value,
    name_strategy: Zir.Inst.NameStrategy,
    is_tuple: bool,
) CompileError!Air.Inst.Ref {
    var new_decl_arena = std.heap.ArenaAllocator.init(sema.gpa);
    errdefer new_decl_arena.deinit();
    const new_decl_arena_allocator = new_decl_arena.allocator();

    const struct_obj = try new_decl_arena_allocator.create(Module.Struct);
    const struct_ty = try Type.Tag.@"struct".create(new_decl_arena_allocator, struct_obj);
    const new_struct_val = try Value.Tag.ty.create(new_decl_arena_allocator, struct_ty);
    const mod = sema.mod;
    const new_decl_index = try sema.createAnonymousDeclTypeNamed(block, src, .{
        .ty = Type.type,
        .val = new_struct_val,
    }, name_strategy, "struct", inst);
    const new_decl = mod.declPtr(new_decl_index);
    new_decl.owns_tv = true;
    errdefer mod.abortAnonDecl(new_decl_index);
    struct_obj.* = .{
        .owner_decl = new_decl_index,
        .fields = .{},
        .zir_index = inst,
        .layout = layout,
        .status = .have_field_types,
        .known_non_opv = false,
        .is_tuple = is_tuple,
        .namespace = .{
            .parent = block.namespace,
            .ty = struct_ty,
            .file_scope = block.getFileScope(),
        },
    };

    const target = mod.getTarget();

    // Fields
    const fields_len = try sema.usizeCast(block, src, fields_val.sliceLen(mod));
    try struct_obj.fields.ensureTotalCapacity(new_decl_arena_allocator, fields_len);
    var i: usize = 0;
    while (i < fields_len) : (i += 1) {
        const elem_val = try fields_val.elemValue(sema.mod, sema.arena, i);
        const field_struct_val = elem_val.castTag(.aggregate).?.data;
        // TODO use reflection instead of magic numbers here
        // name: []const u8
        const name_val = field_struct_val[0];
        // type: type,
        const type_val = field_struct_val[1];
        // default_value: ?*const anyopaque,
        const default_value_val = field_struct_val[2];
        // is_comptime: bool,
        const is_comptime_val = field_struct_val[3];
        // alignment: comptime_int,
        const alignment_val = field_struct_val[4];

        if (!try sema.intFitsInType(alignment_val, Type.u32, null)) {
            return sema.fail(block, src, "alignment must fit in 'u32'", .{});
        }
        const abi_align = @intCast(u29, (try alignment_val.getUnsignedIntAdvanced(target, sema)).?);

        if (layout == .Packed) {
            if (abi_align != 0) return sema.fail(block, src, "alignment in a packed struct field must be set to 0", .{});
            if (is_comptime_val.toBool()) return sema.fail(block, src, "packed struct fields cannot be marked comptime", .{});
        }
        if (layout == .Extern and is_comptime_val.toBool()) {
            return sema.fail(block, src, "extern struct fields cannot be marked comptime", .{});
        }

        const field_name = try name_val.toAllocatedBytes(
            Type.initTag(.const_slice_u8),
            new_decl_arena_allocator,
            mod,
        );

        if (is_tuple) {
            const field_index = std.fmt.parseUnsigned(u32, field_name, 10) catch {
                return sema.fail(
                    block,
                    src,
                    "tuple cannot have non-numeric field '{s}'",
                    .{field_name},
                );
            };

            if (field_index >= fields_len) {
                return sema.fail(
                    block,
                    src,
                    "tuple field {} exceeds tuple field count",
                    .{field_index},
                );
            }
        }
        const gop = struct_obj.fields.getOrPutAssumeCapacity(field_name);
        if (gop.found_existing) {
            // TODO: better source location
            return sema.fail(block, src, "duplicate struct field {s}", .{field_name});
        }

        const default_val = if (default_value_val.optionalValue()) |opt_val| blk: {
            const payload_val = if (opt_val.pointerDecl()) |opt_decl|
                mod.declPtr(opt_decl).val
            else
                opt_val;
            break :blk try payload_val.copy(new_decl_arena_allocator);
        } else Value.initTag(.unreachable_value);
        if (is_comptime_val.toBool() and default_val.tag() == .unreachable_value) {
            return sema.fail(block, src, "comptime field without default initialization value", .{});
        }

        var buffer: Value.ToTypeBuffer = undefined;
        const field_ty = try type_val.toType(&buffer).copy(new_decl_arena_allocator);
        gop.value_ptr.* = .{
            .ty = field_ty,
            .abi_align = abi_align,
            .default_val = default_val,
            .is_comptime = is_comptime_val.toBool(),
            .offset = undefined,
        };

        if (field_ty.zigTypeTag() == .Opaque) {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "opaque types have unknown size and therefore cannot be directly embedded in structs", .{});
                errdefer msg.destroy(sema.gpa);

                try sema.addDeclaredHereNote(msg, field_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        if (field_ty.zigTypeTag() == .NoReturn) {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "struct fields cannot be 'noreturn'", .{});
                errdefer msg.destroy(sema.gpa);

                try sema.addDeclaredHereNote(msg, field_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
        if (struct_obj.layout == .Extern and !try sema.validateExternType(field_ty, .struct_field)) {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "extern structs cannot contain fields of type '{}'", .{field_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);

                const src_decl = sema.mod.declPtr(block.src_decl);
                try sema.explainWhyTypeIsNotExtern(msg, src.toSrcLoc(src_decl), field_ty, .struct_field);

                try sema.addDeclaredHereNote(msg, field_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        } else if (struct_obj.layout == .Packed and !(validatePackedType(field_ty))) {
            const msg = msg: {
                const msg = try sema.errMsg(block, src, "packed structs cannot contain fields of type '{}'", .{field_ty.fmt(sema.mod)});
                errdefer msg.destroy(sema.gpa);

                const src_decl = sema.mod.declPtr(block.src_decl);
                try sema.explainWhyTypeIsNotPacked(msg, src.toSrcLoc(src_decl), field_ty);

                try sema.addDeclaredHereNote(msg, field_ty);
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
    }

    if (layout == .Packed) {
        struct_obj.status = .layout_wip;

        for (struct_obj.fields.values(), 0..) |field, index| {
            sema.resolveTypeLayout(field.ty) catch |err| switch (err) {
                error.AnalysisFail => {
                    const msg = sema.err orelse return err;
                    try sema.addFieldErrNote(struct_ty, index, msg, "while checking this field", .{});
                    return err;
                },
                else => return err,
            };
        }

        var fields_bit_sum: u64 = 0;
        for (struct_obj.fields.values()) |field| {
            fields_bit_sum += field.ty.bitSize(target);
        }

        if (backing_int_val.optionalValue()) |payload| {
            var buf: Value.ToTypeBuffer = undefined;
            const backing_int_ty = payload.toType(&buf);
            try sema.checkBackingIntType(block, src, backing_int_ty, fields_bit_sum);
            struct_obj.backing_int_ty = try backing_int_ty.copy(new_decl_arena_allocator);
        } else {
            var buf: Type.Payload.Bits = .{
                .base = .{ .tag = .int_unsigned },
                .data = @intCast(u16, fields_bit_sum),
            };
            struct_obj.backing_int_ty = try Type.initPayload(&buf.base).copy(new_decl_arena_allocator);
        }

        struct_obj.status = .have_layout;
    }

    try new_decl.finalizeNewArena(&new_decl_arena);
    return sema.analyzeDeclVal(block, src, new_decl_index);
}

fn zirAddrSpaceCast(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.BinNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const addrspace_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const ptr_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = extra.node };

    const dest_addrspace = try sema.analyzeAddressSpace(block, addrspace_src, extra.lhs, .pointer);
    const ptr = try sema.resolveInst(extra.rhs);
    const ptr_ty = sema.typeOf(ptr);

    try sema.checkPtrOperand(block, ptr_src, ptr_ty);

    var ptr_info = ptr_ty.ptrInfo().data;
    const src_addrspace = ptr_info.@"addrspace";
    if (!target_util.addrSpaceCastIsValid(sema.mod.getTarget(), src_addrspace, dest_addrspace)) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "invalid address space cast", .{});
            errdefer msg.destroy(sema.gpa);
            try sema.errNote(block, src, msg, "address space '{s}' is not compatible with address space '{s}'", .{ @tagName(src_addrspace), @tagName(dest_addrspace) });
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    ptr_info.@"addrspace" = dest_addrspace;
    const dest_ptr_ty = try Type.ptr(sema.arena, sema.mod, ptr_info);
    const dest_ty = if (ptr_ty.zigTypeTag() == .Optional)
        try Type.optional(sema.arena, dest_ptr_ty)
    else
        dest_ptr_ty;

    try sema.requireRuntimeBlock(block, src, ptr_src);
    // TODO: Address space cast safety?

    return block.addInst(.{
        .tag = .addrspace_cast,
        .data = .{ .ty_op = .{
            .ty = try sema.addType(dest_ty),
            .operand = ptr,
        } },
    });
}

fn resolveVaListRef(sema: *Sema, block: *Block, src: LazySrcLoc, zir_ref: Zir.Inst.Ref) CompileError!Air.Inst.Ref {
    const va_list_ty = try sema.getBuiltinType("VaList");
    const va_list_ptr = try Type.ptr(sema.arena, sema.mod, .{
        .pointee_type = va_list_ty,
        .mutable = true,
        .@"addrspace" = .generic,
    });

    const inst = try sema.resolveInst(zir_ref);
    return sema.coerce(block, va_list_ptr, inst, src);
}

fn zirCVaArg(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.BinNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const va_list_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = extra.node };

    const va_list_ref = try sema.resolveVaListRef(block, va_list_src, extra.lhs);
    const arg_ty = try sema.resolveType(block, ty_src, extra.rhs);

    if (!try sema.validateExternType(arg_ty, .param_ty)) {
        const msg = msg: {
            const msg = try sema.errMsg(block, ty_src, "cannot get '{}' from variadic argument", .{arg_ty.fmt(sema.mod)});
            errdefer msg.destroy(sema.gpa);

            const src_decl = sema.mod.declPtr(block.src_decl);
            try sema.explainWhyTypeIsNotExtern(msg, ty_src.toSrcLoc(src_decl), arg_ty, .param_ty);

            try sema.addDeclaredHereNote(msg, arg_ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.c_va_arg, arg_ty, va_list_ref);
}

fn zirCVaCopy(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const va_list_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };

    const va_list_ref = try sema.resolveVaListRef(block, va_list_src, extra.operand);
    const va_list_ty = try sema.getBuiltinType("VaList");

    try sema.requireRuntimeBlock(block, src, null);
    return block.addTyOp(.c_va_copy, va_list_ty, va_list_ref);
}

fn zirCVaEnd(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const va_list_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };

    const va_list_ref = try sema.resolveVaListRef(block, va_list_src, extra.operand);

    try sema.requireRuntimeBlock(block, src, null);
    return block.addUnOp(.c_va_end, va_list_ref);
}

fn zirCVaStart(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const src = LazySrcLoc.nodeOffset(@bitCast(i32, extended.operand));

    const va_list_ty = try sema.getBuiltinType("VaList");
    try sema.requireRuntimeBlock(block, src, null);
    return block.addInst(.{
        .tag = .c_va_start,
        .data = .{ .ty = va_list_ty },
    });
}

fn zirTypeName(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ty = try sema.resolveType(block, ty_src, inst_data.operand);

    var anon_decl = try block.startAnonDecl();
    defer anon_decl.deinit();

    const bytes = try ty.nameAllocArena(anon_decl.arena(), sema.mod);

    const new_decl = try anon_decl.finish(
        try Type.Tag.array_u8_sentinel_0.create(anon_decl.arena(), bytes.len),
        try Value.Tag.bytes.create(anon_decl.arena(), bytes[0 .. bytes.len + 1]),
        0, // default alignment
    );

    return sema.analyzeDeclRef(new_decl);
}

fn zirFrameType(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    return sema.failWithUseOfAsync(block, src);
}

fn zirFrameSize(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    return sema.failWithUseOfAsync(block, src);
}

fn zirFloatToInt(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const dest_ty = try sema.resolveType(block, ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const operand_ty = sema.typeOf(operand);

    _ = try sema.checkIntType(block, ty_src, dest_ty);
    try sema.checkFloatType(block, operand_src, operand_ty);

    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        const result_val = try sema.floatToInt(block, operand_src, val, operand_ty, dest_ty);
        return sema.addConstant(dest_ty, result_val);
    } else if (dest_ty.zigTypeTag() == .ComptimeInt) {
        return sema.failWithNeededComptime(block, operand_src, "value being casted to 'comptime_int' must be comptime-known");
    }

    try sema.requireRuntimeBlock(block, inst_data.src(), operand_src);
    if (dest_ty.intInfo(sema.mod.getTarget()).bits == 0) {
        if (block.wantSafety()) {
            const ok = try block.addBinOp(if (block.float_mode == .Optimized) .cmp_eq_optimized else .cmp_eq, operand, try sema.addConstant(operand_ty, Value.zero));
            try sema.addSafetyCheck(block, ok, .integer_part_out_of_bounds);
        }
        return sema.addConstant(dest_ty, Value.zero);
    }
    const result = try block.addTyOp(if (block.float_mode == .Optimized) .float_to_int_optimized else .float_to_int, dest_ty, operand);
    if (block.wantSafety()) {
        const back = try block.addTyOp(.int_to_float, operand_ty, result);
        const diff = try block.addBinOp(.sub, operand, back);
        const ok_pos = try block.addBinOp(if (block.float_mode == .Optimized) .cmp_lt_optimized else .cmp_lt, diff, try sema.addConstant(operand_ty, Value.one));
        const ok_neg = try block.addBinOp(if (block.float_mode == .Optimized) .cmp_gt_optimized else .cmp_gt, diff, try sema.addConstant(operand_ty, Value.negative_one));
        const ok = try block.addBinOp(.bool_and, ok_pos, ok_neg);
        try sema.addSafetyCheck(block, ok, .integer_part_out_of_bounds);
    }
    return result;
}

fn zirIntToFloat(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const dest_ty = try sema.resolveType(block, ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const operand_ty = sema.typeOf(operand);

    try sema.checkFloatType(block, ty_src, dest_ty);
    _ = try sema.checkIntType(block, operand_src, operand_ty);

    if (try sema.resolveMaybeUndefVal(operand)) |val| {
        const result_val = try val.intToFloatAdvanced(sema.arena, operand_ty, dest_ty, sema.mod, sema);
        return sema.addConstant(dest_ty, result_val);
    } else if (dest_ty.zigTypeTag() == .ComptimeFloat) {
        return sema.failWithNeededComptime(block, operand_src, "value being casted to 'comptime_float' must be comptime-known");
    }

    try sema.requireRuntimeBlock(block, inst_data.src(), operand_src);
    return block.addTyOp(.int_to_float, dest_ty, operand);
}

fn zirIntToPtr(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();

    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;

    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const operand_res = try sema.resolveInst(extra.rhs);
    const operand_coerced = try sema.coerce(block, Type.usize, operand_res, operand_src);

    const type_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ptr_ty = try sema.resolveType(block, src, extra.lhs);
    try sema.checkPtrType(block, type_src, ptr_ty);
    const elem_ty = ptr_ty.elemType2();
    const target = sema.mod.getTarget();
    const ptr_align = try ptr_ty.ptrAlignmentAdvanced(target, sema);

    if (try sema.resolveDefinedValue(block, operand_src, operand_coerced)) |val| {
        const addr = val.toUnsignedInt(target);
        if (!ptr_ty.isAllowzeroPtr() and addr == 0)
            return sema.fail(block, operand_src, "pointer type '{}' does not allow address zero", .{ptr_ty.fmt(sema.mod)});
        if (addr != 0 and ptr_align != 0 and addr % ptr_align != 0)
            return sema.fail(block, operand_src, "pointer type '{}' requires aligned address", .{ptr_ty.fmt(sema.mod)});

        const val_payload = try sema.arena.create(Value.Payload.U64);
        val_payload.* = .{
            .base = .{ .tag = .int_u64 },
            .data = addr,
        };
        return sema.addConstant(ptr_ty, Value.initPayload(&val_payload.base));
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    if (block.wantSafety() and try sema.typeHasRuntimeBits(elem_ty)) {
        if (!ptr_ty.isAllowzeroPtr()) {
            const is_non_zero = try block.addBinOp(.cmp_neq, operand_coerced, .zero_usize);
            try sema.addSafetyCheck(block, is_non_zero, .cast_to_null);
        }

        if (ptr_align > 1) {
            const val_payload = try sema.arena.create(Value.Payload.U64);
            val_payload.* = .{
                .base = .{ .tag = .int_u64 },
                .data = ptr_align - 1,
            };
            const align_minus_1 = try sema.addConstant(
                Type.usize,
                Value.initPayload(&val_payload.base),
            );
            const remainder = try block.addBinOp(.bit_and, operand_coerced, align_minus_1);
            const is_aligned = try block.addBinOp(.cmp_eq, remainder, .zero_usize);
            try sema.addSafetyCheck(block, is_aligned, .incorrect_alignment);
        }
    }
    return block.addBitCast(ptr_ty, operand_coerced);
}

fn zirErrSetCast(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.BinNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = extra.node };
    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const operand_ty = sema.typeOf(operand);
    try sema.checkErrorSetType(block, dest_ty_src, dest_ty);
    try sema.checkErrorSetType(block, operand_src, operand_ty);

    // operand must be defined since it can be an invalid error value
    const maybe_operand_val = try sema.resolveDefinedValue(block, operand_src, operand);

    if (disjoint: {
        // Try avoiding resolving inferred error sets if we can
        if (!dest_ty.isAnyError() and dest_ty.errorSetNames().len == 0) break :disjoint true;
        if (!operand_ty.isAnyError() and operand_ty.errorSetNames().len == 0) break :disjoint true;
        if (dest_ty.isAnyError()) break :disjoint false;
        if (operand_ty.isAnyError()) break :disjoint false;
        for (dest_ty.errorSetNames()) |dest_err_name|
            if (operand_ty.errorSetHasField(dest_err_name))
                break :disjoint false;

        if (dest_ty.tag() != .error_set_inferred and operand_ty.tag() != .error_set_inferred)
            break :disjoint true;

        try sema.resolveInferredErrorSetTy(block, dest_ty_src, dest_ty);
        try sema.resolveInferredErrorSetTy(block, operand_src, operand_ty);
        for (dest_ty.errorSetNames()) |dest_err_name|
            if (operand_ty.errorSetHasField(dest_err_name))
                break :disjoint false;

        break :disjoint true;
    }) {
        const msg = msg: {
            const msg = try sema.errMsg(
                block,
                src,
                "error sets '{}' and '{}' have no common errors",
                .{ operand_ty.fmt(sema.mod), dest_ty.fmt(sema.mod) },
            );
            errdefer msg.destroy(sema.gpa);
            try sema.addDeclaredHereNote(msg, operand_ty);
            try sema.addDeclaredHereNote(msg, dest_ty);
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    if (maybe_operand_val) |val| {
        if (!dest_ty.isAnyError()) {
            const error_name = val.castTag(.@"error").?.data.name;
            if (!dest_ty.errorSetHasField(error_name)) {
                const msg = msg: {
                    const msg = try sema.errMsg(
                        block,
                        src,
                        "'error.{s}' not a member of error set '{}'",
                        .{ error_name, dest_ty.fmt(sema.mod) },
                    );
                    errdefer msg.destroy(sema.gpa);
                    try sema.addDeclaredHereNote(msg, dest_ty);
                    break :msg msg;
                };
                return sema.failWithOwnedErrorMsg(msg);
            }
        }

        return sema.addConstant(dest_ty, val);
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    if (block.wantSafety() and !dest_ty.isAnyError() and sema.mod.backendSupportsFeature(.error_set_has_value)) {
        const err_int_inst = try block.addBitCast(Type.err_int, operand);
        const ok = try block.addTyOp(.error_set_has_value, dest_ty, err_int_inst);
        try sema.addSafetyCheck(block, ok, .invalid_error_code);
    }
    return block.addBitCast(dest_ty, operand);
}

fn zirPtrCast(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const dest_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const operand_ty = sema.typeOf(operand);
    const target = sema.mod.getTarget();

    try sema.checkPtrType(block, dest_ty_src, dest_ty);
    try sema.checkPtrOperand(block, operand_src, operand_ty);

    const operand_info = operand_ty.ptrInfo().data;
    const dest_info = dest_ty.ptrInfo().data;
    if (!operand_info.mutable and dest_info.mutable) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "cast discards const qualifier", .{});
            errdefer msg.destroy(sema.gpa);

            try sema.errNote(block, src, msg, "consider using '@constCast'", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    if (operand_info.@"volatile" and !dest_info.@"volatile") {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "cast discards volatile qualifier", .{});
            errdefer msg.destroy(sema.gpa);

            try sema.errNote(block, src, msg, "consider using '@volatileCast'", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }
    if (operand_info.@"addrspace" != dest_info.@"addrspace") {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "cast changes pointer address space", .{});
            errdefer msg.destroy(sema.gpa);

            try sema.errNote(block, src, msg, "consider using '@addrSpaceCast'", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    const dest_is_slice = dest_ty.isSlice();
    const operand_is_slice = operand_ty.isSlice();
    if (dest_is_slice and !operand_is_slice) {
        return sema.fail(block, dest_ty_src, "illegal pointer cast to slice", .{});
    }
    const ptr = if (operand_is_slice and !dest_is_slice)
        try sema.analyzeSlicePtr(block, operand_src, operand, operand_ty)
    else
        operand;

    const dest_elem_ty = dest_ty.elemType2();
    try sema.resolveTypeLayout(dest_elem_ty);
    const dest_align = dest_ty.ptrAlignment(target);

    const operand_elem_ty = operand_ty.elemType2();
    try sema.resolveTypeLayout(operand_elem_ty);
    const operand_align = operand_ty.ptrAlignment(target);

    // If the destination is less aligned than the source, preserve the source alignment
    const aligned_dest_ty = if (operand_align <= dest_align) dest_ty else blk: {
        // Unwrap the pointer (or pointer-like optional) type, set alignment, and re-wrap into result
        if (dest_ty.zigTypeTag() == .Optional) {
            var buf: Type.Payload.ElemType = undefined;
            var dest_ptr_info = dest_ty.optionalChild(&buf).ptrInfo().data;
            dest_ptr_info.@"align" = operand_align;
            break :blk try Type.optional(sema.arena, try Type.ptr(sema.arena, sema.mod, dest_ptr_info));
        } else {
            var dest_ptr_info = dest_ty.ptrInfo().data;
            dest_ptr_info.@"align" = operand_align;
            break :blk try Type.ptr(sema.arena, sema.mod, dest_ptr_info);
        }
    };

    if (dest_is_slice) {
        const operand_elem_size = operand_elem_ty.abiSize(target);
        const dest_elem_size = dest_elem_ty.abiSize(target);
        if (operand_elem_size != dest_elem_size) {
            return sema.fail(block, dest_ty_src, "TODO: implement @ptrCast between slices changing the length", .{});
        }
    }

    if (dest_align > operand_align) {
        const msg = msg: {
            const msg = try sema.errMsg(block, src, "cast increases pointer alignment", .{});
            errdefer msg.destroy(sema.gpa);

            try sema.errNote(block, operand_src, msg, "'{}' has alignment '{d}'", .{
                operand_ty.fmt(sema.mod), operand_align,
            });
            try sema.errNote(block, dest_ty_src, msg, "'{}' has alignment '{d}'", .{
                dest_ty.fmt(sema.mod), dest_align,
            });

            try sema.errNote(block, src, msg, "consider using '@alignCast'", .{});
            break :msg msg;
        };
        return sema.failWithOwnedErrorMsg(msg);
    }

    if (try sema.resolveMaybeUndefVal(ptr)) |operand_val| {
        if (!dest_ty.ptrAllowsZero() and operand_val.isUndef()) {
            return sema.failWithUseOfUndef(block, operand_src);
        }
        if (!dest_ty.ptrAllowsZero() and operand_val.isNull()) {
            return sema.fail(block, operand_src, "null pointer casted to type {}", .{dest_ty.fmt(sema.mod)});
        }
        if (dest_ty.zigTypeTag() == .Optional and sema.typeOf(ptr).zigTypeTag() != .Optional) {
            return sema.addConstant(dest_ty, try Value.Tag.opt_payload.create(sema.arena, operand_val));
        }
        return sema.addConstant(aligned_dest_ty, operand_val);
    }

    try sema.requireRuntimeBlock(block, src, null);
    if (block.wantSafety() and operand_ty.ptrAllowsZero() and !dest_ty.ptrAllowsZero() and
        try sema.typeHasRuntimeBits(dest_ty.elemType2()))
    {
        const ptr_int = try block.addUnOp(.ptrtoint, ptr);
        const is_non_zero = try block.addBinOp(.cmp_neq, ptr_int, .zero_usize);
        const ok = if (operand_is_slice) ok: {
            const len = try sema.analyzeSliceLen(block, operand_src, operand);
            const len_zero = try block.addBinOp(.cmp_eq, len, .zero_usize);
            break :ok try block.addBinOp(.bit_or, len_zero, is_non_zero);
        } else is_non_zero;
        try sema.addSafetyCheck(block, ok, .cast_to_null);
    }

    return block.addBitCast(aligned_dest_ty, ptr);
}

fn zirConstCast(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const operand = try sema.resolveInst(extra.operand);
    const operand_ty = sema.typeOf(operand);
    try sema.checkPtrOperand(block, operand_src, operand_ty);

    var ptr_info = operand_ty.ptrInfo().data;
    ptr_info.mutable = true;
    const dest_ty = try Type.ptr(sema.arena, sema.mod, ptr_info);

    if (try sema.resolveMaybeUndefVal(operand)) |operand_val| {
        return sema.addConstant(dest_ty, operand_val);
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addBitCast(dest_ty, operand);
}

fn zirVolatileCast(sema: *Sema, block: *Block, extended: Zir.Inst.Extended.InstData) CompileError!Air.Inst.Ref {
    const extra = sema.code.extraData(Zir.Inst.UnNode, extended.operand).data;
    const src = LazySrcLoc.nodeOffset(extra.node);
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = extra.node };
    const operand = try sema.resolveInst(extra.operand);
    const operand_ty = sema.typeOf(operand);
    try sema.checkPtrOperand(block, operand_src, operand_ty);

    var ptr_info = operand_ty.ptrInfo().data;
    ptr_info.@"volatile" = false;
    const dest_ty = try Type.ptr(sema.arena, sema.mod, ptr_info);

    if (try sema.resolveMaybeUndefVal(operand)) |operand_val| {
        return sema.addConstant(dest_ty, operand_val);
    }

    try sema.requireRuntimeBlock(block, src, null);
    return block.addBitCast(dest_ty, operand);
}

fn zirTruncate(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const src = inst_data.src();
    const dest_ty_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const dest_scalar_ty = try sema.resolveType(block, dest_ty_src, extra.lhs);
    const operand = try sema.resolveInst(extra.rhs);
    const dest_is_comptime_int = try sema.checkIntType(block, dest_ty_src, dest_scalar_ty);
    const operand_ty = sema.typeOf(operand);
    const operand_scalar_ty = try sema.checkIntOrVectorAllowComptime(block, operand_ty, operand_src);
    const is_vector = operand_ty.zigTypeTag() == .Vector;
    const dest_ty = if (is_vector)
        try Type.vector(sema.arena, operand_ty.vectorLen(), dest_scalar_ty)
    else
        dest_scalar_ty;

    if (dest_is_comptime_int) {
        return sema.coerce(block, dest_ty, operand, operand_src);
    }

    const target = sema.mod.getTarget();
    const dest_info = dest_scalar_ty.intInfo(target);

    if (try sema.typeHasOnePossibleValue(dest_ty)) |val| {
        return sema.addConstant(dest_ty, val);
    }

    if (operand_scalar_ty.zigTypeTag() != .ComptimeInt) {
        const operand_info = operand_ty.intInfo(target);
        if (try sema.typeHasOnePossibleValue(operand_ty)) |val| {
            return sema.addConstant(operand_ty, val);
        }

        if (operand_info.signedness != dest_info.signedness) {
            return sema.fail(block, operand_src, "expected {s} integer type, found '{}'", .{
                @tagName(dest_info.signedness), operand_ty.fmt(sema.mod),
            });
        }
        if (operand_info.bits < dest_info.bits) {
            const msg = msg: {
                const msg = try sema.errMsg(
                    block,
                    src,
                    "destination type '{}' has more bits than source type '{}'",
                    .{ dest_ty.fmt(sema.mod), operand_ty.fmt(sema.mod) },
                );
                errdefer msg.destroy(sema.gpa);
                try sema.errNote(block, dest_ty_src, msg, "destination type has {d} bits", .{
                    dest_info.bits,
                });
                try sema.errNote(block, operand_src, msg, "operand type has {d} bits", .{
                    operand_info.bits,
                });
                break :msg msg;
            };
            return sema.failWithOwnedErrorMsg(msg);
        }
    }

    if (try sema.resolveMaybeUndefValIntable(operand)) |val| {
        if (val.isUndef()) return sema.addConstUndef(dest_ty);
        if (!is_vector) {
            return sema.addConstant(
                dest_ty,
                try val.intTrunc(operand_ty, sema.arena, dest_info.signedness, dest_info.bits, sema.mod),
            );
        }
        var elem_buf: Value.ElemValueBuffer = undefined;
        const elems = try sema.arena.alloc(Value, operand_ty.vectorLen());
        for (elems, 0..) |*elem, i| {
            const elem_val = val.elemValueBuffer(sema.mod, i, &elem_buf);
            elem.* = try elem_val.intTrunc(operand_scalar_ty, sema.arena, dest_info.signedness, dest_info.bits, sema.mod);
        }
        return sema.addConstant(
            dest_ty,
            try Value.Tag.aggregate.create(sema.arena, elems),
        );
    }

    try sema.requireRuntimeBlock(block, src, operand_src);
    return block.addTyOp(.trunc, dest_ty, operand);
}

fn zirAlignCast(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].pl_node;
    const extra = sema.code.extraData(Zir.Inst.Bin, inst_data.payload_index).data;
    const align_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const ptr_src: LazySrcLoc = .{ .node_offset_builtin_call_arg1 = inst_data.src_node };
    const dest_align = try sema.resolveAlign(block, align_src, extra.lhs);
    const ptr = try sema.resolveInst(extra.rhs);
    const ptr_ty = sema.typeOf(ptr);

    try sema.checkPtrOperand(block, ptr_src, ptr_ty);

    var ptr_info = ptr_ty.ptrInfo().data;
    ptr_info.@"align" = dest_align;
    var dest_ty = try Type.ptr(sema.arena, sema.mod, ptr_info);
    if (ptr_ty.zigTypeTag() == .Optional) {
        dest_ty = try Type.Tag.optional.create(sema.arena, dest_ty);
    }

    if (try sema.resolveDefinedValue(block, ptr_src, ptr)) |val| {
        if (try val.getUnsignedIntAdvanced(sema.mod.getTarget(), null)) |addr| {
            if (addr % dest_align != 0) {
                return sema.fail(block, ptr_src, "pointer address 0x{X} is not aligned to {d} bytes", .{ addr, dest_align });
            }
        }
        return sema.addConstant(dest_ty, val);
    }

    try sema.requireRuntimeBlock(block, inst_data.src(), ptr_src);
    if (block.wantSafety() and dest_align > 1 and
        try sema.typeHasRuntimeBits(ptr_info.pointee_type))
    {
        const val_payload = try sema.arena.create(Value.Payload.U64);
        val_payload.* = .{
            .base = .{ .tag = .int_u64 },
            .data = dest_align - 1,
        };
        const align_minus_1 = try sema.addConstant(
            Type.usize,
            Value.initPayload(&val_payload.base),
        );
        const actual_ptr = if (ptr_ty.isSlice())
            try sema.analyzeSlicePtr(block, ptr_src, ptr, ptr_ty)
        else
            ptr;
        const ptr_int = try block.addUnOp(.ptrtoint, actual_ptr);
        const remainder = try block.addBinOp(.bit_and, ptr_int, align_minus_1);
        const is_aligned = try block.addBinOp(.cmp_eq, remainder, .zero_usize);
        const ok = if (ptr_ty.isSlice()) ok: {
            const len = try sema.analyzeSliceLen(block, ptr_src, ptr);
            const len_zero = try block.addBinOp(.cmp_eq, len, .zero_usize);
            break :ok try block.addBinOp(.bit_or, len_zero, is_aligned);
        } else is_aligned;
        try sema.addSafetyCheck(block, ok, .incorrect_alignment);
    }
    return sema.bitCast(block, dest_ty, ptr, ptr_src, null);
}

fn zirBitCount(
    sema: *Sema,
    block: *Block,
    inst: Zir.Inst.Index,
    air_tag: Air.Inst.Tag,
    comptime comptimeOp: fn (val: Value, ty: Type, target: std.Target) u64,
) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    _ = try sema.checkIntOrVector(block, operand, operand_src);
    const target = sema.mod.getTarget();
    const bits = operand_ty.intInfo(target).bits;

    if (try sema.typeHasOnePossibleValue(operand_ty)) |val| {
        return sema.addConstant(operand_ty, val);
    }

    const result_scalar_ty = try Type.smallestUnsignedInt(sema.arena, bits);
    switch (operand_ty.zigTypeTag()) {
        .Vector => {
            const vec_len = operand_ty.vectorLen();
            const result_ty = try Type.vector(sema.arena, vec_len, result_scalar_ty);
            if (try sema.resolveMaybeUndefVal(operand)) |val| {
                if (val.isUndef()) return sema.addConstUndef(result_ty);

                var elem_buf: Value.ElemValueBuffer = undefined;
                const elems = try sema.arena.alloc(Value, vec_len);
                const scalar_ty = operand_ty.scalarType();
                for (elems, 0..) |*elem, i| {
                    const elem_val = val.elemValueBuffer(sema.mod, i, &elem_buf);
                    const count = comptimeOp(elem_val, scalar_ty, target);
                    elem.* = try Value.Tag.int_u64.create(sema.arena, count);
                }
                return sema.addConstant(
                    result_ty,
                    try Value.Tag.aggregate.create(sema.arena, elems),
                );
            } else {
                try sema.requireRuntimeBlock(block, src, operand_src);
                return block.addTyOp(air_tag, result_ty, operand);
            }
        },
        .Int => {
            if (try sema.resolveMaybeUndefVal(operand)) |val| {
                if (val.isUndef()) return sema.addConstUndef(result_scalar_ty);
                try sema.resolveLazyValue(val);
                return sema.addIntUnsigned(result_scalar_ty, comptimeOp(val, operand_ty, target));
            } else {
                try sema.requireRuntimeBlock(block, src, operand_src);
                return block.addTyOp(air_tag, result_scalar_ty, operand);
            }
        },
        else => unreachable,
    }
}

fn zirByteSwap(sema: *Sema, block: *Block, inst: Zir.Inst.Index) CompileError!Air.Inst.Ref {
    const inst_data = sema.code.instructions.items(.data)[inst].un_node;
    const src = inst_data.src();
    const operand_src: LazySrcLoc = .{ .node_offset_builtin_call_arg0 = inst_data.src_node };
    const operand = try sema.resolveInst(inst_data.operand);
    const operand_ty = sema.typeOf(operand);
    const scalar_ty = try sema.checkIntOrVector(block, operand, operand_src);
    const target = sema.mod.getTarget();
    const bits = scalar_ty.intInfo(target).bits;
    if (bits % 8 != 0) {
        return sema.fail(
            block,
            operand_src,
            "@byteSwap requires the number of bits to be evenly divisible by 8, but {} has {} bits",
            .{ scalar_ty.fmt(sema.mod), bits },
        );
    }

    if (try sema.typeHasOnePossibleValue(operand_ty)) |val| {
        return sema.addConstant(operand_ty, val);
    }
