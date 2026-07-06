#pragma once
#include "../ir/pass_manager.hpp"

namespace vexa {
namespace passes {

// Basic loop reroll pass to simplify the unrolled loops.
// It searches for vexa.unrolled.marker function calls in code to identify locations loop starts (see arch.cpp).
// And starts pattern matching to detect the edges of the loop and iteration count.
// Then recreates the unrolled loop in a real loop structure.
// Supports just lineer loop bodies, not with branches.
// Its just a PoC i wanted to work on, so im not too confident about this pass.
// And im sure the way pass works is not the best.
// It may fail on real life samples and edge cases.

class loop_reroll : public vexa::ir::pass
{
public:
    loop_reroll(vexa::context* ctx) : pass(ctx) {}
    bool run(llvm::Function* func) override;
    bool is_recursive() override {
        return true;
    }
};

}
}