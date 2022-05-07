const DwarfInfo = @This();

const std = @import("std");
const assert = std.debug.assert;
const dwarf = std.dwarf;
const leb = std.leb;
const log = std.log.scoped(.macho);
const math = std.math;
const mem = std.mem;

const Allocator = mem.Allocator;
pub const AbbrevLookupTable = std.AutoHashMap(u64, struct { pos: usize, len: usize });
pub const SubprogramLookupByName = std.StringHashMap(struct { addr: u64, size: u64 });

debug_info: []const u8,
debug_abbrev: []const u8,
debug_str: []const u8,

pub fn getCompileUnitIterator(self: DwarfInfo) CompileUnitIterator {
    return .{ .ctx = self };
}

const CompileUnitIterator = struct {
    ctx: DwarfInfo,
    pos: usize = 0,

    pub fn next(self: *CompileUnitIterator) !?CompileUnit {
        if (self.pos >= self.ctx.debug_info.len) return null;

        var stream = std.io.fixedBufferStream(self.ctx.debug_info);
        var creader = std.io.countingReader(stream.reader());
        const reader = creader.reader();

        const cuh = try CompileUnit.Header.read(reader);
        const total_length = cuh.length + @as(u64, if (cuh.is_64bit) @sizeOf(u64) else @sizeOf(u32));
        const offset = math.cast(usize, creader.bytes_read) orelse return error.Overflow;

        const cu = CompileUnit{
            .cuh = cuh,
            .debug_info_off = offset,
        };

        self.pos += (math.cast(usize, total_length) orelse return error.Overflow);

        return cu;
    }
};

pub fn genSubprogramLookupByName(
    self: DwarfInfo,
    compile_unit: CompileUnit,
    abbrev_lookup: AbbrevLookupTable,
    lookup: *SubprogramLookupByName,
) !void {
    var abbrev_it = compile_unit.getAbbrevEntryIterator(self);
    while (try abbrev_it.next(abbrev_lookup)) |entry| switch (entry.tag) {
        dwarf.TAG.subprogram => {
            var attr_it = entry.getAttributeIterator(self, compile_unit.cuh);

            var name: ?[]const u8 = null;
            var low_pc: ?u64 = null;
            var high_pc: ?u64 = null;

            while (try attr_it.next()) |attr| switch (attr.name) {
                dwarf.AT.name => if (attr.getString(self, compile_unit.cuh)) |str| {
                    name = str;
                },
                dwarf.AT.low_pc => {
                    if (attr.getAddr(self, compile_unit.cuh)) |addr| {
                        low_pc = addr;
                    }
                    if (try attr.getConstant(self)) |constant| {
                        low_pc = @intCast(u64, constant);
                    }
                },
                dwarf.AT.high_pc => {
                    if (attr.getAddr(self, compile_unit.cuh)) |addr| {
                        high_pc = addr;
                    }
                    if (try attr.getConstant(self)) |constant| {
                        high_pc = @intCast(u64, constant);
                    }
                },
                else => {},
            };

            if (name == null or low_pc == null or high_pc == null) continue;

            try lookup.putNoClobber(name.?, .{ .addr = low_pc.?, .size = high_pc.? });
        },
        else => {},
    };
}

pub fn genAbbrevLookupByKind(self: DwarfInfo, off: usize, lookup: *AbbrevLookupTable) !void {
    const data = self.debug_abbrev[off..];
    var stream = std.io.fixedBufferStream(data);
    var creader = std.io.countingReader(stream.reader());
    const reader = creader.reader();

    while (true) {
        const kind = try leb.readULEB128(u64, reader);

        if (kind == 0) break;

        const pos = math.cast(usize, creader.bytes_read) orelse return error.Overflow;
        _ = try leb.readULEB128(u64, reader); // TAG
        _ = try reader.readByte(); // CHILDREN

        while (true) {
            const name = try leb.readULEB128(u64, reader);
            const form = try leb.readULEB128(u64, reader);

            if (name == 0 and form == 0) break;
        }

        const next_pos = math.cast(usize, creader.bytes_read) orelse return error.Overflow;

        try lookup.putNoClobber(kind, .{
            .pos = pos,
            .len = next_pos - pos - 2,
   