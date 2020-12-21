#include "systemc.h"
#include "../isa.h"

#ifndef FETCH_IF_DECL
	#define FETCH_IF_DECL
	struct fetch_if: virtual public sc_interface {
		virtual sc_uint<32> getIR() = 0;
		virtual sc_uint<32> getPC() = 0;
		virtual void setPC(sc_uint<32> pc) = 0;
		virtual void flush() = 0;
	};
#endif