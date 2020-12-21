#include "systemc.h"
#include "..\isa.h"
#include "..\memory\memory_interface.cpp"
#include "writeback_interface.cpp"
#include "..\regfile_interface.cpp"

#ifndef WB_D
	#define WB_D
	SC_MODULE(WBStage), public wb_if {
		private:
			sc_signal<sc_uint<32>> ir;
		public:
			sc_port<regfile_if> regIf;
			sc_port<memory_if> memoryIf;
			sc_in<bool> clock;
			
			SC_CTOR(WBStage) {
				SC_METHOD(wb);
				sensitive << clock.pos();
				dont_initialize();
			}
			
			virtual sc_uint<32> getIR() {
				return ir.read();
			}
			
			void wb() {
				ir = memoryIf->getIR();
				sc_uint<5> rd = (memoryIf->getIR() & 0xF80) >> 7;
				switch (memoryIf->getWBOp().op) {
					case WB_WRITE_REG:
						printf("WB: WB_WRITE_REG\n");
						if (rd != 0)
							regIf->write(rd, memoryIf->getWBOp().lmd);
						break;
					default:	// WB_NOP
						printf("WB: WB_NOP\n");
				}
			}
	};
#endif