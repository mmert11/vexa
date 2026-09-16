#include <vexa/vexa.h>

vexa::context::context(vexa::engine *_engine, vexa::arch _arch)
{
    llvm_context = std::make_unique<llvm::LLVMContext>();

    bitwuzla_options.set(bw::Option::PRODUCE_MODELS, 1);
    bitwuzla_options.set(bitwuzla::Option::REWRITE_LEVEL, (int)2);
    bitwuzla_options.set(bitwuzla::Option::PREPROCESS, (int)1);
    bitwuzla_options.set(bitwuzla::Option::ABSTRACTION, (int)1);
    bitwuzla_options.set(bw::Option::TIME_LIMIT_PER, 5000);
    bitwuzla = std::make_unique<bw::Bitwuzla>(term_manager, bitwuzla_options);

    engine = _engine;
    memory = std::make_shared<vexa::memory>(this);
    symex = std::make_shared<vexa::symex>(this);
    builder = std::make_shared<ir::builder>(this);

    // init cpu
    if (_arch == arch::x86_64)
        cpu = std::make_shared<vexa::amd64::Amd64Cpu>(this);
    else
        THROW("only x86_64 is supported");
}

void vexa::context::event_handler(vexa::event_kind event_k)
{
    auto found = event_callbacks.find(event_k);
    if (found != event_callbacks.end()) {
        found->second(*engine);
        return;
    }

    return;
}

void vexa::context::set_option(vexa::option opt, int val)
{
    VEXA_ASSERT(opt < option::COUNT);
    options[opt] = val;
}

int vexa::context::get_option(vexa::option opt)
{
    VEXA_ASSERT(opt < option::COUNT);
    return options.at(opt);
}