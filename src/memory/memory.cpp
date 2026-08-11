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

    // concrete write
    auto page = addr->get_page();

    if (addr->is_concrete()) {
        uint64_t base = addr->as_uint64();
        int byte_size = val->size() / 8;

        for (int i = 0; i < byte_size; i++) {
            bw::Term byte = val->extract(i * 8 + 7, i * 8);
            if (!byte.is_value())
                byte = context->bitwuzla->simplify(byte);
            page->write(base + i, std::move(byte));
        }
    }
}

// internal read function
vexa::value *vexa::memory::read(vexa::pointer *addr, int size)
{
    addr->simplify();

    // helper
    auto page = addr->get_page();
    auto read_byte = [&](uint64_t address) {
        const std::optional<bw::Term> *byte = page->find(address);
        if (byte && *byte)
            return **byte;

        return context->term_manager.mk_const(
            context->term_manager.mk_bv_sort(8),
            "noinit_" + std::to_string(reinterpret_cast<uintptr_t>(page.get())) + "_"
                + std::to_string(address));
    };

    if (addr->is_concrete()) {
        uint64_t base = addr->as_uint64();
        int byte_size = size / 8;
        bw::Term value = read_byte(base + byte_size - 1);

        for (int i = byte_size - 2; i >= 0; i--)
            value =
                context->term_manager.mk_term(bw::Kind::BV_CONCAT, {value, read_byte(base + i)});

        if (!value.is_value())
            value = context->bitwuzla->simplify(value);

        return context->symex->value(std::move(value));
    }
    else {
        return context->symex->symbolic(
            "read_" + std::to_string(std::hash<bw::Term>{}(addr->as_expr())), size);
    }
}

vexa::mem_state vexa::memory::take_snapshot() const
{
    vexa::mem_state state;
    state.pages.reserve(pages.size());
    for (const auto &page : pages)
        state.pages.push_back({page, page->memory});
    return state;
}

void vexa::memory::restore_snapshot(const vexa::mem_state &ss)
{
    pages.resize(ss.pages.size());
    for (size_t i = 0; i < ss.pages.size(); ++i) {
        pages[i] = ss.pages[i].page;
        pages[i]->memory = ss.pages[i].memory;
    }
}
