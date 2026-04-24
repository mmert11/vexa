#include <LIEF/Abstract/Header.hpp>
#include <LIEF/Abstract/Section.hpp>
#include <LIEF/Abstract/Symbol.hpp>
#include <LIEF/ELF/Section.hpp>
#include <LIEF/ELF/Segment.hpp>
#include <LIEF/Object.hpp>
#include <vexa/vexa.h>
#include <chrono>

#include <llvm/Support/FileSystem.h>

vexa::engine::engine(vexa::arch arch) : _arch(arch)
{
    TRY()
    // init classes
    context = new vexa::context(_arch);
    memory = context->memory;
    symex = context->symex;
    builder = context->builder;
    cpu = context->cpu;

#ifdef _WIN32
    LSiMBA::PythonPath = "py";
#else
    LSiMBA::PythonPath = "python3";
#endif

    CATCH()
}

void writeIRToFile(llvm::Module *module, const std::string &filename)
{
    std::error_code EC;
    llvm::raw_fd_ostream outFile(filename, EC, llvm::sys::fs::OF_None);

    if (EC)
        return;

    module->print(outFile, nullptr);
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
    writeIRToFile(context->llvm_module.get(), "no_opt.ll");
}

#define SIMPLIFY_MBA 0
void vexa::engine::optimize()
{
    vexa::ir::pass_manager manager(context);
    manager.add_pass<ir::simplify_cfg_and_dce>();
    manager.add_pass<ir::mem2reg_and_sroa>();
    manager.add_pass<passes::loop_reroll>();
    manager.add_pass<passes::constant_propagation>();
    manager.add_pass<passes::strength_recovery::analysis>();
    manager.add_pass<ir::Oz>();
    manager.add_pass<passes::strength_recovery>();

    auto start = std::chrono::high_resolution_clock::now();
    manager.run();
    auto end = std::chrono::high_resolution_clock::now();
    auto _time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[optimize] optimized the function with -Oz " << "(" << _time << ")" << std::endl;

    ir.clear();

#if SIMPLIFY_MBA
    std::cout << "[SiMBA++] running SiMBA for mba simplifying" << std::endl;

    LSiMBA::UseExternalSimplifier = "/home/mert/Masaüstü/GAMBA/src/simplify_general.py";
    LSiMBA::ShouldWalkSubAST = false;
    LSiMBA::LLVMParser Parser(
        context->llvm_module.get(),
        true,  // 2. RunParallel
        true,  // 3. UseFastCheck
        true, // 4. IsExternal
        true,  // 5. DetectSimplify
        false, // 6. UseCaching
        false  // 7. Prove
    );

    int replaced = Parser.simplify();
    std::cout << "[SiMBA++] simplified " << replaced << " MBA expressions" << std::endl;
#endif

    llvm::raw_string_ostream rso(ir);
    context->llvm_module->print(rso, nullptr);
    rso.flush();
}

void vexa::engine::print_ir()
{
    writeIRToFile(context->llvm_module.get(), "output.ll");

    const size_t asm_count = cpu->p_manager.lifted_count;
    const size_t ir_before_opt = instr_count;
    const size_t ir_after_opt = builder->get_instr_count();

    double expansion =
        asm_count ? double(ir_before_opt) / double(asm_count) : 0.0;

    double reduction =
        ir_before_opt ? 100.0 * (1.0 - double(ir_after_opt) / double(ir_before_opt)) : 0.0;

    std::cout << "\n────────────────────────────────────────\n\n";
    std::cout << ir << "\n";

    std::cout << "lifted          : " << std::dec << asm_count
              << " insts in " << time << "\n";

    std::cout << "ir generated    : " << ir_before_opt
              << " ir-insts (×" << std::fixed << std::setprecision(2)
              << expansion << " expansion)\n";

    std::cout << "optimized into  : " << ir_after_opt
              << " ir-insts (−" << std::fixed << std::setprecision(2)
              << reduction << "%)\n";

    std::cout << "\n────────────────────────────────────────\n";
    std::cout << "VEXA · LLVM IR Lifter / Deobfuscator © github.com/mmert11\n\n";
}

std::vector<uint8_t> vexa::engine::recompile(bool optimize)
{
    return builder->recompile(_arch, builder->get_function(), optimize);
}

void vexa::engine::write_memory(uint64_t address, std::vector<uint8_t> buffer)
{
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        auto addr = symex->concrete(address + i, 64);
        memory->write(addr, symex->concrete(buffer[i], 8));
    }
}

void vexa::engine::map_binary(vexa::binary& binary)
{
    TRY()

    if (binary.is_elf())
    {
        LIEF::ELF::Binary *elf_binary = binary.as_elf();

        if (!elf_binary)
            THROW("error while parsing elf");

        // map segments to the memory
        for (const LIEF::ELF::Segment &segment : elf_binary->segments())
        {
            if (segment.type() != LIEF::ELF::Segment::TYPE::LOAD)
                continue;

            uint64_t v_size = segment.virtual_size();
            uint64_t v_addr = segment.virtual_address();

            auto content = segment.content();
            std::vector<uint8_t> data(content.begin(), content.end());
            write_memory(v_addr, data);
        }
    }
    else if (binary.is_pe())
    {
        LIEF::PE::Binary* pe_binary = binary.as_pe();

        if (!pe_binary)
            THROW("error while parsing elf");

        // map sections to the memory
        for (const LIEF::PE::Section &section : pe_binary->sections())
        {
            auto content = section.content();
            std::vector<uint8_t> data(content.begin(), content.end());

            if (data.empty())
                continue;

            uint64_t v_addr = pe_binary->optional_header().imagebase() + section.virtual_address();
            write_memory(v_addr, data);
        }

        // write image base to PEB+0x10 in memory
        z3::expr teb = *cpu->registers[x64::GS].expression + symex->concrete(0x60, 64);
        z3::expr peb = memory->read(teb, 64);
        z3::expr image_base = peb + symex->concrete(0x10, 64);
        memory->write(image_base, symex->concrete(pe_binary->imagebase(), 64));
    }
    else
        THROW("unknown binary type");

    CATCH()
}

void vexa::engine::patch(vexa::binary& binary, std::vector<uint8_t> object_file, uint64_t va, std::string section_name)
{
    TRY()
    // parse the object file
    vexa::binary object_f(object_file);
    LIEF::Binary* object = object_f.lief();

    // is this an actual object file?
    if (object->header().object_type() != LIEF::Header::OBJECT_TYPES::OBJECT)
        THROW("error while parsing object file");

    // get the section
    LIEF::Section* section = nullptr;
    for (auto &s : object->sections())
    {
        if (s.name() == section_name)
        {
            section = &s;
            break;
        }
    }

    if (!section)
        THROW("section is not found in the object file: " + section_name);

    // read the section content
    auto _code_content = section->content();
    std::vector<uint8_t> code_content = std::vector<uint8_t>(_code_content.begin(), _code_content.end());

    // get vexa_entry symbol
    LIEF::Symbol* entry_symbol = object->get_symbol("vexa_entry");
    if (!entry_symbol)
        THROW("vexa_entry symbol is not found in the object file, make sure its compiled in vexa");

    if (binary.is_elf())
    {
        LIEF::ELF::Binary* elf = binary.as_elf();
        uint64_t image_base = elf->is_pie() ? 0 : elf->imagebase();
        // create the vexa segment
        LIEF::ELF::Segment vexa_segment{};
        vexa_segment.type(LIEF::ELF::Segment::TYPE::LOAD);
        vexa_segment.add(LIEF::ELF::Segment::FLAGS::X);
        vexa_segment.add(LIEF::ELF::Segment::FLAGS::R);
        vexa_segment.alignment(0x1000);
        vexa_segment.content(code_content);

        // relocate the program header table manually with PHDR_RELOC::PIE_SHIFT method
        // so we dont let lief to decide to relocate the phdr or not
        // this method shifts the whole binary with + 0x1000
        // see https://github.com/lief-project/LIEF/blob/f986c6dd17b297cef88f8f1f8d1cdc987827e671/src/ELF/Binary.cpp#L2655
        elf->relocate_phdr_table(LIEF::ELF::Binary::PHDR_RELOC::PIE_SHIFT);
        LIEF::ELF::Segment* new_segment = elf->add(vexa_segment);
        va += 0x1000;

        std::vector<uint8_t> relocated = fix_relocations(object_f, code_content, new_segment->virtual_address() - 0x1000, image_base);
        // update the binary with fixed relocations
        new_segment->content(relocated);

        // calculate the address to create a relative jump
        uint64_t new_func_va = image_base + new_segment->virtual_address() + entry_symbol->value();
        int32_t calc_relative = static_cast<int32_t>(new_func_va - (va + 5));

        std::vector<uint8_t> jmp_bytes = {0xE9};
        jmp_bytes.push_back(static_cast<uint8_t>(calc_relative & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 8) & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 16) & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 24) & 0xFF));

        // patch the target address
        elf->patch_address(va, jmp_bytes);
    }
    else if (binary.is_pe())
    {
        LIEF::PE::Binary* pe = binary.as_pe();
        uint64_t image_base = pe->optional_header().imagebase();

        // create the vexa section
        LIEF::PE::Section vexa_section{".vexa", code_content};
        vexa_section.characteristics(
            static_cast<uint32_t>(LIEF::PE::Section::CHARACTERISTICS::CNT_CODE) |
            static_cast<uint32_t>(LIEF::PE::Section::CHARACTERISTICS::MEM_EXECUTE) |
            static_cast<uint32_t>(LIEF::PE::Section::CHARACTERISTICS::MEM_READ)
        );
        LIEF::PE::Section* new_section = pe->add_section(vexa_section); // PE::virtual_address() actually returns RVA, not VA
        std::vector<uint8_t> relocated = fix_relocations(object_f, code_content, new_section->virtual_address(), image_base);

        // update the binary with fixed relocations
        new_section->content(relocated);

        // calculate the address to create a relative jump
        uint64_t new_func_va = image_base + new_section->virtual_address() + entry_symbol->value();
        int32_t calc_relative = static_cast<int32_t>(new_func_va - (va + 5));

        std::vector<uint8_t> jmp_bytes = {0xE9};
        jmp_bytes.push_back(static_cast<uint8_t>(calc_relative & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 8) & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 16) & 0xFF));
        jmp_bytes.push_back(static_cast<uint8_t>((calc_relative >> 24) & 0xFF));

        // patch the target address
        pe->patch_address(va, jmp_bytes);
    }
    else
        THROW("binary type is not supported");
    CATCH()
}

std::vector<uint8_t> vexa::engine::fix_relocations(vexa::binary &object_file, std::vector<uint8_t> code_content, uint64_t new_section_rva, uint64_t image_base)
{
    TRY()
    if (!object_file.is_elf())
        THROW("unsupported binary type");

    for (auto &reloc : object_file.as_elf()->relocations())
    {
        std::string &symbol_name = reloc.symbol()->name();
        if (reloc.symbol() == nullptr || symbol_name == "") continue;

        // calculate the virtual address of symbol
        // global variable names are formatted as var_1234h by builder::recompile
        uint64_t symbol_va = 0;
        if (symbol_name.substr(0, 4) == "var_") // if it starts with "var_", its a global variable
            symbol_va = std::stoull(symbol_name.substr(4), nullptr, 16); // extract the va from string
        else // else, its a local function call (not supported in recompiler yet)
            symbol_va = image_base + new_section_rva + reloc.symbol()->value();

        uint64_t patch_offset = reloc.address();
        uint64_t patch_va = image_base + new_section_rva + patch_offset; // virtual address of patch location

        switch (reloc.type())
        {
        case LIEF::ELF::Relocation::TYPE::X86_64_PC32:
        case LIEF::ELF::Relocation::TYPE::X86_64_PLT32:
        {
            // relative addressing
            int32_t relative_value = static_cast<int32_t>(symbol_va + reloc.addend() - patch_va);
            memcpy(&code_content[patch_offset], &relative_value, 4);
            break;
        }
        case LIEF::ELF::Relocation::TYPE::X86_64_64:
        {
            // absolute 64
            uint64_t absolute_va = symbol_va + reloc.addend();
            memcpy(&code_content[patch_offset], &absolute_va, 8);
            break;
        }
        case LIEF::ELF::Relocation::TYPE::X86_64_32:
        {
            // absolute 32
            uint32_t absolute_va_32 = static_cast<uint32_t>(symbol_va + reloc.addend());
            memcpy(&code_content[patch_offset], &absolute_va_32, 4);
            break;
        }
        default:
            THROW("unsupported relocation type " + std::to_string((uint64_t)reloc.type()));
        }
    }

    return code_content;
    CATCH()
}

void vexa::engine::mark_symbolic(uint64_t address, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        auto addr = symex->concrete(address + i, 64);
        auto sym = symex->symbolic("input_" + std::to_string(address + i), 8);
        memory->write(addr, sym);
    }
}

vexa::context* vexa::engine::get_context()
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
