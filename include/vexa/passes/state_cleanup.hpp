#pragma once
#include "../ir/pass_manager.hpp"

namespace vexa {
namespace passes {

// Erase the dead stores to state structure. Especially flags.
// Do not run this pass if you want a full recoverage on CPU state.
// But that will result to complex calculations of flags at the end of the function.

class state_cleanup : public vexa::ir::pass
{
public:
    // see engine.hpp for mode descriptions
    state_cleanup(vexa::context* ctx) : pass(ctx) {}
    bool run(llvm::Function* func) override;
    bool is_recursive() override {
        return true;
    }
};

}
}