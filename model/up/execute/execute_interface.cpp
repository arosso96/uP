#include "systemc.h"
#include "../isa.h"

#ifndef EXEC_IF_DECL
	#define EXEC_IF_DECL
	struct execute_if: virtual public sc_interface {
		virtual MemInstrT getMemOp() = 0;
		virtual wbOpsT getWBOp() = 0;
		virtual sc_uint<32> getIR() = 0;
		virtual void flush() = 0;
	};
#endif