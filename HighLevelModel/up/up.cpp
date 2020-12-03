#include "systemc.h"
#include "../mem/mem_interface.cpp"

SC_MODULE (up) {
	
	private:
		sc_uint<32> pc;
		sc_int<32> regs[32]; // Registers
		sc_uint<32> ir[5]; // 5 stage pipeline (Fetch, Decode, Execute, Memory, Write Back)
		// Execute
		enum execOpsT {ALU_ADD};
		sc_signal<execOpsT> execOp;
		sc_int<32> deA, deB;	// decode to exec
		// Memory
		enum memOpsT {WRITE};
		sc_signal<memOpsT> memOp[2];
		sc_int<32> aluOut, emRs2; // exec to mem
		// Write Back
		enum wbOpsT {WRITE_REG};
		sc_signal<wbOpsT> wbOp[3];
		sc_int<32> memOut; // mem to wb
		
	public:
	
	sc_in<bool> clock;
	sc_port<mem_if> imem;
	sc_port<mem_if> dmem;
	
	SC_CTOR (up) {
		pc = 0;
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
		
		for (int i = 1; i < 3; i++)
			wbOp[i] = wbOp[i-1];
		
		// Fetch
		ir[0] = imem->read32(pc++);
		decode(ir[0], &deA, &deB, &execOp, memOp, wbOp);
		exec(execOp, deA, deB, &aluOut, &emRs2);
		memory(memOp[1], aluOut, emRs2, &memOut);
		wb(wbOp[2], memOut, ir[5]);
	}
	
	private:
		void decode(unsigned int opcode, sc_int<32> *a, sc_int<32> *b, sc_signal<execOpsT> *eO, sc_signal<memOpsT> mO[], sc_signal<wbOpsT> wbO[]) {
			// TODO: parse opcode
			*a = 1;
			*b = 2;
			*eO = ALU_ADD;
			mO[0] = WRITE;
			wbO[0] = WRITE_REG;
		}
		
		void exec(execOpsT op, sc_int<32> a, sc_int<32> b, sc_int<32> *aluOut, sc_int<32> *emRs2) {
			// TODO: implement all operations
			*emRs2 = b;
			if (op == ALU_ADD)
				*aluOut = a+b;
		}
		
		void memory(memOpsT op, sc_int<32> aluOut, sc_int<32> rs2, sc_int<32> *memOut) {
			// TODO: implement all operations
			if (op == WRITE) {
				dmem->write32(aluOut, rs2);
				*memOut = rs2;
			}
		}
		
		void wb(wbOpsT op, sc_int<32> in, sc_uint<32> ir) {
			// TODO: implement all operations
			if (op == WRITE_REG)
				regs[ir & 0x1F] = in;
		}
};