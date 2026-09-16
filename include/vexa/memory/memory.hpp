#pragma once

#include "../context.hpp"
#include "../value/value.hpp"

namespace vexa
{

struct mem_cell
{
    vexa::value *original_val;
    uint8_t total_size; // in bytes
    uint8_t which_byte;
    vexa::value *extracted_cache;
};

struct mem_page
{
    using storage_t = std::unordered_map<uint64_t, mem_cell>;

    mem_page(uint64_t size) : memory(std::make_shared<storage_t>())
    {
        if (size > 0)
            memory->reserve(size);
    }

    const mem_cell *find(uint64_t address) const
    {
        auto it = memory->find(address);
        if (it != memory->end())
            return &it->second;
        auto initial = initial_memory.find(address);
        return initial == initial_memory.end() ? nullptr : &initial->second;
    }

    void initialize(uint64_t address, vexa::value *value)
    {
        if (sealed) {
            write(address, value);
            return;
        }
        initial_memory.insert_or_assign(address, mem_cell{value, 1, 0});
    }

    void seal() { sealed = true; }

    void write(uint64_t address, vexa::value* value)
    {
        if (!memory.unique())
            memory = std::make_shared<storage_t>(*memory);
        memory->insert_or_assign(address, mem_cell{value, 1, 0});
    }

    void write(uint64_t address, mem_cell cell)
    {
        if (!memory.unique())
            memory = std::make_shared<storage_t>(*memory);
        memory->insert_or_assign(address, cell);
    }

    friend class memory;
    storage_t initial_memory;
    std::shared_ptr<storage_t> memory;
    bool sealed = false;
};

struct mem_state
{
    struct page_state
    {
        std::shared_ptr<mem_page> page;
        std::shared_ptr<mem_page::storage_t> memory;
    };

    std::vector<page_state> pages;
};

class memory
{
  public:
    memory();
    memory(vexa::context *_context);

    std::shared_ptr<mem_page> allocate(uint64_t size = 0);
    void write(vexa::pointer *addr, vexa::value *val);
    vexa::value *read(vexa::pointer *addr, int size);

    mem_state take_snapshot() const;
    void restore_snapshot(const mem_state &ss);

  private:
    vexa::context *context;
    std::vector<std::shared_ptr<mem_page>> pages;
    using hash_ty = std::hash<bw::Term>;
    hash_ty hash;
};
} // namespace vexa