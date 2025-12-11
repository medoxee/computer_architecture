#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

/*
 * 			IN THE NAME OF ALLAH,
 * History    : 08-12-2025, 17:30
 * Developer  : MOHAMED ARRAF (https://www.linkedin.com/in/mohamedarraf)
 * Title      : vcpu (Virtual CPU) for 8bits systems.
 * Description: This is an imeplementation of an 8bits CPU that does
 * 		until now addition, substraction operations, others will
 * 		be added soon INCHAALLAH.
 * Topics     : Fetch-Decode-Execute cycle, Von Neumann architecture,
 * 		bit shiftting/masking, RAM, CU, ALU, registers.
 * Notice     : Variables A, B are set from DOWN to UP addresses of ram
 * 		in order to seperate instructions from values (its not good).
 * 		Source code is full of debugging commented printf functions.
 */

#define ADD 0x01
#define SUB 0x02
#define COMP 0x03
#define STR 0x0a
#define A_ADDR 0xff
#define B_ADDR 0xfe
#define ACC_ADDR_IN_RAM 0xfc
#define RAM_SIZE 256

struct	cpu{
	int	ram[RAM_SIZE]; // RAM size 256, each slot has 32bits
	uint8_t	pc; // Program Counter, addr of next instr to fetch
	int	ir; // instruction register
	uint8_t	r0;
	uint8_t r1; // r0, r1 : general purpose registers
	uint8_t	acc; // Accumelator, register that stores result of ALU
} cpu_1;

void	alu(uint8_t	*acc, uint8_t	opcode, uint8_t	r0, uint8_t	r1)
{
	// printf("i am in alu\n");
	// ADD operation
	if (opcode == ADD)
	{
		// printf("i am in add\n");
		*acc = r0;
		*acc += r1;
	}
	else if (opcode == SUB)
	{
		*acc = ~(r1) + 0b00000001;
		*acc += r0;
	}
	else if (opcode == COMP)
	{
		/*
		 * NOT YET :/
		*acc = r0;
		*acc ^= r1; // xor
		*acc = !(*acc); // xnor
		*/
	}
}

void	decode(int	ir, uint8_t	*opcode, int	*r2, int	*r3)
{
	// printf("i am in decode\n");
	*opcode = ir >> 24; // isolate opcode by shiftting right 24 bits
	*r2 = (ir >> 12) & 0x000000ff; // isolate operand A
	*r3 = ir & 0x000000ff; // operand B
}

void	fetch(uint8_t	src_addr, int	*ir, uint8_t *rx, int	is_addr)
{
	// printf("i am in fetch\nir = %d\n", *ir);
	// check what to LOAD is addr or value
	if (is_addr)
	{
		// printf("condition 1, ir = %d\n", *ir);
		*ir = cpu_1.ram[src_addr];
	}
	else
		*rx = cpu_1.ram[src_addr];
}

void	control_unit(void)
{
	uint8_t	opcode; // operation code (add, sub, ...)
	int	r2;
	int	r3; // r2, r3 : registers of addresses for A, B
	
	cpu_1.pc = 0x00; // location of 1st instruction in ram
	while (cpu_1.pc <= 0x01)
	{
		// printf("i am in while loop\n");
		// fetch...
		fetch(cpu_1.pc, &cpu_1.ir, 0, 1);
		// cpu_1.pc++;
		// printf("pc = %x\n", cpu_1.pc);
		// decode...
		decode(cpu_1.ir, &opcode, &r2, &r3);
		cpu_1.ir = 0; // not more needed
		// fetch operands
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
		cpu_1.pc++; // point to next instruction addr in ram
		// sleep(1);
		// printf("------------------------------------\n");
	}
}

int	main(void)
{
	uint8_t	result; // final result of cpu
	
	/*
	 * input can be set below,
	 * MAX A=255 B=0 or A=0 B=255
	 * Notices:
	 * 	For substraction, A MUST be >= B
	 * 	Addition	: operation = ADD
	 * 	Substraction	: operation = SUB
	 * 	Do not touche anything else or it will break!
	 */
	// start of user input
	int	A = 5;
	int	B = 5;
	int	operation = ADD;
	// end of user input
	// result = 99;
	cpu_1.ram[0x00] = (operation << 24) +((int)A_ADDR << 12) + (int)B_ADDR;
	cpu_1.ram[A_ADDR] = A;
	cpu_1.ram[B_ADDR] = B;
	// printf("part 1 done\n");
	cpu_1.ram[0x01] = 0x0a0fc0fd; // STR acc, Oxfd
	// printf("str done\n");
	control_unit();
	result = cpu_1.ram[0xfd]; // 0xfd is the addr that cu stored value of acc
	printf("8bits CPU (Virtual), by MOHAMED ARRAF\n");
	printf("Result = %" PRIu8 "\n", result);
	// PRIu8 to print uint8_t
	return 0;
}
