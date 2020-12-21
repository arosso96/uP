#include "systemc.h"
#include "../isa.h"

#ifndef DEC_IF_DECL
	#define DEC_IF_DECL
	struct decode_if: virtual public sc_interface {
		virtual ExecInstrT getExecOp() = 0;
		virtual memOpsT getMemOp() = 0;
		virtual wbOpsT getWBOp() = 0;
		virtual sc_uint<32> getIR() = 0;
		virtual void flush() = 0;
	};
#endif