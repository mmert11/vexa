#pragma once
#include "arch.hpp"

namespace vexa
{
    namespace amd64
    {
		static constexpr reg_t RAX = 0;
		static constexpr reg_t RBX = 1;
		static constexpr reg_t RCX = 2;
		static constexpr reg_t RDX = 3;
		static constexpr reg_t RSI = 4;
		static constexpr reg_t RDI = 5;
		static constexpr reg_t RSP = 6;
		static constexpr reg_t RBP = 7;
		static constexpr reg_t RIP = 8;
		static constexpr reg_t R8 = 9;
		static constexpr reg_t R9 = 10;
		static constexpr reg_t R10 = 11;
		static constexpr reg_t R11 = 12;
		static constexpr reg_t R12 = 13;
		static constexpr reg_t R13 = 14;
		static constexpr reg_t R14 = 15;
		static constexpr reg_t R15 = 16;
		static constexpr reg_t R8D = 17;
		static constexpr reg_t R9D = 18;
		static constexpr reg_t R10D = 19;
		static constexpr reg_t R11D = 20;
		static constexpr reg_t R12D = 21;
		static constexpr reg_t R13D = 22;
		static constexpr reg_t R14D = 23;
		static constexpr reg_t R15D = 24;
		static constexpr reg_t R8W = 25;
		static constexpr reg_t R9W = 26;
		static constexpr reg_t R10W = 27;
		static constexpr reg_t R11W = 28;
		static constexpr reg_t R12W = 29;
		static constexpr reg_t R13W = 30;
		static constexpr reg_t R14W = 31;
		static constexpr reg_t R15W = 32;
		static constexpr reg_t EAX = 33;
		static constexpr reg_t EBX = 34;
		static constexpr reg_t ECX = 35;
		static constexpr reg_t EDX = 36;
		static constexpr reg_t ESI = 37;
		static constexpr reg_t EDI = 38;
		static constexpr reg_t ESP = 39;
		static constexpr reg_t EBP = 40;
		static constexpr reg_t EIP = 41;
		static constexpr reg_t AX = 42;
		static constexpr reg_t BX = 43;
		static constexpr reg_t CX = 44;
		static constexpr reg_t DX = 45;
		static constexpr reg_t SI = 46;
		static constexpr reg_t DI = 47;
		static constexpr reg_t SP = 48;
		static constexpr reg_t BP = 49;
		static constexpr reg_t IP = 50;
		static constexpr reg_t AH = 51;
		static constexpr reg_t BH = 52;
		static constexpr reg_t CH = 53;
		static constexpr reg_t DH = 54;
		static constexpr reg_t AL = 55;
		static constexpr reg_t BL = 56;
		static constexpr reg_t CL = 57;
		static constexpr reg_t DL = 58;
		static constexpr reg_t SIL = 59;
		static constexpr reg_t DIL = 60;
		static constexpr reg_t SPL = 61;
		static constexpr reg_t BPL = 62;
		static constexpr reg_t R8B = 63;
		static constexpr reg_t R9B = 64;
		static constexpr reg_t R10B = 65;
		static constexpr reg_t R11B = 66;
		static constexpr reg_t R12B = 67;
		static constexpr reg_t R13B = 68;
		static constexpr reg_t R14B = 69;
		static constexpr reg_t R15B = 70;
		static constexpr reg_t PC = 71;
		static constexpr reg_t SS = 72;
		static constexpr reg_t ES = 73;
		static constexpr reg_t GS = 74;
		static constexpr reg_t FS = 75;
		static constexpr reg_t DS = 76;
		static constexpr reg_t CS = 77;
		static constexpr reg_t GSBASE = 78;
		static constexpr reg_t FSBASE = 79;
		static constexpr reg_t XMM0 = 80;
		static constexpr reg_t XMM1 = 81;
		static constexpr reg_t XMM2 = 82;
		static constexpr reg_t XMM3 = 83;
		static constexpr reg_t XMM4 = 84;
		static constexpr reg_t XMM5 = 85;
		static constexpr reg_t XMM6 = 86;
		static constexpr reg_t XMM7 = 87;
		static constexpr reg_t XMM8 = 88;
		static constexpr reg_t XMM9 = 89;
		static constexpr reg_t XMM10 = 90;
		static constexpr reg_t XMM11 = 91;
		static constexpr reg_t XMM12 = 92;
		static constexpr reg_t XMM13 = 93;
		static constexpr reg_t XMM14 = 94;
		static constexpr reg_t XMM15 = 95;
		static constexpr reg_t ST0 = 96;
		static constexpr reg_t ST1 = 97;
		static constexpr reg_t ST2 = 98;
		static constexpr reg_t ST3 = 99;
		static constexpr reg_t ST4 = 100;
		static constexpr reg_t ST5 = 101;
		static constexpr reg_t ST6 = 102;
		static constexpr reg_t ST7 = 103;
		static constexpr reg_t MM0 = 104;
		static constexpr reg_t MM1 = 105;
		static constexpr reg_t MM2 = 106;
		static constexpr reg_t MM3 = 107;
		static constexpr reg_t MM4 = 108;
		static constexpr reg_t MM5 = 109;
		static constexpr reg_t MM6 = 110;
		static constexpr reg_t MM7 = 111;
		static constexpr reg_t AF = 112;
		static constexpr reg_t CF = 113;
		static constexpr reg_t DF = 114;
		static constexpr reg_t OF = 115;
		static constexpr reg_t PF = 116;
		static constexpr reg_t SF = 117;
		static constexpr reg_t ZF = 118;
		static constexpr reg_t TOTAL_COUNT = 119;

		class Amd64Cpu : public vexa::cpu
        {
		public:
			Amd64Cpu(vexa::context* _context);
			remill::Register* get_return_register() override;
        };
    }
}
