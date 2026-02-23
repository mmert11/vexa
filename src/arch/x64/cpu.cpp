#include <vexa/vexa.h>
// #pragma GCC optimize ("O0")

vexa::x64::cpu64::cpu64(
    std::shared_ptr<ir::builder> _builder,
    std::shared_ptr<vexa::symex> _symex,
    std::shared_ptr<vexa::memory> _memory,
    std::shared_ptr<vexa::context> _context) : cpu(_builder, _symex, _memory, _context)
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

    // rename the registers (rax, rbx, rflags etc.)
    for (auto &reg : llvm_register_table)
    {
        vexa::value var = builder->symvar(reg.second, getRegisterStr(reg.first));
        registers[reg.first] = var;
    }

    // address space for gs segment is 256
    llvm::Type* gs_ptr_type = llvm::PointerType::get(i64_t, 256);
    registers[GS] = builder->load(i64_t, builder->inttoptr(builder->get_const_int(0, 64), "", gs_ptr_type), "gs");
    symex->set(registers[GS].as_llvm(), symex->symbolic("gs", 64));

    write_register(vexa::x64::RSP, builder->get_const_int(0, 64));

    // initialize stack
    vexa::value stack = builder->alloca(builder->get_int_ty(8), 4096, registers[x64::RSP].as_expr(), "stack");
    stack_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack, builder->get_const_int(2048, 64), "stack_ptr");
    original_sp = registers[x64::RSP];

    init_handlers();

    CATCH()
}

void vexa::x64::cpu64::run()
{
    TRY()
    
    while (true)
    {
        vexa::value rip = read_register(x64::RIP);
        if (rip.is_symbolic())
            THROW("rip is symbolic!");

        std::vector<uint8_t> read_bytes;
        uint64_t address = rip.as_uint64();
        if (lifted_blocks.count(rip.as_uint64()))
        {
            //goto explore_other_paths;
        }

        for (int i = 0; i < 15; i++)
        {
            auto byte = memory->read(symex->concrete(rip.as_uint64() + i, 64), 8);
            if (byte.is_numeral())
                read_bytes.push_back(static_cast<uint8_t>(byte.as_uint64()));
            else break;
        }

        ZydisDisassembledInstruction instruction;

        if (!disassemble(read_bytes, address, instruction))
        {
            std::cout << "[!] disassemble fail" << std::endl;
            goto explore_other_paths;
        }

        std::cout << std::hex << instruction.runtime_address << " " << instruction.text << std::endl;
        lift(instruction);
        continue;

    explore_other_paths:

        if (unexplored_paths.empty())
        {
            std::cout << "[!] no more paths to explore" << std::endl;
            break;
        }
        
        std::cout << "[!] exploring new path" << std::endl;

        path_state path = unexplored_paths.top();
        unexplored_paths.pop();

        restore_snapshot(path.ss);
        write_register(x64::RIP, path.rip);
        builder->set_ip(path.bb);
    }

    CATCH()
}

vexa::value vexa::x64::cpu64::lift(ZydisDisassembledInstruction instruction)
{
    TRY()
    auto handler = handlers.find(instruction.info.mnemonic);
    if (handler == handlers.end())
        THROW(std::string("unimplemented handler: ") + ZydisMnemonicGetString(instruction.info.mnemonic));

    llvm::BasicBlock *basic_block = builder->basic_block(utils::addr_to_str(instruction.runtime_address));

    builder->jump(basic_block);
    builder->set_ip(basic_block);

    vexa::value new_ip = handler->second(instruction);
    write_register(x64::RIP, new_ip);

    lifted_blocks[instruction.runtime_address] = basic_block;
    lifted_count++;
    return new_ip;
    CATCH()
}

void vexa::x64::cpu64::write_register(vexa::reg_t reg, vexa::value value)
{
    TRY()
    register_desc r_info = REG_INFO(reg);
    vexa::value r64_vl = read_register(r_info.base_id);
    std::string r_str = getRegisterStr(reg);

    // resize the value's bitwidth to match the target register's bitwidth
    value = builder->resize(value, r_info.size_bits);

    // direct write for 64-bit registers
    if (r_info.size_bits == 64)
    {
        registers[r_info.base_id] = value;
        //builder->store(value, registers[r_info.base_id]);
        return;
    }

    vexa::value final_v;
    if (r_info.size_bits == 32)
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
            builder->band(r64_vl, builder->get_const_int(clear_mask, 64), r_str);

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

    registers[r_info.base_id] = final_v;
    //builder->store(final_v, registers[r_info.base_id]);
    CATCH()
}

vexa::value vexa::x64::cpu64::read_register(vexa::reg_t reg)
{
    TRY()

    register_desc r_info = REG_INFO(reg);
    vexa::value r64_vl = registers[r_info.base_id];
    //vexa::value r64_vl = builder->load(builder->get_int_ty(64), registers[r_info.base_id], getRegisterStr(reg)); //registers[r_info.base_id];

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

vexa::value vexa::x64::cpu64::resolve_imm_address(ZydisDisassembledInstruction instruction)
{
    ZyanU64 final;
    if (instruction.operands[0].imm.is_relative)
        ZydisCalcAbsoluteAddress(&instruction.info, &instruction.operands[0], read_register(x64::RIP).as_uint64(), &final);
    else
        final = instruction.operands[0].imm.value.u;
    return builder->get_const_int(final, 64);
}

vexa::value vexa::x64::cpu64::resolve_mem_address(ZydisDecodedOperand operand)
{
    TRY()

    vexa::value res = builder->get_const_int(0, 64);

    // segment
    if (operand.mem.segment != ZYDIS_REGISTER_NONE)
    {
        if (operand.mem.segment == ZYDIS_REGISTER_GS)
            res = registers[GS];
    }

    // displacement
    res = builder->add(res, builder->get_const_int(operand.mem.disp.value, 64), "displacement");
    
    // base
    if (operand.mem.base != ZYDIS_REGISTER_NONE)
    {
        // translate zydis register to vexa register
        reg_t reg = zydis_register_table.at(operand.mem.base);
        vexa::value reg_v = read_register(reg);
        res = builder->add(res, reg_v, getRegisterStr(reg));
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

    /*
    if (addr_expr.is_numeral())
        return false;
        */

    // if addr == sp
    if (z3::eq(addr_expr, sp_expr))
        return true;

    // heuristic detection
    if (addr_expr.is_app() && addr_expr.args().size() == 2 && addr_expr.decl().decl_kind() == Z3_OP_BADD)
    {
        z3::expr arg0 = addr_expr.arg(0);
        z3::expr arg1 = addr_expr.arg(1);

        z3::expr* offset_expr = nullptr;
        if (z3::eq(arg0, sp_expr)) {
            offset_expr = &arg1;
        }
        else if (z3::eq(arg1, sp_expr)) {
            offset_expr = &arg0;
        }
        
        if (offset_expr && offset_expr->is_numeral())
        {
            // expr -> get_numeral_int64_t causes crash due to overflow, so we manually cast it
            int64_t offset_int = static_cast<int64_t>(offset_expr->get_numeral_uint64());
            if (offset_int <= 32)
                return true;
        }
    }

    z3::context& ctx = addr.as_expr().ctx();
    z3::solver s(ctx);

    // use solver to prove it if its a stack access or not
    z3::expr rsp_min = ctx.bv_val(0x0000100000000000, 64);
    z3::expr rsp_max = ctx.bv_val(0x00007FFFFFFFF000, 64);
    
    //s.add(original_sp.as_expr() >= rsp_min);
    //s.add(original_sp.as_expr() <= rsp_max);
    s.add(addr.as_expr() > original_sp.as_expr() + 32);

    return s.check() == z3::unsat;
    CATCH()
}

std::pair<vexa::value, vexa::value> vexa::x64::cpu64::resolve_indirect_jmp(vexa::value v)
{
    if (llvm::SelectInst *sel = llvm::dyn_cast<llvm::SelectInst>(v.as_llvm()))
    {
        const llvm::DataLayout *DL = &context->llvm_module->getDataLayout();
        llvm::Value *true_v = sel->getTrueValue();
        llvm::Value *false_v = sel->getFalseValue();
        auto rs = std::pair<vexa::value, vexa::value>(
            vexa::value(true_v, DL, symex->get(true_v), symex),
            vexa::value(false_v, DL, symex->get(false_v), symex));

        if (rs.first.is_symbolic() || rs.second.is_symbolic())
            goto fail;

        return rs;
    }

fail:
    std::cout << "r13 :" << read_register(x64::R13).as_expr() << std::endl;
    std::cout << "rax :" << v.as_expr() << std::endl;
    THROW("failed to resolve indirect jump");
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
