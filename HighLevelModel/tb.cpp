#include "systemc.h"
#include "up/up.cpp"
#include "mem/dmem.cpp"
#include "mem/imem.cpp"
#include "constants.h"

SC_MODULE (tb) {
	private:
		sc_signal<bool> c;
		up cpu;
		imem im;
		dmem dm;
	
	public:
	
	SC_CTOR (tb) : cpu("CPU"), im("IRAM"), dm("DRAM")
	{
		unsigned int mR[4] = {
			0b00000000000100000000000100110011, // add r0, r1, r2 -> mov r1 content in r2
			0b00000000010000000000000011101111, // jal 0x00004, r1
			0b01000000000000001000000100110011, // sub r1, r0, r2 -> r2=r1-r0
			0b10101010101010101010000010010111 // auipc r2, 0xAAAAA
		};
		im.load(mR, 4);
		dm.load(mR, 0);
		cpu.imem(im);
		cpu.dmem(dm);
		cpu.clock(c);
		SC_THREAD(genClock);
	}
	
	void genClock() {
		while(1) {
			c = !c;
			wait(5, SC_NS); // 5ns => f = 100MHz
		}
	}
};


// sc_main in top level function like in C++ main
int sc_main(int argc, char* argv[]) {
	tb test("TEST_BENCH");
	sc_start(sc_time(STOP_TIME, SC_NS));
	return(0);
}
