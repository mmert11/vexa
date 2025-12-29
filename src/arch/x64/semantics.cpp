#include <vexa/vexa.h>

#define semantic(handler) vexa::value vexa::x64::cpu64::handler(ZydisDisassembledInstruction instruction)
#define lambda(handler) [this](ZydisDisassembledInstruction instruction) { return handler(instruction); }
#define zydis_reg(reg) zydis_register_table.at(reg)
#define next_rip() builder->get_const_int(read_register(x64::RIP).as_uint64() + instruction.info.length, 64)

#define update_zf(result)                                                                    \
    vexa::value zf = builder->cmpeq(result, builder->get_const_int(0, result.size()), "zf"); \
    write_register(x64::ZF, zf);

#define update_sf(result)                                                                          \
    vexa::value sf = builder->resize(                                                              \
        builder->bshr(result, builder->get_const_int(result.size() - 1, result.size()), "sf_bit"), \
        1);                                                                                        \
    write_register(x64::SF, sf);

void vexa::x64::cpu64::init_handlers()
{
    handlers = {
        {ZYDIS_MNEMONIC_MOV, lambda(MOV)},
        {ZYDIS_MNEMONIC_ADD, lambda(ADD)},
        {ZYDIS_MNEMONIC_SUB, lambda(SUB)},
        {ZYDIS_MNEMONIC_JMP, lambda(JMP)},
        {ZYDIS_MNEMONIC_JNZ, lambda(JNZ)},
        {ZYDIS_MNEMONIC_CMP, lambda(CMP)},
        {ZYDIS_MNEMONIC_CMOVNZ, lambda(CMOVNZ)},
        {ZYDIS_MNEMONIC_AND, lambda(AND)},
        {ZYDIS_MNEMONIC_OR, lambda(OR)},
        {ZYDIS_MNEMONIC_XOR, lambda(XOR)},
        {ZYDIS_MNEMONIC_NOT, lambda(NOT)},
        {ZYDIS_MNEMONIC_RET, lambda(RET)}
    };
}

void vexa::x64::cpu64::write_operand(ZydisDecodedOperand operand, vexa::value v)
{
    TRY()
    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_REGISTER:
        write_register(zydis_reg(operand.reg.value), v);
        break;
    default: THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }
    CATCH()
}

vexa::value vexa::x64::cpu64::read_operand(ZydisDecodedOperand operand)
{
    TRY()
    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_IMMEDIATE: return builder->get_const_int(operand.imm.value.u, operand.imm.size);
    case ZYDIS_OPERAND_TYPE_REGISTER:
    {
        vexa::value v = read_register(zydis_reg(operand.reg.value));
        return v;
    }
    default: THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }
    CATCH()
}

semantic(MOV)
{
    TRY()
    write_operand(instruction.operands[0], read_operand(instruction.operands[1]));
    return next_rip();
    CATCH()
}

semantic(ADD)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    vexa::value v = builder->add(op1, op2, "add");

    write_operand(instruction.operands[0], v);
    update_zf(v);
    update_sf(v);

    return next_rip();
    CATCH()
}

semantic(SUB)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    vexa::value v = builder->sub(op1, op2, "add");

    write_operand(instruction.operands[0], v);
    update_zf(v);
    update_sf(v);

    return next_rip();
    CATCH()
}

semantic(CMP)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    vexa::value zf = builder->cmpeq(op1, op2, "zf");
    write_register(x64::ZF, zf);
    return next_rip();
    CATCH()
}

semantic(JMP)
{
    TRY()
    if (instruction.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
        return resolve_imm_address(instruction);

    vexa::value op1 = read_operand(instruction.operands[0]);
    if (op1.is_concrete()) // means this is an unconditional jump
        return op1;

    // conditional indirect jump
    auto [t, f] = resolve_indirect_jmp(op1);
    vexa::value cond = builder->cmpeq(op1, t, "indr_cond");

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(t.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(f.as_uint64()));

    path_state path_s = {take_snapshot(), f, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);
    return t;
    CATCH()
}

semantic(JNZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);

    if (zf.is_concrete())
    {
        uint64_t conc_zf = zf.as_uint64();
        if (conc_zf) return next;
        else return dest;
    }

    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(0, 8), "jnz");
    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(instruction.runtime_address));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);
    return dest;
    CATCH()
}

semantic(CMOVNZ)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);

    vexa::value cond = builder->cmpeq(read_register(x64::ZF), builder->get_const_int(1, 8), "zf_cond");
    vexa::value sl = builder->select(cond, op1, op2, "cmovnz");
    write_operand(instruction.operands[0], sl);
    return next_rip();
    CATCH()
}

semantic(AND)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    vexa::value result = builder->band(op1, op2, "and");

    write_operand(instruction.operands[0], result);

    update_zf(result);
    update_sf(result);

    //write_register(x64::CF, builder->get_const_int(0, 1));                                   
    //write_register(x64::OF, builder->get_const_int(0, 1));
    return next_rip();
    CATCH()
}

semantic(OR)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    
    vexa::value result = builder->bor(op1, op2, "or");
    
    write_operand(instruction.operands[0], result);
    
    update_zf(result);
    update_sf(result);
    
    //write_register(x64::CF, builder->get_const_int(0, 1));
    //write_register(x64::OF, builder->get_const_int(0, 1));
    return next_rip();
    CATCH()
}

semantic(XOR)
{
    TRY()
    vexa::value op1 = read_operand(instruction.operands[0]);
    vexa::value op2 = read_operand(instruction.operands[1]);
    
    vexa::value result = builder->bxor(op1, op2, "or");
    
    write_operand(instruction.operands[0], result);
    
    update_zf(result);
    update_sf(result);
    
    //write_register(x64::CF, builder->get_const_int(0, 1));
    //write_register(x64::OF, builder->get_const_int(0, 1));
    return next_rip();
    CATCH()
}

semantic(NOT)
{
    TRY()
    vexa::value op = read_operand(instruction.operands[0]);
    vexa::value result = builder->bnot(op, "not");
    
    write_operand(instruction.operands[0], result);
    return next_rip();    
    CATCH()
}

semantic(RET)
{
    TRY()
    builder->ret(read_register(x64::RAX).as_llvm());
    return builder->get_const_int(0, 64);
    CATCH()
}