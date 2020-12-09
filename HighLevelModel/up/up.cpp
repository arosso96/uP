#include "systemc.h"
#include "../mem/mem_interface.cpp"

#include "fetch/fetch.cpp"
#include "decode/decode.cpp"
#include "execute/execute.cpp"
#include "memory/memory.cpp"
#include "writeback/writeback.cpp"
#include "regfile.cpp"


SC_MODULE (up) {
	private:
		//Regfile
		Regfile regfile;
	
		// Fetch
		FetchStage fStage;

		// Decode
		DecodeStage dStage;
		
		// Execute
		ExecuteStage eStage;
		
		// Memory
		MemoryStage mStage;
		
		// Write Back
		WBStage wbStage;
	public:
	
	sc_in<bool> clock;
	sc_port<mem_if> imem;
	sc_port<mem_if> dmem;
	
	SC_CTOR (up) : fStage("FETCH"), dStage("DECODE"), eStage("EXECUTE"), mStage("MEMORY"), wbStage("WB"), regfile("REGFILE") {
		fStage.imem(imem);
		fStage.clock(clock);
		
		dStage.regIf(regfile);
		dStage.fetchIn(fStage);
		dStage.clock(clock);
		
		eStage.decodeIf(dStage);
		eStage.clock(clock);
		
		mStage.dmem(dmem);
		mStage.fetchIf(fStage);
		mStage.decodeIf(dStage);
		mStage.executeIf(eStage);
		mStage.clock(clock);
		
		wbStage.regIf(regfile);
		wbStage.memoryIf(mStage);
		wbStage.clock(clock);
	}
	
};