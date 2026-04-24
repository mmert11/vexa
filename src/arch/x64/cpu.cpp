#include <vexa/vexa.h>

#define INIT_REGISTER(REG, INIT_VALUE) \
        {vexa::value alloca = builder->alloca(i64_t, getRegisterStr(REG) + "_alloca"); \
        registers[REG].pointer = alloca; \
        registers[REG].expression = std::make_shared<z3::expr>(INIT_VALUE.as_expr()); \
        write_register(REG, INIT_VALUE);}

vexa::x64::cpu64::cpu64(vexa::context* _context) : cpu(_context)
{
    TRY()
    const std::map<reg_t, int> llvm_register_table = {
        {x64::RAX, 0},
        {x64::RBX, 1},
        {x64::RCX, 2},
        {x64::RDX, 3},
        {x64::RSI, 4},
        {x64::RDI, 5},
        {x64::RBP, 6},
        {x64::RSP, 7},
        {x64::R8, 8},
        {x64::R9, 9},
        {x64::R10, 10},
        {x64::R11, 11},
        {x64::R12, 12},
        {x64::R13, 13},
        {x64::R14, 14},
        {x64::R15, 15},
        {x64::RFLAGS, 16}
    };

    llvm::Type *i64_t = builder->get_int_ty(64);

    // define general purpose registers as int64_t arguments
    std::vector<llvm::Type *> args;
    for (unsigned int i = 0; i < llvm_register_table.size(); i++)
        args.push_back(i64_t);

    // create the function
    llvm::Function *function = builder->create_function("vexa_lifted", args);
    builder->set_function(function);

    // create the entry block
    llvm::BasicBlock *entry_bb = builder->basic_block("entry");
    builder->set_ip(entry_bb);

    // rename the arguments (rax, rbx, rflags etc.)
    for (auto &reg : llvm_register_table)
    {
        // get the register from function arguments
        vexa::value var = builder->argument(reg.second, getRegisterStr(reg.first));
        // initialize register
        INIT_REGISTER(reg.first, var);
    }

    // also init the rip because we didnt get the rip from function parameters
    vexa::value rip = builder->get_const_int(0, 64);
    INIT_REGISTER(x64::RIP, rip);

    // initialize gs segment, address space for gs segment is 256
    llvm::Type *gs_ptr_type = llvm::PointerType::get(i64_t, 256);
    vexa::value gs = builder->load(i64_t, builder->inttoptr(builder->get_const_int(0, 64), "", gs_ptr_type), "gs");
    symex->set(gs.as_llvm(), symex->symbolic("gs", 64));
    INIT_REGISTER(x64::GS, gs);

    // concretize stack pointer
    write_register(vexa::x64::RSP, builder->get_const_int(0, 64));
    // initialize stack
    vexa::value stack = builder->alloca(builder->get_int_ty(8), builder->get_const_int(0, 64).as_expr(), "stack", 8192);
    stack_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack, builder->get_const_int(4096, 64), "stack_ptr");
    original_sp = read_register(x64::RSP);

    // push zero to the stack, this is as our return flag
    // (look at the ret handler semantics)
    vexa::value null_ret = builder->get_const_int(0, 64);
    push64(null_ret);

    init_handlers();

    CATCH()
}

#define VCFG_RECOVERAGE 0
#define VIP() registers[x64::R8].expression->as_uint64()

void aaa(z3::expr e)
{
    z3::solver s(e.ctx());
    s.add(e == 1);
    if (s.check() == z3::sat)
    {
        std::cout << "FOUND MODEL ---------" << std::endl;
        std::cout << s.get_model() << std::endl;
    }
    else
    {
        std::cout << "z3 basaramadi.." << std::endl;
    }
}

void vexa::x64::cpu64::run()
{
    TRY()

    while (true)
    {
        z3::expr rip = *registers[x64::RIP].expression;
        if (!rip.is_numeral())
        {
            std::cout << rip << std::endl;
            THROW("rip is symbolic!");
        }

        uint64_t rip_uint64 = rip.as_uint64();

        // means we had an internal exception, stop lifting and break
        if (rip_uint64 == (uint64_t)-1)
            THROW("internal exception occured");

        std::vector<uint8_t> read_bytes;
        if (rip_uint64 == 0)
            goto ret;

#if VCFG_RECOVERAGE
        if (lifted_blocks.count(current_vip) && vbranching)
        {
            builder->jump(lifted_blocks[current_vip]);
            goto explore_other_paths;
        }
#else
        // create a loop
        if (p_manager.lifted_blocks.count(rip_uint64))
        {
            std::cout << "created a loop" << std::endl;
            builder->jump(p_manager.lifted_blocks[rip_uint64]);
            goto explore_other_paths;
        }
#endif

        // read 15 bytes at rip
        for (int i = 0; i < 15; i++)
        {
            auto byte = memory->read(symex->concrete(rip_uint64 + i, 64), 8);
            if (byte.is_numeral())
                read_bytes.push_back(static_cast<uint8_t>(byte.as_uint64()));
            else
                break;
        }

        ZydisDisassembledInstruction instruction;
        if (!disassemble(read_bytes, rip_uint64, instruction))
        {
            std::cout << "[error] disassemble fail" << std::endl;
            builder->unreachable();
            goto explore_other_paths;
        }

        std::cout << std::hex << instruction.runtime_address << " " << instruction.text << std::endl;
        lift(instruction);
        continue;

ret:
        std::cout << "[engine] function ended" << std::endl;
        // std::cout << read_register(x64::RAX).as_expr().simplify() << std::endl;
        builder->ret(read_register(x64::RAX));
        //aaa(_read_register(x64::RAX));

explore_other_paths:

        if (p_manager.unexplored_paths.empty())
        {
            std::cout << "[engine] no more paths to explore\n"
                      << std::endl;
            break;
        }

        std::cout << "[engine] exploring new path" << std::endl;

        path_state path = p_manager.unexplored_paths.top();
        p_manager.unexplored_paths.pop();

        builder->set_ip(path.bb);
        restore_snapshot(path.ss);
        write_register(x64::RIP, path.rip);
    }

    CATCH()
}

void vexa::x64::cpu64::lift(ZydisDisassembledInstruction instruction)
{
    TRY()
    auto handler = handlers.find(instruction.info.mnemonic);
    if (handler == handlers.end())
        THROW(std::string("unimplemented handler: ") + ZydisMnemonicGetString(instruction.info.mnemonic));

    llvm::BasicBlock *basic_block = builder->basic_block(utils::addr_to_str(instruction.runtime_address));
    builder->jump(basic_block);
    builder->set_ip(basic_block);

#if VCFG_RECOVERAGE
    if (vbranching)
    {
        lifted_blocks[VIP()] = basic_block;
        lifted_count++;
    }
#else
    p_manager.lifted_blocks[instruction.runtime_address] = basic_block;
    p_manager.lifted_count++;
#endif

    vexa::value new_ip = handler->second(instruction);
    write_register(x64::RIP, new_ip);

#if VCFG_RECOVERAGE
    if (vbranching)
    {
        uint64_t read_vip = VIP();
        if (read_vip != current_vip)
        {
            if (!is_conditional)
            {
                if (lifted_blocks.count(read_vip))
                    lifted_blocks.erase_range(read_vip, current_vip);
            }
            current_vip = read_vip;
        }
        else
            vbranching = false;
    }
#endif

    return;
    CATCH()
}

void vexa::x64::cpu64::write_register(vexa::reg_t reg, vexa::value value)
{
    TRY()
    // gather register info and value
    register_desc r_info = REG_INFO(reg);
    vexa::register_t register_ = registers[r_info.base_id];
    vexa::value alloca_ptr = register_.pointer;
    vexa::value r64_vl = builder->_load(builder->get_int_ty(64), alloca_ptr, *register_.expression, getRegisterStr(reg));

    // resize the value's bitwidth to match the target register's bitwidth
    value = builder->resize(value, r_info.size_bits);
    vexa::value final_v;

    // direct write for 64-bit registers
    if (r_info.size_bits == 64)
        final_v = value;
    else if (r_info.size_bits == 32)
    {
        // resize and reset upper 32 bits
        final_v = builder->resize(builder->resize(value, 32), 64);
    }
    else
    {
        // create masks for bit operations
        const uint64_t size_mask = (1ULL << r_info.size_bits) - 1;
        const uint64_t clear_mask = ~(size_mask << r_info.offset_bits);

        // reset the bits we will overwrite
        vexa::value masked_r =
            builder->band(r64_vl, builder->get_const_int(clear_mask, 64), getRegisterStr(reg));

        // truncate and extend the value to extract the necessary bits
        vexa::value masked_v =
            builder->resize(builder->resize(value, r_info.size_bits), 64);

        // reposition the value
        vexa::value shifted =
            builder->bshl(masked_v, builder->get_const_int(r_info.offset_bits, 64),
                          "shifted_" + value.as_llvm()->getName().str());

        // insert the value into register
        final_v = builder->bor(masked_r, shifted, getRegisterStr(r_info.base_id));
    }

    builder->_store(final_v, alloca_ptr);
    registers[r_info.base_id].expression = std::make_shared<z3::expr>(final_v.as_expr());
    CATCH()
}

// a register write function for internal usage
void vexa::x64::cpu64::_write_register(vexa::reg_t reg, z3::expr expression)
{
    vexa::register_desc r_info = REG_INFO(reg);
    if (r_info.size_bits == 64)
    {
        registers[reg].expression = std::make_shared<z3::expr>(expression);
        return;
    }

    // handle non 64-bit values
    THROW("unimplemented function");
}

// a register read function for internal usage
z3::expr vexa::x64::cpu64::_read_register(vexa::reg_t reg)
{
    vexa::register_desc r_info = REG_INFO(reg);
    if (r_info.size_bits == 64)
        return *registers[reg].expression;

    // handle non 64-bit values
    THROW("unimplemented function");
}

vexa::value vexa::x64::cpu64::read_register(vexa::reg_t reg)
{
    TRY()

    // gather register info and value
    register_desc r_info = REG_INFO(reg);
    vexa::register_t register_ = registers[r_info.base_id];
    vexa::value alloca_ptr = register_.pointer;
    vexa::value r64_vl = builder->_load(builder->get_int_ty(64), alloca_ptr, *register_.expression, getRegisterStr(reg));

    vexa::value ret;
    if (r_info.size_bits == 64)
        ret = r64_vl;
    else if (r_info.offset_bits == 0)
        ret = builder->resize(r64_vl, r_info.size_bits);
    else
    {
        const uint64_t mask = (1ULL << r_info.size_bits) - 1;

        vexa::value shifted_r =
            builder->bshr(
                r64_vl,
                builder->get_const_int(r_info.offset_bits, 64),
                getRegisterStr(reg));

        vexa::value masked_r =
            builder->band(
                shifted_r,
                builder->get_const_int(mask, 64),
                "masked_" + getRegisterStr(reg));

        ret = masked_r;
    }

    return ret;

    CATCH()
}

vexa::value vexa::x64::cpu64::resolve_imm_address(ZydisDisassembledInstruction instruction, uint8_t operand_idx)
{
    ZyanU64 final;
    if (instruction.operands[operand_idx].imm.is_relative)
        ZydisCalcAbsoluteAddress(&instruction.info, &instruction.operands[operand_idx], instruction.runtime_address, &final);
    else
        final = instruction.operands[operand_idx].imm.value.u;
    return builder->get_const_int(final, 64);
}

vexa::value vexa::x64::cpu64::resolve_mem_address(ZydisDisassembledInstruction instruction, ZydisDecodedOperand operand)
{
    TRY()

    vexa::value res = builder->get_const_int(0, 64);

    // segment
    if (operand.mem.segment != ZYDIS_REGISTER_NONE && operand.mem.segment != ZYDIS_REGISTER_SS && operand.mem.segment != ZYDIS_REGISTER_DS)
    {
        if (operand.mem.segment == ZYDIS_REGISTER_GS)
            res = read_register(x64::GS);
        else
        {
            THROW("unsupported segment access " + std::to_string(operand.mem.segment));
        }
    }

    // displacement
    res = builder->add(res, builder->get_const_int(operand.mem.disp.value, 64), "displacement");

    // base
    if (operand.mem.base != ZYDIS_REGISTER_NONE)
    {
        // relative address resolving
        if (operand.mem.base == ZYDIS_REGISTER_RIP)
        {
            uint64_t next_rip = instruction.runtime_address + instruction.info.length;
            res = builder->add(res, builder->get_const_int(next_rip, 64), "rip_base");
        }
        else
        {
            // translate zydis register to vexa register
            reg_t reg = zydis_register_table.at(operand.mem.base);
            vexa::value reg_v = read_register(reg);
            res = builder->add(res, reg_v, getRegisterStr(reg));
        }
    }

    // index
    if (operand.mem.index != ZYDIS_REGISTER_NONE)
    {
        reg_t reg = zydis_register_table.at(operand.mem.index);
        vexa::value index_v = read_register(reg);

        // scale
        if (operand.mem.scale > 0)
        {
            vexa::value scale_v = builder->get_const_int(operand.mem.scale, 64);
            index_v = builder->mul(index_v, scale_v, "mem_scale");
        }

        res = builder->add(res, index_v, "mem_index");
    }

    return res;
    CATCH()
}

bool vexa::x64::cpu64::is_stack_access(vexa::value addr)
{
    TRY()
    z3::expr addr_expr = addr.as_expr();
    z3::expr sp_expr = original_sp.as_expr();
    int64_t addr_int64 = addr.is_concrete() ? static_cast<int64_t>(addr.as_uint64()) : 0;

    if (addr.is_concrete() && addr_int64 <= 0)
        return true;

    return false;

    z3::context &ctx = addr.as_expr().ctx();
    z3::solver s(ctx);

    // use solver to prove it if its a stack access or not
    s.add(z3::sgt(addr.as_expr(), original_sp.as_expr() + 32));
    return s.check() == z3::unsat;
    CATCH()
}

bool vexa::x64::disassemble(std::vector<uint8_t> data, uint64_t &address, ZydisDisassembledInstruction &instruction)
{
    TRY()
    return ZYAN_SUCCESS(ZydisDisassembleIntel(
                            ZYDIS_MACHINE_MODE_LONG_64,
                            address,
                            data.data(),
                            data.size(),
                            &instruction));
    CATCH()
}

std::string vexa::x64::getRegisterStr(vexa::reg_t reg)
{
    return reg_to_str.at(reg);
}
