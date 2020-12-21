#include "systemc.h"
#include "../isa.h"

#ifndef WB_IF_DECL
	#define WB_IF_DECL
	struct wb_if: virtual public sc_interface {
		virtual sc_uint<32> getIR() = 0;
	};
#endif