#include <vexa/vexa.h>

#include <vexa/passes/constant_propagation.hpp>
#include <vexa/passes/loop_reroll.hpp>
#include <vexa/passes/state2alloca.hpp>
#include <vexa/passes/state_cleanup.hpp>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <chrono>

#include <LIEF/Abstract/Header.hpp>
#include <LIEF/Abstract/Section.hpp>
#include <LIEF/Abstract/Symbol.hpp>
#include <LIEF/ELF/Section.hpp>
#include <LIEF/ELF/Segment.hpp>
#include <LIEF/Object.hpp>

#include <quill/std/Chrono.h>

vexa::engine::engine(vexa::arch arch) : _arch(arch)
{
    // init modules (cpu, memory, builder etc.)
    context = new vexa::context(std::shared_ptr<vexa::engine>(this), _arch);
    memory = context->memory;
    symex = context->symex;
    builder = context->builder;
    cpu = context->cpu;

    LOG_INFO(logger, "Initialized the engine");
}

void vexa::engine::run(uint64_t pc)
{
    LOG_INFO(logger, "Started symbolic execution and lifting");

    auto start = std::chrono::high_resolution_clock::now();
    cpu->run(pc);
    auto end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    ir.clear();
    llvm::raw_string_ostream rso(ir);
    cpu->vexa_lifted->print(rso, nullptr);
    rso.flush();

    instr_count = builder->instr_count();
    logger->flush_log();
}

void vexa::engine::optimize()
{
    vexa::ir::pass_manager manager(context);
    manager.add_pass<ir::simplify_cfg_and_dce>();

    if (get_option(option::LOOP_REROLL))
        manager.add_pass<passes::loop_reroll>();

    if (get_option(option::Z3_CONSTANT_PROPAGATION))
        manager.add_pass<passes::constant_propagation>();

    manager.add_pass<passes::state2alloca>();
    manager.add_pass<ir::Oz>();

    if (get_option(option::STATE_CLEANUP))
        manager.add_pass<passes::state_cleanup>();

    auto start = std::chrono::high_resolution_clock::now();
    manager.run(cpu->vexa_lifted);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    LOG_INFO(logger, "Optimized the function ({})", time);
    ir.clear();

    llvm::raw_string_ostream rso(ir);
    cpu->vexa_lifted->print(rso, nullptr);
    rso.flush();
}

void vexa::engine::print()
{
    logger->flush_log();
    const size_t asm_count = cpu->lifted_count;
    const size_t ir_before_opt = instr_count;
    const size_t ir_after_opt = builder->instr_count();

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
    return builder->recompile(_arch, cpu->vexa_lifted, optimize);
}

void vexa::engine::write_memory(uint64_t address, std::span<const uint8_t> buffer)
{
    for (unsigned int i = 0; i < buffer.size(); i++)
    {
        /*
        vexa::value* addr = symex->concrete(address + i, 64);
        vexa::pointer* ptr = symex->pointer(addr, cpu->global_memory);
        memory->write(ptr, symex->concrete(buffer[i], 8));
        */

        cpu->global_memory->concrete_memory[address + i] = context->z3_context->bv_val(buffer[i], 8);
    }
}

std::vector<uint8_t> vexa::engine::read_binary_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        THROW("Couldn't open {}", filename);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return buffer;
    } else {
        THROW("Couldn't read {}", filename);
    }
}

void vexa::engine::map_binary(vexa::binary& binary)
{
    if (binary.is_elf())
    {
        LIEF::ELF::Binary *elf_binary = binary.as_elf();

        if (!elf_binary)
            THROW("error while parsing elf");

        // map segments to the memory
        for (const LIEF::ELF::Segment &segment : elf_binary->segments())
        {
            uint64_t v_size = segment.virtual_size();
            uint64_t v_addr = segment.virtual_address();

            write_memory(v_addr, segment.content());
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

            if (content.empty())
                continue;

            uint64_t v_addr = pe_binary->optional_header().imagebase() + section.virtual_address();
            write_memory(v_addr, content);
        }
    }
    else if (binary.is_raw())
    {
        write_memory(0, binary.raw_data());
    }
    else
        THROW("Unknown binary type");
}

void vexa::engine::patch(vexa::binary& binary, std::vector<uint8_t> object_file, uint64_t va)
{
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
        if (s.name() == ".text")
        {
            section = &s;
            break;
        }
    }

    if (!section)
        THROW(".text section is not found in the object file");

    // read the section content
    auto _code_content = section->content();
    std::vector<uint8_t> code_content = std::vector<uint8_t>(_code_content.begin(), _code_content.end());

    // get vexa_entry symbol
    LIEF::Symbol* entry_symbol = object->get_symbol("vexa_entry");
    if (!entry_symbol)
        THROW("vexa_entry symbol is not found in the object file, make sure its compiled in vexa");

    if (binary.is_elf())
    {
        // ELF BINARIES
        //

        LIEF::ELF::Binary* elf = binary.as_elf();
        uint64_t image_base = elf->is_pie() ? 0 : elf->imagebase();
        // create the vexa segment
        LIEF::ELF::Segment vexa_segment{};
        vexa_segment.type(LIEF::ELF::Segment::TYPE::LOAD);
        vexa_segment.add(LIEF::ELF::Segment::FLAGS::X);
        vexa_segment.add(LIEF::ELF::Segment::FLAGS::R);
        vexa_segment.alignment(0x1000);
        vexa_segment.content(code_content);

        // relocate the program header table manually with PHDR_RELOC::PIE_SHIFT method                                              │
        // so we dont let lief to decide to relocate the phdr or not                                                                 │
        // this method shifts the whole binary with +0x1000                                                                          │
        // see https://github.com/lief-project/LIEF/blob/f986c6dd17b297cef88f8f1f8d1cdc987827e671/src/ELF/Binary.cpp#L2655
        // (only for PIE binaries)
        uint64_t shift_offset = 0;
        if (elf->is_pie())
        {
            elf->relocate_phdr_table(LIEF::ELF::Binary::PHDR_RELOC::PIE_SHIFT);
            shift_offset = 0x1000;
            va += shift_offset;
        }
        else
        {
            elf->relocate_phdr_table(LIEF::ELF::Binary::PHDR_RELOC::AUTO);
        }

        LIEF::ELF::Segment* new_segment = elf->add(vexa_segment);

        std::vector<uint8_t> relocated = fix_relocations(
                                             object_f, code_content, new_segment->virtual_address(), image_base, shift_offset);

        // update the binary with fixed relocations
        new_segment->content(relocated);

        // calculate the address to create a relative jump
        uint64_t new_func_va = new_segment->virtual_address() + entry_symbol->value();
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
        // PE BINARIES
        //

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
        std::vector<uint8_t> relocated = fix_relocations(
                                             object_f, code_content, image_base + new_section->virtual_address(), image_base);

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
}

std::vector<uint8_t> vexa::engine::fix_relocations(vexa::binary &object_file, std::vector<uint8_t> code_content,
        uint64_t new_section_rva, uint64_t image_base, uint64_t shift_offset)
{
    if (!object_file.is_elf())
        THROW("unsupported binary type");

    for (auto &reloc : object_file.as_elf()->relocations())
    {
        if (!reloc.has_symbol())
            continue;

        std::string &symbol_name = reloc.symbol()->name();
        if (symbol_name.empty())
            continue;

        uint64_t symbol_va = 0;
        if (symbol_name == "IMAGE_BASE")
        {
            symbol_va = image_base + shift_offset;
        }
        else
        {
            symbol_va = new_section_rva + reloc.symbol()->value();
        }

        uint64_t patch_offset = reloc.address();
        uint64_t patch_va = new_section_rva + patch_offset; // virtual address of patch location

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
            THROW("unsupported relocation type {}", std::to_string((uint64_t)reloc.type()));
        }
    }

    return code_content;
}

void vexa::engine::set_callback(vexa::event_kind kind, vexa::event_callback_t callback)
{
    context->event_callbacks[kind] = callback;
}

void vexa::engine::reset()
{
    context = new vexa::context(std::shared_ptr<vexa::engine>(this), _arch);
    memory = context->memory;
    symex = context->symex;
    builder = context->builder;
    cpu = context->cpu;
}

void vexa::engine::set_option(vexa::option opt, int v)
{
    context->set_option(opt, v);
}

int vexa::engine::get_option(vexa::option opt)
{
    return context->get_option(opt);
}

// FOR DEBUG
void vexa::engine::mark_symbolic(uint64_t address, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        auto addr = symex->concrete(address + i, 64);
        auto sym = symex->symbolic("input_" + std::to_string(address + i), 8);
        //memory->write(addr, sym);
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
