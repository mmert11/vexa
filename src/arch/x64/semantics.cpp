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

#define push64(val)                                                                                                           \
    {                                                                                                                         \
        vexa::value sp_offset = builder->add(read_register(x64::RSP), builder->get_const_int(-8, 64), "new_sp");              \
        vexa::value new_sp = builder->inttoptr(sp_offset, "direct_write");                                                    \
        if (is_stack_access(sp_offset))                                                                                       \
            new_sp = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, sp_offset);                                     \
        builder->store(val, new_sp);                                                                                          \
        write_register(x64::RSP, builder->add(read_register(x64::RSP),                                                        \
                                              builder->get_const_int(-(instruction.info.operand_width / 8), 64), "new_rsp")); \
    }

#define pop64()({                                                                                       \
    vexa::value current_rsp = read_register(x64::RSP);                                                   \
    vexa::value sp_ptr = builder->inttoptr(current_rsp, "direct_read");                                  \
    if (is_stack_access(current_rsp))                                                                    \
        sp_ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, current_rsp, "pop_sp");        \
    vexa::value popped_val = builder->load(builder->get_int_ty(64), sp_ptr, "popped_val");               \
    write_register(x64::RSP, builder->add(current_rsp, builder->get_const_int(8, 64), "rsp_increment")); \
    popped_val;                                                                                          \
})

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
        {ZYDIS_MNEMONIC_SETLE, lambda(SETLE)}};
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

        builder->store(v, ptr);
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
    switch (operand.type)
    {
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
    {
        if (operand.imm.is_relative)
            return resolve_imm_address(instruction, operand_idx);

        if (operand.imm.is_signed)
            return builder->get_const_int(operand.imm.value.s, instruction.info.operand_width);
        else
            return builder->get_const_int(operand.imm.value.u, instruction.info.operand_width);
    }

    case ZYDIS_OPERAND_TYPE_REGISTER:
    {
        vexa::value v = read_register(zydis_reg(operand.reg.value));
        return v;
    }
    case ZYDIS_OPERAND_TYPE_MEMORY:
    {
        vexa::value address = resolve_mem_address(instruction, operand);
        vexa::value ptr;

        if (is_stack_access(address))
            ptr = builder->inbounds_gep(builder->get_int_ty(8), stack_ptr, address, "stack_read_ptr");
        else
            ptr = builder->inttoptr(address, "ptr");

        vexa::value v = builder->load(builder->get_int_ty(instruction.info.operand_width), ptr, "load");
        return v;
    }
    default:
        THROW(std::string("unimplemented operand type: ") + std::to_string(operand.type));
    }
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
        if (lifted_blocks.count(resolved_addr.as_uint64()))
            lifted_blocks.erase_range(resolved_addr.as_uint64(), read_register(x64::RIP).as_uint64());
        return resolved_addr;
    }

    vbranching = true;

    vexa::value op1 = read_operand(instruction, 0);
    if (op1.is_concrete()) // means this is an unconditional jump
    {
        if (op1.as_uint64() == 0)
        {
            return builder->get_const_int((uint64_t)-1, 64);
        }
        return op1;
    }

    std::cout << "[engine] forking path" << std::endl;

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

#ifdef OPAQUE_SOLVING
    if (zf.is_concrete())
    {
        printf("[simplifier] opaque predicate detected, zf: %ld\n", zf.as_uint64());
        if (zf.as_uint64() == 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

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

    std::cout << "[engine] forking path" << std::endl;

    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(0, 8), "jnz");
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));
    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(instruction.runtime_address));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);
    return dest;
    CATCH()
}

semantic(JNL)
{
    TRY()
    vexa::value sf = read_register(x64::SF);
    vexa::value of = read_register(x64::OF);

    vexa::value cond_val = builder->cmpeq(sf, of, "jnl_cond");

    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

#ifdef OPAQUE_SOLVING
    if (cond_val.is_concrete())
    {
        uint64_t res = cond_val.as_uint64();
        printf("[simplifier] opaque predicate detected (JNL), met: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JNL)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond_val, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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
        llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(instruction.runtime_address));
        llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(instruction.runtime_address));

        path_state path = {take_snapshot(), next_r, else_bb};
        unexplored_paths.push(path);

        builder->jump_if(cond, then_bb, else_bb);
        builder->set_ip(then_bb);
        write_operand(instruction, 0, op2);
        return next_r;
    }
    else
    {
        vexa::value sl = builder->select(cond, op2, op1, "cmovnz");
        write_operand(instruction, 0, sl);
        return next_r;
    }

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
    vexa::value last_bit = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, 64), "last_bit"), 1);
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
    vexa::value original_msb = builder->resize(builder->bshr(dst, builder->get_const_int(size - 1, 64), "orig_msb"), 1);
    vexa::value of = builder->select(is_count_zero, old_of, original_msb, "of");

    write_operand(instruction, 0, result);
    write_register(x64::CF, cf);
    write_register(x64::OF, of);

    vexa::value is_zero = builder->cmpeq(result, builder->get_const_int(0, size), "is_zero");
    write_register(x64::ZF, builder->select(is_count_zero, read_register(x64::ZF), is_zero, "zf"));

    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, 64), "res_msb"), 1);
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

    vexa::value res_msb = builder->resize(builder->bshr(result, builder->get_const_int(size - 1, 64), "res_msb"), 1);
    write_register(x64::SF, builder->select(is_count_zero, read_register(x64::SF), res_msb, "sf"));

    return next_rip();
    CATCH()
}

semantic(ROL)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    uint16_t width = val.size();

    vexa::value mask = builder->get_const_int(width - 1, width);
    vexa::value count = builder->band(builder->resize(count_op, width), mask, "");
    vexa::value shl = builder->bshl(val, count, "");
    vexa::value sub_count = builder->sub(builder->get_const_int(width, width), count, "");
    vexa::value shr_count = builder->band(sub_count, mask, "");
    vexa::value shr = builder->bshr(val, shr_count, "");
    vexa::value result = builder->bor(shl, shr, "");
    write_operand(instruction, 0, result);

    vexa::value cf_val = builder->extract(result, 0, 0, "new_cf");
    write_register(x64::CF, cf_val);

    vexa::value msb = builder->extract(result, width - 1, width - 1, "msb");
    vexa::value lsb = builder->extract(result, 0, 0, "lsb");
    vexa::value of_val = builder->bxor(msb, lsb, "");
    write_register(x64::OF, of_val);

    return next_rip();
    CATCH()
}

semantic(ROR)
{
    TRY()
    vexa::value val = read_operand(instruction, 0);
    vexa::value count_op = read_operand(instruction, 1);
    uint16_t width = val.size();

    vexa::value mask = builder->get_const_int(width - 1, width);
    vexa::value count = builder->band(builder->resize(count_op, width), mask, "masked_count");

    vexa::value shr = builder->bshr(val, count, "shr");

    vexa::value sub_count = builder->sub(builder->get_const_int(width, width), count, "");
    vexa::value shl_count = builder->band(sub_count, mask, "");
    vexa::value shl = builder->bshl(val, shl_count, "shl");

    vexa::value result = builder->bor(shr, shl, "ror_result");
    write_operand(instruction, 0, result);

    vexa::value cf_val = builder->extract(result, width - 1, width - 1, "new_cf");
    write_register(x64::CF, cf_val);

    vexa::value msb = builder->extract(result, width - 1, width - 1, "msb");
    vexa::value msb_minus_1 = builder->extract(result, width - 2, width - 2, "msb_minus_1");
    vexa::value of_val = builder->bxor(msb, msb_minus_1, "of_xor");
    write_register(x64::OF, of_val);

    return next_rip();
    CATCH()
}

semantic(RET)
{
    TRY()
    vexa::value next_rip = pop64();
    vbranching = true;
    return next_rip;
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

    if (cond_val.is_concrete())
    {
        uint64_t res = cond_val.as_uint64();
        printf("[simplifier] opaque predicate detected, met: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }

    std::cout << "[engine] forking path (JBE)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jbe_cmp");
    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
    CATCH()
}

semantic(JZ)
{
    TRY()
    vexa::value zf = read_register(x64::ZF);
    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

#ifdef OPAQUE_SOLVING
    if (zf.is_concrete())
    {
        printf("[simplifier] opaque predicate detected (JZ), zf: %ld\n", zf.as_uint64());

        if (zf.as_uint64() != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JZ)" << std::endl;

    vexa::value cond = builder->cmpeq(zf, builder->get_const_int(1, 8), "jz_cond");

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond_val.is_concrete())
    {
        uint64_t res = cond_val.as_uint64();
        printf("[simplifier] opaque predicate detected (JLE), met: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JLE)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jle_final_cmp");
    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond_val.is_concrete())
    {
        uint64_t res = cond_val.as_uint64();
        printf("[simplifier] opaque predicate detected (JNLE), met: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JNLE)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    vexa::value cond = builder->cmpne(cond_val, builder->get_const_int(0, cond_val.size()), "jnle_final_cmp");
    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
    CATCH()
}

semantic(JNS)
{
    TRY()
    vexa::value sf = read_register(x64::SF);

    vexa::value next = next_rip();
    vexa::value dest = resolve_imm_address(instruction);
    uint64_t rip = read_register(x64::RIP).as_uint64();

#ifdef OPAQUE_SOLVING
    if (sf.is_concrete())
    {
        uint64_t sf_val = sf.as_uint64();
        printf("[simplifier] opaque predicate detected (JNS), sf: %ld\n", sf_val);

        if (sf_val == 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JNS)" << std::endl;

    vexa::value cond = builder->cmpeq(sf, builder->get_const_int(0, sf.size()), "jns_cond");

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond.is_concrete())
    {
        uint64_t res = cond.as_uint64();
        printf("[simplifier] opaque predicate detected (JNB), taken: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JNB)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond.is_concrete())
    {
        uint64_t res = cond.as_uint64();
        printf("[simplifier] opaque predicate detected (JNBE), taken: %ld\n", res);

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), rip);
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), rip);
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JNBE)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond.is_concrete())
    {
        uint64_t res = cond.as_uint64();
        std::cout << "[engine] opaque predicate detected (JL), taken: " << res << std::endl;

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), next.as_uint64());
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), next.as_uint64());
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JL)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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

#ifdef OPAQUE_SOLVING
    if (cond.is_concrete())
    {
        uint64_t res = cond.as_uint64();
        std::cout << "[engine] opaque predicate detected (JL), taken: " << res << std::endl;

        if (res != 0)
        {
            if (lifted_blocks.count(dest.as_uint64()))
                lifted_blocks.erase_range(dest.as_uint64(), next.as_uint64());
            return dest;
        }
        else
        {
            if (lifted_blocks.count(next.as_uint64()))
                lifted_blocks.erase_range(next.as_uint64(), next.as_uint64());
            return next;
        }
    }
#endif

    std::cout << "[engine] forking path (JS)" << std::endl;

    llvm::BasicBlock *then_bb = builder->basic_block(utils::addr_to_str(dest.as_uint64()));
    llvm::BasicBlock *else_bb = builder->basic_block(utils::addr_to_str(next.as_uint64()));

    path_state path_s = {take_snapshot(), next, else_bb};
    unexplored_paths.push(path_s);

    builder->jump_if(cond, then_bb, else_bb);
    builder->set_ip(then_bb);

    return dest;
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