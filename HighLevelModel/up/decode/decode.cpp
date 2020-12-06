#include "decode.h"

decodeOpsT typeR(unsigned int opcode, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_uint<5> *rd, sc_uint<10> *funct) {
	*rs1 = (opcode & 0xF8000) >> 15;
	*rs2 = (opcode & 0x1F00000) >> 20;
	*rd = (opcode & 0xF80) >> 7;
	*funct = ((opcode & 0xFE000000) >> 22) | ((opcode & 0x7000) >> 12);
	// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
	switch (opcode & 0x7F) {
		case 0b0110011:
			switch (*funct & 0x7) { // funct3
				case 0b000:		// ADD & SUB
					switch((*funct >> 8) & 1) {
						case 0:		// ADD
							return ADD;
						case 1:		// SUB
							return SUB;
						default:
							return NOP;
					}
				case 0b001:	// SLL
					return SLL;
				case 0b010:	// SLT
					return SLT;
				case 0b011:	// SLTU
					return SLTU;
				case 0b100:	// XOR
					return XOR;
				case 0b101:	// SRL & SRA
					switch((*funct >> 8) & 1) {
						case 0:		// SRL
							return SRL;
						case 1:		// SRA
							return SRA;
						default:
							return NOP;
					}
					return ;
				case 0b110:	// OR
					return OR;
				case 0b111:	// AND
					return AND;
				default:
					return NOP;
			}
		default:
			return NOP;
	}
}

decodeOpsT typeI(unsigned int opcode, sc_uint<5> *rs1, sc_uint<32> *imm, sc_uint<5> *rd, sc_uint<3> *funct) {
	*rs1 = (opcode & 0xF8000) >> 15;
	*rd = (opcode & 0xF80) >> 7;
	*imm = (opcode & 0xFFF00000) >> 20;
	*funct = (opcode & 0x7000) >> 12;
	// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
	switch (opcode & 0x7F) {
		case 0b0000011:		// L types
			switch (*funct) {
				case 0b000:		// LB
					return LB;
				case 0b001:		// LH
					return LH;
				case 0b010:		// LW
					return LW;
				case 0b100:		// LBU
					return LBU;
				case 0b101:		// LHU
					return LHU;
				default:
					return NOP;
			}
		case 0b0010011:		// ALU_Imm types
			switch (*funct) {
				case 0b000:		// ADDI
					return ADDI;
				case 0b010:		// SLTI
					return SLTI;
				case 0b011:		// SLTIU
					return SLTIU;
				case 0b100:		// XORI
					return XORI;
				case 0b110:		// ORI
					return ORI;
				case 0b111:		// ANDI
					return ANDI;
				case 0b001:		// SLLI
					return SLLI;
				case 0b101:		// SRLI & SRAI
					switch ((*imm >> 10) & 1) {
						case 0:	//SRLI
							return SRLI;
						case 1:	//SRAI
							return SRAI;
						default:
							return NOP;
					}
				default:
					return NOP;
			}
		case 0b1110011:		// E types
			switch (*imm) {
				case 0:			// ECALL
					return ECALL;
				case 1:			// EBREAK
					return EBREAK;
				default:
					return NOP;
			}
		case 0b1100111:		// JALR
			return JALR;
		case 0b0001111:		// FENCE
			return FENCE;
		default:
			return NOP;
	}
}

decodeOpsT typeS(unsigned int opcode, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_uint<32> *imm, sc_uint<3> *funct) {
	*rs1 = (opcode & 0xF8000) >> 15;
	*rs2 = (opcode & 0x1F00000) >> 20;
	*imm = ((opcode & 0xFE000000) >> 20) | ((opcode & 0xF80) >> 7);
	*funct = (opcode & 0x7000) >> 12;
	// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
	switch (opcode & 0x7F) {
		case 0b1100011:		// B types
			switch (*funct) {
				case 0b000:		// BEQ
					return BEQ;
				case 0b001:		// BNE
					return BNE;
				case 0b100:		// BLT
					return BLT;
				case 0b101:		// BGE
					return BGE;
				case 0b110:		// BLTU
					return BLTU;
				case 0b111:		// BGEU
					return BGEU;
				default:
					return NOP;
			}
		case 0b0100011:		// S types
			switch (*funct) {
				case :		// SB
					return SB;
				case :		// SH
					return SH;
				case :		// SW
					return SW;
				default:
					return NOP;
			}
		default:
			return NOP;
	}
}

decodeOpsT typeU(unsigned int opcode, sc_uint<32> *imm, sc_uint<5> *rd) {
	*imm = opcode & 0xFFFFF000;
	*rd = (opcode & 0xF80) >> 7;
	// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
	switch (opcode & 0x7F) {
		case 0b0110111:		// LUI
		return LUI;
		case 0b0010111:		// AUIPC
		return AUIPC;
		case 0b1101111:		// JAL, TODO: check better!
		return JAL;
		default:
		return NOP;
	}
}

DecodeStage::void decode(unsigned int opcode, sc_int<32> *a, sc_int<32> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> mO[], sc_signal<wbOpsT> wbO[]) {
	// TODO: parse opcode
	*a = 1;
	*b = 2;
	*eO = ALU_ADD;
	mO[0] = WRITE;
	wbO[0] = WRITE_REG;
}