#include "systemc.h"

#ifndef DECODE_D
	#define DECODE_D
	class DecodeStage {
		private:
			sc_int<32> regs[32];
			
		public:
			// The NOP is just an ADD rd = r0, rs1 = r0, rs2 = r0
			// ^ -> TODO: may be exploited with low power techniques to save power (ex: clock or power gating, or simply freeze FF such that input doesn't cahnge + disable MEM and WB writing)
			enum decodeOpsT {NOP, LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, FENCE, ECALL, EBREAK};
			
			DecodeStage(sc_int<32> *regs);
			
			void decode(unsigned int opcode, sc_int<32> *a, sc_int<32> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> *mO, sc_signal<wbOpsT> *wbO);
	}
#endif