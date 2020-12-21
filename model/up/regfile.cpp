#include "systemc.h"
#include "regfile_interface.cpp"

#ifndef RF_D
	#define RF_D
	SC_MODULE(Regfile), public regfile_if {
		private:
			sc_signal<sc_int<32>> regs[31];
		public:
						
			SC_CTOR(Regfile) {
				FILE *f = fopen("regfile.log", "w");
				fclose(f);
				for (int i=0;i<31;i++)
					regs[i] = 0;
			}
			
			virtual void write(sc_uint<5> rd, sc_int<32> data) {
				FILE *f = fopen("regfile.log", "a");
				fprintf(f, "%d WROTE TO REG %d, DATA: 0x%08x\n", sc_time_stamp().to_default_time_units(), (unsigned int) rd, (int) data);
				fclose(f);
				if (rd == 0)
					return;
				regs[((int) rd) - 1] = data;
			}
			
			virtual sc_int<32> read(sc_uint<5> rd) {
				FILE *f = fopen("regfile.log", "a");
				fclose(f);
				if (rd == 0) {
					fprintf(f, "%d READ FROM REG %d, DATA: 0x00000000\n", sc_time_stamp().to_default_time_units(), (unsigned int) rd);
					fclose(f);
					return 0;
				}
				fprintf(f, "%d READ FROM REG %d, DATA: 0x%08x\n", sc_time_stamp().to_default_time_units(), (unsigned int) rd, (int) regs[((int) rd) - 1].read());
				fclose(f);
				return regs[((int) rd) - 1];
			}
	};
#endif