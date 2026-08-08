#include <vexa/vexa.h>
#include <vexa/passes/state2alloca.hpp>

remill::Register* vexa::passes::state2alloca::sub_to_parent(uint32_t offset)
{
    remill::Register* parent = nullptr;
    auto callback = [&](const remill::Register* r) {
        if (offset == r->offset)
        {
            remill::Register* found_r = (remill::Register*)r;
            while (found_r->parent) {
                found_r = (remill::Register*)found_r->parent;
            }

            parent = found_r;
        }
    };
    cpu->arch->ForEachRegister(callback);
    return parent;
}

bool vexa::passes::state2alloca::run(llvm::Function* func)
{
    auto& cpu = context->cpu;
    std::unordered_map<uint32_t, llvm::Value*> offset_to_alloca;

    llvm::BasicBlock* first_bb = &cpu->vexa_lifted->front();
    llvm::BasicBlock* entry_bb = builder->basic_block("entry");
    builder->SetInsertPoint(entry_bb);
    entry_bb->moveBefore(&cpu->vexa_lifted->front());

    // we had to handle this state offset 2080 separetely, hard to explain...
    llvm::AllocaInst* slot = builder->CreateAlloca(builder->getInt32Ty());
    {
        auto* addr = builder->CreateInBoundsGEP(
                         builder->getInt8Ty(), cpu->state_ptr.l, builder->getInt64(2080));
        builder->CreateStore(
            builder->CreateLoad(builder->getInt32Ty(), addr), slot);
    }
    offset_to_alloca[2080] = slot;

    // create alloca for every register
    auto create_alloca = [&](const remill::Register* r) {
        if (!r->parent)
        {
            llvm::Value* alloca = builder->CreateAlloca(builder->getIntNTy(r->size * 8), builder->getIntN(64, 1), r->name);
            offset_to_alloca[r->offset] = alloca;
        }
    };
    cpu->arch->ForEachRegister(create_alloca);

    // load from state and store to the alloca for every register
    auto init_alloca = [&](const remill::Register* r) {
        if (!r->parent)
        {
            auto reg_addr = builder->CreateInBoundsGEP(builder->getInt8Ty(), cpu->state_ptr.l, builder->getIntN(64, r->offset), r->name);
            llvm::Value* load = builder->CreateLoad(builder->getIntNTy(r->size * 8), reg_addr);
            builder->CreateStore(load, offset_to_alloca[r->offset]);
        }
    };
    cpu->arch->ForEachRegister(init_alloca);
    builder->CreateBr(first_bb);

    for (auto& BB : llvm::make_early_inc_range(*func))
    {
        if (&BB == entry_bb) continue;

        for (auto& I : llvm::make_early_inc_range(BB))
        {
            llvm::GetElementPtrInst* GEP = llvm::dyn_cast<llvm::GetElementPtrInst>(&I);
            if (!GEP) continue;

            if (GEP->getPointerOperand() != cpu->state_ptr.l)
                continue;

            llvm::APInt offset_(64, 0);
            if (!GEP->accumulateConstantOffset(context->llvm_module->getDataLayout(), offset_))
                continue;
            uint64_t offset = offset_.getZExtValue();

            auto found = offset_to_alloca.find(offset);
            if (found != offset_to_alloca.end())
            {
                // offsets are match, basically replace state gep to alloca
                GEP->replaceAllUsesWith(offset_to_alloca[offset]);
                GEP->eraseFromParent();
                continue;
            }

            remill::Register* parent_r = sub_to_parent(offset);
            if (!parent_r)
                continue;

            auto found2 = offset_to_alloca.find(parent_r->offset);
            if (found2 == offset_to_alloca.end())
                continue;

            llvm::Value* alloca_ptr = found2->second;
            VEXA_ASSERT(alloca_ptr);

            builder->SetInsertPoint(GEP);
            llvm::Value* offset_gep =
                builder->CreateInBoundsGEP(builder->getInt8Ty(), alloca_ptr, builder->getInt64(offset - parent_r->offset));

            GEP->replaceAllUsesWith(offset_gep);
            GEP->eraseFromParent();
            offset_gep->setName(parent_r->name);
        }
    }

    // copy all allocas to state before every ret instruction
    for (auto& BB : *func)
    {
        llvm::Instruction* term = BB.getTerminator();
        if (llvm::isa<llvm::ReturnInst>(term))
        {
            builder->SetInsertPoint(term);
            for (auto& [offset, alloca] : offset_to_alloca)
            {
                llvm::Type* ty = llvm::cast<llvm::AllocaInst>(alloca)->getAllocatedType();
                auto* reg_addr = builder->CreateInBoundsGEP(builder->getInt8Ty(), cpu->state_ptr.l, builder->getInt64(offset));
                builder->CreateStore(builder->CreateLoad(ty, alloca), reg_addr);
            }
        }
    }

    return true;
}