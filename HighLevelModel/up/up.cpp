#include "systemc.h"
#include "../mem/mem_interface.cpp"

#include "decode/decode_tmp.cpp"





SC_MODULE (up) {
		// TODO: improve printf
	private:
		sc_signal<sc_uint<32>> pc;
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
		dStage.setRegFile(regs);
		pc = 0;
		deA = 0;
		deB = 0;
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
		dont_initialize();
	}
	
	bool pipelineEmpty() {
		return true;
	}
	
	void cycle() { 
		// TODO: add hazard check -> put NOP instead of fetched ir and don't increase pc
		// TODO: add jump prediction and pipeline flush
		
		// shift ir of instrs on the pipeline (usefull for hazards check??)
		for (int i = 1; i < 5; i++)
			ir[i] = ir[i-1];
		
		memOp[1] = memOp[0];
		
		for (int i = 1; i < 3; i++) {
			wbOp[i] = wbOp[i-1];
			rd[i] = rd[i-1];
		}
		
		// Fetch
		ir[0] = imem->read32(pc.read());
		// next pc
		pc.write(pc.read()+1);
		dStage.decode(ir[0].read(), &deA, &deB, &execOp, memOp, wbOp, rd);
		exec(execOp, deA, deB, &aluOut, &emRs2);
		memory(memOp[1], aluOut, emRs2, &memOut);
		wb(wbOp[2], memOut, rd[2]);
	}
	
	private:
	
		
		
		void exec(execOpsT op, sc_int<32> a, sc_int<32> b, sc_signal<sc_int<32>> *aluOut, sc_signal<sc_int<32>> *emRs2) {
			// TODO: implement all operations
			*emRs2 = b;
			if (op == ALU_ADD) {
				printf("ALU_ADD\n");
				*aluOut = a+b;
			}
		}
		
		void memory(memOpsT op, sc_int<32> aluOut, sc_int<32> rs2, sc_signal<sc_int<32>> *memOut) {
			// TODO: implement all operations
			if (op == MEM_ALU_OUT) {
				printf("MEM_ALU_OUT\n");
				*memOut = aluOut;
			}
		}
		
		void wb(wbOpsT op, sc_int<32> in, sc_uint<5> rd) {
			// TODO: implement all operations
			switch (op) {
				case WB_WRITE_REG:
					printf("WB_WRITE_REG\n");
					regs[(int) rd] = in;
					break;
				default:	// WB_NOP
					printf("WB_NOP");
			}
		}
};