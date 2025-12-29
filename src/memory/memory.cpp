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
    int size = val.get_sort().bv_size();
    bool addr_is_concrete = addr.is_numeral();
    bool val_is_concrete = val.is_numeral();

    for (int i = 0; i < size / 8; i++)
    {
        z3::expr target_byte = val.extract(i * 8 + 7, i * 8);
        z3::expr target_addr = addr + context->z3_context->bv_val(i, 64);
        
        // write into z3 array
        *mem = z3::store(*mem, target_addr, target_byte);

        // if the address and value is concrete,
        // cache the value to improve performance
        if (addr_is_concrete && val_is_concrete) 
            concrete_cache[addr.as_uint64() + i] = 
                static_cast<uint8_t>(target_byte.simplify().as_uint64());
        else if (addr_is_concrete) concrete_cache.erase(addr.as_uint64() + i);
        else concrete_cache.clear();
    }
    CATCH()
}

z3::expr vexa::memory::read(z3::expr addr, int size)
{
    TRY()
    
    if (addr.is_numeral() && size == 8) 
    {
        uint64_t raw_addr = addr.as_uint64();
        auto it = concrete_cache.find(raw_addr);
        if (it != concrete_cache.end()) {
            return context->z3_context->bv_val(it->second, 8).simplify();
        }
    }

    if (size == 8) return z3::select(*mem, addr).simplify();

    z3::expr val_expr = z3::select(*mem, addr + (size / 8) - 1);
    for (int i = (size / 8) - 2; i >= 0; i--)
        val_expr = z3::concat(val_expr, z3::select(*mem, addr + context->z3_context->bv_val(i, 64)));

    return val_expr.simplify();
    CATCH()
}

vexa::mem_state vexa::memory::take_snapshot()
{
    return mem_state{*mem, concrete_cache};
}

void vexa::memory::restore_snapshot(vexa::mem_state ss)
{
    *mem = ss.mem;
    concrete_cache = ss.concrete_cache;
}