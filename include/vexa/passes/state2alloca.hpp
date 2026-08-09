#pragma once
#include "../ir/pass_manager.hpp"

namespace remill
{
struct Register;
}

namespace vexa
{
namespace passes
{

class state2alloca : public vexa::ir::pass
{
  public:
    state2alloca(vexa::context *ctx) : pass(ctx) {}
    bool run(llvm::Function *func) override;
    bool is_recursive() override { return false; }

  private:
    remill::Register *sub_to_parent(uint32_t offset);
};

} // namespace passes
} // namespace vexa