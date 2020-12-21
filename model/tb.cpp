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
		unsigned int mR[14] = {
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		0 	  	FDEMW
			0b10101010101010101010000100010111, // auipc r2, 0xAAAAA							4		 FDEMW
			0b00000000001000000000000010110011, // add r0, r2, r1 -> mov r2 content in r1		8		  FSSSDEMW
			0b00000000000100011000000110010011, // addi r3, 1, r3 -> 							12			  FDEMW
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		16
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		20
			0b00000001100000000000000001101111, // jal (jump to instr before sw r2 48-24 = +24)	24
			0b10101010101010101010000100010111, // auipc r2, 0xAAAAA							28
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		32
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		36
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		40
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		44
			0b00000000000000000000000000110011, // add r0, r0, r0 -> mov r0 content in r0		48
			0b00000000001000000010000000100011  // sw r2, dmem[0]								52
		};
		im.load(mR, 14);
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
