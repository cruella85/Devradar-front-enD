comptime {
    @Type(.{ .Struct = .{
        .layout = .Auto,
        .fields = &.{.{
            .name = "foo",
            .type = u32,
            .default_value = null,
            .is_comptime = false,
            .alignment = 4,
        }},
        .decls = &.{},
        .is_tuple = true,
    } });
}
comptime {
    @Type(.{ .Struct = .{
        .layout = .Auto,
        .fields = &.{.{
            .name = "3",
            .type = u32,
  