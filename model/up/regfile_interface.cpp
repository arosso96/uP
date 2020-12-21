#include "systemc.h"

#ifndef RF_IF_DECL
	#define RF_IF_DECL
	struct regfile_if: virtual public sc_interface {
		virtual void write(sc_uint<5> rd, sc_int<32> data) = 0;
		virtual sc_int<32> read(sc_uint<5> rd) = 0;
	};
#endif