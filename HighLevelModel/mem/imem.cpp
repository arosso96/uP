#include "systemc.h"
#include "../constants.h"
#include "mem_interface.cpp"

SC_MODULE (imem), public mem_if {
	private:
		unsigned int memData[IMEM_SIZE];
		
	public:
	SC_CTOR (imem) {
		for(unsigned int i = 0; i < IMEM_SIZE; i++)
			memData[i] = 0;
	}
  
	virtual unsigned int read32(unsigned int addr) {
		if (addr < IMEM_SIZE) {
			cout << sc_time_stamp() << "IMEM READ @ " << addr << ", READ" << memData[addr] << "\n";
			return memData[addr];
		}
		cout << sc_time_stamp() << "IMEM READ, ADDRESS " << addr << " OUT OF RANGE\n";
		return 0;
	}
	virtual void write32(unsigned int addr, unsigned int data) {
		cout << sc_time_stamp() << "ERROR IMEM CANNOT BE WROTE\n";
	}
	void load(unsigned int data[], int size) {
		memcpy(memData, data, size);
		cout << sc_time_stamp() << "IMEM LOAD\n";
	}
};