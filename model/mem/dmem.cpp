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
  
	virtual sc_int<32> read32(unsigned int addr) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM READ @  %014x, READ %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) memData[addr]);
			return (sc_int<32>) memData[addr];
		}
		fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
		return 0;
	}
	virtual sc_int<16> read16(unsigned int addr) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM READ @  %014x, READ %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) memData[addr]);
			return (sc_int<16>) memData[addr];
		}
		fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
		return 0;
	}
	virtual sc_int<8> read8(unsigned int addr) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM READ @  %014x, READ %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) memData[addr]);
			return (sc_int<8>) memData[addr];
		}
		fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
		return 0;
	}
	virtual void write32(unsigned int addr, sc_int<32> data) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM WRITE @  %014x, WROTE %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) data);
			memData[addr] = (unsigned int) data;
		} else
			fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
	}
	virtual void write16(unsigned int addr, sc_int<16> data) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM WRITE @  %014x, WROTE %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) data);
			memData[addr] = (unsigned int) data; //TODO: correct ??
		} else
			fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
	}
	virtual void write8(unsigned int addr, sc_int<8> data) {
		addr = addr/4;
		fLog = fopen("dmem.log", "a");
		if (addr < DMEM_SIZE) {
			fprintf(fLog, "%d DMEM WRITE @  %014x, WROTE %032x\n", (int) sc_time_stamp().to_default_time_units(), (int) addr, (int) data);
			memData[addr] = (unsigned int) data; //TODO: correct ??
		} else
			fprintf(fLog, "%d DMEM READ, ADDRESS %d OUT OF RANGE\n", (int) sc_time_stamp().to_default_time_units(), (int) addr);
		fclose(fLog);
	}
	void load(unsigned int data[], int size) {
		fLog = fopen("dmem.log", "a");
		memcpy(memData, data, size);
		fprintf(fLog, "%d DMEM LOAD\n", (int) sc_time().value());
		fclose(fLog);
	}
};