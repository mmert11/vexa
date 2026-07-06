#pragma once

#include <unordered_set>
#include <Zydis/Zydis.h>

#include "../ir/builder.hpp"
#include "../memory/memory.hpp"

#include <llvm/IR/InstVisitor.h>
#include <remill/Arch/Arch.h>

#define LLVM_INST(val) llvm::dyn_cast<llvm::Instruction>(val)

namespace vexa {
typedef uint16_t reg_t;

class cpu
{
public:
    class symbolic_executor : public llvm::InstVisitor<symbolic_executor, vexa::shared_value>
    {
    public:
        symbolic_executor() {}
        symbolic_executor(vexa::context* c, vexa::cpu* _cpu) :
            context(c), cpu(_cpu), builder(_cpu->builder), symex(context->symex), memory(context->memory), DL(context->llvm_module->getDataLayout()) {}

        void execute_block(llvm::BasicBlock* BB);
        vexa::dual_value execute(llvm::Instruction* I);

        vexa::shared_value visitBinaryOperator(llvm::BinaryOperator& I);
        vexa::shared_value visitAllocaInst(llvm::AllocaInst& I);
        vexa::shared_value visitStoreInst(llvm::StoreInst& I);
        vexa::shared_value visitLoadInst(llvm::LoadInst& I);
        vexa::shared_value visitInstruction(llvm::Instruction &I);
        vexa::shared_value visitGetElementPtrInst(llvm::GetElementPtrInst &I);
        vexa::shared_value visitICmpInst(llvm::ICmpInst& I);
        vexa::shared_value visitZExtInst(llvm::ZExtInst& I);
        vexa::shared_value visitSExtInst(llvm::SExtInst& I);
        vexa::shared_value visitTruncInst(llvm::TruncInst& I);
        vexa::shared_value visitCallInst(llvm::CallInst& I);
        vexa::shared_value visitSelectInst(llvm::SelectInst& I);
        vexa::shared_value visitFreezeInst(llvm::FreezeInst& I);
    private:
        void write_memory_intrinsic(llvm::CallInst& intrinsic_call, size_t size);
        void read_memory_intrinsic(llvm::CallInst& intrinsic_call, size_t size);

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
        vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*>  lifted_blocks;
    };

    struct path_state
    {
        snapshot ss;
        uint64_t pc;
        llvm::BasicBlock* bb;
        uint64_t vip = 0;
    };

    class path_manager
    {
    public:
        path_manager(vexa::cpu* _cpu, ir::builder* _builder) : cpu(_cpu), builder(_builder) {}
        resolved_path_t resolve_path(vexa::shared_value v);
        uint64_t branching(vexa::dual_value condition, resolved_path_t path, uint64_t fallthrough_pc);
        uint64_t branching(vexa::dual_value condition, uint64_t jump_pc, uint64_t fallthrough_pc);
        uint64_t direct_branch(uint64_t addr);

        int lifted_count = 0;
        bool vbranching = false;
        bool is_conditional = false;
        uint64_t current_vip = 0;

        vexa::cpu* cpu;
        vexa::ir::builder* builder;

        std::stack<path_state> unexplored_paths;
        vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*> lifted_blocks;
    private:
        void handle_loops(uint64_t addr);
    };

    friend symbolic_executor;
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
    uint64_t lift_instruction(remill::Instruction inst);
    void replace_remill_intrinsics();
    vexa::dual_value value_to_pointer(llvm::Value* addr);
    vexa::dual_value get_next_pc(llvm::BasicBlock* BB);
    vexa::dual_value get_condition(llvm::BasicBlock* BB);
    void handle_conditional_moves(remill::Instruction inst, llvm::BasicBlock* block);

    snapshot take_snapshot();
    void restore_snapshot(snapshot ss);
    remill::Register* get_register(reg_t r);
    virtual remill::Register* get_return_register() = 0;
    reg_t str2reg(std::string r);

    llvm::Function* vexa_lifted = nullptr;
    vexa::context* context;
    std::shared_ptr<ir::builder> builder;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::mem_page> global_memory;

    std::shared_ptr<symbolic_executor> executor;
    path_manager p_manager;

    // internal
    llvm::Value* stack_ptr;
    vexa::dual_value next_pc, branch_taken;
    uint64_t program_counter = 0;
    std::unordered_map<std::string, reg_t> str_to_reg;
    std::unordered_map<reg_t, remill::Register*> registers;

    remill::Arch::ArchPtr arch;
    remill::DecodingContext dec_context;
    std::optional<remill::IntrinsicTable> intrinsics;
    std::optional<remill::InstructionLifter> lifter;
    llvm::Value *state_ptr, *pc_arg, *mem_ptr;
};
} // namespace vexa
