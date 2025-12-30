#pragma once
#include "builder.hpp"
#include "../context.hpp"
#include "../engine/engine.hpp"

#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>

#include <fstream>

namespace vexa::ir
{
    class recompiler
    {
    public:
        recompiler(std::shared_ptr<vexa::context> c);
        std::vector<uint8_t> recompile(vexa::arch arch, bool optimize = true);
    private:
        std::shared_ptr<vexa::context> context;
    };
}