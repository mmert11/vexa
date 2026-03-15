#pragma once
#include "../context.hpp"
#include "../ir/builder.hpp"
#include "../arch/x64/cpu.hpp"
#include "../memory/memory.hpp"

#include <CSiMBA.h>
#include <LLVMParser.h>

#define MBA_SOLVING

namespace vexa
{
    class engine
    {
    public:
        engine(vexa::arch arch = arch::x86_64);
        std::shared_ptr<vexa::context> get_context();
        std::shared_ptr<ir::builder> get_builder();
        std::shared_ptr<vexa::memory> get_memory();
        std::shared_ptr<vexa::symex> get_symex();
        std::shared_ptr<vexa::cpu> get_cpu();

        void write_memory(uint64_t address, std::vector<uint8_t> buffer);
        void run();

        void optimize();
        void print_ir();
        std::vector<uint8_t> recompile(vexa::arch arch, bool optimize = true);
    private:
        vexa::arch _arch;
        std::shared_ptr<vexa::context> context;
        std::shared_ptr<vexa::ir::builder> builder;
        std::shared_ptr<vexa::memory> memory;
        std::shared_ptr<vexa::cpu> cpu;
        std::shared_ptr<vexa::symex> symex;

        std::string ir;
        std::chrono::milliseconds time;
        int instr_count;
    };
}
