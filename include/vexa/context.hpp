#pragma once
#include <list>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <bitwright.hpp>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "value/value.hpp"

#ifdef linux
#undef linux
#endif

namespace vexa
{
enum class arch
{
    x86_64
};

enum class os
{
    linux,
    windows
};

namespace ir
{
class builder;
}

class cpu;
class symex;
class memory;
class engine;

enum class option
{
    CONCRETIZE_RDTSC,
    FORK_AT_CMOVS,
    OPAQUE_SOLVING,
    MODE,
    CFG_JOIN_POLICY,
    STATE_CLEANUP,
    CONSTANT_PROPAGATION,
    LOOP_REROLL,
    COUNT,
};

enum class event_kind
{
    CONDITIONAL_TAKEN,
    CONDITIONAL_FALLTHROUGH,
    PATH_FORKING,
    DIRECT_JUMP,
    INDIRECT_JUMP,
    INSTRUCTION_LIFT
};

enum mode_t : int
{
    SYMBOLIC_EXPLORATION,
    CFG_RECOVERY,
    VCFG_RECOVERY
};

enum cfg_join_policy_t : int
{
    MERGE_BY_PC,
    SPECIALIZE_BY_PATH
};

using event_callback_t = std::function<void(vexa::engine &)>;
class context
{
  public:
    context(vexa::engine *engine, vexa::arch _arch);
    vexa::engine* engine;
    std::shared_ptr<vexa::cpu> cpu;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::ir::builder> builder;

    std::unique_ptr<llvm::LLVMContext> llvm_context;
    std::unique_ptr<llvm::Module> llvm_module;

    bwr::Context bitwright_ctx;
    std::unique_ptr<bwr::Engine> bitwright_engine;

    llvm::FunctionCallee MarkerFunc;

    // options
    void set_option(option opt, int v);
    int get_option(option opt);

    std::unordered_map<option, int> options = {
        {option::CONCRETIZE_RDTSC, 1},
        {option::FORK_AT_CMOVS, 1},
        {option::OPAQUE_SOLVING, 1},
        {option::CFG_JOIN_POLICY, 0},
        {option::MODE, 0},
        {option::STATE_CLEANUP, 1},
        {option::CONSTANT_PROPAGATION, 1},
        {option::LOOP_REROLL, 0}};

    void event_handler(vexa::event_kind event_k);
    std::unordered_map<vexa::event_kind, event_callback_t> event_callbacks;
};
} // namespace vexa
