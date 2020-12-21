#include "systemc.h"
#include "..\isa.h"
#include "..\fetch\fetch_interface.cpp"
#include "..\execute\execute_interface.cpp"
#include "..\memory\memory_interface.cpp"
#include "..\regfile_interface.cpp"
#include "decode_interface.cpp"

#ifndef DECODE_D
	#define DECODE_D
	SC_MODULE(DecodeStage), public decode_if {
		private:
			sc_signal<ExecInstrT> execOpS;
			sc_signal<memOpsT> memOpS;
			sc_signal<wbOpsT> wbOpS;
			sc_signal<sc_uint<32>> ir;
			sc_signal<bool> flushS, flushPrev;
			
		public:
			sc_port<regfile_if> regIf;
			sc_port<fetch_if> fetchIn;
			sc_port<execute_if> executeIf;
			sc_port<memory_if> memIf;
			sc_in<bool> clock;
			
			SC_CTOR(DecodeStage) {
				flushS = false;
				flushPrev = false;
				SC_METHOD(decode);
				sensitive << clock.pos();
				dont_initialize();
			}
			
			virtual void flush() {
				flushS = !flushS;
			}
			
			virtual ExecInstrT getExecOp() {
				ExecInstrT execOp;
				if (flushS.read() == flushPrev.read())
					return execOpS.read();
				flushPrev = flushS;
				return execOp;
			}
			
			virtual memOpsT getMemOp() {
				if (flushS.read() == flushPrev.read())
					return memOpS.read();
				flushPrev = flushS;
				return MEM_ALU_OUT;
			}
			
			virtual wbOpsT getWBOp() {
				if (flushS.read() == flushPrev.read())
					return wbOpS.read();
				flushPrev = flushS;
				return WB_NOP;
			}
			
			virtual sc_uint<32> getIR() {
				if (flushS.read() == flushPrev.read())
					return ir.read();
				flushPrev = flushS;
				return 0b00000000000000000000000000110011; //add r0, r0, r0
			}
			
			bool hazard_check() {
				sc_uint<5> rs1D, rs2D, rd, rs1T, rs2T;
				sc_int<32> imm;
				decodeOpsT op;
				sc_uint<32> irE, irM, irWB, irD;
				int tD, t;
				irD = fetchIn->getIR();
				irE = ir;
				irM = executeIf->getIR();
				irWB = memIf->getIR();
				tD = decodeOp(irD, &op, &rs1D, &rs2D, &rd, &imm);
				if (tD > 0) { // not NOP or U
					t = decodeOp(irE, &op, &rs1T, &rs2T, &rd, &imm);
					if (t >= 0 && t != 1) {	//not S or NOP
						if (rd == rs1D && rs1D != 0)
							return true;
						if (rd == rs2D && tD != 2 && rs2D != 0)
							return true;
					}
					t = decodeOp(irM, &op, &rs1T, &rs2T, &rd, &imm);
					if (t >= 0 && t != 1) {	//not S or NOP
						if (rd == rs1D && rs1D != 0)
							return true;
						if (rd == rs2D && tD != 2 && rs2D != 0)
							return true;
					}
					t = decodeOp(irWB, &op, &rs1T, &rs2T, &rd, &imm);
					if (t >= 0 && t != 1) {	//not S or NOP
						if (rd == rs1D && rs1D != 0)
							return true;
						if (rd == rs2D && tD != 2 && rs2D != 0)
							return true;
					}
				}
				return false;
			}
			
			void decode() {
				if (hazard_check()) {
					printf("\n\nHAZARD\n\n");
					ir.write(0b00000000000000000000000000110011);
					ExecInstrT execOp;
					execOpS.write(execOp);
					memOpS.write(MEM_ALU_OUT);
					wbOpS.write(WB_NOP);
					fetchIn->setPC(fetchIn->getPC());
					fetchIn->flush();
					return;
				}
				ExecInstrT execOp;
				sc_uint<32> opcode = fetchIn->getIR();
				ir.write(opcode);
				printf("DECODE: ");
				sc_int<32> imm;
				sc_uint<5> rd;
				sc_uint<5> rs1, rs2;
				execOp.pc = (sc_int<32>) fetchIn->getPC();
				switch(opcode & 0x7F) {
					case 0b0110111:
					case 0b0010111:
					case 0b1101111:		// U
						execOp.b = 0;
						execOp.a = 0;
						execOp.op = ALU_ADDPCI;
						memOpS.write(MEM_ALU_OUT);
						wbOpS.write(WB_WRITE_REG);
						switch (typeU(opcode, &imm, &rd)) {
							case LUI:
								execOp.op = ALU_ADDI;
								printf("LUI %d, %08x", (int) rd, (int) imm);
								break;
							case AUIPC:
								printf("AUIPC %d, %08x", (int) rd, (int) imm);
								break;
							case JAL:
								execOp.b = (sc_int<32>) fetchIn->getPC()+4;
								memOpS.write(MEM_RS2_AND_JUMP);
								printf("JAL %d, %08x", (int) rd, (int) imm);
								break;
							default:
								setNop();
								return;
						}
						execOp.imm = imm;
						break;
					case 0b1100011:
					case 0b0100011:		// S
						switch (typeS(opcode, &rs1, &rs2, &imm)) {
							case BEQ:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_EQ;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case BNE:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_NEQ;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case BLT:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_LT;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case BGE:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_NLT;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case BLTU:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_LTU;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case BGEU:
								aluIop("SB", rd, rs1, imm, ALU_ADDPCI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								execOp.op = ALU_ADDPCI_CMP_NLTU;
								memOpS.write(MEM_RS2_AND_COND_JUMP);
								wbOpS.write(WB_NOP);
								break;
							case SB:
								aluIop("SB", rd, rs1, imm, ALU_ADDI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2) & ((sc_int<32>) 0xFF);
								memOpS.write(MEM_WRITE8_RS2_OUT);
								wbOpS.write(WB_NOP);
								break;
							case SH:
								aluIop("SB", rd, rs1, imm, ALU_ADDI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2) & ((sc_int<32>) 0xFFFF);
								memOpS.write(MEM_WRITE16_RS2_OUT);
								wbOpS.write(WB_NOP);
								break;
							case SW:
								aluIop("SW", rd, rs1, imm, ALU_ADDI, &execOp);
								execOp.b = regIf->read((sc_uint<5>) rs2);
								memOpS.write(MEM_WRITE32_RS2_OUT);
								wbOpS.write(WB_NOP);
								break;
							default:
								setNop();
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
								aluIop("LB", rd, rs1, imm, ALU_ADDI, &execOp);
								memOpS.write(MEM_READ_LMD8S_OUT);
								break;
							case LH:
								aluIop("LH", rd, rs1, imm, ALU_ADDI, &execOp);
								memOpS.write(MEM_READ_LMD16S_OUT);
								break;
							case LW:
								aluIop("LW", rd, rs1, imm, ALU_ADDI, &execOp);
								memOpS.write(MEM_READ_LMD32_OUT);
								break;
							case LBU:
								aluIop("LB", rd, rs1, imm, ALU_ADDI, &execOp);
								memOpS.write(MEM_READ_LMD8U_OUT);
								break;
							case LHU:
								aluIop("LHU", rd, rs1, imm, ALU_ADDI, &execOp);
								memOpS.write(MEM_READ_LMD16U_OUT);
								break;
							case ADDI:
								aluIop("ALUI", rd, rs1, imm, ALU_ADDI, &execOp);
								break;
							case SLTI:
								aluIop("SLTI", rd, rs1, imm, ALU_SLTI, &execOp);
								break;
							case SLTIU:
								aluIop("SLTIU", rd, rs1, imm, ALU_SLTUI, &execOp);
								break;
							case XORI:
								aluIop("XORI", rd, rs1, imm, ALU_XORI, &execOp);
								break;
							case ORI:
								aluIop("ORI", rd, rs1, imm, ALU_ORI, &execOp);
								break;
							case ANDI:
								aluIop("ANDI", rd, rs1, imm, ALU_ANDI, &execOp);
								break;
							case SLLI:
								aluIop("SLLI", rd, rs1, imm, ALU_SLLI, &execOp);
								break;
							case SRLI:
								aluIop("SRLI", rd, rs1, imm, ALU_SRLI, &execOp);
								break;
							case SRAI:
								aluIop("SRAI", rd, rs1, imm, ALU_SRAI, &execOp);
								break;
							case ECALL:
								// TODO
								break;
							case EBREAK:
								// TODO
								break;
							case JALR:
								aluIop("JALR", rd, rs1, imm, ALU_ADDI, &execOp);
								execOp.b = (sc_int<32>) fetchIn->getPC()+4;
								memOpS.write(MEM_RS2_AND_JUMP);
								// TODO: force LSB to zero
								break;
							case FENCE:
								// TODO
								break;
							default:
								setNop();
								return;
						}
						break;
					case 0b0110011:		// R
						switch (typeR(opcode, &rs1, &rs2, &rd)) {
							case ADD:
								printf("ADD %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_ADD;
								break;
							case SUB:
								printf("SUB %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SUB;
								break;
							case SLL:
								printf("SLL %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SLL;
								break;
							case SLT:
								printf("SLT %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SLT;
								break;
							case SLTU:
								printf("SLTU %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SLTU;
								break;
							case XOR:
								printf("XOR %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_XOR;
								break;
							case SRL:
								printf("SRL %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SRL;
								break;
							case SRA:
								printf("SRA %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_SRA;
								break;
							case OR:
								printf("OR %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_OR;
								break;
							case AND:
								printf("AND %d, %d, %d", (int) rd, (int) rs1, (int) rs2);
								execOp.op = ALU_AND;
								break;
							default:
								setNop();
								return;
						}
						execOp.a = regIf->read((sc_uint<5>) rs1);
						execOp.b = regIf->read((sc_uint<5>) rs2);
						memOpS.write(MEM_ALU_OUT);
						wbOpS.write(WB_WRITE_REG);
						break;
					default:			// NOP
						setNop();
						return;
				}
				execOpS.write(execOp);
				printf("\n");
			}
			
		private:
			int decodeOp(unsigned int opcode, decodeOpsT *op, sc_uint<5> *rs1, sc_uint<5> *rs2, sc_uint<5> *rd, sc_int<32> *imm) {
				switch(opcode & 0x7F) {
					case 0b0110111:
					case 0b0010111:
					case 0b1101111:		// U
						*op = typeU(opcode, imm, rd);
						return 0;
					case 0b1100011:
					case 0b0100011:		// S
						*op = typeS(opcode, rs1, rs2, imm);
						return 1;
					case 0b0000011:
					case 0b0010011:
					case 0b1110011:
					case 0b1100111:
					case 0b0001111:		// I
						*op = typeI(opcode, rs1, imm, rd);
						return 2;
					case 0b0110011:		// R
						*op = typeR(opcode, rs1, rs2, rd);
						return 3;
					default:			// NOP
						*op = NOP;
						return -1;
				}
			}
			
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

			void setNop() {
				ExecInstrT execOp;
				execOp.a = 0;
				execOp.b = 0;
				execOp.op = ALU_ADD;			// ALU_OUT = 0+0
				execOp.imm = 0;
				execOp.pc = 0;
				execOpS.write(execOp);
				memOpS.write(MEM_ALU_OUT);		// LMD = ALU_OUT = 0
				wbOpS.write(WB_WRITE_REG);	// R0 is hardwired to 0 (does nothing)
				printf("ADD 0, 0, 0\n");
			}
			
			void aluIop(const char* opName, sc_uint<5> rd ,sc_uint<5> rs1, sc_int<32> imm, execOpsT eOp, ExecInstrT *execOp) {
				execOp->a = regIf->read((sc_uint<5>) rs1);
				if ((imm >> 11) & 1 == 1)
					execOp->imm = 0xFFFFF000 | imm;
				else
					execOp->imm = imm;
				execOp->b = 0;
				printf("%s %d, %d, %08x", opName, (int) rd, (int) rs1, (int) imm);
				execOp->op = eOp;
				memOpS.write(MEM_ALU_OUT);
				wbOpS.write(WB_WRITE_REG);
			}
	};
#endif