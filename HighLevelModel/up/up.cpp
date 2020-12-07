#include "systemc.h"
#include "../mem/mem_interface.cpp"

#include "decode/decode.cpp"





SC_MODULE (up) {
	private:
		sc_signal<sc_uint<32>> pc[3];
		sc_signal<sc_int<32>> regs[32]; // Registers
		sc_signal<sc_uint<32>> ir[4]; // 5 stage pipeline (Fetch, Decode, Execute, Memory, Write Back)

		// Decode
		DecodeStage dStage;
		
		// Execute
		//enum execOpsT {ALU_ADD};
		sc_signal<execOpsT> execOp;
		sc_signal<sc_int<32>> deA, deB, deImm;	// decode to exec
		sc_signal<sc_uint<1>> comp;
		// Memory
		//enum memOpsT {WRITE};
		sc_signal<memOpsT> memOp[2];
		sc_signal<sc_int<32>> aluOut, emRs2; // exec to mem
		// Write Back
		//enum wbOpsT {WRITE_REG};
		sc_signal<wbOpsT> wbOp[3];
		sc_signal<sc_int<32>> memOut; // mem to wb
		
	public:
	
	sc_in<bool> clock;
	sc_port<mem_if> imem;
	sc_port<mem_if> dmem;
	
	SC_CTOR (up) : dStage("DECODE") {
		// Init with default value
		regs[0] = 0;
		dStage.setRegFile(regs);
		deA = 0;
		deB = 0;
		deImm = 0;
		comp = 0;
		for (int i=0; i < 4; i++)
			ir[0] = 0b00000000000000000000000000110011;
		execOp = ALU_ADD;
		for (int i = 0; i < 2; i++)
			memOp[i] = MEM_ALU_OUT;
		aluOut = 0;
		emRs2 = 0;
		for (int i=0; i < 3; i++) {
			wbOp[i] = WB_NOP;
			pc[0] = 0;
		}
		memOut = 0;
		SC_METHOD(cycle);
		sensitive << clock.pos();
	}
	
	bool pipelineEmpty() {
		return true;
	}
	
	void cycle() { 
		// TODO: add forwarding
		// TODO: add hazard check -> put NOP instead of fetched ir and don't increase pc
		// TODO: add jump prediction and pipeline flush
		printf("%d ns\n------------------------------\n", (int) sc_time().value());
		
		// Pipeline shifts
		// shift ir of instrs on the pipeline (usefull for hazards check??)
		for (int i = 1; i < 4; i++)
			ir[i] = ir[i-1];
		
		memOp[1] = memOp[0];
		
		for (int i = 1; i < 3; i++) {
			wbOp[i] = wbOp[i-1];
			pc[i] = pc[i-1];
		}
		
		// Fetch
		printf("PC: %d\n", (int) pc[0].read());
		sc_uint<32> t = (sc_uint<32>) imem->read32(pc[0].read());
		ir[0] = t;
		printf("FETCH: %08x\n", (int) t);
		
		// next pc
		pc[0].write(pc[0].read()+4);
		
		// Decode
		dStage.decode(ir[0].read(), pc[1], &deA, &deB, &deImm, &execOp, memOp, wbOp);
		//Exec
		exec(execOp, deA, deB, deImm, pc[2], &aluOut, &emRs2, &comp);
		// Memory
		if(memory(memOp[1], aluOut, emRs2, &memOut, pc, comp)) {
			// Flush pipeline
			ir[0] = 0b00000000000000000000000000110011;
			ir[1] = 0b00000000000000000000000000110011;
			ir[2] = 0b00000000000000000000000000110011;
			execOp = ALU_ADD;
			memOp[1] = MEM_ALU_OUT;
			memOp[0] = MEM_ALU_OUT;
			wbOp[1] = WB_NOP;
			wbOp[0] = WB_NOP;
		}
		// Write Back
		wb(wbOp[2], memOut, ir[3]);
		printf("------------------------------\n\n");
	}
	
	private:
	
		
		
		void exec(execOpsT op, sc_int<32> a, sc_int<32> b, sc_int<32> imm, sc_uint<32> pc, sc_signal<sc_int<32>> *aluOut, sc_signal<sc_int<32>> *emRs2, sc_signal<sc_uint<1>> *comp) {
			// TODO: implement all operations
			*comp = 0;
			*emRs2 = b;
			
			
			if (op == ALU_ADD) {
				printf("Exec: ALU_ADD\n");
				*aluOut = a+b;
			} else if (op == ALU_ADDU) {
				printf("Exec: ALU_ADDU\n");
				*aluOut = ((sc_uint<32>)a)+((sc_uint<32>)b);
			} else if (op == ALU_SUB) {
				printf("Exec: ALU_SUB\n");
				*aluOut = a-b;
			} else if (op == ALU_SLT) {
				printf("Exec: ALU_SLT\n");
				*aluOut = a; //TODO
			} else if (op == ALU_SLTU) {
				printf("Exec: ALU_SLTU\n");
				*aluOut = a; //TODO
			} else if (op == ALU_XOR) {
				printf("Exec: ALU_XOR\n");
				*aluOut = a^b;
			} else if (op == ALU_OR) {
				printf("Exec: ALU_OR\n");
				*aluOut = a|b;
			} else if (op == ALU_AND) {
				printf("Exec: ALU_AND\n");
				*aluOut = a&b;
			} else if (op == ALU_SLL) {
				printf("Exec: ALU_SLL\n");
				*aluOut = a<<b;
			} else if (op == ALU_SRL) {
				printf("Exec: ALU_SRL\n");
				*aluOut = a>>b;
			} else if (op == ALU_SRA) {
				printf("Exec: ALU_SRA\n");
				sc_int<64> t = (sc_int<64>) a;
				*aluOut = (sc_int<32>) t>>b;
			} else if (op == ALU_ADDI) {
				printf("Exec: ALU_ADDI\n");
				*aluOut = a+imm;
			} else if (op == ALU_ADDUI) {
				printf("Exec: ALU_ADDUI\n");
				*aluOut = ((sc_uint<32>)a)+((sc_uint<32>)imm);
			} else if (op == ALU_SUBI) {
				printf("Exec: ALU_SUBI\n");
				*aluOut = a-imm;
			} else if (op == ALU_SLTI) {
				printf("Exec: ALU_SLTI\n");
				*aluOut = a; //TODO
			} else if (op == ALU_SLTUI) {
				printf("Exec: ALU_SLTUI\n");
				*aluOut = a; //TODO
			} else if (op == ALU_XORI) {
				printf("Exec: ALU_XORI\n");
				*aluOut = a^imm;;
			} else if (op == ALU_ORI) {
				printf("Exec: ALU_ORI\n");
				*aluOut = a|imm;
			} else if (op == ALU_ANDI) {
				printf("Exec: ALU_ANDI\n");
				*aluOut = a&imm;
			} else if (op == ALU_SLLI) {
				printf("Exec: ALU_SLLI\n");
				*aluOut = a << imm;
			} else if (op == ALU_SRLI) {
				printf("Exec: ALU_SRLI\n");
				*aluOut = a >> imm;
			} else if (op == ALU_SRAI) {
				printf("Exec: ALU_SRAI\n");
				sc_int<64> t = (sc_int<64>) a;
				*aluOut = (sc_int<32>) t>>imm;
			} else if (op == ALU_ADDPCI) {
				printf("Exec: ALU_ADDPCI %d %d\n", (int) pc, (int) imm);
				*aluOut = ((sc_int<32>) pc)+imm;
			} else if (op == ALU_ADDPCI_CMP_EQ) {
				printf("Exec: ALU_ADDPCI_CMP_EQ\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (a == b)
					*comp = 1;
			} else if (op == ALU_ADDPCI_CMP_NEQ) {
				printf("Exec: ALU_ADDPCI_CMP_NEQ\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (a != b)
					*comp = 1;
			} else if (op == ALU_ADDPCI_CMP_LT) {
				printf("Exec: ALU_ADDPCI_CMP_LT\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (a < b)
					*comp = 1;
			} else if (op == ALU_ADDPCI_CMP_NLT) {
				printf("Exec: ALU_ADDPCI_CMP_NLT\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (a >= b)
					*comp = 1;
			} else if (op == ALU_ADDPCI_CMP_NLTU) {
				printf("Exec: ALU_ADDPCI_CMP_NLTU\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (((sc_uint<32>) a) < ((sc_uint<32>) b))
					*comp = 1;
			} else if (op == ALU_ADDPCI_CMP_LTU) {
				printf("Exec: ALU_ADDPCI_CMP_LTU\n");
				*aluOut = ((sc_int<32>) pc)+imm;
				if (((sc_uint<32>) a) >= ((sc_uint<32>) b))
					*comp = 1;
			}
			
		}
		
		bool memory(memOpsT op, sc_int<32> aluOut, sc_int<32> rs2, sc_signal<sc_int<32>> *memOut, sc_signal<sc_uint<32>> *pc, sc_uint<1> comp) {
			// TODO: implement all operations
			// 
			sc_int<32> ts;
			sc_uint<32> tu;
			if (op == MEM_ALU_OUT) {
				printf("Memory: MEM_ALU_OUT\n");
				*memOut = aluOut;
			} else if (op == MEM_RS2_AND_JUMP) {
				printf("Memory: MEM_ALU_OUT_AND_JUMP\n");
				*pc = (sc_uint<32>) aluOut;
				*memOut = rs2;
				// flush pipeline
				return true;
			} else if (op == MEM_RS2_AND_COND_JUMP) {
				printf("Memory: MEM_ALU_OUT_AND_JUMP\n");
				if (comp == 1) {
					*pc = (sc_uint<32>) aluOut;
					// flush pipeline
					return true;
				}
				*memOut = rs2;
			} else if (op == MEM_READ_LMD8S_OUT) {
				printf("Memory: MEM_READ_LMD8S_OUT\n");
				ts = dmem->read8((unsigned int) aluOut);
				*memOut = ts;
			} else if (op == MEM_READ_LMD16S_OUT) {
				printf("Memory: MEM_READ_LMD16S_OUT\n");
				ts = dmem->read16((unsigned int) aluOut);
				*memOut = ts;
			} else if (op == MEM_READ_LMD32_OUT) {
				printf("Memory: MEM_READ_LMD32_OUT\n");
				*memOut = dmem->read32((unsigned int) aluOut);
			} else if (op == MEM_READ_LMD8U_OUT) {
				printf("Memory: MEM_READ_LMD8U_OUT\n");
				tu = (sc_uint<8>) dmem->read8((unsigned int) aluOut);
				*memOut = (sc_int<32>) tu;
			} else if (op == MEM_READ_LMD16U_OUT) {
				printf("Memory: MEM_READ_LMD16U_OUT\n");
				tu = (sc_uint<16>) dmem->read16((unsigned int) aluOut);
				*memOut = (sc_int<32>) tu;
			} else if (op == MEM_WRITE8_RS2_OUT) {
				printf("Memory: MEM_WRITE8_RS2_OUT\n");
				dmem->write8((unsigned int) aluOut, (sc_int<8>) rs2);
				*memOut = rs2;
			} else if (op == MEM_WRITE16_RS2_OUT) {
				printf("Memory: MEM_WRITE16_RS2_OUT\n");
				dmem->write16((unsigned int) aluOut, (sc_int<16>) rs2);
				*memOut = rs2;
			} else if (op == MEM_WRITE32_RS2_OUT) {
				printf("Memory: MEM_WRITE32_RS2_OUT\n");
				dmem->write32((unsigned int) aluOut, (sc_int<32>)rs2);
				*memOut = rs2;
			}
			return false;
		}
		
		void wb(wbOpsT op, sc_int<32> in, sc_uint<32> ir) {
			// TODO: implement all operations
			sc_uint<5> rd = (ir & 0xF80) >> 7;
			switch (op) {
				case WB_WRITE_REG:
					printf("WB: WB_WRITE_REG\n");
					if (rd != 0)
						regs[(int) rd] = in;
					break;
				default:	// WB_NOP
					printf("WB: WB_NOP\n");
			}
		}
};