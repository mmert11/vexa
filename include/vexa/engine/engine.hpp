#pragma once
#include "../arch/amd64.hpp"
#include "../binary/binary.hpp"
#include "../context.hpp"
#include "../ir/builder.hpp"
#include "../memory/memory.hpp"
#include "../symex/symex.hpp"

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>

namespace vexa
{
class engine
{
  public:
    engine(vexa::arch arch = vexa::arch::x86_64);
    vexa::context *get_context();
    std::shared_ptr<vexa::ir::builder> get_builder();
    std::shared_ptr<vexa::memory> get_memory();
    std::shared_ptr<vexa::symex> get_symex();
    std::shared_ptr<vexa::cpu> get_cpu();

    void run(uint64_t pc);
    void write_memory(uint64_t address, std::span<const uint8_t> buffer);
    void mark_symbolic(uint64_t address, uint32_t size);

    // options
    void set_option(option opt, int v);
    int get_option(option opt);

    // map binary to memory.
    // supports elf & pe.
    void map_binary(vexa::binary &binary);

    // event callbacks
    void set_callback(vexa::event_kind kind, vexa::event_callback_t callback);

    // utils
    void optimize();
    void print();
    void reset();

    // recompiles the lifted module
    std::vector<uint8_t> recompile(bool optimize = true);
    // patches the binary at given rva
    // and relocates the symbols
    void patch(vexa::binary &binary, std::vector<uint8_t> object_file, uint64_t va);

    std::string ir;

  private:
    vexa::arch _arch;
    vexa::context *context;
    std::shared_ptr<vexa::ir::builder> builder;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::cpu> cpu;
    std::shared_ptr<vexa::symex> symex;

    std::chrono::milliseconds time;
    int instr_count;

    std::vector<uint8_t> fix_relocations(
        vexa::binary &object_file,
        std::vector<uint8_t> code_content,
        uint64_t new_section_rva,
        uint64_t image_base,
        uint64_t shift_offset = 0);
    std::vector<uint8_t> read_binary_file(const std::string &filename);
};
} // namespace vexa
