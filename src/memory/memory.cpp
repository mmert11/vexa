#include <vexa/vexa.h>

vexa::memory::memory() {}
vexa::memory::memory(vexa::context *_context) : context(_context) {}

std::shared_ptr<vexa::mem_page> vexa::memory::allocate(uint64_t size)
{
    std::shared_ptr<mem_page> new_page = std::make_shared<mem_page>(size);
    pages.push_back(new_page);
    return new_page;
}

// internal write function
void vexa::memory::write(vexa::pointer *addr, vexa::value *val)
{
    addr->simplify();
    VEXA_ASSERT(addr->is_concrete());

    // concrete write
    auto &page = addr->get_page()->concrete_memory;
    uint64_t base = addr->as_uint64();
    int byte_size = val->size() / 8;

    for (int i = 0; i < byte_size; i++)
        page[base + i] = context->bitwuzla->simplify(val->extract(i * 8 + 7, i * 8));
}

// internal read function
vexa::value *vexa::memory::read(vexa::pointer *addr, int size)
{
    addr->simplify();
    VEXA_ASSERT(addr->is_concrete());

    // helper for symbolic reads
    //
    auto page = addr->get_page();
    auto &concrete_memory = page->concrete_memory;
    auto read_byte = [&](uint64_t address) {
        auto it = concrete_memory.find(address);
        if (it != concrete_memory.end() && it->second)
            return *it->second;

        return context->term_manager.mk_const(
            context->term_manager.mk_bv_sort(8),
            "noinit_" + std::to_string(reinterpret_cast<uintptr_t>(page.get())) + "_"
                + std::to_string(address));
    };

    // concrete address read
    //
    uint64_t base = addr->as_uint64();
    int byte_size = size / 8;
    bw::Term value = read_byte(base + byte_size - 1);

    for (int i = byte_size - 2; i >= 0; i--)
        value = context->term_manager.mk_term(bw::Kind::BV_CONCAT, {value, read_byte(base + i)});

    return context->symex->value(context->bitwuzla->simplify(value));
}

vexa::mem_state vexa::memory::take_snapshot()
{
    vexa::mem_state state;
    for (auto &page : pages)
        state.pages.push_back(*page);

    return state;
}

void vexa::memory::restore_snapshot(vexa::mem_state ss)
{
    unsigned int i;
    for (i = 0; i < ss.pages.size(); i++)
        *pages[i] = ss.pages[i];

    if (pages.size() > ss.pages.size())
        pages.erase(pages.begin() + ss.pages.size(), pages.end());
}
