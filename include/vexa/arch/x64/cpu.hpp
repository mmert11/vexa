#pragma once

#include "../arch.hpp"

namespace vexa
{
    namespace x64
    {
        // RAX
		static constexpr reg_t RAX = 0;
		static constexpr reg_t EAX = 1;
		static constexpr reg_t AX = 2;
		static constexpr reg_t AL = 3;
		static constexpr reg_t AH = 4;

		// RBX
		static constexpr reg_t RBX = 5;
		static constexpr reg_t EBX = 6;
		static constexpr reg_t BX = 7;
		static constexpr reg_t BL = 8;
		static constexpr reg_t BH = 9;

		// RCX
		static constexpr reg_t RCX = 10;
		static constexpr reg_t ECX = 11;
		static constexpr reg_t CX = 12;
		static constexpr reg_t CL = 13;
		static constexpr reg_t CH = 14;

		// RDX
		static constexpr reg_t RDX = 15;
		static constexpr reg_t EDX = 16;
		static constexpr reg_t DX = 17;
		static constexpr reg_t DL = 18;
		static constexpr reg_t DH = 19;

		// RDI
		static constexpr reg_t RDI = 20;
		static constexpr reg_t EDI = 21;
		static constexpr reg_t DI = 22;
		static constexpr reg_t DIL = 23;

		// RSI
		static constexpr reg_t RSI = 24;
		static constexpr reg_t ESI = 25;
		static constexpr reg_t SI = 26;
		static constexpr reg_t SIL = 27;

		// RBP
		static constexpr reg_t RBP = 28;
		static constexpr reg_t EBP = 29;
		static constexpr reg_t BP = 30;
		static constexpr reg_t BPL = 31;

		// RSP
		static constexpr reg_t RSP = 32;
		static constexpr reg_t ESP = 33;
		static constexpr reg_t SP = 34;
		static constexpr reg_t SPL = 35;

		// R8
		static constexpr reg_t R8 = 36;
		static constexpr reg_t R8D = 37;
		static constexpr reg_t R8W = 38;
		static constexpr reg_t R8B = 39;

		// R9
		static constexpr reg_t R9 = 40;
		static constexpr reg_t R9D = 41;
		static constexpr reg_t R9W = 42;
		static constexpr reg_t R9B = 43;

		// R10
		static constexpr reg_t R10 = 44;
		static constexpr reg_t R10D = 45;
		static constexpr reg_t R10W = 46;
		static constexpr reg_t R10B = 47;

		// R11
		static constexpr reg_t R11 = 48;
		static constexpr reg_t R11D = 49;
		static constexpr reg_t R11W = 50;
		static constexpr reg_t R11B = 51;

		// R12
		static constexpr reg_t R12 = 52;
		static constexpr reg_t R12D = 53;
		static constexpr reg_t R12W = 54;
		static constexpr reg_t R12B = 55;

		// R13
		static constexpr reg_t R13 = 56;
		static constexpr reg_t R13D = 57;
		static constexpr reg_t R13W = 58;
		static constexpr reg_t R13B = 59;

		// R14
		static constexpr reg_t R14 = 60;
		static constexpr reg_t R14D = 61;
		static constexpr reg_t R14W = 62;
		static constexpr reg_t R14B = 63;

		// R15
		static constexpr reg_t R15 = 64;
		static constexpr reg_t R15D = 65;
		static constexpr reg_t R15W = 66;
		static constexpr reg_t R15B = 67;

		// RFLAGS
		static constexpr reg_t RFLAGS = 68;
		static constexpr reg_t CF = 69;
		static constexpr reg_t ZF = 70;
		static constexpr reg_t SF = 71;
		static constexpr reg_t OF = 72;

		static constexpr reg_t RIP = 73;

		static constexpr reg_t NB_REGS = 74;

		static const std::map<reg_t, register_desc> register_table =
		{
			// RIP
			{ RIP, { RIP, 64, 0} },

			// RAX
			{ RAX, { RAX, 64, 0 } },  { EAX, { RAX, 32, 0 } },
			{ AX,  { RAX, 16, 0 } },  { AL,  { RAX, 8, 0 } },
			{ AH,  { RAX, 8, 8 } },

			// RBX
			{ RBX, { RBX, 64, 0 } },  { EBX, { RBX, 32, 0 } },
			{ BX,  { RBX, 16, 0 } },  { BL,  { RBX, 8, 0 } },
			{ BH,  { RBX, 8, 8 } },

			// RCX
			{ RCX, { RCX, 64, 0 } },  { ECX, { RCX, 32, 0 } },
			{ CX,  { RCX, 16, 0 } },  { CL,  { RCX, 8, 0 } },
			{ CH,  { RCX, 8, 8 } },

			// RDX
			{ RDX, { RDX, 64, 0 } },  { EDX, { RDX, 32, 0 } },
			{ DX,  { RDX, 16, 0 } },  { DL,  { RDX, 8, 0 } },
			{ DH,  { RDX, 8, 8 } },

			// RDI
			{ RDI, { RDI, 64, 0 } },  { EDI, { RDI, 32, 0 } },
			{ DI,  { RDI, 16, 0 } },  { DIL, { RDI, 8, 0 } },

			// RSI
			{ RSI, { RSI, 64, 0 } },  { ESI, { RSI, 32, 0 } },
			{ SI,  { RSI, 16, 0 } },  { SIL, { RSI, 8, 0 } },

			// RBP
			{ RBP, { RBP, 64, 0 } },  { EBP, { RBP, 32, 0 } },
			{ BP,  { RBP, 16, 0 } },  { BPL, { RBP, 8, 0 } },

			// RSP
			{ RSP, { RSP, 64, 0 } },  { ESP, { RSP, 32, 0 } },
			{ SP,  { RSP, 16, 0 } },  { SPL, { RSP, 8, 0 } },

			// R8
			{ R8,  { R8, 64, 0 } },   { R8D, { R8, 32, 0 } },
			{ R8W, { R8, 16, 0 } },   { R8B, { R8, 8, 0 } },

			// R9
			{ R9,  { R9, 64, 0 } },   { R9D, { R9, 32, 0 } },
			{ R9W, { R9, 16, 0 } },   { R9B, { R9, 8, 0 } },

			// R10
			{ R10, { R10, 64, 0 } },  { R10D, { R10, 32, 0 } },
			{ R10W, { R10, 16, 0 } }, { R10B, { R10, 8, 0 } },

			// R11
			{ R11, { R11, 64, 0 } },  { R11D, { R11, 32, 0 } },
			{ R11W, { R11, 16, 0 } }, { R11B, { R11, 8, 0 } },

			// R12
			{ R12, { R12, 64, 0 } },  { R12D, { R12, 32, 0 } },
			{ R12W, { R12, 16, 0 } }, { R12B, { R12, 8, 0 } },

			// R13
			{ R13, { R13, 64, 0 } },  { R13D, { R13, 32, 0 } },
			{ R13W, { R13, 16, 0 } }, { R13B, { R13, 8, 0 } },

			// R14
			{ R14, { R14, 64, 0 } },  { R14D, { R14, 32, 0 } },
			{ R14W, { R14, 16, 0 } }, { R14B, { R14, 8, 0 } },

			// R15
			{ R15, { R15, 64, 0 } },  { R15D, { R15, 32, 0 } },
			{ R15W, { R15, 16, 0 } }, { R15B, { R15, 8, 0 } },

			// RFLAGS
			{ RFLAGS, { RFLAGS, 64, 0} }, { CF, { RFLAGS, 1, 0 } },
			{ ZF, { RFLAGS, 1, 6 } }, { SF, {RFLAGS, 1, 7 } },
			{ OF, { RFLAGS, 1, 11 } }
		};

		#define x64dcl(instr) vexa::value instr(ZydisDisassembledInstruction inst)
		class cpu64 : public cpu
        {
		public:
			cpu64(std::shared_ptr<ir::builder> _builder, std::shared_ptr<vexa::symex> _symex,
            	std::shared_ptr<vexa::memory> _memory, std::shared_ptr<vexa::context> _context);
            void run() override;
			void write_register(reg_t reg, vexa::value value) override;
    		vexa::value read_register(reg_t reg) override;
		private:
			vexa::value lift(ZydisDisassembledInstruction instruction);
			vexa::value read_operand(ZydisDisassembledInstruction instruction, uint8_t operand_idx);
			void write_operand(ZydisDecodedOperand op, vexa::value value);

			void init_handlers();
			std::unordered_map<ZydisMnemonic, std::function<vexa::value(ZydisDisassembledInstruction)>> handlers;

			vexa::value resolve_imm_address(ZydisDisassembledInstruction instruction);
			std::pair<vexa::value, vexa::value> resolve_indirect_jmp(vexa::value v);

			x64dcl(MOV);
			x64dcl(ADD);
			x64dcl(SUB);
			x64dcl(JMP);
			x64dcl(JNZ);
			x64dcl(CMP);
			x64dcl(CMOVNZ);
			x64dcl(AND);
			x64dcl(OR);
			x64dcl(XOR);
			x64dcl(NOT);
			x64dcl(RET);

			x64dcl(IMUL);
			x64dcl(LEA);
			x64dcl(SHL);
			x64dcl(SHR);
			x64dcl(ROL);
			x64dcl(ROR);
			x64dcl(NEG);
			x64dcl(TEST);
			x64dcl(SETZ);
			x64dcl(SETNZ);

			const std::map<ZydisRegister, reg_t> zydis_register_table = {
				{ ZYDIS_REGISTER_RFLAGS, RFLAGS },

				{ ZYDIS_REGISTER_RAX, RAX },
				{ ZYDIS_REGISTER_EAX, EAX },
				{ ZYDIS_REGISTER_AX,  AX  },
				{ ZYDIS_REGISTER_AH,  AH  },
				{ ZYDIS_REGISTER_AL,  AL  },

				{ ZYDIS_REGISTER_RBX, RBX },
				{ ZYDIS_REGISTER_EBX, EBX },
				{ ZYDIS_REGISTER_BX,  BX  },
				{ ZYDIS_REGISTER_BH,  BH  },
				{ ZYDIS_REGISTER_BL,  BL  },

				{ ZYDIS_REGISTER_RCX, RCX },
				{ ZYDIS_REGISTER_ECX, ECX },
				{ ZYDIS_REGISTER_CX,  CX  },
				{ ZYDIS_REGISTER_CH,  CH  },
				{ ZYDIS_REGISTER_CL,  CL  },

				{ ZYDIS_REGISTER_RDX, RDX },
				{ ZYDIS_REGISTER_EDX, EDX },
				{ ZYDIS_REGISTER_DX,  DX  },
				{ ZYDIS_REGISTER_DH,  DH  },
				{ ZYDIS_REGISTER_DL,  DL  },

				{ ZYDIS_REGISTER_RSI, RSI },
				{ ZYDIS_REGISTER_ESI, ESI },
				{ ZYDIS_REGISTER_SI,  SI  },
				{ ZYDIS_REGISTER_SIL, SIL },

				{ ZYDIS_REGISTER_RDI, RDI },
				{ ZYDIS_REGISTER_EDI, EDI },
				{ ZYDIS_REGISTER_DI,  DI  },
				{ ZYDIS_REGISTER_DIL, DIL },

				{ ZYDIS_REGISTER_RBP, RBP },
				{ ZYDIS_REGISTER_EBP, EBP },
				{ ZYDIS_REGISTER_BP,  BP  },
				{ ZYDIS_REGISTER_BPL, BPL },

				{ ZYDIS_REGISTER_RSP, RSP },
				{ ZYDIS_REGISTER_ESP, ESP },
				{ ZYDIS_REGISTER_SP,  SP  },
				{ ZYDIS_REGISTER_SPL, SPL },

				{ ZYDIS_REGISTER_R8,  R8  },
				{ ZYDIS_REGISTER_R8D, R8D },
				{ ZYDIS_REGISTER_R8W, R8W },
				{ ZYDIS_REGISTER_R8B, R8B },

				{ ZYDIS_REGISTER_R9,  R9  },
				{ ZYDIS_REGISTER_R9D, R9D },
				{ ZYDIS_REGISTER_R9W, R9W },
				{ ZYDIS_REGISTER_R9B, R9B },

				{ ZYDIS_REGISTER_R10,  R10  },
				{ ZYDIS_REGISTER_R10D, R10D },
				{ ZYDIS_REGISTER_R10W, R10W },
				{ ZYDIS_REGISTER_R10B, R10B },

				{ ZYDIS_REGISTER_R11,  R11  },
				{ ZYDIS_REGISTER_R11D, R11D },
				{ ZYDIS_REGISTER_R11W, R11W },
				{ ZYDIS_REGISTER_R11B, R11B },

				{ ZYDIS_REGISTER_R12,  R12  },
				{ ZYDIS_REGISTER_R12D, R12D },
				{ ZYDIS_REGISTER_R12W, R12W },
				{ ZYDIS_REGISTER_R12B, R12B },

				{ ZYDIS_REGISTER_R13,  R13  },
				{ ZYDIS_REGISTER_R13D, R13D },
				{ ZYDIS_REGISTER_R13W, R13W },
				{ ZYDIS_REGISTER_R13B, R13B },

				{ ZYDIS_REGISTER_R14,  R14  },
				{ ZYDIS_REGISTER_R14D, R14D },
				{ ZYDIS_REGISTER_R14W, R14W },
				{ ZYDIS_REGISTER_R14B, R14B },

				{ ZYDIS_REGISTER_R15,  R15  },
				{ ZYDIS_REGISTER_R15D, R15D },
				{ ZYDIS_REGISTER_R15W, R15W },
				{ ZYDIS_REGISTER_R15B, R15B }
			};
        };

		std::string getRegisterStr(vexa::reg_t reg);
		bool disassemble(std::vector<uint8_t> data, uint64_t &address, ZydisDisassembledInstruction& instruction);

		static const std::unordered_map<reg_t, std::string> reg_to_str =
		{
			{RIP, "rip"},
			{RAX, "rax"}, {EAX, "eax"}, {AX, "ax"}, {AL, "al"}, {AH, "ah"},
			{RBX, "rbx"}, {EBX, "ebx"}, {BX, "bx"}, {BL, "bl"}, {BH, "bh"},
			{RCX, "rcx"}, {ECX, "ecx"}, {CX, "cx"}, {CL, "cl"}, {CH, "ch"},
			{RDX, "rdx"}, {EDX, "edx"}, {DX, "dx"}, {DL, "dl"}, {DH, "dh"},
			{RDI, "rdi"}, {EDI, "edi"}, {DI, "di"}, {DIL, "dil"},
			{RSI, "rsi"}, {ESI, "esi"}, {SI, "si"}, {SIL, "sil"},
			{RBP, "rbp"}, {EBP, "ebp"}, {BP, "bp"}, {BPL, "bpl"},
			{RSP, "rsp"}, {ESP, "esp"}, {SP, "sp"}, {SPL, "spl"},
			{R8, "r8"},   {R8D, "r8d"}, {R8W, "r8w"}, {R8B, "r8b"},
			{R9, "r9"},   {R9D, "r9d"}, {R9W, "r9w"}, {R9B, "r9b"},
			{R10, "r10"}, {R10D, "r10d"}, {R10W, "r10w"}, {R10B, "r10b"},
			{R11, "r11"}, {R11D, "r11d"}, {R11W, "r11w"}, {R11B, "r11b"},
			{R12, "r12"}, {R12D, "r12d"}, {R12W, "r12w"}, {R12B, "r12b"},
			{R13, "r13"}, {R13D, "r13d"}, {R13W, "r13w"}, {R13B, "r13b"},
			{R14, "r14"}, {R14D, "r14d"}, {R14W, "r14w"}, {R14B, "r14b"},
			{R15, "r15"}, {R15D, "r15d"}, {R15W, "r15w"}, {R15B, "r15b"},
			{RFLAGS, "rflags"}, {CF, "cf"}, {ZF, "zf"}, {SF, "sf"}, {OF, "of"}
		};
    }
}
