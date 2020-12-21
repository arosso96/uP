#include "systemc.h"
#include "../isa.h"

#ifndef MEMO_IF_DECL
	#define MEMO_IF_DECL
	struct memory_if: virtual public sc_interface {
		virtual WBInstrT getWBOp() = 0;
		virtual sc_uint<32> getIR() = 0;
	};
#endif