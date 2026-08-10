#pragma once
#include <list>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>

#include <bitwuzla/cpp/bitwuzla.h>
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
    // Decides whether or not to fork paths at conditional moves
    // 0: disabled
    // 1: enabled
    FORK_AT_CMOVS,

    // Solves opaque predicates during symbolic execution.
    // However, it can prevent CFG recovery.
    // Shouldn't be used with CFG_RECOVERY option.
    // 0: disabled,
    // 1: enabled
    OPAQUE_SOLVING,

    // Sets the CPU behaviour during symbolic execution and lifting.
    // 0: Symbolic Exploration: discovers the all execution paths in the function,
    //      doesn't handle the loops. Works fine for linear functions.
    // 1: CFG Recovery: Tries to recover the CFG, rebuilds loops.
    //      However, its fragile on CFF & VM obfuscation, because
    //      it recovers CFG via tracing the RIP register.
    // 2: VCFG Recovery (semi-auto): Similar to CFG Recovery mode, but focuses on
    //      recovering the obfuscated CFGs. Like CFF and VM obfuscation.
    //      It requires custom implementation for detecting the dispatchers and
    //      VIP (for VM obfuscation).
    // see vexa::mode_t
    MODE,

    // Controls how CFG joins are reconstructed.
    // see vexa::cfg_join_policy_t
    // 0: merge every visit to the same VPC,
    // 1: specialize sibling paths while still rebuilding loops.
    CFG_JOIN_POLICY,

    // Cleans every dead register stores except for return register.
    // 0: disabled,
    // 1: enabled
    STATE_CLEANUP,

    // Simplifies expressions and propagates constants.
    // 0: disabled,
    // 1: enabled
    CONSTANT_PROPAGATION,

    // Tries to re-roll unrolled loops.
    // 0: disabled,
    // 1: enabled
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
    context(std::shared_ptr<vexa::engine> engine, vexa::arch _arch);
    std::shared_ptr<vexa::engine> engine;
    std::shared_ptr<vexa::cpu> cpu;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::ir::builder> builder;

    std::unique_ptr<llvm::LLVMContext> llvm_context;
    std::unique_ptr<llvm::Module> llvm_module;
    bw::TermManager term_manager;
    bw::Options bitwuzla_options;
    std::unique_ptr<bw::Bitwuzla> bitwuzla;
    llvm::FunctionCallee MarkerFunc;

    // options
    void set_option(option opt, int v);
    int get_option(option opt);

    std::unordered_map<option, int> options = {
        {option::FORK_AT_CMOVS, 1},
        {option::OPAQUE_SOLVING, 1},
        {option::MODE, 0},
        {option::STATE_CLEANUP, 1},
        {option::CONSTANT_PROPAGATION, 1},
        {option::LOOP_REROLL, 1},
        {option::CFG_JOIN_POLICY, 0}};

    void event_handler(vexa::event_kind event_k);
    std::unordered_map<vexa::event_kind, event_callback_t> event_callbacks;
};
} // namespace vexa