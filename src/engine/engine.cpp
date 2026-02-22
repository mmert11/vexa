#include <vexa/vexa.h>
#include <chrono>

vexa::engine::engine(vexa::arch arch) : _arch(arch)
{
    TRY()
    // init classes
    context = std::make_shared<vexa::context>();
    memory = std::make_shared<vexa::memory>(context);
    symex = std::make_shared<vexa::symex>(context);
    builder = std::make_shared<ir::builder>(context, symex, memory);

    // init cpu
    if (arch == arch::x86_64)
        cpu = std::make_shared<vexa::x64::cpu64>(builder, symex, memory, context);
    else
        THROW("only x86_64 is supported");

    CATCH()
}

void vexa::engine::run()
{
    auto start = std::chrono::high_resolution_clock::now();
    cpu->run();
    auto end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    ir.clear();
    llvm::raw_string_ostream rso(ir);
    context->llvm_module->print(rso, nullptr);
    rso.flush();

    instr_count = builder->get_instr_count();
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
    const size_t asm_count      = cpu->lifted_count;
    const size_t ir_before_opt  = instr_count;
    const size_t ir_after_opt   = builder->get_instr_count();

    double expansion =
        asm_count ? double(ir_before_opt) / double(asm_count) : 0.0;

    double reduction =
        ir_before_opt ? 100.0 * (1.0 - double(ir_after_opt) / double(ir_before_opt)) : 0.0;

    std::cout << "\n────────────────────────────────────────\n\n";
    std::cout << ir << "\n";

    std::cout << "lifted          : " << std::dec << asm_count
              << " asm-insts in " << time << "\n";

    std::cout << "ir generated    : " << ir_before_opt
              << " ir-insts (×" << std::fixed << std::setprecision(2)
              << expansion << " expansion)\n";

    std::cout << "optimized into  : " << ir_after_opt
              << " ir-insts (−" << std::fixed << std::setprecision(2)
              << reduction << "%)\n";

    std::cout << "\n────────────────────────────────────────\n";
    std::cout << "VEXA · LLVM IR Lifter / Deobfuscator © github.com/mmert11\n\n";
}


void vexa::engine::write_memory(uint64_t address, std::vector<uint8_t> buffer)
{
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        auto addr = symex->concrete(address + i, 64);
        memory->write(addr, symex->concrete(buffer[i], 8));
    }
}

std::shared_ptr<vexa::context> vexa::engine::get_context()
{
    return context;
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
