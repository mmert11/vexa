#pragma once

#include "../context.hpp"
#include "../value/value.hpp"

#include <map>

namespace vexa
{
struct mem_page
{
    using storage_t = std::unordered_map<uint64_t, std::optional<bw::Term>>;

    mem_page(uint64_t size) : memory(std::make_shared<storage_t>())
    {
        if (size > 0)
            memory->reserve(size);
    }

    const std::optional<bw::Term> *find(uint64_t address) const
    {
        auto it = memory->find(address);
        if (it != memory->end())
            return &it->second;
        auto initial = initial_memory.find(address);
        return initial == initial_memory.end() ? nullptr : &initial->second;
    }

    void initialize(uint64_t address, bw::Term value)
    {
        if (sealed) {
            write(address, std::move(value));
            return;
        }
        initial_memory.insert_or_assign(address, std::move(value));
    }

    void seal() { sealed = true; }

    void write(uint64_t address, bw::Term value)
    {
        if (!memory.unique())
            memory = std::make_shared<storage_t>(*memory);
        memory->insert_or_assign(address, std::move(value));
    }

  private:
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
};
} // namespace vexa