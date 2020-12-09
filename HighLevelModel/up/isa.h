#ifndef ISA_D
	#define ISA_D
	enum execOpsT {
		ALU_ADD, ALU_ADDU, ALU_SUB, ALU_SLT, ALU_SLTU, ALU_XOR, ALU_OR, ALU_AND, ALU_SLL, ALU_SRL, ALU_SRA, // A, B
		ALU_ADDI, ALU_ADDUI, ALU_SUBI, ALU_SLTI, ALU_SLTUI, ALU_XORI, ALU_ORI, ALU_ANDI, ALU_SLLI, ALU_SRLI, ALU_SRAI, // A, Imm
		ALU_ADDPCI, ALU_ADDPCI_CMP_EQ, ALU_ADDPCI_CMP_NEQ, ALU_ADDPCI_CMP_LT, ALU_ADDPCI_CMP_NLT, ALU_ADDPCI_CMP_NLTU, ALU_ADDPCI_CMP_LTU // PC, Imm & compare
		};
	enum memOpsT {MEM_ALU_OUT, MEM_RS2_AND_JUMP, MEM_RS2_AND_COND_JUMP, MEM_READ_LMD8S_OUT, MEM_READ_LMD16S_OUT, MEM_READ_LMD32_OUT, MEM_READ_LMD8U_OUT, MEM_READ_LMD16U_OUT, MEM_WRITE8_RS2_OUT, MEM_WRITE16_RS2_OUT, MEM_WRITE32_RS2_OUT};
	enum wbOpsT {WB_WRITE_REG, WB_NOP};
	// The NOP is just an ADD rd = r0, rs1 = r0, rs2 = r0
	// ^ -> TODO: may be exploited with low power techniques to save power (ex: clock or power gating, or simply freeze FF such that input doesn't cahnge + disable MEM and WB writing)
	enum decodeOpsT {NOP, LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, FENCE, ECALL, EBREAK};
	
	class ExecInstrT {
		
		public:
		
		execOpsT op;
		sc_int<32> a, b, imm, pc;
		
		// constructor
		ExecInstrT (execOpsT op_ = ALU_ADD, sc_int<32> a_ = 0, sc_int<32> b_ = 0, sc_int<32> imm_ = 0, sc_int<32> pc_ = 0) {
			op = op_;
			a = a_;
			b = b_;
			imm = imm_;
			pc = pc_;
		}

		inline bool operator == (const ExecInstrT & eit) const {
			return (eit.op == op && eit.a == a && eit.b == b && eit.imm == imm  && eit.pc == pc);
		}

		inline ExecInstrT& operator = (const ExecInstrT& eit) {
			op = eit.op;
			a = eit.a;
			b = eit.b;
			imm = eit.imm;
			pc = eit.pc;
			return *this;
		}

		inline friend void sc_trace(sc_trace_file *tf, const ExecInstrT & v,
		const std::string & NAME ) {
		}

		inline friend ostream& operator << ( ostream& os,  ExecInstrT const & v ) {
		  os << "(" << v.op << ")";
		  return os;
		}

	};
	
	class MemInstrT {
		
		public:
		
		memOpsT op;
		sc_int<32> aluOut, rs2;
		bool comp;
		
		// constructor
		MemInstrT (memOpsT op_ = MEM_ALU_OUT, sc_int<32> aluOut_ = 0, sc_int<32> rs2_ = 0, bool comp_ = false) {
			op = op_;
			aluOut = aluOut_;
			rs2 = rs2_;
			comp = comp_;
		}

		inline bool operator == (const MemInstrT & v) const {
			return (v.op == op && v.aluOut == aluOut && v.rs2 == rs2 && v.comp == comp);
		}

		inline MemInstrT& operator = (const MemInstrT& v) {
			op = v.op;
			aluOut = v.aluOut;
			rs2 = v.rs2;
			return *this;
		}

		inline friend void sc_trace(sc_trace_file *tf, const MemInstrT & v,
		const std::string & NAME ) {
		}

		inline friend ostream& operator << ( ostream& os,  MemInstrT const & v ) {
		  os << "(" << v.op << ")";
		  return os;
		}

	};
	
	class WBInstrT {
		
		public:
		
		wbOpsT op;
		sc_int<32> lmd;
		
		// constructor
		WBInstrT (wbOpsT op_ = WB_NOP, sc_int<32> lmd_ = 0) {
			op = op_;
			lmd = lmd_;
		}

		inline bool operator == (const WBInstrT & v) const {
			return (v.op == op && v.lmd == lmd);
		}

		inline WBInstrT& operator = (const WBInstrT& v) {
			op = v.op;
			lmd = v.lmd;
			return *this;
		}

		inline friend void sc_trace(sc_trace_file *tf, const WBInstrT & v,
		const std::string & NAME ) {
		}

		inline friend ostream& operator << ( ostream& os,  WBInstrT const & v ) {
		  os << "(" << v.op << ")";
		  return os;
		}

	};
#endif