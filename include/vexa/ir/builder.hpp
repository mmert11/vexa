#pragma once
#include "../context.hpp"
#include "../value/global.hpp"
#include "../value/value.hpp"
#include "pass_manager.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>

namespace vexa
{
namespace ir
{

class builder : public llvm::IRBuilder<>
{
  public:
    builder(vexa::context *_context)
        : IRBuilder<>(*_context->llvm_context), context(_context), symex(context->symex) {};
    llvm::Function *create_function(std::string name, std::vector<llvm::Type *> args);
    void add_attribute(llvm::Attribute::AttrKind attr);
    void inline_asm(std::string asmCode);
    int instr_count();
    std::vector<uint8_t>
    recompile(vexa::arch arch, llvm::Function *func = nullptr, bool optimize = true);
    void set_function(llvm::Function *f);
    llvm::Function *get_function();
    llvm::BasicBlock *basic_block(std::string name = "");
    vexa::global global_var(llvm::Type *type, std::string name);
    vexa::dual_value inbounds_gep(llvm::Type *type, llvm::Value *base, llvm::Value *offset);
    vexa::dual_value
    inttoptr(llvm::Value *v, llvm::Type *destTy, std::shared_ptr<vexa::mem_page> page);
    vexa::dual_value get_value_by_name(std::string name);
    void push_ip();
    void pop_ip();
    void deleteLater(llvm::Instruction *I);
    void eraseDeletedInstructions();

  private:
    vexa::context *context;
    std::shared_ptr<vexa::symex> symex;
    llvm::Function *function;
    std::stack<llvm::IRBuilderBase::InsertPoint> insert_points;
    std::vector<llvm::WeakVH> toDelete;
};

} // namespace ir
} // namespace vexa