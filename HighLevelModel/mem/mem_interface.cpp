#include "systemc.h"
#ifndef MEM_IF_DECL
	#define MEM_IF_DECL
	struct mem_if: virtual public sc_interface {
		virtual sc_int<32> read32(unsigned int)=0;
		virtual sc_int<16> read16(unsigned int)=0;
		virtual sc_int<8> read8(unsigned int)=0;
		virtual void write32(unsigned int, sc_int<32>)=0;
		virtual void write16(unsigned int, sc_int<16>)=0;
		virtual void write8(unsigned int, sc_int<8>)=0;
	};
#endif