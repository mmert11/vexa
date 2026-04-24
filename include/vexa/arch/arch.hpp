#pragma once

#include <unordered_set>
#include <Zydis/Zydis.h>

#include "../ir/builder.hpp"
#include "../memory/memory.hpp"

namespace vexa {
typedef uint16_t reg_t;

struct register_desc
{
    reg_t base_id;
    uint16_t size_bits;
    uint16_t offset_bits;
};

struct register_t
{
    vexa::value pointer;
    std::shared_ptr<z3::expr> expression;
};

struct resolved_path_t
{
    resolved_path_t(vexa::value _true_ip, vexa::value _false_ip) : true_ip(_true_ip), false_ip(_false_ip) {}
    vexa::value true_ip, false_ip;
};

using registers_ = std::map<reg_t, register_t>;
struct cpu_state
{
    registers_ registers;
};

struct snapshot
{
    cpu_state cpu_ss;
    mem_state mem_ss;
    vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*>  lifted_blocks;
};

struct path_state
{
    snapshot ss;
    vexa::value rip;
    llvm::BasicBlock* bb;
    uint64_t vip = 0;
};

class cpu;
// Manages control flow graph recovery and marks the unrolled loop edges
class path_manager
{
public:
    path_manager(vexa::cpu* _cpu, ir::builder* _builder) : cpu(_cpu), builder(_builder) {}
    vexa::resolved_path_t resolve_path(vexa::value v);
    vexa::value branching(vexa::value condition, vexa::value true_addr, vexa::value false_addr);
    vexa::value branching(vexa::value addr);

    int lifted_count = 0;
    bool vbranching = false;
    bool is_conditional = false;
    uint64_t current_vip = 0;

    vexa::cpu* cpu;
    vexa::ir::builder* builder;

    std::stack<path_state> unexplored_paths;
    vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*> lifted_blocks;
    std::unordered_set<llvm::BasicBlock*> uloop_backedges;
private:
    void handle_loops(uint64_t addr);
};

class cpu
{
public:
    cpu(vexa::context* _context) :
        context(_context), builder(context->builder), symex(context->symex), memory(context->memory), p_manager(this, builder.get()) {};

    virtual void run() = 0;
    virtual void write_register(reg_t reg, vexa::value value) = 0;
    virtual void _write_register(reg_t reg, z3::expr value) = 0;

    virtual vexa::value read_register(reg_t reg) = 0;
    virtual z3::expr _read_register(reg_t reg) = 0;
    z3::expr bv_bool(bool v);

    vexa::value stack_ptr, original_sp;
    snapshot take_snapshot();
    void restore_snapshot(snapshot ss);

    registers_ registers;

    vexa::context* context;
    std::shared_ptr<ir::builder> builder;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::memory> memory;
    path_manager p_manager;
};
} // namespace vexa
