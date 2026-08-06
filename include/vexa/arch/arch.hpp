#pragma once

#include <unordered_set>
#include <unordered_map>
#include <Zydis/Zydis.h>
#include <functional>

#include "../ir/builder.hpp"
#include "../memory/memory.hpp"

#include <llvm/IR/InstVisitor.h>
#include <remill/Arch/Arch.h>

#define LLVM_INST(val) llvm::dyn_cast<llvm::Instruction>(val)
#define VEXA_EXEC(val) emulate->run(llvm::dyn_cast<llvm::Instruction>(val))
#define VEXA_SYM_VAL(llvm_ptr) symex->get(llvm_ptr)
#define VEXA_SYM_PTR(llvm_ptr) vexa::to_ptr(symex->get(llvm_ptr))

namespace vexa {
typedef uint16_t reg_t;

class cpu
{
public:
    class emulator : public llvm::InstVisitor<emulator, vexa::value*>
    {
    public:
        emulator() {}
        emulator(vexa::cpu* _cpu) :
            context(_cpu->context), cpu(_cpu), builder(_cpu->builder), symex(context->symex), memory(context->memory), DL(context->llvm_module->getDataLayout()) {}

        void run_block(llvm::BasicBlock* BB);
        vexa::dual_value run(llvm::Instruction* I);

        vexa::value* visitBinaryOperator(llvm::BinaryOperator& I);
        vexa::value* visitAllocaInst(llvm::AllocaInst& I);
        vexa::value* visitStoreInst(llvm::StoreInst& I);
        vexa::value* visitLoadInst(llvm::LoadInst& I);
        vexa::value* visitInstruction(llvm::Instruction &I);
        vexa::value* visitGetElementPtrInst(llvm::GetElementPtrInst &I);
        vexa::value* visitICmpInst(llvm::ICmpInst& I);
        vexa::value* visitZExtInst(llvm::ZExtInst& I);
        vexa::value* visitSExtInst(llvm::SExtInst& I);
        vexa::value* visitTruncInst(llvm::TruncInst& I);
        vexa::value* visitCallInst(llvm::CallInst& I);
        vexa::value* visitSelectInst(llvm::SelectInst& I);
        vexa::value* visitFreezeInst(llvm::FreezeInst& I);
        vexa::value* visitExtractElementInst(llvm::ExtractElementInst& I);
    private:
        void write_memory_intrinsic(llvm::CallInst& intrinsic_call, size_t size);
        void read_memory_intrinsic(llvm::CallInst& intrinsic_call, size_t size);
        vexa::value* handle_llvm_intrinsics(llvm::CallInst& I, llvm::Function* callee, llvm::Intrinsic::ID id);

        vexa::context* context;
        vexa::cpu* cpu;
        std::shared_ptr<ir::builder> builder;
        std::shared_ptr<vexa::symex> symex;
        std::shared_ptr<vexa::memory> memory;
        llvm::DataLayout DL;
    };

    struct resolved_path_t
    {
        resolved_path_t(uint64_t _true_ip, uint64_t _false_ip) : true_ip(_true_ip), false_ip(_false_ip) {}
        uint64_t true_ip, false_ip;
    };

    struct snapshot
    {
        mem_state mem_ss;
        uint64_t pc, vpc;
        llvm::BasicBlock* bb;
        bool vjmp;
        std::unordered_map<uint64_t, llvm::BasicBlock *> path;
    };

    friend emulator;
    enum class internal_lifter_status
    {
        successful,
        explore_other_paths,
        function_ended,
        terminate
    };

    cpu(vexa::context* _context);
    void initialize_arch();
    void run();
    void run(uint64_t pc);
    internal_lifter_status process_instruction();
    internal_lifter_status lift_instruction(remill::Instruction inst);
    vexa::value* stack_access(uint64_t offset);
    void replace_remill_intrinsics();
    vexa::dual_value value_to_pointer(llvm::Value* addr);
    vexa::dual_value get_next_pc(llvm::BasicBlock* BB);
    vexa::dual_value get_condition(llvm::BasicBlock* BB);
    void handle_conditional_moves(remill::Instruction inst, llvm::BasicBlock* block);
    bool opaque_solver(vexa::value* condition, bool &result);
    snapshot take_snapshot(uint64_t pc, llvm::BasicBlock* bb);
    void restore_snapshot(snapshot ss);
    remill::Register* get_register(reg_t r);
    virtual remill::Register* get_return_register() = 0;
    vexa::value* read_register(reg_t r);
    void write_register(reg_t, vexa::value* val);
    reg_t str2reg(std::string r);
    std::map<std::string, reg_t> str_to_reg;
    std::map<reg_t, remill::Register*> registers;

    // path manager stuff
    bool is_ite(vexa::value* v);
    resolved_path_t resolve_ite(vexa::value* v);
    void branching(vexa::dual_value condition, uint64_t jump_pc, uint64_t fallthrough_pc);
    bool CFG_recovery(vexa::mode_t mode);
    bool VJMP = false;
    int lifted_count = 0;
    uint64_t VPC = 0;
    std::stack<snapshot> unexplored_paths;

    //vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*> CFG;
    std::unordered_map<uint64_t, llvm::BasicBlock *> CFG, VCFG, PATH;

    remill::Instruction instruction;
    llvm::BasicBlock* block;
    vexa::dual_value stack_ptr, next_pc, branch_taken;
    uint64_t PC = 0;

    remill::Arch::ArchPtr arch;
    remill::DecodingContext dec_context;
    std::optional<remill::IntrinsicTable> intrinsics;
    std::optional<remill::InstructionLifter> lifter;
    llvm::Value *state_ptr, *pc_arg, *mem_ptr;

    llvm::Function* vexa_lifted = nullptr;
    vexa::context* context;
    std::shared_ptr<ir::builder> builder;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::mem_page> global_memory;
    std::shared_ptr<cpu::emulator> emulate;
private:
    vexa::mode_t mode;
};
} // namespace vexa
