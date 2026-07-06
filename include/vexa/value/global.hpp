#pragma once

#include <llvm/IR/Value.h>
#include <z3++.h>

namespace vexa {

class global
{
public:
    global() = default;
    global(llvm::GlobalVariable* _lval, const llvm::DataLayout* _DL);

    uint64_t size() const;
    std::string name() const;
    llvm::Value* as_llvm() const;

private:
    llvm::GlobalVariable* val;
    const llvm::DataLayout* DL;
};

}