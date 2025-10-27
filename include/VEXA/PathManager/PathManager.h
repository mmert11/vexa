#include "../SymbolicState/SymbolicState.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <z3++.h>

#include <stack>

namespace VEXA
{
    struct Path
    {
    public:
        std::shared_ptr<VEXA::SymbolicState> snapshot;
        std::map<int, std::shared_ptr<llvm::Value>> registers;
        llvm::BasicBlock* block;
    };

    class PathManager
    {
    public:
        std::stack<Path> paths;
    };
}
