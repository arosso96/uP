#include "systemc.h"
#include "..\isa.h"
#include "..\execute\execute_interface.cpp"
#include "..\decode\decode_interface.cpp"
#include "memory_interface.cpp"

#ifndef MEMORY_D
	#define MEMORY_D
	SC_MODULE(MemoryStage), public memory_if {
		private:
			sc_signal<WBInstrT> wbOpS;
			sc_signal<sc_uint<32>> ir;
		public:
			
			sc_port<execute_if> executeIf;
			sc_port<decode_if> decodeIf;
			// jump
			sc_port<fetch_if> fetchIf;
			sc_port<mem_if> dmem;
			sc_in<bool> clock;
			
			SC_CTOR(MemoryStage) {
				SC_METHOD(memory);
				sensitive << clock.pos();
				dont_initialize();
			}
			
			virtual WBInstrT getWBOp() {
				return wbOpS.read();
			}
			virtual sc_uint<32> getIR() {
				return ir.read();
			}
			
			void memory() {
				sc_int<32> ts;
				sc_uint<32> tu;
				WBInstrT wbOp;
				wbOp.op = executeIf->getWBOp();
				ir = executeIf->getIR();
				if (executeIf->getMemOp().op == MEM_ALU_OUT) {
					printf("Memory: MEM_ALU_OUT\n");
					wbOp.lmd = executeIf->getMemOp().aluOut;
				} else if (executeIf->getMemOp().op == MEM_RS2_AND_JUMP) {
					printf("Memory: MEM_RS2_OUT_AND_JUMP\n");
					// jump to aluOut
					fetchIf->setPC((sc_uint<32>) executeIf->getMemOp().aluOut);
					printf("\nWRITING JUMP SIGNAL TO TRUE\n");
					wbOp.lmd = executeIf->getMemOp().rs2;
					// flush pipeline
					fetchIf->flush();
					decodeIf->flush();
					executeIf->flush();
				} else if (executeIf->getMemOp().op == MEM_RS2_AND_COND_JUMP) {
					printf("Memory: MEM_RS2_OUT_AND_COND_JUMP\n");
					if (executeIf->getMemOp().comp == 1) {
						// jump to aluOut
						fetchIf->setPC((sc_uint<32>)executeIf->getMemOp().aluOut);
						// flush pipeline
						fetchIf->flush();
						decodeIf->flush();
						executeIf->flush();
					}
					wbOp.lmd = executeIf->getMemOp().rs2;
				} else if (executeIf->getMemOp().op == MEM_READ_LMD8S_OUT) {
					printf("Memory: MEM_READ_LMD8S_OUT\n");
					ts = dmem->read8((unsigned int) executeIf->getMemOp().aluOut);
					wbOp.lmd = ts;
				} else if (executeIf->getMemOp().op == MEM_READ_LMD16S_OUT) {
					printf("Memory: MEM_READ_LMD16S_OUT\n");
					ts = dmem->read16((unsigned int) executeIf->getMemOp().aluOut);
					wbOp.lmd = ts;
				} else if (executeIf->getMemOp().op == MEM_READ_LMD32_OUT) {
					printf("Memory: MEM_READ_LMD32_OUT\n");
					wbOp.lmd = dmem->read32((unsigned int) executeIf->getMemOp().aluOut);
				} else if (executeIf->getMemOp().op == MEM_READ_LMD8U_OUT) {
					printf("Memory: MEM_READ_LMD8U_OUT\n");
					tu = (sc_uint<8>) dmem->read8((unsigned int) executeIf->getMemOp().aluOut);
					wbOp.lmd = (sc_int<32>) tu;
				} else if (executeIf->getMemOp().op == MEM_READ_LMD16U_OUT) {
					printf("Memory: MEM_READ_LMD16U_OUT\n");
					tu = (sc_uint<16>) dmem->read16((unsigned int) executeIf->getMemOp().aluOut);
					wbOp.lmd = (sc_int<32>) tu;
				} else if (executeIf->getMemOp().op == MEM_WRITE8_RS2_OUT) {
					printf("Memory: MEM_WRITE8_RS2_OUT\n");
					dmem->write8((unsigned int) executeIf->getMemOp().aluOut, (sc_int<8>) executeIf->getMemOp().rs2);
					wbOp.lmd = executeIf->getMemOp().rs2;
				} else if (executeIf->getMemOp().op == MEM_WRITE16_RS2_OUT) {
					printf("Memory: MEM_WRITE16_RS2_OUT\n");
					dmem->write16((unsigned int) executeIf->getMemOp().aluOut, (sc_int<16>) executeIf->getMemOp().rs2);
					wbOp.lmd = executeIf->getMemOp().rs2;
				} else if (executeIf->getMemOp().op == MEM_WRITE32_RS2_OUT) {
					printf("Memory: MEM_WRITE32_RS2_OUT\n");
					dmem->write32((unsigned int) executeIf->getMemOp().aluOut, (sc_int<32>) executeIf->getMemOp().rs2);
					wbOp.lmd = executeIf->getMemOp().rs2;
				}
				wbOpS.write(wbOp);
			}
	};
#endif