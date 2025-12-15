#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#define ADD 0x01
#define SUB 0x02
#define COMP 0x03
#define MUL 0x04
#define AND 0x06
#define OR 0x07
#define STR 0x0a
#define A_ADDR 0xff
#define B_ADDR 0xfe
#define ACC_ADDR_IN_RAM 0xfc
#define RAM_SIZE 256
#define BITS 8

/*
 * 			IN THE NAME OF ALLAH,
 * History    : 08-12-2025
 * Developer  : MOHAMED ARRAF (https://www.linkedin.com/in/mohamedarraf)
 * Title      : vcpu (Virtual CPU) for 8bits systems.
 * Description: This is an imeplementation of an 8bits CPU that does
 * 		until now addition, substraction, multiplication operations,
 * 		others will be added soon INCHAALLAH.
 * Topics     : Fetch-Decode-Execute cycle, Von Neumann architecture,
 * 		bit shiftting/masking, RAM, CU, ALU, registers.
 * Notice     : Variables A, B are set from DOWN to UP addresses of ram
 * 		in order to seperate instructions from values (its not good).
 * 		Source code is full of debugging commented printf functions.
 */

struct	cpu{
	int	ram[RAM_SIZE];
	uint8_t	pc;		// Program Counter, addr of next instr to fetch
	int	ir;		// instruction register
	uint8_t	r0;
	uint8_t r1;		// r0, r1 : general purpose registers
	uint8_t	acc;		// Accumelator, register that stores result of ALU
} cpu_1;

void	alu(uint8_t	*acc, uint8_t	opcode, uint8_t	r0, uint8_t	r1)
{
	uint8_t	r_shifts;	// used in while loops
	// printf("i am in alu\n");
	// ADD operation
	if (opcode == ADD)
	{
		// printf("i am in add\n");
		*acc = r0 + r1;
	}
	else if (opcode == SUB)
	{
		*acc = ~(r1) + 0b00000001;
		*acc += r0;
	}
	else if (opcode == MUL)
	{
		/*
		 * initilze acc to 0x00
		 * r_shift: n times to shift r1
		 * loop until all bits of r1 multiplied by r0
		 * if LSB of shifted r1 == 1; result of mult is r0(shifted by r_shift)
		 * else if LSB of shifted r1 == 0; 0 will not affect the addition
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
	else if (opcode == AND)
		*acc = r0 & r1;
	else if (opcode == OR)
		*acc = r0 | r1;
}

void	decode(int	*ir, uint8_t	*opcode, int	*r2, int	*r3)
{
	// printf("i am in decode\n");
	*opcode = *ir >> 24;		// isolate opcode
	*r2 = (*ir >> 12) & 0x000000ff;
	*r3 = *ir & 0x000000ff;		// operands A, B
	*ir = 0;			// free ir
}

void	fetch(uint8_t	src_addr, int	*ir, uint8_t *rx, int	is_addr)
{
	// printf("i am in fetch\nir = %d\n", *ir);
	if (is_addr)
	{
		// printf("condition 1, ir = %d\n", *ir);
		*ir = cpu_1.ram[src_addr];	// load addr
	}
	else
		*rx = cpu_1.ram[src_addr];	// load value
}

void	control_unit(void)
{
	uint8_t	opcode;	// operation code (add, sub, ...)
	int	r2;
	int	r3;	// r2, r3 : registers of addresses for A, B
	
	cpu_1.pc = 0x00;	// location of 1st instruction in ram
	while (cpu_1.pc <= 0x01)
	{
		// fetch...
		fetch(cpu_1.pc, &cpu_1.ir, 0, 1);
		// printf("pc = %x\n", cpu_1.pc);
		// decode...
		decode(&cpu_1.ir, &opcode, &r2, &r3);
		// fetch operands...
		fetch(r2, &cpu_1.ir, &cpu_1.r0, 0);
		// printf("fetch op A done\n");
		fetch(r3, &cpu_1.ir, &cpu_1.r1, 0); // LOAD value from r2/r3 to r0/r1
		// execute...
		if (opcode != STR)
		{
			alu(&cpu_1.acc, opcode, cpu_1.r0, cpu_1.r1);
			cpu_1.ram[ACC_ADDR_IN_RAM] = cpu_1.acc;
		}
		else
		{
			// printf("STR detected\n");
			// printf("r1 = %d/ r0 = %d\n", cpu_1.r1, cpu_1.r0);
			cpu_1.ram[r3] = cpu_1.r0;
		}
		cpu_1.pc++;	// point to next instruction addr in ram
		// sleep(1);
		// printf("------------------------------------\n");
	}
}

int	main(void)
{
	uint8_t	result;	// final result of cpu
	
	/*
	 * Input can be set below,
	 * Prevent overflow
	 * Notices:
	 * 	For substraction, A MUST be >= B
	 * 	Addition	: operation = ADD
	 * 	Substraction	: operation = SUB
	 * 	Do not touche anything else or it will break!
	 */
	// start of user input
	int	A = 1;
	int	B = 1;
	int	operation = OR;
	// end of user input
	cpu_1.ram[0x00] = (operation << 24) +((int)A_ADDR << 12) + (int)B_ADDR;
	cpu_1.ram[A_ADDR] = A;
	cpu_1.ram[B_ADDR] = B;
	cpu_1.ram[0x01] = 0x0a0fc0fd;	// STR acc, Oxfd
	control_unit();
	result = cpu_1.ram[0xfd];	// 0xfd addr of acc in ram
	printf("8bits CPU emulator, by @medoxe\n");
	printf("Result = %" PRIu8 "\n", result);
	// PRIu8 to print uint8_t
	return 0;
}
