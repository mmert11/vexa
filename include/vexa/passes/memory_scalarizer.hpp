#pragma once
#include "../ir/pass_manager.hpp"

namespace vexa
{
namespace passes
{

// Scalarizing the load/stores that operates on allocas.
// In short, transforming i64 i32 i16 operations to multiple i8 operations.
// So LLVM can optimize the overlapped read/writes.

class memory_scalarizer : public vexa::ir::pass
{
  public:
    memory_scalarizer(vexa::context *ctx) : pass(ctx) {}
    bool run(llvm::Function *func) override;
    bool is_recursive() override { return false; }
};

} // namespace passes
} // namespace vexa