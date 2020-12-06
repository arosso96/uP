#include "systemc.h"
#include "..\isa.h"

#ifndef DECODE_D
	#define DECODE_D
	SC_MODULE(DecodeStage) {
		private:
			sc_signal<sc_int<32>> *regs;
			
		public:
			
			SC_CTOR(DecodeStage) {
			}
			
			void setRegFile(sc_signal<sc_int<32>> *regs) {
				this->regs = regs;
			}
			
			void decode(unsigned int opcode, sc_uint<32> pc, sc_signal<sc_int<32>> *a, sc_signal<sc_int<32>> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> *mO, sc_signal<wbOpsT> *wbO) {
				printf("DECODE: ");
				sc_int<32> imm;
				sc_uint<5> rd;
				sc_uint<5> rs1, rs2;
				switch(opcode & 0x7F) {
					case 0b0110111:
					case 0b0010111:
					case 0b1101111:		// U
						*a = imm;
						*eO = ALU_ADDU;
						*mO = MEM_ALU_OUT;
						*wbO = WB_WRITE_REG;
						switch (typeU(opcode, &imm, &rd)) {
							case LUI:
								*b = 0;
								printf("LUI %d, %08x", (int) rd, (int) imm);
								break;
							case AUIPC:
								*b = (sc_int<32>) pc;
								printf("AUIPC %d, %08x", (int) rd, (int) imm);
								break;
							case JAL:
								*b = (sc_int<32>) pc+4;
								*mO = MEM_RS2_AND_JUMP;
								printf("JAL %d, %08x", (int) rd, (int) imm);
								break;
							default:
								setNop(a,b,eO,mO,wbO);
								return;
						}
						break;
					case 0b1100011:
					case 0b0100011:		// S
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
								return;
						}
						break;
					case 0b0000011:
					case 0b0010011:
					case 0b1110011:
					case 0b1100111:
					case 0b0001111:		// I
						switch (typeI(opcode, &rs1, &imm, &rd)) {
							case LB:
								aluI("LB", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								*mO = MEM_WRITE_LMD8S_OUT;
								break;
							case LH:
								aluI("LH", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								*mO = MEM_WRITE_LMD16S_OUT;
								break;
							case LW:
								aluI("LW", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								*mO = MEM_WRITE_LMD32_OUT;
								break;
							case LBU:
								aluI("LB", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								*mO = MEM_WRITE_LMD8U_OUT;
								break;
							case LHU:
								aluI("LHU", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								*mO = MEM_WRITE_LMD16U_OUT;
								break;
							case ADDI:
								aluI("ALUI", rd, rs1, imm, ALU_ADD, a, b, eO, mO, wbO);
								break;
							case SLTI:
								aluI("SLTI", rd, rs1, imm, ALU_SLT, a, b, eO, mO, wbO);
								break;
							case SLTIU:
								aluI("SLTIU", rd, rs1, imm, ALU_SLTU, a, b, eO, mO, wbO);
								break;
							case XORI:
								aluI("XORI", rd, rs1, imm, ALU_XOR, a, b, eO, mO, wbO);
								break;
							case ORI:
								aluI("ORI", rd, rs1, imm, ALU_OR, a, b, eO, mO, wbO);
								break;
							case ANDI:
								aluI("ANDI", rd, rs1, imm, ALU_AND, a, b, eO, mO, wbO);
								break;
							case SLLI:
								aluI("SLLI", rd, rs1, imm, ALU_SLL, a, b, eO, mO, wbO);
								break;
							case SRLI:
								aluI("SRLI", rd, rs1, imm, ALU_SRL, a, b, eO, mO, wbO);
								break;
							case SRAI:
								aluI("SRAI", rd, rs1, imm, ALU_SRA, a, b, eO, mO, wbO);
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
								return;
						}
						break;
					case 0b0110011:		// R
						switch (typeR(opcode, &rs1, &rs2, &rd)) {
							case ADD:
								printf("ADD %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_ADD;
								break;
							case SUB:
								printf("SUB %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SUB;
								break;
							case SLL:
								printf("SLL %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SLL;
								break;
							case SLT:
								printf("SLT %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SLT;
								break;
							case SLTU:
								printf("SLTU %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SLTU;
								break;
							case XOR:
								printf("XOR %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_XOR;
								break;
							case SRL:
								printf("SRL %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SRL;
								break;
							case SRA:
								printf("SRA %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_SRA;
								break;
							case OR:
								printf("OR %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_OR;
								break;
							case AND:
								printf("AND %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								*eO = ALU_AND;
								break;
							default:
								setNop(a,b,eO,mO,wbO);
								return;
						}
						*a = regs[(int) rs1];
						*b = regs[(int) rs2];
						*mO = MEM_ALU_OUT;
						*wbO = WB_WRITE_REG;
						break;
					default:			// NOP
						setNop(a,b,eO,mO,wbO);
						return;
				}
				printf("\n");
			}
			
		private:
			decodeOpsT typeR(unsigned int opcode, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_uint<5> *rd) {
			sc_uint<10> funct;
			*rs1 = (opcode & 0xF8000) >> 15;
			*rs2 = (opcode & 0x1F00000) >> 20;
			*rd = (opcode & 0xF80) >> 7;
			funct = ((opcode & 0xFE000000) >> 22) | ((opcode & 0x7000) >> 12);
			// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
			switch (opcode & 0x7F) {
				case 0b0110011:
					switch (funct & 0x7) { // funct3
						case 0b000:		// ADD & SUB
							switch((funct >> 8) & 1) {
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
							switch((funct >> 8) & 1) {
								case 0:		// SRL
									return SRL;
								case 1:		// SRA
									return SRA;
								default:
									return NOP;
							}
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
				funct = (opcode & 0x7000) >> 12;
				// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
				switch (opcode & 0x7F) {
					case 0b0000011:		// L types
						switch (funct) {
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
						switch (funct) {
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
				funct = (opcode & 0x7000) >> 12;
				// TODO: change with a more flexible configuration that can be reused (example a array as a parameter to the function containing opcode => return execFunct)
				switch (opcode & 0x7F) {
					case 0b1100011:		// B types
						switch (funct) {
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
						switch (funct) {
							case 0b000:		// SB
								return SB;
							case 0b001:		// SH
								return SH;
							case 0b010:		// SW
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
					case 0b1101111:		// JAL, TypeJ immediate !
						*imm = ((opcode & 0x80000000) >> 11) | ((opcode & 0x7FE00000) >> 20) | ((opcode & 0x100000) >> 9) | (opcode & 0xFF000);
						return JAL;
					default:
						return NOP;
				}
			}

			void setNop(sc_signal<sc_int<32>> *a, sc_signal<sc_int<32>> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> mO[], sc_signal<wbOpsT> wbO[]) {
				*a = 0;
				*b = 0;
				*eO = ALU_ADD;			// ALU_OUT = 0+0
				*mO = MEM_ALU_OUT;		// LMD = ALU_OUT = 0
				*wbO = WB_WRITE_REG;	// R0 is hardwired to 0 (does nothing)
				print("ADD 0, 0, 0\n");
			}
			
			void aluI(String opName, sc_uint<5> rd ,sc_uint<5> rs1, sc_int<32> imm, execOpsT execOp, sc_signal<sc_int<32>> *a, sc_signal<sc_int<32>> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> *mO, sc_signal<wbOpsT> *wbO) {
				*a = regs[(int) rs1];
				if ((imm >> 11) & 1 == 1)
					*b = 0xFFFFF000 | imm;
				else
					*b = imm;
				printf("%s %d, %d, %08x", opName, (int) rd, (int) rs1, (int) *b);
				*eO = execOp;
				*mO = MEM_ALU_OUT;
				*wbO = WB_WRITE_REG;
			}
	};
#endif