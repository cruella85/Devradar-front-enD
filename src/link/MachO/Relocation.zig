const Relocation = @This();

const std = @import("std");
const aarch64 = @import("../../arch/aarch64/bits.zig");
const assert = std.debug.assert;
const log = std.log.scoped(.link);
const macho = std.macho;
const math = std.math;
const mem = std.mem;
const meta = std.meta;

const Atom = @import("Atom.zig");
const MachO = @import("../MachO.zig");
const SymbolWithLoc = MachO.SymbolWithLoc;

type: u4,
target: SymbolWithLoc,
offset: u32,
addend: i64,
pcrel: bool,
length: u2,
dirty: bool = true,

pub fn fmtType(self: Relocation, target: std.Target) []const u8 {
    switch (target.cpu.arch) {
        .aarch64 => return @tagName(@intToEnum(macho.reloc_type_arm64, self.type)),
        .x86_64 => return @tagName(@intToEnum(macho.reloc_type_x86_64, self.type)),
        else => unreachable,
    }
}

pub fn getTargetAtomIndex(self: Relocation, macho_file: *MachO) ?Atom.Index {
    switch (macho_file.base.options.target.cpu.arch) {
        .aarch64 => switch (@intToEnum(macho.reloc_type_arm64, self.type)) {
            .ARM64_RELOC_GOT_LOAD_PAGE21,
            .ARM64_RELOC_GOT_LOAD_PAGEOFF12,
            .ARM64_RELOC_POINTER_TO_GOT,
            => return macho_file.getGotAtomIndexForSymbol(self.target),
            else => {},
        },
        .x86_64 => switch (@intToEnum(macho.reloc_type_x86_64, self.type)) {
            .X86_64_RELOC_GOT,
            .X86_64_RELOC_GOT_LOAD,
            => return macho_file.getGotAtomIndexForSymbol(self.target),
            else => {},
        },
        else => unreachable,
    }
    if (macho_file.getStubsAtomIndexForSymbol(self.target)) |stubs_atom| return stubs_atom;
    return macho_file.getAtomIndexForSymbol(self.target);
}

pub fn resolve(self: Relocation, macho_file: *MachO, atom_index: Atom.Index, base_offset: u64) !void {
    const arch = macho_file.base.options.target.cpu.arch;
    const atom = macho_file.getAtom(atom_index);
    const source_sym = atom.getSymbol(macho_file);
    const source_addr = source_sym.n_value + self.offset;

    const target_atom_index = self.getTargetAtomIndex(macho_file) orelse return;
    const target_atom = macho_file.getAtom(target_atom_index);
    const target_addr = @intCast(i64, target_atom.getSymbol(macho_file).n_value) + self.addend;

    log.debug("  ({x}: [() => 0x{x} ({s})) ({s})", .{
        source_addr,
        target_addr,
        macho_file.getSymbolName(self.target),
        self.fmtType(macho_file.base.options.target),
    });

    switch (arch) {
        .aarch64 => return self.resolveAarch64(macho_file, source_addr, target_addr, base_offset),
        .x86_64 => return self.resolveX8664(macho_file, source_addr, target_addr, base_offset),
        else => unreachable,
    }
}

fn resolveAarch64(
    self: Relocation,
    macho_file: *MachO,
    source_addr: u64,
    target_addr: i64,
    base_offset: u64,
) !void {
    const rel_type = @intToEnum(macho.reloc_type_arm64, self.type);
    if (rel_type == .ARM64_RELOC_UNSIGNED) {
        var buffer: [@sizeOf(u64)]u8 = undefined;
        const code = blk: {
            switch (self.length) {
                2 => {
                    mem.writeIntLittle(u32, buffer[0..4], @truncate(u32, @bitCast(u64, target_addr)));
                    break :blk buffer[0..4];
                },
                3 => {
                    mem.writeIntLittle(u64, &buffer, @bitCast(u64, target_addr));
                    break :blk &buffer;
                },
                else => unreachable,
            }
        };
        return macho_file.base.file.?.pwriteAll(code, base_offset + self.offset);
    }

    var buffer: [@sizeOf(u32)]u8 = undefined;
    const amt = try macho_file.base.file.?.preadAll(&buffer, base_offset + self.offset);
    if (amt != buffer.len) return error.InputOutput;

    switch (rel_type) {
        .ARM64_RELOC_BRANCH26 => {
            const displacement = math.cast(
                i28,
                @intCast(i64, target_addr) - @intCast(i64, source_addr),
            ) orelse unreachable; // TODO codegen should never allow for jump larger than i28 displacement
            var inst = aarch64.Instruction{
                .unconditional_branch_immediate = mem.bytesToValue(meta.TagPayload(
                    aarch64.Instruction,
                    aarch64.Instruction.unconditional_branch_immediate,
                ), &buffer),
            };
            inst.unconditional_branch_immediate.imm26 = @truncate(u26, @bitCast(u28, displacement >> 2));
            mem.writeIntLittle(u32, &buffer, inst.toU32());
        },
        .ARM64_RELOC_PAGE21,
        .ARM64_RELOC_GOT_LOAD_PAGE21,
        .ARM64_RELOC_TLVP_LOAD_PAGE21,
        => {
            const source_page = @intCast(i32, source_addr >> 12);
            const target_page = @intCast(i32, target_addr >> 12);
            const pages = @bitCast(u21, @intCast(i21, target_page - source_page));
            var inst = aarch64.Instruction{
                .pc_relative_address = mem.bytesToValue(meta.TagPayload(
                    aarch64.Instruction,
                    aarch64.Instruction.pc_relative_address,
                ), &buffer),
            };
            inst.pc_relative_address.immhi = @truncate(u19, pages >> 2);
            inst.pc_relative_address.immlo = @truncate(u2, p