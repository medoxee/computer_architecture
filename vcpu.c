#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#define ALU_OP 0b00
#define LOAD 0b01
#define STORE 0b10
#define ADD 0b00
#define SUB 0b01
#define COMP 0b10
#define MUL 0b11
#define R0 0b00
#define R1 0b01
#define R2 0b10
#define A_ADDR 0xf
#define B_ADDR 0xe
#define RESULT_ADDR 0xd
#define RAM_SIZE 16
#define BITS 8

/*
 * 			IN THE NAME OF ALLAH,
 * History    : 08-12-2025
 * Developer  : MOHAMED ARRAF (https://www.linkedin.com/in/mohamedarraf)
 * Title      : vcpu (Virtual CPU) for 8bits systems.
 * Description: This is an implementation of an 8bits CPU based on Von Neumann's
 * 		architecture that does until now addition, substraction,
 * 		multiplication operations, others will be added soon INCHAALLAH.
 * Topics     : Fetch-Decode-Execute cycle, Von Neumann architecture,
 * 		bit shiftting/masking, RAM, CU, ALU, registers.
 * Notice     : Beta version.
 * 		Variables A, B are set from DOWN to UP addresses of ram in order
 * 		to seperate instructions from values (its not good).
 * 		Source code is full of debugging commented printf functions.
 */

struct	cpu{
	uint8_t	ram[RAM_SIZE];
	uint8_t	pc;		// Program Counter, addr of next instr to fetch
	uint8_t	ir;		// instruction register
	uint8_t	read_enable;
	uint8_t write_enable;	// rw enable signals
	uint8_t alu_enable;	// enable alu to perfom operations
	uint8_t rx[2];		// rx: general purpose registers
	uint8_t	acc;		// Accumelator, register that stores result of
				// ALU
} cpu_0;

void	alu(uint8_t	*acc, uint8_t	operation, uint8_t	r0,
		uint8_t	r1)
{
	uint8_t	r_shifts;	// used in while loops
	/*
	printf("A = %d\nB = %d\nOP = %.2b\n", r0, r1, operation);
	*/
	if (operation == ADD)
	{
		*acc = r0 + r1;
	}
	else if (operation == SUB)
	{
		*acc = ~(r1) + 0b00000001;
		*acc += r0;
	}
	else if (operation == MUL)
	{
		/*
		 * initilze acc to 0x00
		 * r_shift: n times to shift r1
		 * loop until all bits of r1 multiplied by r0
		 * if LSB of shifted r1 == 1; result of mult is r0
		 * (shifted by r_shift)
		 * else if LSB of shifted r1 == 0; 0 will not affect the
		 * addition
		 */
		*acc = 0x00;
		r_shifts = 0;
		while (r_shifts < BITS)
		{
			if ((r1 >> r_shifts) & 0x01)
				*acc += r0 << r_shifts;
			r_shifts++;
		}
	}
	/*
	else if (operation == AND)
		*acc = r0 & r1;
	else if (operation == OR)
		*acc = r0 | r1;
	*/
	cpu_0.rx[0] = *acc;
}

void	decode(uint8_t	*ir, uint8_t	*opcode, uint8_t	*r0,
		uint8_t	*r1)
{
	*opcode = *ir >> 6;		// isolate opcode
	if (*opcode != ALU_OP)
	{
		*r0 = (*ir & 0x30) >> 4;
		*r1 = *ir & 0x0f;	// operands A, B
		if (*opcode == LOAD)
			cpu_0.read_enable = 1;
		else if (*opcode == STORE)
			cpu_0.write_enable = 1;
	}
	else
	{
		*opcode = *ir >> 4;
		*r0 = (*ir & 0x0b) >> 2;
		*r1 = *ir & 0x03;
		cpu_0.alu_enable = 1;
	}
	*ir = 0;			// free ir
}

void	fetch(uint8_t	src_addr, uint8_t	*ir)
{
	*ir = cpu_0.ram[src_addr];
}

void	control_unit(void)
{
	uint8_t	opcode;	// operation code (add, sub, ...)
	uint8_t	r0;
	uint8_t	r1;	// r0, r1 : registers of addr A, B
	
	cpu_0.pc = 0x0;	// addr of 1st instruction in ram
	while (cpu_0.pc <= 0x3)
	{
		fetch(cpu_0.pc, &cpu_0.ir);
		decode(&cpu_0.ir, &opcode, &r0, &r1);
		/*
		printf("instruction %d\n", cpu_0.pc);
		printf("opcode: %.2b\nr0: %.2b\nr1: %.4b\n", opcode,
				cpu_0.rx[r0], cpu_0.rx[r0]);
		*/
		// execute...
		if (cpu_0.read_enable)
		{
			cpu_0.rx[r0] = cpu_0.ram[r1];	
			cpu_0.read_enable = 0;
		}
		else if (cpu_0.write_enable)
		{
			cpu_0.ram[r1] = cpu_0.rx[r0];
			cpu_0.write_enable = 0;
		}
		else if (cpu_0.alu_enable)
		{
			alu(&cpu_0.acc, opcode, cpu_0.rx[0], cpu_0.rx[1]);
			cpu_0.alu_enable = 0;
		}
		cpu_0.pc++;
	}
}

int	main(void)
{
	/*
	 * Input can be set below,
	 * Prevent overflow
	 * Notices:
	 * 	For substraction, A MUST be >= B
	 * 	Addition	: operation = ADD
	 * 	Substraction	: operation = SUB
	 * 	Do not touche anything else or it will break!
	 */
	uint8_t	result;	// final result of cpu	
	// start of user input
	uint8_t	A = 10;
	uint8_t	B = 5;
	uint8_t	operation = SUB;

	// end of user input
	cpu_0.ram[A_ADDR] = A;
	cpu_0.ram[B_ADDR] = B;
	/*
	 * Below, initelizing the instructions:
	 * 	LOAD  R0, 0xf
	 * 	LOAD  R1, 0xe
	 * 	OP    R0, R1	(OP: Operation like ADD, SUB...)
	 * 	STORE RX, 0xd
	 */
	cpu_0.ram[0x0] = ((uint8_t)LOAD << 6) + ((uint8_t)R0 << 4) +
		((uint8_t)A_ADDR);
	cpu_0.ram[0x1] = ((uint8_t)LOAD << 6) + ((uint8_t)R1 << 4) +
		((uint8_t)B_ADDR);
	cpu_0.ram[0x2] = ((uint8_t)operation << 4) + ((uint8_t)R0 << 2) +
		((uint8_t)R1);

	cpu_0.ram[0x3] = ((uint8_t)STORE  << 6) +  ((uint8_t)R2 << 4) +
		((uint8_t)RESULT_ADDR);
	cpu_0.read_enable = 0;
	cpu_0.write_enable = 0;
	cpu_0.alu_enable = 0;
	control_unit();
	result = cpu_0.ram[RESULT_ADDR];
	printf("8bits CPU emulator, by @medoxe\n");
	printf("Result = %" PRIu8 "\n", result);
	return 0;
}
