#pragma once
#include "../context.hpp"

namespace vexa {
namespace ir {
class builder;
class pass
{
public:
    pass(vexa::context* _context) : context(_context), builder(context->builder), symex(context->symex), memory(context->memory), cpu(context->cpu) {}
    virtual bool run() = 0;
    virtual bool is_recursive() = 0;

    bool did_run = false;
    vexa::context* context;
    std::shared_ptr<ir::builder> builder;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::cpu> cpu;
};

class simplify_cfg_and_dce : public pass
{
public:
    simplify_cfg_and_dce(vexa::context* ctx) : pass(ctx) {}
    bool run() override;
    bool is_recursive() override {
        return true;
    }
};

class mem2reg_and_sroa : public pass
{
public:
    mem2reg_and_sroa(vexa::context* ctx) : pass(ctx) {}
    bool run() override;
    bool is_recursive() override {
        return true;
    }
};

class Oz : public pass
{
public:
    Oz(vexa::context* ctx) : pass(ctx) {}
    bool run() override;
    bool is_recursive() override {
        return false;
    }
};

class pass_manager
{
public:
    pass_manager(vexa::context* _context) : context(_context) {};
    template <typename T, typename... Args>
    void add_pass() {
        pipeline.push_back(std::make_shared<T>(context));
    }
    void run();
private:
    vexa::context* context;
    std::vector<std::shared_ptr<pass>> pipeline;
};
}
}