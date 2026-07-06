#pragma once
#include "../context.hpp"
#include "../symex/symex.hpp"
#include "../ir/builder.hpp"
#include "../arch/amd64.hpp"
#include "../memory/memory.hpp"
#include "../binary/binary.hpp"

namespace vexa
{

enum class option : int
{
    // Cleans every dead register stores except for return register.
    // 0: disabled,
    // 1: enabled
    STATE_CLEANUP,

    // Simplifies expressions and propagates constants.
    // 0: disabled,
    // 1: enabled
    Z3_CONSTANT_PROPAGATION,

    // Tries to re-roll unrolled loops.
    // 0: disabled,
    // 1: enabled
    LOOP_REROLL,

    COUNT,
};

class engine
{
public:
    engine(vexa::arch arch = arch::x86_64);
    vexa::context* get_context();
    std::shared_ptr<ir::builder> get_builder();
    std::shared_ptr<vexa::memory> get_memory();
    std::shared_ptr<vexa::symex> get_symex();
    std::shared_ptr<vexa::cpu> get_cpu();

    void run(uint64_t pc);
    void write_memory(uint64_t address, std::vector<uint8_t> buffer);
    void mark_symbolic(uint64_t address, uint32_t size);

    // options
    void set_option(option opt, int v);
    int get_option(option opt);

    // map binary to memory.
    // supports elf & pe.
    void map_binary(vexa::binary& binary);

    // utils
    void optimize();
    void print_ir();
    void reset();

    // recompiles the lifted module
    std::vector<uint8_t> recompile(bool optimize = true);
    // patches the binary at given rva
    // and relocates the symbols
    void patch(vexa::binary& binary, std::vector<uint8_t> object_file, uint64_t va, std::string section_name = ".text");
private:
    vexa::arch _arch;
    vexa::context* context;
    std::shared_ptr<vexa::ir::builder> builder;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::cpu> cpu;
    std::shared_ptr<vexa::symex> symex;

    std::unordered_map<option, int> options = {
        {option::STATE_CLEANUP, 2},
        {option::Z3_CONSTANT_PROPAGATION, 1},
        {option::LOOP_REROLL, 1}
    };

    std::string ir;
    std::chrono::milliseconds time;
    int instr_count;

    std::vector<uint8_t> fix_relocations(vexa::binary &object_file, std::vector<uint8_t> code_content, uint64_t new_section_rva, uint64_t image_base);
};
}
