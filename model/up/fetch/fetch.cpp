#include "systemc.h"
#include "../isa.h"
#include "fetch_interface.cpp"
#include "../../mem/mem_interface.cpp"

#ifndef FETCH_D
	#define FETCH_D
	SC_MODULE(FetchStage), public fetch_if {
		private:
			sc_signal<sc_uint<32>> ir, pc;
			sc_signal<sc_uint<32>, SC_MANY_WRITERS> jpc;
			sc_signal<bool, SC_MANY_WRITERS> jump, jumpPrev, flushS, flushPrev;
			int c;
		
		public:
			sc_port<mem_if> imem;
			sc_in<bool> clock;
			
			SC_CTOR(FetchStage) {
				ir = 0b00000000000000000000000000110011;
				jump = false;
				jumpPrev = false;
				flushS = false;
				flushPrev = false;
				pc.write(0);
				SC_METHOD(fetch);
				sensitive << clock.pos();
				dont_initialize();
			}
			
			void fetch() {
				int pc_out = (int) pc.read();
				if (jump != jumpPrev) {
					jumpPrev = jump;
					ir = (sc_uint<32>) imem->read32((unsigned int) jpc.read());
					pc_out = (int) jpc.read();
					pc.write(jpc.read() + 4);
				}
				else {
					ir = (sc_uint<32>) imem->read32((unsigned int) pc.read());
					pc.write(pc.read() + 4);
				}
				printf("----- %d -----\nPC: %08x\nFETCH: %08x\n", (int) sc_time_stamp().to_default_time_units(), pc_out, (int)ir.read());
			}
			
			virtual sc_uint<32> getIR() {
				if (flushS.read() == flushPrev.read())
					return ir.read();
				flushPrev = flushS;
				return 0b00000000000000000000000000110011; //add r0, r0, r0
				
			}
			
			virtual sc_uint<32> getPC() {
				return this->pc.read()-4;
			}
			
			virtual void setPC(sc_uint<32> pc) {
				this->jpc.write(pc);
				jump = !jump;
			}
			
			virtual void flush() {
				flushS = !flushS;
			}
	};
#endif