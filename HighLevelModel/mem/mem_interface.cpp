#include "systemc.h"
#ifndef MEM_IF_DECL
	#define MEM_IF_DECL
	struct mem_if: virtual public sc_interface {
		virtual unsigned int read32(unsigned int)=0;
		virtual void write32(unsigned int, unsigned int)=0;
	};
#endif