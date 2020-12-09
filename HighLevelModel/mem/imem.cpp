#include "systemc.h"
#include "../constants.h"
#include "mem_interface.cpp"

SC_MODULE (imem), public mem_if {
	private:
		unsigned int memData[IMEM_SIZE];
		FILE *fLog;
		
	public:
	SC_CTOR (imem) {
		fLog = fopen("imem.log", "w");
		fclose(fLog);
		for(unsigned int i = 0; i < IMEM_SIZE; i++)
			memData[i] = 0;
	}
  
	virtual sc_int<32> read32(unsigned int addr) {
		addr = addr/4;
		fLog = fopen("imem.log", "a");
		if (addr < IMEM_SIZE) {
			fprintf(fLog, "%d IMEM READ @  %014x, READ %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) memData[addr]);
			return (sc_int<32>) memData[addr];
		}
		fprintf(fLog, "%d IMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
		return 0;
	}
	virtual sc_int<16> read16(unsigned int addr) {
		return 0;
	}
	virtual sc_int<8> read8(unsigned int addr) {
		return 0;
	}
	virtual void write32(unsigned int addr, sc_int<32> data) {
		addr = addr/4;
		fLog = fopen("imem.log", "a");
		fprintf(fLog, "%d ERROR IMEM CANNOT BE WROTE\n", (int) sc_time_stamp().to_default_time_units());
		fclose(fLog);
	}
	virtual void write16(unsigned int addr, sc_int<16> data) {
	}
	virtual void write8(unsigned int addr, sc_int<8> data) {
	}
	void load(unsigned int data[], int size) {
		fLog = fopen("imem.log", "a");
		for(int i=0; i<IMEM_SIZE;i++)
			if(i<size)
				memData[i]=data[i];
			else
				memData[i] = 0b00000000000000000000000000110011; //NOP (add r0, r0, r0)
		fprintf(fLog, "%d IMEM LOAD\n", (int) sc_time().value());
		fclose(fLog);
	}
};