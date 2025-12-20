#pragma once
#include <map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <z3++.h>

#include "value/value.hpp"

namespace vexa
{
    class context
    {
    public:
        context();
        std::shared_ptr<llvm::LLVMContext> llvm_context;
        std::shared_ptr<llvm::Module> llvm_module;
        std::shared_ptr<z3::context> z3_context;
    };
}
