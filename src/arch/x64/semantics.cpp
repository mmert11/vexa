#include <vexa/vexa.h>

#define OPAQUE_SOLVING

#define semantic(handler) vexa::value vexa::x64::cpu64::handler(ZydisDisassembledInstruction instruction)
#define lambda(handler) [this](ZydisDisassembledInstruction instruction) { return handler(instruction); }
#define zydis_reg(reg) zydis_register_table.at(reg)
#define next_rip() builder->get_const_int(instruction.runtime_address + instruction.info.length, 64)

#define update_zf(result)                                                                    \
    vexa::value zf = builder->cmpeq(result, builder->get_const_int(0, result.size()), "zf"); \
    write_register(x64::ZF, zf);

#define update_sf(result)                                                                      \
    vexa::value sf = builder->resize(                                                          \
        builder->bshr(result, builder->get_const_int(result.size() - 1, result.size()), "sf"), \
        1);                                                                                    \
    write_register(x64::SF, sf);

#define update_cf(cf) write_register(x64::CF, cf);
#define update_of(of) write_register(x64::OF, of);

void vexa::x64::cpu64::init_handlers()
{
    handlers = {
        {ZYDIS_MNEMONIC_MOV, lambda(MOV)},
        {ZYDIS_MNEMONIC_MOVSXD, lambda(MOVZSXD)},
        {ZYDIS_MNEMONIC_MOVZX, lambda(MOVZSXD)},
        {ZYDIS_MNEMONIC_ADD, lambda(ADD)},
        {ZYDIS_MNEMONIC_XADD, lambda(XADD)},
        {ZYDIS_MNEMONIC_SUB, lambda(SUB)},
        {ZYDIS_MNEMONIC_JMP, lambda(JMP)},
        {ZYDIS_MNEMONIC_JNZ, lambda(JNZ)},
        {ZYDIS_MNEMONIC_JNL, lambda(JNL)},
        {ZYDIS_MNEMONIC_CMP, lambda(CMP)},
        {ZYDIS_MNEMONIC_CMOVNZ, lambda(CMOVNZ)},
        {ZYDIS_MNEMONIC_AND, lambda(AND)},
        {ZYDIS_MNEMONIC_OR, lambda(OR)},
        {ZYDIS_MNEMONIC_XOR, lambda(XOR)},
        {ZYDIS_MNEMONIC_NOT, lambda(NOT)},
        {ZYDIS_MNEMONIC_IMUL, lambda(IMUL)},
        {ZYDIS_MNEMONIC_MUL, lambda(MUL)},
        {ZYDIS_MNEMONIC_RET, lambda(RET)},
        {ZYDIS_MNEMONIC_SHL, lambda(SHL)},
        {ZYDIS_MNEMONIC_SHR, lambda(SHR)},
        {ZYDIS_MNEMONIC_SAR, lambda(SAR)},
        {ZYDIS_MNEMONIC_ROL, lambda(ROL)},
        {ZYDIS_MNEMONIC_ROR, lambda(ROR)},
        {ZYDIS_MNEMONIC_PUSH, lambda(PUSH)},
        {ZYDIS_MNEMONIC_POP, lambda(POP)},
        {ZYDIS_MNEMONIC_PUSHF, lambda(PUSHF)},
        {ZYDIS_MNEMONIC_POPF, lambda(POPF)},
        {ZYDIS_MNEMONIC_PUSHFQ, lambda(PUSHF)},
        {ZYDIS_MNEMONIC_POPFQ, lambda(POPF)},
        {ZYDIS_MNEMONIC_CDQE, lambda(CDQE)},
        {ZYDIS_MNEMONIC_LEA, lambda(LEA)},
        {ZYDIS_MNEMONIC_NOP, lambda(NOP)},
        {ZYDIS_MNEMONIC_TEST, lambda(TEST)},
        {ZYDIS_MNEMONIC_INC, lambda(INC)},
        {ZYDIS_MNEMONIC_CLC, lambda(CLC)},
        {ZYDIS_MNEMONIC_NEG, lambda(NEG)},
        {ZYDIS_MNEMONIC_XCHG, lambda(XCHG)},
        {ZYDIS_MNEMONIC_BSF, lambda(BSF)},
        {ZYDIS_MNEMONIC_BTR, lambda(BTR)},
        {ZYDIS_MNEMONIC_BTC, lambda(BTC)},
        {ZYDIS_MNEMONIC_STC, lambda(STC)},
        {ZYDIS_MNEMONIC_BT, lambda(BT)},
        {ZYDIS_MNEMONIC_ENDBR64, lambda(ENDBR64)},
        {ZYDIS_MNEMONIC_CALL, lambda(CALL)},
        {ZYDIS_MNEMONIC_DEC, lambda(DEC)},
        {ZYDIS_MNEMONIC_BSWAP, lambda(BSWAP)},
        {ZYDIS_MNEMONIC_JBE, lambda(JBE)},
        {ZYDIS_MNEMONIC_JZ, lambda(JZ)},
        {ZYDIS_MNEMONIC_JLE, lambda(JLE)},
        {ZYDIS_MNEMONIC_JNLE, lambda(JNLE)},
        {ZYDIS_MNEMONIC_JNS, lambda(JNS)},
        {ZYDIS_MNEMONIC_SETB, lambda(SETB)},
        {ZYDIS_MNEMONIC_SETZ, lambda(SETZ)},
        {ZYDIS_MNEMONIC_SETNBE, lambda(SETNBE)},
        {ZYDIS_MNEMONIC_IDIV, lambda(IDIV)},
        {ZYDIS_MNEMONIC_JNB, lambda(JNB)},
        {ZYDIS_MNEMONIC_JNBE, lambda(JNBE)},
        {ZYDIS_MNEMONIC_LEAVE, lambda(LEAVE)},
        {ZYDIS_MNEMONIC_SETL, lambda(SETL)},
        {ZYDIS_MNEMONIC_MOVSX, lambda(MOVZSXD)},
        {ZYDIS_MNEMONIC_JL, lambda(JL)},
        {ZYDIS_MNEMONIC_SETNL, lambda(SETNL)},
        {ZYDIS_MNEMONIC_JS, lambda(JS)},
        {ZYDIS_MNEMONIC_SETLE, lambda(SETLE)},
        {ZYDIS_MNEMONIC_RCL, lambda(RCL)},
        {ZYDIS_MNEMONIC_RCR, lambda(RCR)},
        {ZYDIS_MNEMONIC_CWD, lambda(CWD)},
        {ZYDIS_MNEMONIC_SBB, lambda(SBB)},
        {ZYDIS_MNEMONIC_CMOVO, lambda(CMOVO)},
        {ZYDIS_MNEMONIC_SHRD, lambda(SHRD)},
        {ZYDIS_MNEMONIC_BTS, lambda(BTS)},
        {ZYDIS_MNEMONIC_BSR, lambda(BSR)},
        {ZYDIS_MNEMONIC_LAHF, lambda(LAHF)},
        {ZYDIS_MNEMONIC_ADC, lambda(ADC)},
        {ZYDIS_MNEMONIC_CMC, lambda(CMC)},
        {ZYDIS_MNEMONIC_SHLD, lambda(SHLD)},
        {ZYDIS_MNEMONIC_SETNZ, lambda(SETNZ)}
    };
}

void vexa::x64::cpu64::write_operand(ZydisDisassembledInstruction instruction, uint8_t operand_idx, vexa::value v)
{
    TRY()

    ZydisDecodedOperand operand = instruction.operands[operand_idx];

    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_REGISTER:
        write_register(zydis_reg(operand.reg.value), v);
        break;
    case ZYDIS_OPERAND_TYPE_MEMORY:
    {
        vexa::value address = resolve_mem_address(instruction, operand);
        vexa::value ptr;

        if (is_stack_access(address))
            ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, address, "stack_write_ptr");
        else
            ptr = builder->inttoptr(address, "ptr");

        vexa::value safe_v = builder->resize(v, operand.size);
        builder->store(safe_v, ptr);
        break;
    }
    default:
        THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }

    CATCH()
}

vexa::value vexa::x64::cpu64::read_operand(ZydisDisassembledInstruction instruction, uint8_t operand_idx)
{
    TRY()

    ZydisDecodedOperand operand = instruction.operands[operand_idx];
    vexa::value result;

    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
    {
        if (operand.imm.is_relative)
            result = resolve_imm_address(instruction, operand_idx);
        else if (operand.imm.is_signed)
            result = builder->get_const_int(operand.imm.value.s, instruction.info.operand_width);
        else
            result = builder->get_const_int(operand.imm.value.u, instruction.info.operand_width);

        break;
    }
    case ZYDIS_OPERAND_TYPE_REGISTER:
    {
        vexa::value v = read_register(zydis_reg(operand.reg.value));
        result = v;
        break;
    }
    case ZYDIS_OPERAND_TYPE_MEMORY:
    {
        vexa::value address = resolve_mem_address(instruction, operand);
        vexa::value ptr;

        if (is_stack_access(address))
            ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, address, "stack_read_ptr");
        else
            ptr = builder->inttoptr(address, "ptr");

        result = builder->load(builder->get_int_ty(operand.size), ptr, "load");
        break;
    }
    default:
        THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }

    return result;
    CATCH()
}

semantic(MOV)
{
    TRY()
    write_operand(instruction, 0, read_operand(instruction, 1));
    return next_rip();
    CATCH()
}

semantic(MOVZSXD)
{
    TRY()
    vexa::value src = read_operand(instruction, 1);
    uint32_t dst_size = instruction.operands[0].size;

    bool is_signed = (instruction.info.mnemonic == ZYDIS_MNEMONIC_MOVSX) ||
                     (instruction.info.mnemonic == ZYDIS_MNEMONIC_MOVSXD);
    vexa::value extended_val = builder->resize(src, dst_size, is_signed);
    write_operand(instruction, 0, extended_val);

    return next_rip();
    CATCH()
}

semantic(LEA)
{
    TRY()
    vexa::value addr = resolve_mem_address(instruction, instruction.operands[1]);
    write_operand(instruction, 0, addr);
    return next_rip();
    CATCH()
}

semantic(ADD)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value v = builder->add(op1, op2, "add");

    write_operand(instruction, 0, v);

    update_zf(v);
    update_sf(v);

    vexa::value cf = builder->cmpltu(v, op1, "add_cf");
    update_cf(builder->resize(cf, 1));

    vexa::value xor1 = builder->bxor(op1, v);
    vexa::value xor2 = builder->bxor(op2, v);
    vexa::value and_val = builder->band(xor1, xor2);
    uint32_t size = instruction.operands[0].size;
    vexa::value of_bit = builder->bshr(and_val, builder->get_const_int(size - 1, size));
    update_of(builder->resize(of_bit, 1, false));

    return next_rip();
    CATCH()
}

semantic(XADD)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);

    vexa::value res = builder->add(dst, src, "xadd_res");

    write_operand(instruction, 1, dst);
    write_operand(instruction, 0, res);

    update_zf(res);
    update_sf(res);

    vexa::value cf = builder->cmpltu(res, dst, "xadd_cf");
    update_cf(builder->resize(cf, 1));

    vexa::value xor1 = builder->bxor(dst, res);
    vexa::value xor2 = builder->bxor(src, res);
    vexa::value and_val = builder->band(xor1, xor2);
    uint32_t size = instruction.operands[0].size;
    vexa::value of_bit = builder->bshr(and_val, builder->get_const_int(size - 1, size));
    update_of(builder->resize(of_bit, 1, false));

    return next_rip();
    CATCH()
}

semantic(SUB)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value v = builder->sub(op1, op2, "sub");

    write_operand(instruction, 0, v);
    update_zf(v);
    update_sf(v);

    vexa::value cf = builder->cmpltu(op1, op2, "sub_cf");
    update_cf(builder->resize(cf, 1));

    vexa::value xor1 = builder->bxor(op1, op2);
    vexa::value xor2 = builder->bxor(op1, v);
    vexa::value and_val = builder->band(xor1, xor2);
    uint32_t size = instruction.operands[0].size;
    vexa::value of_bit = builder->bshr(and_val, builder->get_const_int(size - 1, size));
    update_of(builder->resize(of_bit, 1, false));

    return next_rip();
    CATCH()
}

semantic(IMUL)
{
    TRY()
    switch (instruction.info.operand_count_visible)
    {
    case 1:
    {
        vexa::value op1 = read_operand(instruction, 0);
        uint8_t operand_size = op1.size();
        vexa::value flag;

        switch (operand_size)
        {
        case 8:
        {
            vexa::value al = read_register(x64::AL);
            vexa::value op_16 = builder->resize(op1, 16, true);
            vexa::value al_16 = builder->resize(al, 16, true);

            vexa::value mul_16 = builder->mul(op_16, al_16, "imul8_");
            write_register(x64::AX, mul_16);

            vexa::value trunc_8 = builder->resize(mul_16, 8);
            vexa::value sext_16 = builder->resize(trunc_8, 16, true);
            flag = builder->cmpne(mul_16, sext_16, "imul_flag");
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

            vexa::value sext_32 = builder->resize(l_32, 32, true);
            flag = builder->cmpne(mul_32, sext_32, "imul_flag");
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

            vexa::value sext_64 = builder->resize(l_32, 64, true);
            flag = builder->cmpne(mul_64, sext_64, "imul_flag");
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

            vexa::value sext_128 = builder->resize(l_64, 128, true);
            flag = builder->cmpne(mul_128, sext_128, "imul_flag");
            break;
        }
        default:
            THROW("imul error");
        }

        update_cf(flag);
        update_of(flag);
        break;
    }
    case 2:
    {
        vexa::value op0 = read_operand(instruction, 0);
        vexa::value op1 = read_operand(instruction, 1);
        uint32_t size = op0.size();

        vexa::value ext_op0 = builder->resize(op0, size * 2, true);
        vexa::value ext_op1 = builder->resize(op1, size * 2, true);
        vexa::value ext_mul = builder->mul(ext_op0, ext_op1, "imul2_ext");

        vexa::value trunc_res = builder->resize(ext_mul, size);
        write_operand(instruction, 0, trunc_res);

        vexa::value sext_res = builder->resize(trunc_res, size * 2, true);
        vexa::value flag = builder->cmpne(ext_mul, sext_res, "imul_flag");
        update_cf(flag);
        update_of(flag);
        break;
    }
    case 3:
    {
        vexa::value op1 = read_operand(instruction, 1);
        vexa::value op2 = read_operand(instruction, 2);
        uint32_t size = op1.size();

        vexa::value ext_op1 = builder->resize(op1, size * 2, true);
        vexa::value ext_op2 = builder->resize(op2, size * 2, true);
        vexa::value ext_mul = builder->mul(ext_op1, ext_op2, "imul3_ext");

        vexa::value trunc_res = builder->resize(ext_mul, size);
        write_operand(instruction, 0, trunc_res);

        vexa::value sext_res = builder->resize(trunc_res, size * 2, true);
        vexa::value flag = builder->cmpne(ext_mul, sext_res, "imul_flag");
        update_cf(flag);
        update_of(flag);
        break;
    }
    default:
        THROW("imul error");
    }

    return next_rip();
    CATCH();
}

semantic(MUL)
{
    TRY()
    if (instruction.info.operand_count_visible != 1)
        THROW("mul operand count error");

    vexa::value op1 = read_operand(instruction, 0);
    uint8_t operand_size = op1.size();
    vexa::value flag;

    switch (operand_size)
    {
    case 8:
    {
        vexa::value al = read_register(x64::AL);
        vexa::value op_16 = builder->resize(op1, 16, false);
        vexa::value al_16 = builder->resize(al, 16, false);

        vexa::value mul_16 = builder->mul(op_16, al_16, "mul8_");
        write_register(x64::AX, mul_16);

        vexa::value ah = builder->extract(mul_16, 15, 8, "ah");
        flag = builder->cmpne(ah, builder->get_const_int(0, 8), "mul_flag");
        break;
    }
    case 16:
    {
        vexa::value ax = read_register(x64::AX);
        vexa::value op_32 = builder->resize(op1, 32, false);
        vexa::value ax_32 = builder->resize(ax, 32, false);

        vexa::value mul_32 = builder->mul(op_32, ax_32, "mul16_");
        vexa::value h_32 = builder->extract(mul_32, 31, 16, "dx");
        vexa::value l_32 = builder->resize(mul_32, 16);
        l_32.as_llvm()->setName("ax");

        write_register(x64::DX, h_32);
        write_register(x64::AX, l_32);

        flag = builder->cmpne(h_32, builder->get_const_int(0, 16), "mul_flag");
        break;
    }
    case 32:
    {
        vexa::value eax = read_register(x64::EAX);
        vexa::value op_64 = builder->resize(op1, 64, false);
        vexa::value eax_64 = builder->resize(eax, 64, false);

        vexa::value mul_64 = builder->mul(op_64, eax_64, "mul32_");
        vexa::value h_32 = builder->extract(mul_64, 63, 32, "edx");
        vexa::value l_32 = builder->resize(mul_64, 32);
        l_32.as_llvm()->setName("eax");

        write_register(x64::EDX, h_32);
        write_register(x64::EAX, l_32);

        flag = builder->cmpne(h_32, builder->get_const_int(0, 32), "mul_flag");
        break;
    }
    case 64:
    {
        vexa::value rax = read_register(x64::RAX);
        vexa::value op_128 = builder->resize(op1, 128, false);
        vexa::value rax_128 = builder->resize(rax, 128, false);

        vexa::value mul_128 = builder->mul(op_128, rax_128, "mul64_");
        vexa::value h_64 = builder->extract(mul_128, 127, 64, "rdx");
        vexa::value l_64 = builder->resize(mul_128, 64);
        l_64.as_llvm()->setName("rax");

        write_register(x64::RDX, h_64);
        write_register(x64::RAX, l_64);

        flag = builder->cmpne(h_64, builder->get_const_int(0, 64), "mul_flag");
        break;
    }
    default:
        THROW("mul size error");
    }

    update_cf(flag);
    update_of(flag);

    return next_rip();
    CATCH();
}

semantic(CDQE)
{
    TRY();
    write_register(x64::RAX, builder->resize(read_register(x64::EAX), 64, true));
    return next_rip();
    CATCH();
}

semantic(CMP)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value sub = builder->sub(op1, op2);

    update_zf(sub);
    update_sf(sub);

    vexa::value cf = builder->cmpltu(op1, op2, "cmp_cf");
    write_register(x64::CF, builder->resize(cf, 1));

    vexa::value a_xor_b = builder->bxor(op1, op2);
    vexa::value a_xor_res = builder->bxor(op1, sub);
    vexa::value combined = builder->band(a_xor_b, a_xor_res);

    uint32_t size = instruction.operands[0].size;
    vexa::value of_bit = builder->bshr(combined, builder->get_const_int(size - 1, size));
    vexa::value of = builder->resize(of_bit, 1, false);
    write_register(x64::OF, of);

    return next_rip();
    CATCH()
}

semantic(JMP)
{
    TRY()
    if (instruction.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
    {
        auto resolved_addr = resolve_imm_address(instruction);
        return p_manager.branching(resolved_addr);
    }

    p_manager.vbranching = true;

    vexa::value op1 = read_operand(instruction, 0);
    if (op1.is_concrete()) // means this is an unconditional jump
    {
        if (op1.as_uint64() == 0)
            return builder->get_const_int((uint64_t)-1, 64); // exception signal
        return p_manager.branching(op1);
    }

    // conditional indirect jump
    vexa::resolved_path_t r_path = p_manager.resolve_path(op1);
    vexa::value cond = builder->cmpeq(op1, builder->get_const_int(r_path.true_ip.as_uint64(), 64), "indr_cond");
    return p_manager.branching(cond, r_path.true_ip, r_path.false_ip);
    CATCH()
}

// TODO: implement solving opaque predicates using z3 properly
bool is_always_true(z3::expr e)
{
    z3::solver s(e.ctx());
    s.add(e == 0);
    return s.check() == z3::unsat;
}

bool is_always_false(z3::expr e)
{
    z3::solver s(e.ctx());
    s.add(e != 0);
    return s.check() == z3::unsat;
}

semantic(JNZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    /*
    // disabled
    if (false) // so we use solver to solve it
    {
        // but condition may be too complicated to be simplified
        lifted_blocks.erase(dest.as_uint64());
        auto symbolic_cond = zf.as_expr().simplify();
        if (is_always_true(symbolic_cond))
        {
            std::cout << "opaque predicate solved, zf: 1" << std::endl;
            write_register(x64::ZF, builder->get_const_int(1, 1));
            return next;
        }
        if (is_always_false(symbolic_cond))
        {
            std::cout << "opaque predicate solved, zf: 0" << std::endl;
            write_register(x64::ZF, builder->get_const_int(0, 1));
            return dest;
        }
    }
    */

    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(0, 8), "jnz");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JNL)
{
    TRY()
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);
    vexa::value cond = builder->cmpeq(sf, of, "jnl_cond");
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(CMOVNZ)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value next_r = next_rip();

    vexa::value cond = builder->cmpeq(read_register(x64::ZF), builder->get_const_int(0, 8), "zf_cond");

    if (cond.is_symbolic())
    {
        p_manager.branching(cond, next_r, next_r);
        vexa::value sl = builder->select(cond, op2, op1, "cmovnz");
        write_operand(instruction, 0, op2);
    }
    else
    {
        vexa::value sl = builder->select(cond, op2, op1, "cmovnz");
        write_operand(instruction, 0, sl);
    }

    return next_r;
    CATCH()
}

semantic(AND)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value result = builder->band(op1, op2, "and");

    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    update_cf(builder->get_const_int(0, 1));
    update_of(builder->get_const_int(0, 1));

    return next_rip();
    CATCH()
}

semantic(OR)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value result = builder->bor(op1, op2, "or");

    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    update_cf(builder->get_const_int(0, 1));
    update_of(builder->get_const_int(0, 1));

    return next_rip();
    CATCH()
}

semantic(XOR)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value result = builder->bxor(op1, op2, "xor");

    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    update_cf(builder->get_const_int(0, 1));
    update_of(builder->get_const_int(0, 1));

    return next_rip();
    CATCH()
}

semantic(NOT)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value result = builder->bnot(op1, "not");
    write_operand(instruction, 0, result);
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
    vexa::value last_bit = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size), "last_bit"), 1);
    vexa::value old_of = read_register(x64::OF);
    vexa::value of = builder->select(is_count_zero,
                                     old_of,
                                     builder->bxor(cf, last_bit, "cf_xor_msb"),
                                     "of");

    write_operand(instruction, 0, result);
    update_cf(cf);
    update_of(of);

    vexa::value is_zero = builder->cmpeq(result, builder->get_const_int(0, size), "is_zero");
    vexa::value final_zf = builder->select(is_count_zero, read_register(x64::ZF), is_zero, "zf");
    vexa::value final_sf = builder->select(is_count_zero, read_register(x64::SF), last_bit, "sf");

    // we dont use zf/sf helpers here because flag assigments are conditional
    write_register(x64::ZF, final_zf); // update_zf(result);
    write_register(x64::SF, final_sf); // update_sf(result);

    return next_rip();
    CATCH()
}

semantic(SHR)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count = read_operand(instruction, 1);
    uint8_t size = dst.size();

    uint64_t mask = (size == 64) ? 63 : 31;
    vexa::value masked_count = builder->band(count, builder->get_const_int(mask, 8), "masked_count");
    vexa::value result = builder->bshr(dst, masked_count, "shr_result");

    vexa::value is_count_zero = builder->cmpeq(builder->get_const_int(0, 8), masked_count, "is_count_zero");
    vexa::value cf_bit_idx = builder->sub(masked_count, builder->get_const_int(1, 8), "cf_idx");
    vexa::value cf_idx_clamp = builder->band(cf_bit_idx, builder->get_const_int(size - 1, 8), "cf_idx_clamp");
    vexa::value last_shifted_bit = builder->resize(builder->band(builder->bshr(dst, cf_idx_clamp, "lsb"),
                                   builder->get_const_int(1, size), "lsb_and"),
                                   1);

    vexa::value old_cf = read_register(x64::CF);
    vexa::value cf = builder->select(is_count_zero, old_cf, last_shifted_bit, "cf");

    vexa::value old_of = read_register(x64::OF);
    vexa::value original_msb = builder->resize(builder->bshr(dst, builder->get_const_int(size - 1, size), "orig_msb"), 1);
    vexa::value of = builder->select(is_count_zero, old_of, original_msb, "of");

    write_operand(instruction, 0, result);
    write_register(x64::CF, cf);
    write_register(x64::OF, of);

    vexa::value is_zero = builder->cmpeq(result, builder->get_const_int(0, size), "is_zero");
    write_register(x64::ZF, builder->select(is_count_zero, read_register(x64::ZF), is_zero, "zf"));

    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size), "res_msb"), 1);
    write_register(x64::SF, builder->select(is_count_zero, read_register(x64::SF), res_msb, "sf"));

    return next_rip();
    CATCH()
}

semantic(SAR)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count = read_operand(instruction, 1);
    uint8_t size = dst.size();

    uint64_t mask = (size == 64) ? 63 : 31;
    vexa::value masked_count = builder->band(count, builder->get_const_int(mask, 8), "masked_count");

    vexa::value result = builder->bashr(dst, masked_count, "sar_result");
    vexa::value is_count_zero = builder->cmpeq(builder->get_const_int(0, 8), masked_count, "is_count_zero");

    vexa::value cf_bit_idx = builder->sub(masked_count, builder->get_const_int(1, 8), "cf_idx");
    vexa::value cf_idx_clamp = builder->band(cf_bit_idx, builder->get_const_int(size - 1, 8), "cf_idx_clamp");
    vexa::value last_shifted_bit = builder->resize(builder->band(builder->bshr(dst, cf_idx_clamp, "lsb"),
                                   builder->get_const_int(1, size), "lsb_and"),
                                   1);

    vexa::value old_cf = read_register(x64::CF);
    vexa::value cf = builder->select(is_count_zero, old_cf, last_shifted_bit, "cf");

    vexa::value old_of = read_register(x64::OF);
    vexa::value of = builder->select(is_count_zero, old_of, builder->get_const_int(0, 1), "of");

    write_operand(instruction, 0, result);
    write_register(x64::CF, cf);
    write_register(x64::OF, of);

    vexa::value is_zero = builder->cmpeq(result, builder->get_const_int(0, size), "is_zero");
    write_register(x64::ZF, builder->select(is_count_zero, read_register(x64::ZF), is_zero, "zf"));

    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size), "res_msb"), 1);
    write_register(x64::SF, builder->select(is_count_zero, read_register(x64::SF), res_msb, "sf"));

    return next_rip();
    CATCH()
}

semantic(ROL)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    uint32_t size = dst.size();

    uint32_t mask_val = (instruction.info.operand_width == 64) ? 63 : 31;
    vexa::value count = builder->band(builder->resize(count_op, size),
                                      builder->get_const_int(mask_val, size), "rol_count");

    vexa::value left = builder->bshl(dst, count, "rol_shl");

    vexa::value rev_count = builder->sub(builder->get_const_int(size, size), count, "rol_sub");
    vexa::value rev_count_clamped = builder->band(rev_count,
                                    builder->get_const_int(size - 1, size),
                                    "rol_rev_clamp");
    vexa::value right = builder->bshr(dst, rev_count_clamped, "rol_shr");
    vexa::value res = builder->bor(left, right, "rol_res");

    vexa::value new_cf = builder->resize(
                             builder->band(res, builder->get_const_int(1, size)), 1);

    vexa::value msb = builder->bshr(res, builder->get_const_int(size - 1, size));
    vexa::value of_val = builder->bxor(new_cf, builder->resize(msb, 1));

    vexa::value is_zero = builder->cmpeq(count,
                                         builder->get_const_int(0, size), "rol_count_zero");

    write_operand(instruction, 0, builder->select(is_zero, dst, res, "rol_dst"));
    update_cf(builder->select(is_zero, read_register(x64::CF), new_cf, "rol_cf"));
    update_of(builder->select(is_zero, read_register(x64::OF), of_val, "rol_of"));

    return next_rip();
    CATCH()
}

semantic(ROR)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    uint32_t size = dst.size();

    uint32_t mask_val = (instruction.info.operand_width == 64) ? 63 : 31;
    vexa::value count = builder->band(builder->resize(count_op, size),
                                      builder->get_const_int(mask_val, size), "ror_count");

    vexa::value right = builder->bshr(dst, count, "ror_shr");

    vexa::value rev_count = builder->sub(builder->get_const_int(size, size), count, "ror_sub");
    vexa::value rev_count_clamped = builder->band(rev_count,
                                    builder->get_const_int(size - 1, size),
                                    "ror_rev_clamp");
    vexa::value left = builder->bshl(dst, rev_count_clamped, "ror_shl");
    vexa::value res = builder->bor(left, right, "ror_res");

    vexa::value new_cf = builder->resize(
                             builder->bshr(res, builder->get_const_int(size - 1, size)), 1);

    vexa::value bit_n = builder->bshr(res, builder->get_const_int(size - 1, size));
    vexa::value bit_n_1 = builder->bshr(res, builder->get_const_int(size - 2, size));
    vexa::value of_val = builder->bxor(builder->resize(bit_n, 1),
                                       builder->resize(bit_n_1, 1));

    vexa::value is_zero = builder->cmpeq(count,
                                         builder->get_const_int(0, size), "ror_count_zero");

    write_operand(instruction, 0, builder->select(is_zero, dst, res, "ror_dst"));
    update_cf(builder->select(is_zero, read_register(x64::CF), new_cf, "ror_cf"));
    update_of(builder->select(is_zero, read_register(x64::OF), of_val, "ror_of"));

    return next_rip();
    CATCH()
}

semantic(RET)
{
    TRY()
    p_manager.vbranching = true;
    vexa::value address = pop64();
    if (address.is_concrete()) // means this is an unconditional jump
    {
        return p_manager.branching(address);
    }

    vexa::resolved_path_t r_path = p_manager.resolve_path(address);
    vexa::value cond = builder->cmpeq(address, builder->get_const_int(r_path.true_ip.as_uint64(), 64), "indr_cond");
    return p_manager.branching(cond, r_path.true_ip, r_path.false_ip);
    CATCH()
}

semantic(NOP)
{
    return next_rip();
}

semantic(PUSH)
{
    TRY()
    vexa::value op = read_operand(instruction, 0);
    vexa::value sp_offset = builder->add(read_register(x64::RSP), builder->get_const_int(-(instruction.info.operand_width / 8), 64), "new_sp");
    vexa::value new_sp = builder->inttoptr(sp_offset, "direct_write");

    if (is_stack_access(sp_offset))
        new_sp = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, sp_offset);

    builder->store(op, new_sp);
    write_register(x64::RSP, builder->add(read_register(x64::RSP),
                                          builder->get_const_int(-(instruction.info.operand_width / 8), 64), "new_rsp"));

    return next_rip();
    CATCH()
}

semantic(POP)
{
    TRY()
    vexa::value sp = builder->inttoptr(read_register(x64::RSP), "read_rsp");
    if (is_stack_access(sp))
        sp = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, read_register(x64::RSP), "sp");

    vexa::value v = builder->load(builder->get_int_ty(instruction.info.operand_width), sp, "load");
    write_operand(instruction, 0, v);

    vexa::value new_rsp = builder->add(read_register(x64::RSP),
                                       builder->get_const_int(instruction.info.operand_width / 8, 64), "new_rsp");
    vexa::value new_sp = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, new_rsp);

    write_register(x64::RSP, new_rsp);
    return next_rip();
    CATCH()
}

// handle reserve bit
semantic(PUSHF)
{
    TRY()
    uint16_t op_width = instruction.info.operand_width;
    uint16_t op_bytes = op_width / 8;

    vexa::value current_rsp = read_register(x64::RSP);
    vexa::value offset = builder->get_const_int(-op_bytes, 64);
    vexa::value new_rsp_val = builder->add(current_rsp, offset, "new_rsp");

    vexa::value rflags = read_register(x64::RFLAGS);
    vexa::value rflags_truncated = builder->resize(rflags, op_width);

    vexa::value sp_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, new_rsp_val, "sp_ptr");

    builder->store(rflags_truncated, sp_ptr);
    write_register(x64::RSP, new_rsp_val);

    return next_rip();
    CATCH()
}

semantic(POPF)
{
    TRY()
    uint16_t op_width = instruction.info.operand_width;
    uint16_t op_bytes = op_width / 8;

    vexa::value current_rsp = read_register(x64::RSP);
    vexa::value sp_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, current_rsp, "sp_ptr");

    vexa::value loaded_flags = builder->load(builder->get_int_ty(op_width), sp_ptr, "loaded_flags");
    vexa::value extended_flags = builder->resize(loaded_flags, 64);

    write_register(x64::RFLAGS, extended_flags);

    vexa::value next_rsp = builder->add(current_rsp, builder->get_const_int(op_bytes, 64), "next_rsp");
    write_register(x64::RSP, next_rsp);

    return next_rip();
    CATCH()
}

semantic(TEST)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    vexa::value op2 = read_operand(instruction, 1);
    vexa::value res = builder->band(op1, op2, "test");

    update_zf(res);
    update_sf(res);
    update_cf(builder->get_const_int(0, 1));
    update_of(builder->get_const_int(0, 1));

    // TODO: calculate parity flag

    return next_rip();
    CATCH()
}

semantic(INC)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    uint32_t size = op1.size();
    vexa::value one = builder->get_const_int(1, size);
    vexa::value inc = builder->add(op1, one, "inc");

    write_operand(instruction, 0, inc);

    update_zf(inc);
    update_sf(inc);

    vexa::value xor1 = builder->bxor(op1, inc);
    vexa::value xor2 = builder->bxor(one, inc);
    vexa::value and_val = builder->band(xor1, xor2);
    vexa::value of_bit = builder->bshr(and_val, builder->get_const_int(size - 1, size));
    update_of(builder->resize(of_bit, 1, false));

    return next_rip();
    CATCH()
}

semantic(CLC)
{
    TRY()
    update_cf(builder->get_const_int(0, 1));
    return next_rip();
    CATCH()
}

semantic(NEG)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    uint32_t size = val.size();
    vexa::value zero = builder->get_const_int(0, size);

    vexa::value result = builder->sub(zero, val, "neg_res");
    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    vexa::value is_not_zero = builder->cmpne(val, zero, "neg_cf_cond");
    update_cf(builder->resize(is_not_zero, 1));

    uint64_t min_signed = 1ULL << (size - 1);
    vexa::value min_val = builder->get_const_int(min_signed, size);
    vexa::value of = builder->cmpeq(val, min_val, "neg_of_cond");
    update_of(builder->resize(of, 1));

    return next_rip();
    CATCH()
}

semantic(XCHG)
{
    TRY()
    vexa::value val1 = read_operand(instruction, 0);
    vexa::value val2 = read_operand(instruction, 1);

    write_operand(instruction, 0, val2);
    write_operand(instruction, 1, val1);

    return next_rip();
    CATCH()
}

semantic(BSF)
{
    TRY()
    vexa::value src = read_operand(instruction, 1);
    uint32_t size = src.size();
    vexa::value zero = builder->get_const_int(0, size);

    vexa::value is_zero = builder->cmpeq(src, zero, "bsf_zf_cond");
    write_register(x64::ZF, is_zero);

    vexa::value result = builder->cttz(src, "bsf_res");
    write_operand(instruction, 0, result);

    return next_rip();
    CATCH()
}

semantic(BTR)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value bit_idx = read_operand(instruction, 1);
    uint32_t size = val.size();

    vexa::value mask = builder->get_const_int(size - 1, size);
    vexa::value masked_idx = builder->band(bit_idx, mask, "bit_idx_masked");

    vexa::value shifted = builder->bshr(val, masked_idx, "btr_shr");
    vexa::value bit_val = builder->band(shifted, builder->get_const_int(1, size), "bit_val");
    update_cf(builder->resize(bit_val, 1));

    vexa::value one = builder->get_const_int(1, size);
    vexa::value bit_mask = builder->bshl(one, masked_idx, "bit_mask");
    vexa::value inverted_mask = builder->bnot(bit_mask, "inverted_mask");
    vexa::value result = builder->band(val, inverted_mask, "btr_result");

    write_operand(instruction, 0, result);
    return next_rip();
    CATCH()
}

semantic(BTC)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value bit_idx = read_operand(instruction, 1);
    uint32_t size = val.size();

    vexa::value mask = builder->get_const_int(size - 1, size);
    vexa::value masked_idx = builder->band(bit_idx, mask, "bit_idx_masked");

    vexa::value shifted = builder->bshr(val, masked_idx, "btc_shr");
    vexa::value bit_val = builder->band(shifted, builder->get_const_int(1, size), "bit_val");
    update_cf(builder->resize(bit_val, 1));

    vexa::value one = builder->get_const_int(1, size);
    vexa::value bit_mask = builder->bshl(one, masked_idx, "bit_mask");
    vexa::value result = builder->bxor(val, bit_mask, "btc_result");

    write_operand(instruction, 0, result);

    return next_rip();
    CATCH()
}

semantic(STC)
{
    TRY()
    write_register(x64::CF, builder->get_const_int(1, 1));
    return next_rip();
    CATCH()
}

semantic(BT)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value bit_idx = read_operand(instruction, 1);
    uint32_t size = val.size();

    vexa::value mask = builder->get_const_int(size - 1, size);
    vexa::value masked_idx = builder->band(bit_idx, mask, "bt_idx_masked");

    vexa::value shifted = builder->bshr(val, masked_idx, "bt_shr");
    vexa::value bit_val = builder->band(shifted, builder->get_const_int(1, size), "bit_val");

    update_cf(builder->resize(bit_val, 1));
    return next_rip();
    CATCH()
}

semantic(ENDBR64)
{
    TRY()
    return next_rip();
    CATCH()
}

semantic(CALL)
{
    TRY()
    vexa::value operand = read_operand(instruction, 0);

    if (instruction.operands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
        operand = resolve_imm_address(instruction);

    if (operand.is_symbolic())
        THROW("couldnt resolve call!");

    vexa::value ret_address = builder->get_const_int(instruction.runtime_address + instruction.info.length, 64);
    push64(ret_address);

    return operand;
    CATCH()
}

semantic(DEC)
{
    TRY()
    vexa::value op1 = read_operand(instruction, 0);
    uint32_t size = op1.size();

    vexa::value one = builder->get_const_int(1, size);
    vexa::value result = builder->sub(op1, one, "dec_res");

    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    vexa::value orig_msb = builder->resize(builder->bshr(op1, builder->get_const_int(size - 1, size)), 1);
    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size)), 1);

    vexa::value of = builder->band(orig_msb, builder->bnot(res_msb), "dec_of");
    write_register(x64::OF, of);

    return next_rip();
    CATCH()
}

semantic(BSWAP)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value result = builder->bswap(val, "bswap_res");
    write_operand(instruction, 0, result);

    return next_rip();
    CATCH()
}

semantic(JBE)
{
    TRY()
    vexa::value cf = read_register(x64::CF);
    vexa::value zf = read_register(x64::ZF);

    vexa::value cond_val = builder->bor(cf, zf, "jbe_cond");
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jbe_cmp");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(1, 8), "jz_cond");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JLE)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value sf_xor_of = builder->bxor(sf, of, "jle_sf_xor_of");
    vexa::value cond_val = builder->bor(zf, sf_xor_of, "jle_cond");

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jle_final_cmp");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JNLE)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value sf_eq_of = builder->cmpeq(sf, of, "jnle_sf_eq_of");
    vexa::value zf_is_zero = builder->cmpeq(zf, builder->get_const_int(0, zf.size()), "jnle_zf_zero");
    vexa::value cond_val = builder->band(sf_eq_of, zf_is_zero, "jnle_cond");

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jnle_final_cmp");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JNS)
{
    TRY()
    vexa::value sf = read_register(x64::SF);

    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value cond = builder->cmpeq(sf, builder->get_const_int(0, sf.size()), "jns_cond");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(SETB)
{
    TRY()
    vexa::value cf = read_register(x64::CF);
    vexa::value final_val = builder->resize(cf, 8);
    write_operand(instruction, 0, final_val);

    return next_rip();
    CATCH()
}

semantic(SETZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value final_val = builder->resize(zf, 8);
    write_operand(instruction, 0, final_val);

    return next_rip();
    CATCH()
}

semantic(SETNBE)
{
    TRY()
    vexa::value cf = read_register(x64::CF);
    vexa::value zf = read_register(x64::ZF);

    vexa::value cf_is_zero = builder->cmpeq(cf, builder->get_const_int(0, cf.size()), "cf_zero");
    vexa::value zf_is_zero = builder->cmpeq(zf, builder->get_const_int(0, zf.size()), "zf_zero");

    vexa::value cond = builder->band(cf_is_zero, zf_is_zero, "setnbe_cond");
    vexa::value final_val = builder->resize(cond, 8);
    write_operand(instruction, 0, final_val);

    return next_rip();
    CATCH()
}

semantic(IDIV)
{
    TRY()
    if (instruction.info.operand_count_visible != 1)
        THROW("idiv operand count error");

    vexa::value divisor = read_operand(instruction, 0);
    uint8_t size = divisor.size();

    vexa::value quotient, remainder;
    switch (size)
    {
    case 8:
    {
        vexa::value ax = read_register(x64::AX);
        quotient = builder->sdiv(ax, builder->resize(divisor, 16, true), "idiv8_q");
        remainder = builder->srem(ax, builder->resize(divisor, 16, true), "idiv8_r");

        write_register(x64::AL, builder->resize(quotient, 8));
        write_register(x64::AH, builder->resize(remainder, 8));
        break;
    }
    case 16:
    {
        vexa::value dx = read_register(x64::DX);
        vexa::value ax = read_register(x64::AX);
        vexa::value dividend = builder->bor(builder->bshl(builder->resize(dx, 32), builder->get_const_int(16, 32)),
                                            builder->resize(ax, 32), "idiv16_divd");

        vexa::value divisor_32 = builder->resize(divisor, 32, true);
        quotient = builder->sdiv(dividend, divisor_32, "idiv16_q");
        remainder = builder->srem(dividend, divisor_32, "idiv16_r");

        write_register(x64::AX, builder->resize(quotient, 16));
        write_register(x64::DX, builder->resize(remainder, 16));
        break;
    }
    case 32:
    {
        vexa::value edx = read_register(x64::EDX);
        vexa::value eax = read_register(x64::EAX);
        vexa::value dividend = builder->bor(builder->bshl(builder->resize(edx, 64), builder->get_const_int(32, 64)),
                                            builder->resize(eax, 64), "idiv32_divd");

        vexa::value divisor_64 = builder->resize(divisor, 64, true);
        quotient = builder->sdiv(dividend, divisor_64, "idiv32_q");
        remainder = builder->srem(dividend, divisor_64, "idiv32_r");

        write_register(x64::EAX, builder->resize(quotient, 32));
        write_register(x64::EDX, builder->resize(remainder, 32));
        break;
    }
    case 64:
    {
        vexa::value rdx = read_register(x64::RDX);
        vexa::value rax = read_register(x64::RAX);

        vexa::value dividend = builder->bor(builder->bshl(builder->resize(rdx, 128), builder->get_const_int(64, 128)),
                                            builder->resize(rax, 128), "idiv64_divd");

        vexa::value divisor_128 = builder->resize(divisor, 128, true);
        quotient = builder->sdiv(dividend, divisor_128, "idiv64_q");
        remainder = builder->srem(dividend, divisor_128, "idiv64_r");

        write_register(x64::RAX, builder->resize(quotient, 64));
        write_register(x64::RDX, builder->resize(remainder, 64));
        break;
    }
    default:
        THROW("idiv unsupported size");
    }

    return next_rip();
    CATCH()
}

semantic(JNB)
{
    TRY()
    vexa::value cf = read_register(x64::CF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

    vexa::value cond = builder->cmpeq(cf, builder->get_const_int(0, cf.size()), "jnb_cond");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(JNBE)
{
    TRY()
    vexa::value cf = read_register(x64::CF);
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();
    vexa::value cf_zero = builder->cmpeq(cf, builder->get_const_int(0, cf.size()), "cf_is_zero");
    vexa::value zf_zero = builder->cmpeq(zf, builder->get_const_int(0, zf.size()), "zf_is_zero");
    vexa::value cond = builder->band(cf_zero, zf_zero, "jnbe_cond");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(LEAVE)
{
    TRY()
    vexa::value rbp_val = read_register(x64::RBP);
    write_register(x64::RSP, rbp_val);

    vexa::value sp_ptr = builder->inttoptr(rbp_val, "stack_pop_ptr");

    if (is_stack_access(rbp_val))
        sp_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, rbp_val);

    vexa::value old_rbp = builder->load(builder->get_int_ty(64), sp_ptr, "old_rbp");
    write_register(x64::RBP, old_rbp);

    vexa::value final_rsp = builder->add(rbp_val, builder->get_const_int(8, 64), "new_rsp_after_leave");
    write_register(x64::RSP, final_rsp);

    return next_rip();
    CATCH()
}

semantic(SETL)
{
    TRY()
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value cond = builder->cmpne(sf, of, "setl_cond");
    vexa::value result_byte = builder->resize(cond, 8);
    write_operand(instruction, 0, result_byte);

    return next_rip();
    CATCH()
}

semantic(JL)
{
    TRY()
    vexa::value dest = read_operand(instruction, 0);
    vexa::value next = next_rip();
    vexa::value sf = read_register(x64::SF);
    vexa::value exercised_of = read_register(x64::OF);
    vexa::value cond_i64 = builder->bxor(sf, exercised_of, "jl_cond_i64");
    vexa::value cond = builder->cmpne(cond_i64, builder->get_const_int(0, 64), "jl_cond_i1");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(SETNL)
{
    TRY()
    vexa::value dest = read_operand(instruction, 0);
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value xor_res = builder->bxor(sf, of, "setnl_xor");
    vexa::value cond = builder->cmpeq(xor_res, builder->get_const_int(0, xor_res.size()), "setnl_cond_i1");

    vexa::value result_byte = builder->resize(cond, 8);
    write_operand(instruction, 0, result_byte);

    return next_rip();
    CATCH()
}

semantic(JS)
{
    TRY()
    vexa::value dest = read_operand(instruction, 0);
    vexa::value next = next_rip();
    vexa::value sf = read_register(x64::SF);
    vexa::value cond = builder->cmpne(sf, builder->get_const_int(0, sf.size()), "js_cond");
    return p_manager.branching(cond, dest, next);
    CATCH()
}

semantic(SETLE)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value cond_zf = builder->cmpeq(zf, builder->get_const_int(1, zf.size()), "le_zf_cond");
    vexa::value cond_sf_of = builder->cmpne(sf, of, "le_sf_of_cond");
    vexa::value final_cond = builder->bor(cond_zf, cond_sf_of, "setle_final_cond");
    vexa::value result_byte = builder->resize(final_cond, 8);

    write_operand(instruction, 0, result_byte);
    return next_rip();
    CATCH()
}

semantic(RCL)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    vexa::value cf = read_register(x64::CF);
    uint32_t size = dst.size();

    uint32_t mask_val = (instruction.info.operand_width == 64) ? 63 : 31;
    vexa::value count = builder->band(builder->resize(count_op, 32), builder->get_const_int(mask_val, 32), "rcl_mask");

    uint32_t combined_size = size + 1;
    vexa::value real_count = builder->urem(count, builder->get_const_int(combined_size, 32), "rcl_mod");

    vexa::value cf_ext = builder->resize(cf, combined_size);
    vexa::value dst_ext = builder->resize(dst, combined_size);
    vexa::value combined = builder->bor(
                               builder->bshl(dst_ext, builder->get_const_int(1, combined_size), "rcl_shift"), cf_ext, "rcl_combined");

    vexa::value left = builder->bshl(combined, builder->resize(real_count, combined_size));
    vexa::value rev_count = builder->sub(builder->get_const_int(combined_size, 32), real_count);
    vexa::value right = builder->bshr(combined, builder->resize(rev_count, combined_size));
    vexa::value rotated = builder->bor(left, right);

    vexa::value new_cf = builder->resize(builder->band(rotated, builder->get_const_int(1, combined_size)), 1);
    vexa::value res = builder->resize(
                          builder->bshr(rotated, builder->get_const_int(1, combined_size), "rcl_res_shr"), size);

    vexa::value is_zero = builder->cmpeq(count, builder->get_const_int(0, 32));

    write_operand(instruction, 0, builder->select(is_zero, dst, res));
    update_cf(builder->select(is_zero, cf, new_cf));

    vexa::value res_msb = builder->resize(builder->bshr(res, builder->get_const_int(size - 1, size)), 1);
    vexa::value of_val = builder->bxor(res_msb, new_cf);
    update_of(builder->select(is_zero, read_register(x64::OF), of_val));

    return next_rip();
    CATCH()
}

semantic(RCR)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    vexa::value cf = read_register(x64::CF);
    uint32_t size = dst.size();

    uint32_t mask_val = (instruction.info.operand_width == 64) ? 63 : 31;
    vexa::value count = builder->band(builder->resize(count_op, 32), builder->get_const_int(mask_val, 32), "rcr_mask");

    uint32_t combined_size = size + 1;
    vexa::value real_count = builder->urem(count, builder->get_const_int(combined_size, 32), "rcr_mod");

    vexa::value cf_ext = builder->resize(cf, combined_size);
    vexa::value dst_ext = builder->resize(dst, combined_size);
    vexa::value combined = builder->bor(
                               builder->bshl(cf_ext, builder->get_const_int(size, combined_size), "rcr_cf_shift"), dst_ext, "rcr_combined");

    vexa::value right = builder->bshr(combined, builder->resize(real_count, combined_size));
    vexa::value rev_count = builder->sub(builder->get_const_int(combined_size, 32), real_count);
    vexa::value left = builder->bshl(combined, builder->resize(rev_count, combined_size));
    vexa::value rotated = builder->bor(left, right);

    vexa::value new_cf = builder->resize(
                             builder->bshr(rotated, builder->get_const_int(size, combined_size), "rcr_new_cf_shr"), 1);
    vexa::value res = builder->resize(rotated, size);

    vexa::value is_zero = builder->cmpeq(count, builder->get_const_int(0, 32));

    write_operand(instruction, 0, builder->select(is_zero, dst, res));
    update_cf(builder->select(is_zero, cf, new_cf));

    vexa::value bit_n = builder->resize(builder->bshr(res, builder->get_const_int(size - 1, size)), 1);
    vexa::value bit_n_1 = builder->resize(builder->bshr(res, builder->get_const_int(size - 2, size)), 1);
    vexa::value of_val = builder->bxor(bit_n, bit_n_1);
    update_of(builder->select(is_zero, read_register(x64::OF), of_val));

    return next_rip();
    CATCH()
}

semantic(CWD)
{
    TRY()
    vexa::value ax = read_register(x64::AX);
    vexa::value extended = builder->resize(ax, 32, true);
    vexa::value dx = builder->extract(extended, 31, 16, "cwd_dx");
    write_register(x64::DX, dx);
    return next_rip();
    CATCH()
}

semantic(SBB)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);
    vexa::value cf = read_register(x64::CF);

    uint32_t size = dst.size();

    vexa::value cf_ext = builder->resize(cf, size, false);
    vexa::value src_plus_cf = builder->add(src, cf_ext, "sbb_src_cf");
    vexa::value res = builder->sub(dst, src_plus_cf, "sbb_res");

    write_operand(instruction, 0, res);

    update_zf(res);
    update_sf(res);

    vexa::value borrow1 = builder->cmpltu(dst, src, "sbb_cf1");
    vexa::value borrow2 = builder->cmpltu(builder->sub(dst, src), cf_ext, "sbb_cf2");
    vexa::value final_cf = builder->bor(borrow1, borrow2, "sbb_final_cf");
    update_cf(builder->resize(final_cf, 1));

    vexa::value xor1 = builder->bxor(dst, src);
    vexa::value xor2 = builder->bxor(dst, res);
    vexa::value and_val = builder->band(xor1, xor2);
    vexa::value of_bit = builder->bshr(and_val, builder->get_const_int(size - 1, size));
    update_of(builder->resize(of_bit, 1, false));

    return next_rip();
    CATCH()
}

semantic(CMOVO)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);
    vexa::value of = read_register(x64::OF);
    vexa::value condition = builder->cmpeq(of, builder->get_const_int(1, 1), "cmovo_cond");
    vexa::value result = builder->select(condition, src, dst, "cmovo_res");
    write_operand(instruction, 0, result);

    return next_rip();
    CATCH()
}

semantic(SHRD)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);
    vexa::value count_op = read_operand(instruction, 2);
    uint32_t size = dst.size();

    uint32_t mask_val = (instruction.info.operand_width == 64) ? 63 : 31;
    vexa::value count = builder->band(builder->resize(count_op, 32), builder->get_const_int(mask_val, 32), "shrd_count");
    vexa::value shr_dst = builder->bshr(dst, builder->resize(count, size), "shrd_shr_dst");
    vexa::value rev_count = builder->sub(builder->get_const_int(size, 32), count, "shrd_rev_count");
    vexa::value shl_src = builder->bshl(src, builder->resize(rev_count, size), "shrd_shl_src");
    vexa::value result = builder->bor(shr_dst, shl_src, "shrd_result");

    vexa::value is_count_zero = builder->cmpeq(count, builder->get_const_int(0, 32), "shrd_is_zero");
    vexa::value cf_idx = builder->sub(count, builder->get_const_int(1, 32));
    vexa::value last_bit = builder->resize(
                               builder->band(builder->bshr(dst, builder->resize(cf_idx, size)), builder->get_const_int(1, size)),
                               1);

    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size)), 1);
    vexa::value dst_msb = builder->resize(builder->bshr(dst, builder->get_const_int(size - 1, size)), 1);
    vexa::value of_val = builder->bxor(res_msb, dst_msb, "shrd_of");

    write_operand(instruction, 0, builder->select(is_count_zero, dst, result));

    vexa::value current_zf = read_register(x64::ZF);
    vexa::value current_sf = read_register(x64::SF);

    vexa::value new_zf = builder->cmpeq(result, builder->get_const_int(0, size));
    write_register(x64::ZF, builder->select(is_count_zero, current_zf, new_zf));
    write_register(x64::SF, builder->select(is_count_zero, current_sf, res_msb));
    write_register(x64::CF, builder->select(is_count_zero, read_register(x64::CF), last_bit));
    write_register(x64::OF, builder->select(is_count_zero, read_register(x64::OF), of_val));

    return next_rip();
    CATCH()
}

semantic(BTS)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value bit_idx = read_operand(instruction, 1);
    uint32_t size = val.size();

    vexa::value mask = builder->get_const_int(size - 1, size);
    vexa::value masked_idx = builder->band(bit_idx, mask, "bts_idx_masked");

    vexa::value shifted = builder->bshr(val, masked_idx, "bts_shr");
    vexa::value bit_val = builder->band(shifted, builder->get_const_int(1, size), "bit_val");
    update_cf(builder->resize(bit_val, 1));

    vexa::value one = builder->get_const_int(1, size);
    vexa::value bit_mask = builder->bshl(one, masked_idx, "bit_mask");
    vexa::value result = builder->bor(val, bit_mask, "bts_result");
    write_operand(instruction, 0, result);

    return next_rip();
    CATCH()
}

semantic(BSR)
{
    TRY()
    vexa::value src = read_operand(instruction, 1);
    vexa::value dst = read_operand(instruction, 0);
    uint32_t size = src.size();

    vexa::value is_zero = builder->cmpeq(src, builder->get_const_int(0, size), "bsr_is_zero");
    vexa::value last_found_index = dst;

    for (int i = 0; i < (int)size; ++i)
    {
        vexa::value bit_val = builder->band(
                                  builder->bshr(src, builder->get_const_int(i, size)),
                                  builder->get_const_int(1, size));
        vexa::value is_bit_set = builder->cmpeq(bit_val, builder->get_const_int(1, size));
        last_found_index = builder->select(is_bit_set, builder->get_const_int(i, size), last_found_index);
    }

    write_operand(instruction, 0, builder->select(is_zero, dst, last_found_index));
    update_zf(src);

    return next_rip();
    CATCH()
}

semantic(LAHF)
{
    TRY()
    vexa::value rflags = read_register(x64::RFLAGS);
    vexa::value rflags_8 = builder->resize(rflags, 8);
    vexa::value flags_masked = builder->band(rflags_8, builder->get_const_int(0xD5, 8));
    vexa::value res = builder->bor(flags_masked, builder->get_const_int(0x02, 8));

    write_register(x64::AH, res);

    return next_rip();
    CATCH()
}

semantic(ADC)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);
    vexa::value cf = builder->resize(read_register(x64::CF), dst.size());

    uint32_t size = dst.size();

    vexa::value src_plus_cf = builder->add(src, cf);
    vexa::value result = builder->add(dst, src_plus_cf);

    vexa::value carry_out = builder->bor(
                                builder->cmpltu(result, src),
                                builder->cmpltu(src_plus_cf, cf));

    vexa::value dst_msb = builder->bshr(dst, builder->get_const_int(size - 1, size));
    vexa::value src_msb = builder->bshr(src, builder->get_const_int(size - 1, size));
    vexa::value res_msb = builder->bshr(result, builder->get_const_int(size - 1, size));

    vexa::value of = builder->band(
                         builder->cmpeq(dst_msb, src_msb),
                         builder->cmpne(dst_msb, res_msb));

    write_operand(instruction, 0, result);

    update_zf(result);
    update_sf(result);

    update_cf(builder->resize(carry_out, 1));
    update_of(builder->resize(of, 1));

    return next_rip();
    CATCH()
}

semantic(CMC)
{
    TRY()
    vexa::value current_cf = read_register(x64::CF);

    vexa::value toggled_cf = builder->bxor(
                                 current_cf,
                                 builder->get_const_int(1, current_cf.size()),
                                 "cmc_toggle");

    update_cf(toggled_cf);

    return next_rip();
    CATCH()
}

semantic(SHLD)
{
    TRY()
    vexa::value dst = read_operand(instruction, 0);
    vexa::value src = read_operand(instruction, 1);
    vexa::value count = read_operand(instruction, 2);

    uint32_t size = dst.size();
    vexa::value count_mask = builder->get_const_int(size == 64 ? 0x3F : 0x1F, count.size());
    vexa::value masked_count = builder->band(count, count_mask, "shld_count_mask");
    vexa::value is_count_zero = builder->cmpeq(masked_count, builder->get_const_int(0, masked_count.size()));
    vexa::value shift_left = builder->bshl(dst, masked_count);
    vexa::value shift_right_amount = builder->sub(builder->get_const_int(size, masked_count.size()), masked_count);
    vexa::value shift_right = builder->bshr(src, shift_right_amount);

    vexa::value result = builder->bor(shift_left, shift_right, "shld_result");

    vexa::value cf_shift_amount = builder->sub(builder->get_const_int(size, masked_count.size()), masked_count);
    vexa::value last_bit = builder->resize(
                               builder->band(builder->bshr(dst, cf_shift_amount), builder->get_const_int(1, size)),
                               1);

    vexa::value dst_msb = builder->resize(builder->bshr(dst, builder->get_const_int(size - 1, size)), 1);
    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, size)), 1);
    vexa::value of_val = builder->bxor(dst_msb, res_msb, "shld_of");

    write_operand(instruction, 0, builder->select(is_count_zero, dst, result));

    vexa::value current_zf = read_register(x64::ZF);
    vexa::value current_sf = read_register(x64::SF);
    vexa::value current_cf = read_register(x64::CF);
    vexa::value current_of = read_register(x64::OF);

    write_register(x64::ZF, builder->select(is_count_zero, current_zf, builder->cmpeq(result, builder->get_const_int(0, size))));
    write_register(x64::SF, builder->select(is_count_zero, current_sf, res_msb));
    write_register(x64::CF, builder->select(is_count_zero, current_cf, last_bit));
    write_register(x64::OF, builder->select(is_count_zero, current_of, of_val));

    return next_rip();
    CATCH()
}

semantic(SETNZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(0, 1));
    vexa::value final_val = builder->resize(cond, 8);
    write_operand(instruction, 0, final_val);

    return next_rip();
    CATCH()
}