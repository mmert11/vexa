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
        builder->bshr(result, builder->get_const_int(result.size() - 1, result.size()), "sf"), \
        1);                                                                                        \
    write_register(x64::SF, sf);

#define update_cf(cf) write_register(x64::CF, cf);
#define update_of(of) write_register(x64::OF, of);

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
        {ZYDIS_MNEMONIC_IMUL, lambda(IMUL)},
        {ZYDIS_MNEMONIC_RET, lambda(RET)},
        {ZYDIS_MNEMONIC_SHL, lambda(SHL)}
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

vexa::value vexa::x64::cpu64::read_operand(ZydisDisassembledInstruction instruction, uint8_t operand_idx)
{
    TRY()
    ZydisDecodedOperand operand = instruction.operands[operand_idx];
    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
        if (operand.imm.is_signed) return builder->get_const_int(operand.imm.value.s, instruction.info.operand_width); 
        else return builder->get_const_int(operand.imm.value.u, instruction.info.operand_width);
    case ZYDIS_OPERAND_TYPE_REGISTER:
    {
        vexa::value v = read_register(zydis_reg(operand.reg.value));
        auto sym = symex->get(v.as_llvm());
        if (v.is_symbolic() && sym.is_numeral()) { // when z3 can concretize it while llvm cant
            std::cout << "folded constant by z3: " << sym.get_numeral_uint64() << std::endl;
            return builder->get_const_int(sym.get_numeral_int64(), v.size());
        }
        return v;
    }
    default: THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }
    CATCH()
}

semantic(MOV)
{
    TRY()
    write_operand(instruction.operands[0], read_operand(instruction, 1));
    return next_rip();
    CATCH()
}

semantic(ADD)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
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
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value v = builder->sub(op1, op2, "add");

    write_operand(instruction.operands[0], v);
    update_zf(v);
    update_sf(v);

    return next_rip();
    CATCH()
}

// TODO: implement cf/of flag calculations
semantic(IMUL)
{
    TRY()
    switch (instruction.info.operand_count_visible)
    {
    case 1:
    {
        vexa::value op1 = read_operand(instruction, 0);
        uint8_t operand_size = op1.size();

        switch (operand_size)
        {
        case 8:
        {
            vexa::value al = read_register(x64::AL);
            vexa::value op_16 = builder->resize(op1, 16, true);
            vexa::value al_16 = builder->resize(al, 16, true);

            vexa::value mul_16 = builder->mul(op_16, al_16, "imul8_");
            write_register(x64::AX, mul_16);

            break;
        }
        case 16:
        {
            vexa::value ax = read_register(x64::AX);
            vexa::value op_32 = builder->resize(op1, 32, true);
            vexa::value ax_32 = builder->resize(ax, 32, true);

            vexa::value mul_32 = builder->mul(op_32, ax_32, "imul16");
            vexa::value h_32 = builder->extract(mul_32, 31, 16, "dx");
            vexa::value l_32 = builder->resize(mul_32, 16);
            l_32.as_llvm()->setName("ax");
            
            write_register(x64::DX, h_32);
            write_register(x64::AX, l_32);
            break;
        }
        case 32:
        {
            vexa::value eax = read_register(x64::EAX);
            vexa::value op_64 = builder->resize(op1, 64, true);
            vexa::value eax_64 = builder->resize(eax, 64, true);
            vexa::value mul_64 = builder->mul(op_64, eax_64, "imul32");
            
            vexa::value h_32 = builder->extract(mul_64, 63, 32, "edx");
            vexa::value l_32 = builder->resize(mul_64, 32);
            l_32.as_llvm()->setName("eax");

            write_register(x64::EDX, h_32);
            write_register(x64::EAX, l_32);
            break;
        }
        case 64:
        {
            vexa::value rax = read_register(x64::RAX);
            vexa::value op_128 = builder->resize(op1, 128, true);
            vexa::value rax_128 = builder->resize(rax, 128, true);
            vexa::value mul_128 = builder->mul(op_128, rax_128, "imul64");
            
            vexa::value h_64 = builder->extract(mul_128, 127, 64, "rdx");
            vexa::value l_64 = builder->resize(mul_128, 64);
            l_64.as_llvm()->setName("rax");

            write_register(x64::RDX, h_64);
            write_register(x64::RAX, l_64);
            break;
        }
        default: THROW("imul error");
        }
        break;
    }
    case 2: 
    {
        vexa::value op0 = read_operand(instruction, 0);
        vexa::value op1 = read_operand(instruction, 1);
        write_operand(instruction.operands[0], builder->mul(op0, op1, "imul"));
        break;
    }
    case 3:
    {
        vexa::value op1 = read_operand(instruction, 1);
        vexa::value op2 = read_operand(instruction, 2);
        write_operand(instruction.operands[0], builder->mul(op1, op2, "imul"));
        break;
    }
    default: THROW("imul error");
    }

    return next_rip();
    CATCH();
}

semantic(CMP)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
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

    vexa::value op1 = read_operand(instruction, 0);
    if (op1.is_concrete()) // means this is an unconditional jump
    {
        // if destination address is 0, we assume it is unreachable
        if (op1.as_uint64() == 0)
            builder->unreachable();
        return op1;
    }

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

bool is_always_true(z3::expr e) {
    z3::solver s(e.ctx());
    s.add(e == 0);
    return s.check() == z3::unsat;
}

bool is_always_false(z3::expr e) {
    z3::solver s(e.ctx());
    s.add(e != 0);
    return s.check() == z3::unsat;
}

// TODO: implement solving opaque predicates using z3 properly
semantic(JNZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);

    if (zf.is_concrete())
    {
        printf("opaque predicate detected, zf: %ld\n", zf.as_uint64());
        uint64_t conc_zf = zf.as_uint64();
        if (conc_zf) return next;
        else return dest;
    }

    auto symbolic_cond = symex->get(zf.as_llvm());
    if (true) // use solver to solve opaque predicates
    {        
        if (is_always_true(symbolic_cond)) {
            std::cout << "opaque predicate solved, zf: 1" << std::endl;
            return next;
        }
        if (is_always_false(symbolic_cond)) {
            std::cout << "opaque predicate solved, zf: 0" << std::endl;
            return dest;
        }
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
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);

    vexa::value cond = builder->cmpeq(read_register(x64::ZF), builder->get_const_int(1, 8), "zf_cond");
    vexa::value sl = builder->select(cond, op1, op2, "cmovnz");
    write_operand(instruction.operands[0], sl);
    return next_rip();
    CATCH()
}

semantic(AND)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
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
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    
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
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    
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
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value result = builder->bnot(op1, "not");
    
    write_operand(instruction.operands[0], result);
    return next_rip();    
    CATCH()
}

semantic(SHL)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count = read_operand(instruction, 1);
    uint8_t size = dst.size();

    // masking the shift count to clamp it
    // if operand size is 64, mask is 63, else its 31
    uint64_t mask = dst.size() == 64 ? 63 : 31;
    vexa::value masked_count = builder->band(count, builder->get_const_int(mask, 8), "masked_count");
    vexa::value result = builder->bshl(dst, masked_count, "shl");

    // flags calculation
    // if count is zero, no flags should be updated
    // cf must be the last shifted bit
    vexa::value is_count_zero = builder->cmpeq(builder->get_const_int(0, 8), masked_count, "is_count_zero");
    vexa::value old_cf = read_register(x64::CF);
    
    // cf flag calculation
    // sets cf to last shifted bit in destination
    // last_shifted_bit_idx = operand_size - count
    vexa::value last_shifted_bit_idx = builder->sub(builder->get_const_int(size, 8), masked_count, "lsb_idx");
    // if count is 0, lsb index will be "size - 0" and it will cause UB, so we clamp the index with "size - 1"
    // lsb_idx_clamp = last_shifted_bit_idx & (operand_size - 1)
    vexa::value lsb_idx_clamp = builder->band(last_shifted_bit_idx, builder->get_const_int(size - 1, 8), "lsb_idx_clamp");
    // lsb = dst >> lsb_idx_clamp
    // cf = is_count_zero ? old_cf : lsb (last_shifted_bit)
    vexa::value lsb = builder->resize(builder->bshr(dst, lsb_idx_clamp, "last_shifted_bit"), 1);
    vexa::value cf = builder->select(is_count_zero, old_cf, lsb, "cf");

    // of flag calculation
    // of = is_count_zero ? old_of : c ^ last_bit_of_shifted_value
    // last_bit = result >> size - 1
    vexa::value last_bit = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, 64), "last_bit"), 1);
    vexa::value old_of = read_register(x64::OF);
    vexa::value of = builder->select(is_count_zero,
        old_of,
        builder->bxor(cf, last_bit, "cf_xor_msb"),
        "of"
    );

    write_operand(instruction.operands[0], result);
    update_cf(cf);
    update_of(of);

    vexa::value is_zero = builder->cmpeq(result, builder->get_const_int(0, size), "is_zero");
    vexa::value final_zf = builder->select(is_count_zero, read_register(x64::ZF), is_zero, "zf");
    vexa::value final_sf = builder->select(is_count_zero, read_register(x64::SF), last_bit, "sf");

    // we dont use zf/sf helpers here because flag assigments are conditional
    write_register(x64::ZF, final_zf); //update_zf(result);
    write_register(x64::SF, final_sf); //update_sf(result);

    return next_rip();
    CATCH()
}

semantic(RET)
{
    TRY()
    builder->ret(read_register(x64::RAX));
    return builder->get_const_int(0, 64);
    CATCH()
}
