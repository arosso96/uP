#include "systemc.h"
#include "../constants.h"
#include "mem_interface.cpp"

SC_MODULE (dmem), public mem_if {
	private:
		unsigned int memData[DMEM_SIZE];
		FILE *fLog;
	public:
	
	SC_CTOR (dmem) {
		fLog = fopen("dmem.log", "w");
		fclose(fLog);
		for(int i = 0; i < DMEM_SIZE; i++)
			memData[i] = 0;
	}
  
	virtual unsigned int read32(unsigned int addr) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM READ @  %014x, READ %032x\n", (int) sc_time().value(), (int) addr, (int) memData[addr]);
			return memData[addr];
		}
		fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time().value(), (int) addr);
		fclose(fLog);
		return 0;
	}
	virtual void write32(unsigned int addr, unsigned int data) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM WRITE @  %014x, WROTE %032x\n", (int) sc_time().value(), (int) addr, (int) data);
			memData[addr] = data;
		} else
			fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time().value(), (int) addr);
		fclose(fLog);
	}
	void load(unsigned int data[], int size) {
		fLog = fopen("dmem.log", "a");
		memcpy(memData, data, size);
		fprintf(fLog, "%d DMEM LOAD\n", (int) sc_time().value());
		fclose(fLog);
	}
};