#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <vexa/passes/memory_scalarizer.hpp>
#include <vexa/vexa.h>

bool vexa::passes::memory_scalarizer::run(llvm::Function *func)
{
    const llvm::DataLayout &DL = func->getParent()->getDataLayout();

    auto is_alloca = [](llvm::Value *ptr) {
        while (auto *gep = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr)) {
            ptr = gep->getPointerOperand();
        }
        return llvm::dyn_cast<llvm::AllocaInst>(ptr);
    };

    auto get_size = [&](llvm::Type *type) {
        return DL.getTypeStoreSize(type);
    };

    for (auto &BB : *func) {
        for (auto &I : BB) {
            llvm::Instruction *instruction = &I;
            builder->SetInsertPoint(instruction);

            // stores
            //
            if (auto *store = llvm::dyn_cast<llvm::StoreInst>(instruction)) {
                llvm::Value *pointer_operand = store->getPointerOperand();
                llvm::Value *stored_value = store->getValueOperand();

                if (get_size(stored_value->getType()) <= 1 || !is_alloca(pointer_operand))
                    continue;

                uint8_t size_in_bytes = get_size(stored_value->getType());
                for (uint8_t i = 0; i < size_in_bytes; i++) {
                    if (i == 0) {
                        // first byte, no need to shift
                        //
                        llvm::Value *byte =
                            builder->CreateTrunc(stored_value, builder->getInt8Ty());
                        builder->CreateStore(byte, pointer_operand);
                    }
                    else {
                        // shift and extract the byte
                        //
                        llvm::Value *shifted = builder->CreateLShr(
                            stored_value, builder->getIntN(size_in_bytes * 8, i * 8));
                        llvm::Value *truncated =
                            builder->CreateTrunc(shifted, builder->getInt8Ty());
                        llvm::Value *gep = builder->CreateInBoundsGEP(
                            builder->getInt8Ty(), pointer_operand, builder->getInt8(i));
                        builder->CreateStore(truncated, gep);
                    }
                }
                builder->deleteLater(store);
            }
            // loads
            //
            else if (auto *load = llvm::dyn_cast<llvm::LoadInst>(instruction)) {
                llvm::Value *pointer_operand = load->getPointerOperand();
                if (get_size(load->getType()) <= 1 || !is_alloca(pointer_operand))
                    continue;

                uint8_t size_in_bytes = get_size(load->getType());
                llvm::Value *value = nullptr;
                for (uint8_t i = 0; i < size_in_bytes; i++) {
                    if (i == 0) {
                        llvm::Value *byte =
                            builder->CreateLoad(builder->getInt8Ty(), pointer_operand);
                        value = builder->CreateZExt(byte, load->getType());
                    }
                    else {
                        llvm::Value *gep = builder->CreateInBoundsGEP(
                            builder->getInt8Ty(), pointer_operand, builder->getInt8(i));
                        llvm::Value *byte = builder->CreateLoad(builder->getInt8Ty(), gep);
                        llvm::Value *zext = builder->CreateZExt(byte, load->getType());
                        llvm::Value *shifted =
                            builder->CreateShl(zext, builder->getIntN(size_in_bytes * 8, i * 8));
                        value = builder->CreateOr(value, shifted);
                    }
                }
                load->replaceAllUsesWith(value);
                builder->deleteLater(load);
            }
            else {
                continue;
            }
        }
    }

    builder->eraseDeletedInstructions();
    return true;
}