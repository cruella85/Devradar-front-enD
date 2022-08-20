extern "c" fn write(usize, usize, usize) usize;

pub fn main() void {
    print();
    print();
}

fn print() void {
    const msg = @ptrToInt("What is up? This is a longer message that will force the data to be relocated in virtual address space.\n");
    const len = 104;
    _ = write(1, msg, len);
}

// run
//
// What is up? This is a longer message that will force the data to be relocated in virtual address space.
// What is up? This is a longer message that will force the data to be relocated in virtual address space.
//
