#include "systemc.h"
#include "..\isa.h"
#include "..\decode\decode_interface.cpp"
#include "execute_interface.cpp"

#ifndef EXECUTE_D
	#define EXECUTE_D
	SC_MODULE(ExecuteStage), public execute_if {
		private:
			sc_signal<MemInstrT> memOpS;
			sc_signal<wbOpsT> wbOpS;
			sc_signal<sc_uint<32>> ir;
			sc_signal<bool> flushS, flushPrev;
			
		public:
			
			sc_port<decode_if> decodeIf;
			sc_in<bool> clock;
			
			SC_CTOR(ExecuteStage) {
				flushS = false;
				flushPrev = false;
				SC_METHOD(exec);
				sensitive << clock.pos();
				dont_initialize();
			}
			
			virtual void flush() {
				flushS = !flushS;
			}
			
			virtual MemInstrT getMemOp() {
				MemInstrT memOp;
				if (flushS.read() == flushPrev.read())
					return memOpS.read();
				flushPrev = flushS;
				return memOp;
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
			
			void exec() {
				MemInstrT memOp;
				memOp.op = decodeIf->getMemOp();
				wbOpS = decodeIf->getWBOp();
				ir = decodeIf->getIR();
				memOp.comp = 0;
				memOp.rs2 = decodeIf->getExecOp().b;
				if (decodeIf->getExecOp().op == ALU_ADD) {
					printf("Exec: ALU_ADD\n");
					memOp.aluOut = decodeIf->getExecOp().a+decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_ADDU) {
					printf("Exec: ALU_ADDU\n");
					memOp.aluOut = ((sc_uint<32>)decodeIf->getExecOp().a)+((sc_uint<32>)decodeIf->getExecOp().b);
				} else if (decodeIf->getExecOp().op == ALU_SUB) {
					printf("Exec: ALU_SUB\n");
					memOp.aluOut = decodeIf->getExecOp().a-decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_SLT) {
					printf("Exec: ALU_SLT\n");
					memOp.aluOut = decodeIf->getExecOp().a; //TODO
				} else if (decodeIf->getExecOp().op == ALU_SLTU) {
					printf("Exec: ALU_SLTU\n");
					memOp.aluOut = decodeIf->getExecOp().a; //TODO
				} else if (decodeIf->getExecOp().op == ALU_XOR) {
					printf("Exec: ALU_XOR\n");
					memOp.aluOut = decodeIf->getExecOp().a^decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_OR) {
					printf("Exec: ALU_OR\n");
					memOp.aluOut = decodeIf->getExecOp().a|decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_AND) {
					printf("Exec: ALU_AND\n");
					memOp.aluOut = decodeIf->getExecOp().a&decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_SLL) {
					printf("Exec: ALU_SLL\n");
					memOp.aluOut = decodeIf->getExecOp().a<<decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_SRL) {
					printf("Exec: ALU_SRL\n");
					memOp.aluOut = decodeIf->getExecOp().a>>decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_SRA) {
					printf("Exec: ALU_SRA\n");
					sc_int<64> t = (sc_int<64>) decodeIf->getExecOp().a;
					memOp.aluOut = (sc_int<32>) t>>decodeIf->getExecOp().b;
				} else if (decodeIf->getExecOp().op == ALU_ADDI) {
					printf("Exec: ALU_ADDI\n");
					memOp.aluOut = decodeIf->getExecOp().a+decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_ADDUI) {
					printf("Exec: ALU_ADDUI\n");
					memOp.aluOut = ((sc_uint<32>)decodeIf->getExecOp().a)+((sc_uint<32>)decodeIf->getExecOp().imm);
				} else if (decodeIf->getExecOp().op == ALU_SUBI) {
					printf("Exec: ALU_SUBI\n");
					memOp.aluOut = decodeIf->getExecOp().a-decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_SLTI) {
					printf("Exec: ALU_SLTI\n");
					memOp.aluOut = decodeIf->getExecOp().a; //TODO
				} else if (decodeIf->getExecOp().op == ALU_SLTUI) {
					printf("Exec: ALU_SLTUI\n");
					memOp.aluOut = decodeIf->getExecOp().a; //TODO
				} else if (decodeIf->getExecOp().op == ALU_XORI) {
					printf("Exec: ALU_XORI\n");
					memOp.aluOut = decodeIf->getExecOp().a^decodeIf->getExecOp().imm;;
				} else if (decodeIf->getExecOp().op == ALU_ORI) {
					printf("Exec: ALU_ORI\n");
					memOp.aluOut = decodeIf->getExecOp().a|decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_ANDI) {
					printf("Exec: ALU_ANDI\n");
					memOp.aluOut = decodeIf->getExecOp().a&decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_SLLI) {
					printf("Exec: ALU_SLLI\n");
					memOp.aluOut = decodeIf->getExecOp().a << decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_SRLI) {
					printf("Exec: ALU_SRLI\n");
					memOp.aluOut = decodeIf->getExecOp().a >> decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_SRAI) {
					printf("Exec: ALU_SRAI\n");
					sc_int<64> t = (sc_int<64>) decodeIf->getExecOp().a;
					memOp.aluOut = (sc_int<32>) t>>decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI) {
					printf("Exec: ALU_ADDPCI %d %d\n", (int) decodeIf->getExecOp().pc, (int) decodeIf->getExecOp().imm);
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_EQ) {
					printf("Exec: ALU_ADDPCI_CMP_EQ\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (decodeIf->getExecOp().a == decodeIf->getExecOp().b)
						memOp.comp = 1;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_NEQ) {
					printf("Exec: ALU_ADDPCI_CMP_NEQ\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (decodeIf->getExecOp().a != decodeIf->getExecOp().b)
						memOp.comp = 1;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_LT) {
					printf("Exec: ALU_ADDPCI_CMP_LT\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (decodeIf->getExecOp().a < decodeIf->getExecOp().b)
						memOp.comp = 1;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_NLT) {
					printf("Exec: ALU_ADDPCI_CMP_NLT\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (decodeIf->getExecOp().a >= decodeIf->getExecOp().b)
						memOp.comp = 1;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_NLTU) {
					printf("Exec: ALU_ADDPCI_CMP_NLTU\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (((sc_uint<32>) decodeIf->getExecOp().a) < ((sc_uint<32>) decodeIf->getExecOp().b))
						memOp.comp = 1;
				} else if (decodeIf->getExecOp().op == ALU_ADDPCI_CMP_LTU) {
					printf("Exec: ALU_ADDPCI_CMP_LTU\n");
					memOp.aluOut = ((sc_int<32>) decodeIf->getExecOp().pc)+decodeIf->getExecOp().imm;
					if (((sc_uint<32>) decodeIf->getExecOp().a) >= ((sc_uint<32>) decodeIf->getExecOp().b))
						memOp.comp = 1;
				}
				memOpS.write(memOp);
			}
	};
#endif