#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Analysis/ConstantFolding.h>
#include <llvm/IR/InstIterator.h>

#include "../context.hpp"
#include "../value/value.hpp"
#include "../symex/symex.hpp"

namespace vexa
{
    namespace ir
    {
        class builder : private llvm::IRBuilder<>
        {
        public:
            builder(std::shared_ptr<vexa::context> _context, std::shared_ptr<vexa::symex> _symex);

            // utils
            llvm::Type* get_int_ty(unsigned int size);
            vexa::value get_const_int(uint64_t value, int bit_size);
            vexa::value symvar(int param_idx, std::string name);
            int get_instr_count();
            void optimize();
            void unreachable();

            llvm::Function* create_function(std::string name, std::vector<llvm::Type*> args);
            llvm::Function* get_function();
            void set_function(llvm::Function* _function);

            // basic block and branching
            llvm::BasicBlock* basic_block(std::string name);
            llvm::BasicBlock* basic_block(std::string name, llvm::Function* function);
            void set_ip(llvm::BasicBlock* bb);
            void jump(llvm::BasicBlock* bb);
            void jump_if(vexa::value cond, llvm::BasicBlock* then_bb, llvm::BasicBlock* else_bb);
            void ret(vexa::value v);

            // arithmetic
            vexa::value resize(vexa::value value, unsigned int size, bool sign_extend = false);
            vexa::value extract(vexa::value value, uint8_t high, uint8_t low, std::string name);
            void normalize(vexa::value& lhs, vexa::value& rhs, bool sign_extend = false);

            vexa::value add(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value sub(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value mul(vexa::value lhs, vexa::value rhs, std::string name);

            vexa::value cmpeq(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value select(vexa::value cond, vexa::value lhs, vexa::value rhs, std::string name);

            // bit operations
            vexa::value band(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value bshl(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value bshr(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value bor(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value bxor(vexa::value lhs, vexa::value rhs, std::string name);
            vexa::value bnot(vexa::value lhs, std::string name);
        private:
            std::shared_ptr<vexa::context> context;
            std::shared_ptr<vexa::symex> symex;
            llvm::Function* function;
            const llvm::DataLayout* DL;
        };
    }
}
