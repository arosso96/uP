#ifndef ISA_D
	#define ISA_D
	enum execOpsT {ALU_ADD, ALU_ADDU, ALU_SUB, ALU_SLT, ALU_SLTU, ALU_XOR, ALU_OR, ALU_AND, ALU_SLL, ALU_SRL, ALU_SRA, ALU_ADDI, ALU_ADDUI, ALU_SUBI, ALU_SLTI, ALU_SLTUI, ALU_XORI, ALU_ORI, ALU_ANDI, ALU_SLLI, ALU_SRLI, ALU_SRAI};
	enum memOpsT {MEM_ALU_OUT, MEM_RS2_AND_JUMP, MEM_READ_LMD8S_OUT, MEM_READ_LMD16S_OUT, MEM_READ_LMD32_OUT, MEM_READ_LMD8U_OUT, MEM_READ_LMD16U_OUT, MEM_WRITE8_RS2_OUT, MEM_WRITE16_RS2_OUT, MEM_WRITE32_RS2_OUT};
	enum wbOpsT {WB_WRITE_REG, WB_NOP};
	// The NOP is just an ADD rd = r0, rs1 = r0, rs2 = r0
	// ^ -> TODO: may be exploited with low power techniques to save power (ex: clock or power gating, or simply freeze FF such that input doesn't cahnge + disable MEM and WB writing)
	enum decodeOpsT {NOP, LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, FENCE, ECALL, EBREAK};
#endif