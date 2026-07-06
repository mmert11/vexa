#pragma once
#include "../ir/pass_manager.hpp"

namespace vexa {
namespace passes {

// Replacing the values which can be conretized with constants using z3 (see vexa::symex).
class constant_propagation : public vexa::ir::pass
{
public:
    constant_propagation(vexa::context* ctx) : pass(ctx) {}
    bool run(llvm::Function* func) override;
    bool is_recursive() override {
        return false;
    }
};

}
}