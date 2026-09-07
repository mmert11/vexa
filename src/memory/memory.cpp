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

    auto page = addr->get_page();

    // concrete write
    if (addr->is_concrete()) {
        uint64_t base = addr->as_uint64();
        uint8_t byte_size = val->size() / 8;

        for (uint8_t i = 0; i < byte_size; i++) {
            mem_cell cell{val->simplify(), byte_size, i};
            page->write(base + i, std::move(cell));
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
        const mem_cell *cell = page->find(address);
        if (cell) {
            return *cell;
        }

        vexa::value *uninit_read = context->symex->symbolic(
            "uninit_" + std::to_string(reinterpret_cast<uintptr_t>(page.get())) + "_"
                + std::to_string(address),
            8);

        mem_cell uninit_cell{uninit_read, 1, 0};
        //page->write(address, uninit_cell);
        return uninit_cell;
    };

    /*

    value:
    15 20 35 16
    3  2  1  0

    0x1000:
    16 35 20 15
    0  1  2  3

    */

    if (addr->is_concrete()) {
        uint64_t base = addr->as_uint64();
        int byte_size = size / 8;

        mem_cell last_byte = read_byte(base + byte_size - 1);
        vexa::value *last_byte_original_val = last_byte.original_val;
        uint8_t last_byte_total_size = last_byte.total_size;

        std::vector<mem_cell> read_bytes{last_byte};

        for (int j = byte_size - 2; j >= 0; j--) {
            read_bytes.push_back(read_byte(base + j));
        }

        // analyze bytes to determine how we can extract
        // them in the most optimized way
        //
        bool bytes_can_be_merged = true;
        bool bytes_aligned = true;
        for (int i = 0; i < byte_size; i++) {
            mem_cell cell = read_bytes[i];

            if (cell.original_val->as_expr() != last_byte_original_val->as_expr()
                || cell.total_size != last_byte_total_size)
            {
                bytes_can_be_merged = false;
            }

            if (cell.which_byte != last_byte.which_byte - i) {
                bytes_aligned = false;
            }
        }

        bool size_matches = byte_size == last_byte_total_size;
        if (bytes_can_be_merged && bytes_aligned && size_matches) {
            return last_byte_original_val;
        }

        if (bytes_can_be_merged && bytes_aligned) {
            uint32_t low_bit = read_bytes.back().which_byte * 8;
            uint32_t high_bit = low_bit + size - 1;
            return last_byte_original_val->extract(high_bit, low_bit);
        }

        // bytes can't be merged
        // we have to extract the necessary bytes one by one
        //
        vexa::value *value =
            last_byte_original_val->extract(last_byte.which_byte * 8 + 7, last_byte.which_byte * 8);
        for (int i = 1; i < read_bytes.size(); i++) {
            mem_cell byte = read_bytes[i];
            vexa::value *extracted_byte =
                byte.original_val->extract(byte.which_byte * 8 + 7, byte.which_byte * 8);
            value = value->concat(*extracted_byte);
        }

        return value;
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
