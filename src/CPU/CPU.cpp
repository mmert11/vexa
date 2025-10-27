#include <VEXA/VEXA.h>

using namespace VEXA;
using namespace VEXA::X64;

std::unordered_map<ZydisRegister, VEXA::reg_t> VEXA::X64::zydisToVexaReg = {
	// RAX
	{ZydisRegister::ZYDIS_REGISTER_RAX, X64::RAX},
	{ZydisRegister::ZYDIS_REGISTER_EAX, X64::EAX},
	{ZydisRegister::ZYDIS_REGISTER_AX, X64::AX},
	{ZydisRegister::ZYDIS_REGISTER_AH, X64::AH},
	{ZydisRegister::ZYDIS_REGISTER_AL, X64::AL},
	// RBX
	{ZydisRegister::ZYDIS_REGISTER_RBX, X64::RBX},
	{ZydisRegister::ZYDIS_REGISTER_EBX, X64::EBX},
	{ZydisRegister::ZYDIS_REGISTER_BX, X64::BX},
	{ZydisRegister::ZYDIS_REGISTER_BH, X64::BH},
	{ZydisRegister::ZYDIS_REGISTER_BL, X64::BL},
	// RCX
	{ZydisRegister::ZYDIS_REGISTER_RCX, X64::RCX},
	{ZydisRegister::ZYDIS_REGISTER_ECX, X64::ECX},
	{ZydisRegister::ZYDIS_REGISTER_CX, X64::CX},
	{ZydisRegister::ZYDIS_REGISTER_CH, X64::CH},
	{ZydisRegister::ZYDIS_REGISTER_CL, X64::CL},
	// RDX
	{ZydisRegister::ZYDIS_REGISTER_RDX, X64::RDX},
	{ZydisRegister::ZYDIS_REGISTER_EDX, X64::EDX},
	{ZydisRegister::ZYDIS_REGISTER_DX, X64::DX},
	{ZydisRegister::ZYDIS_REGISTER_DH, X64::DH},
	{ZydisRegister::ZYDIS_REGISTER_DL, X64::DL},


	// RFLAGS
	{ZydisRegister::ZYDIS_REGISTER_RFLAGS, X64::RFLAGS}

};

// TODO: implement private constructor for cloning 
VEXA::X86CPU::X86CPU(std::shared_ptr<z3::context> c)
{
	context = c;
	
	// init all registers as zeros
	for (reg_t r = 0; r < X64::NB_REGS; r++) {
		if (X64::reg_info.at(r).size_bits == 64) // only 64 bit registers
			registers[r] = std::make_shared<Value>(
				Value(context->bv_val(0, context->bv_sort(64)))
			);
	}
}

std::shared_ptr<X86CPU> VEXA::X86CPU::clone()
{
	auto new_cpu = std::make_shared<X86CPU>(this->context);
	new_cpu->registers = this->registers;
	return new_cpu;
}

void VEXA::X86CPU::Write(reg_t reg, Value value)
{
	TRY()
	{
		auto reg_info_it = reg_info.find(reg);
		RegInfo info = reg_info_it != reg_info.end() ? reg_info_it->second :
			throw std::runtime_error("cannot find the register, id:" + (int)reg);

		// if register's ptr is used at anywhere else, this state is a clone
		// we need to clone the register
		if (registers.at(info.base_id).use_count() > 1)
			registers[info.base_id] = std::make_shared<VEXA::Value>(*registers[info.base_id]);

		// access to the base register (eax -> rax etc.)
		z3::expr base_reg_exp = registers[info.base_id]->expr();
		z3::expr new_expr = context->bv_val(0, 64);

		if (info.size_bits == 32)
		{
			new_expr = z3::concat(context->bv_val(0, 32), value.expr());
		}
		else if (info.size_bits < 64)
		{
			z3::expr lower_part = (info.offset_bits > 0) ?
				base_reg_exp.extract(info.offset_bits - 1, 0) :
				context->bv_val(0, 0);
			z3::expr upper_part = base_reg_exp.extract(63, info.offset_bits + info.size_bits);
			new_expr = z3::concat(upper_part, z3::concat(value.expr(), lower_part));
		}
		else {
			new_expr = value.expr();
		}

		registers[info.base_id] = std::make_shared<VEXA::Value>(new_expr);
	}
	CATCH("CPU write error")
}

VEXA::Value VEXA::X86CPU::Read(reg_t reg)
{
	TRY()
	{
		RegInfo info = X64::reg_info.at(reg);
		z3::expr base_reg_exp = registers.at(info.base_id)->expr();

		unsigned int high_bit = info.size_bits + info.offset_bits - 1;
		unsigned int low_bit = info.offset_bits;
		z3::expr read_expr = base_reg_exp.extract(high_bit, low_bit);

		return read_expr;
	}
	CATCH("CPU read error")
}