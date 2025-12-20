#include <vexa/vexa.h>

vexa::memory::memory() {}
vexa::memory::memory(std::shared_ptr<vexa::context> _context) : context(_context)
{
    TRY()
    // initialize memory as z3::array(64, 8)
    z3::sort mem_sort = context->z3_context->array_sort(context->z3_context->bv_sort(64), context->z3_context->bv_sort(8));
    mem = std::make_shared<z3::expr>(context->z3_context->constant("memory", mem_sort));

    CATCH()
}

void vexa::memory::write(z3::expr addr, z3::expr val)
{
    TRY()    
    z3::expr addr_expr = addr;
    z3::expr val_expr = val;
    int size = val_expr.get_sort().bv_size();

    if (size <= 0)
        THROW("size must be greater than zero");

    // write value byte by byte
    for (int i = 0; i < size / 8; i++)
    {
        z3::expr target_byte = val_expr.extract(i * 8 + 7, i * 8); // extract(high bit, low bit) (7,0), (13,8) etc.
        z3::expr target_addr = addr_expr + context->z3_context->bv_val(i, 64); // calculate the address for each byte
        *mem = z3::store(*mem, target_addr, target_byte); // write
    }
    CATCH()
}

z3::expr vexa::memory::read(z3::expr addr, int size)
{
    TRY()
    if (size <= 0)
        THROW("size must be greater than zero");

    z3::expr addr_expr = addr;
    z3::expr val_expr = z3::select(*mem, addr_expr + (size / 8) - 1); // read the last byte

    // read and combine the rest if size is greater than 8 bits
    for (int i = (size / 8) - 2; i >= 0; i--)
    {
        z3::expr target_addr = addr_expr + context->z3_context->bv_val(i, 64); // calculate the address for each byte
        z3::expr read_val = z3::select(*mem, target_addr); // read byte
        val_expr = z3::concat(val_expr, read_val); // combine the bytes
    }

    return val_expr.simplify();
    CATCH()
}

vexa::mem_state vexa::memory::take_snapshot()
{
    return mem_state{*mem};
}

void vexa::memory::restore_snapshot(vexa::mem_state ss)
{
    *mem = ss.mem;
}