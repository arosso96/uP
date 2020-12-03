#include "systemc.h"
#ifndef UP_IF_DECL
	#define UP_IF_DECL
	struct up_if: virtual public sc_interface {
		virtual void cycle()=0;
	};
#endif