#include "decode.h"

DecodeStage::DecodeStage(sc_int<32> *regs) {
	this.regs = regs;
}

decodeOpsT typeR(unsigned int opcode, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_uint<5> *rd) {
	sc_uint<10> funct;
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

decodeOpsT typeI(unsigned int opcode, sc_uint<5> *rs1, sc_int<32> *imm, sc_uint<5> *rd) {
	sc_uint<3> funct;
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

decodeOpsT typeS(unsigned int opcode, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_int<32> *imm) {
	sc_uint<3> funct;
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

decodeOpsT typeU(unsigned int opcode, sc_int<32> *imm, sc_uint<5> *rd) {
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

void setNop(sc_int<32> *a, sc_int<32> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> mO[], sc_signal<wbOpsT> wbO[]) {
	*a = 0;
	*b = 0;
	*eO = ALU_ADD;		// ALU_OUT = 0+0
	*mO = MEM_ALU_OUT;	// LMD = ALU_OUT = 0
	*wbO = WB_NOP;		// Do nothing
}

DecodeStage::void decode(unsigned int opcode, sc_int<32> *a, sc_int<32> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> *mO, sc_signal<wbOpsT> *wbO, sc_uint<5> *wbRd) {
	switch(opcode & 0x7F) {
		case 0b0110111:
		case 0b0010111:
		case 0b1101111:		// U
			sc_int<32> imm;
			sc_uint<5> rd;
			switch (typeU(opcode, &imm, &rd)) {
				case LUI:
					// TODO
					break;
				case AUIPC:
					// TODO
					break;
				case JAL:
					// TODO
					break;
				default:
					setNop(a,b,eO,mO,wbO);
			}
			break;
		case 0b1100011:
		case 0b0100011:		// S
			sc_int<32> imm;
			sc_uint<5> rs1, rs2;
			switch (typeS(opcode, &rs1, &rs2, &imm)) {
				case BEQ:
					// TODO
					break;
				case BNE:
					// TODO
					break;
				case BLT:
					// TODO
					break;
				case BGE:
					// TODO
					break;
				case BLTU:
					// TODO
					break;
				case BGEU:
					// TODO
					break;
				case SB:
					// TODO
					break;
				case SH:
					// TODO
					break;
				case SU:
					// TODO
					break;
				default:
					setNop(a,b,eO,mO,wbO);
			}
			break;
		case 0b0000011:
		case 0b0010011:
		case 0b1110011:
		case 0b1100111:
		case 0b0001111:		// I
			sc_int<32> imm;
			sc_uint<5> rd, rs1;
			switch (typeI(opcode, &rs1, &imm, &rd)) {
				case LB:
					// TODO
					break;
				case LH:
					// TODO
					break;
				case LW:
					// TODO
					break;
				case LBU:
					// TODO
					break;
				case LHU:
					// TODO
					break;
				case ADDI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_ADD;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case SLTI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_SLT;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case SLTIU:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_SLTU;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case XORI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_XOR;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case ORI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_OR;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case ANDI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_AND;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case SLLI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_SLL;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case SRLI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_SRL;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case SRAI:
					*a = regs[(int) rs1];
					if ((imm >> 11) & 1 == 1)
						*b = 0xFFFFF000 | imm;
					else
						*b = imm;
					*wbRd = rd;
					*eO = ALU_SRA;
					*mO = MEM_ALU_OUT;
					*wbO = WB_WRITE_REG;
					break;
				case ECALL:
					// TODO
					break;
				case EBREAK:
					// TODO
					break;
				case JALR:
					// TODO
					break;
				case FENCE:
					// TODO
					break;
				default:
					setNop(a,b,eO,mO,wbO);
			}
			break;
		case 0b0110011:		// R
			sc_uint<5> rd, rs1, rs2;
			switch (typeR(opcode, &rs1, &rs2, &rd)) {
				case ADD:
					*eo = ALU_ADD;
					break;
				case SUB:
					*eo = ALU_SUB;
					break;
				case SLL:
					*eo = ALU_SLL;
					break;
				case SLT:
					*eo = ALU_SLT;
					break;
				case SLTU:
					*eo = ALU_SLTU;
					break;
				case XOR:
					*eo = ALU_XOR;
					break;
				case SRL:
					*eo = ALU_SRL;
					break;
				case SRA:
					*eo = ALU_SRA;
					break;
				case OR:
					*eo = ALU_OR;
					break;
				case AND:
					*eo = ALU_AND;
					break;
				default:
					setNop(a,b,eO,mO,wbO);
			}
			*a = regs[(int) rs1];
			*b = regs[(int) rs2];
			*wbRd = rd;
			*mO = MEM_ALU_OUT;
			*wbO = WB_WRITE_REG;
			break;
		default:			// NOP
			setNop(a,b,eO,mO,wbO);
	}
}