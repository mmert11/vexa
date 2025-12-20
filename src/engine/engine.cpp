#include <vexa/vexa.h>

vexa::engine::engine(vexa::arch arch) : _arch(arch)
{
    TRY()
    // init classes
    context = std::make_shared<vexa::context>();
    memory = std::make_shared<vexa::memory>(context);
    symex = std::make_shared<vexa::symex>(context);
    builder = std::make_shared<ir::builder>(context, symex);

    // init cpu
    if (arch == arch::x64)
        cpu = std::make_shared<vexa::x64::cpu64>(builder, symex, memory, context);
    else
        THROW("only x64 is supported");

    CATCH()
}

void vexa::engine::run()
{
    cpu->run();
    ir.clear();

    llvm::raw_string_ostream rso(ir);
    context->llvm_module->print(rso, nullptr);
    rso.flush();
}

void vexa::engine::optimize()
{
    builder->optimize();
    ir.clear();
    
    llvm::raw_string_ostream rso(ir);
    context->llvm_module->print(rso, nullptr);
    rso.flush();
}

void vexa::engine::print_ir()
{
    std::cout << ir << std::endl;
}

void vexa::engine::write_memory(uint64_t address, std::vector<uint8_t> buffer)
{
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        auto addr = symex->concrete(address + i, 64);
        memory->write(addr, symex->concrete(buffer[i], 8));
    }
}

std::shared_ptr<vexa::ir::builder> vexa::engine::get_builder()
{
    return builder;
}

std::shared_ptr<vexa::cpu> vexa::engine::get_cpu()
{
    return cpu;
}

std::shared_ptr<vexa::memory> vexa::engine::get_memory()
{
    return memory;
}

std::shared_ptr<vexa::symex> vexa::engine::get_symex()
{
    return symex;
}
