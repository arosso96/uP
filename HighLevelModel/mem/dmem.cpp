#include "systemc.h"
#include "../constants.h"
#include "mem_interface.cpp"

SC_MODULE (dmem), public mem_if {
	private:
		unsigned int memData[DMEM_SIZE];
		
	public:
	SC_CTOR (dmem) {
		for(int i = 0; i < DMEM_SIZE; i++)
			memData[i] = 0;
	}
  
	virtual unsigned int read32(unsigned int addr) {
		if (addr < DMEM_SIZE) {
			cout << sc_time_stamp() << "DMEM READ @ " << addr << ", READ" << memData[addr] << "\n";
			return memData[addr];
		}
		cout << sc_time_stamp() << "DMEM READ, ADDRESS " << addr << " OUT OF RANGE\n";
		return 0;
	}
	virtual void write32(unsigned int addr, unsigned int data) {
		cout << sc_time_stamp() << "DMEM WRITE @ " << addr << ", WROTE" << data << "\n";
		if (addr < DMEM_SIZE)
			memData[addr] = data;
	}
	void load(unsigned int data[], int size) {
		memcpy(memData, data, size);
		cout << sc_time_stamp() << "DMEM LOAD\n";
	}
};