#pragma once
#include <stdint.h>
#include <map>
#include <unordered_map>
#include <string>
#include <stack>

#include <Zydis/Zydis.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include "../ir/builder.hpp"
#include "../memory/memory.hpp"

namespace vexa
{
    typedef uint16_t reg_t;

    struct register_desc
    {
		reg_t base_id;
		uint16_t size_bits;
		uint16_t offset_bits;
	};

    struct cpu_state
    {
    public: std::map<reg_t, vexa::value> registers;
    };

    struct snapshot
    {
    public:
        cpu_state cpu_ss;
        mem_state mem_ss;
    };

    struct path_state
    {
    public:
        snapshot ss;
        vexa::value rip;
        llvm::BasicBlock* bb;
    };

    class cpu
    {
    public:
        cpu(std::shared_ptr<ir::builder> _builder, std::shared_ptr<vexa::symex> _symex,
            std::shared_ptr<vexa::memory> _memory, std::shared_ptr<vexa::context> _context) :
            builder(_builder), symex(_symex), memory(_memory), context(_context) {};
            
        virtual void run() = 0;
        virtual void write_register(reg_t reg, vexa::value value) = 0;
        virtual vexa::value read_register(reg_t reg) = 0;
        int lifted_count;
        vexa::value stack_ptr, original_sp;

        snapshot take_snapshot();
        void restore_snapshot(snapshot ss);
    
        std::stack<path_state> unexplored_paths;
        std::map<reg_t, vexa::value> registers;
        std::map<uint64_t, llvm::BasicBlock*> lifted_blocks;

        std::shared_ptr<ir::builder> builder;
        std::shared_ptr<vexa::symex> symex;
        std::shared_ptr<vexa::memory> memory;
        std::shared_ptr<vexa::context> context;
    };
}
