#include "systemc.h"
#include "../mem/mem_interface.cpp"

#include "decode/decode.cpp"





SC_MODULE (up) {
	private:
		sc_signal<sc_uint<32>> pc[2];
		sc_signal<sc_int<32>> regs[32]; // Registers
		sc_signal<sc_uint<32>> ir[5]; // 5 stage pipeline (Fetch, Decode, Execute, Memory, Write Back)

		// Decode
		DecodeStage dStage;
		
		// Execute
		//enum execOpsT {ALU_ADD};
		sc_signal<execOpsT> execOp;
		sc_signal<sc_int<32>> deA, deB;	// decode to exec
		// Memory
		//enum memOpsT {WRITE};
		sc_signal<memOpsT> memOp[2];
		sc_signal<sc_int<32>> aluOut, emRs2; // exec to mem
		// Write Back
		//enum wbOpsT {WRITE_REG};
		sc_signal<wbOpsT> wbOp[3];
		sc_signal<sc_uint<5>> rd[3];	// destination register for write back operation
		sc_signal<sc_int<32>> memOut; // mem to wb
		
	public:
	
	sc_in<bool> clock;
	sc_port<mem_if> imem;
	sc_port<mem_if> dmem;
	
	SC_CTOR (up) : dStage("DECODE") {
		// Init with default value
		regs[0] = 0;
		dStage.setRegFile(regs);
		pc[0] = 0;
		deA = 0;
		deB = 0;
		ir[0] = 0b00000000000000000000000000110011;
		execOp = ALU_ADD;
		for (int i = 0; i < 2; i++)
			memOp[i] = MEM_ALU_OUT;
		aluOut = 0;
		emRs2 = 0;
		for (int i=0; i < 3; i++) {
			wbOp[i] = WB_NOP;
			rd[i] = 0;
		}
		memOut = 0;
		SC_METHOD(cycle);
		sensitive << clock.pos();
	}
	
	bool pipelineEmpty() {
		return true;
	}
	
	void cycle() { 
		// TODO: add hazard check -> put NOP instead of fetched ir and don't increase pc
		// TODO: add jump prediction and pipeline flush
		printf("%d ns\n------------------------------\n", (int) sc_time().value());
		
		// Pipeline shifts
		// shift ir of instrs on the pipeline (usefull for hazards check??)
		for (int i = 1; i < 5; i++)
			ir[i] = ir[i-1];
		
		memOp[1] = memOp[0];
		
		pc[1] = pc[0];
		
		for (int i = 1; i < 3; i++) {
			wbOp[i] = wbOp[i-1];
			rd[i] = rd[i-1];
		}
		
		// Fetch
		printf("PC: %d\n", (int) pc[0].read());
		sc_uint<32> t = imem->read32(pc[0].read());
		ir[0] = t;
		printf("FETCH: %08x\n", (int) t);
		
		// next pc
		pc[0].write(pc[0].read()+4);
		
		// Decode
		dStage.decode(ir[0].read(), pc[1], &deA, &deB, &execOp, memOp, wbOp, rd);
		//Exec
		exec(execOp, deA, deB, &aluOut, &emRs2);
		// Memory
		memory(memOp[1], aluOut, emRs2, &memOut, pc);
		// Write Back
		wb(wbOp[2], memOut, rd[2]);
		printf("------------------------------\n\n");
	}
	
	private:
	
		
		
		void exec(execOpsT op, sc_int<32> a, sc_int<32> b, sc_signal<sc_int<32>> *aluOut, sc_signal<sc_int<32>> *emRs2) {
			// TODO: implement all operations
			*emRs2 = b;
			if (op == ALU_ADD) {
				printf("Exec: ALU_ADD\n");
				*aluOut = a+b;
			} else if (op == ALU_ADDU) {
				printf("Exec: ALU_ADDU\n");
				*aluOut = a+b;
			}
		}
		
		void memory(memOpsT op, sc_int<32> aluOut, sc_int<32> rs2, sc_signal<sc_int<32>> *memOut, sc_signal<sc_uint<32>> *pc) {
			// TODO: implement all operations
			if (op == MEM_ALU_OUT) {
				printf("Memory: MEM_ALU_OUT\n");
				*memOut = aluOut;
			} else if (op == MEM_ALU_OUT_AND_JUMP) {
				printf("Memory: MEM_ALU_OUT_AND_JUMP\n");
				*pc = (sc_uint<32>) aluOut;
				*memOut = aluOut;
				// TODO: flush pipeline
			} 
		}
		
		void wb(wbOpsT op, sc_int<32> in, sc_uint<5> rd) {
			// TODO: implement all operations
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