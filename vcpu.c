#include <stdio.h>
#include <stdint.h>

/*
 * 		IN THE NAME OF ALLAH,
 * History    : 08-12-2025, 17:30
 * Developer  : MOHAMED ARRAF (https://www.linkedin.com/in/mohamedarraf)
 * Title      : vcpu (Virtual CPU) for 8bits systems.
 * Description: This is an imeplementation of an 8bits CPU that does
 * 		until now addition operation, others will be added INCHAALLAH.
 * Topics     : Fetch-Decode-Execute cycle, Von Neumann architecture,
 * 		bit shiftting/masking, RAM, CU, ALU, registers.
 */

int	ram[256]; // RAM size 256
uint8_t	pc; // Program Counter, addr of next instr to fetch
int	ir; // instruction register
uint8_t	r0;
uint8_t r1; // r0, r1 : general purpose registers
uint8_t	acc = 9; // Accumelator, register that stores result of ALU

void	alu(uint8_t	*acc, uint8_t	opcode, uint8_t	r0, uint8_t	r1)
{
	// ADD operation
	if (opcode == 0x01)
	{
		*acc = r0;
		*acc += r1;
	}
}

void	decode(int	ir, uint8_t	*opcode, int	*r2, int	*r3)
{
	*opcode = ir >> 24; // isolate opcode by shiftting right 24 bits
	*r2 = (ir >> 12) & 0x000000FF; // isolate operand A
	*r3 = ir & 0x000000FF; // operand B
}

void	fetch(uint8_t	addr, int	*ir, uint8_t *rx, uint8_t	is_addr)
{
	// check what to LOAD is addr or value
	if (is_addr)
		*ir = ram[addr];
	else
		*rx = ram[addr];
}

uint8_t	control_unit(void)
{
	uint8_t	opcode; // operation code (add, sub, ...)
	int	r2;
	int	r3; // r2, r3 : registers of addresses for A, B
	
	pc = 0x00;
	fetch(pc, &ir, 0, 1);
	pc++;
	decode(ir, &opcode, &r2, &r3);
	// printf("0x0%x, 0x0%x\n", r2, r3);
	fetch(r2, 0, &r0, 0);
	fetch(r3, 0, &r1, 0); // LOAD value from r2/r3 to r0/r1
	// printf("%d, %d\n", r0, r1);
	alu(&acc, opcode, r0, r1);
	return acc;
}

int	main(void)
{
	uint8_t	result; // final result of cpu
	
	// input can be set below, change just A, B. MAX A=127 B=0 or A=0 B=127
	int	A = 5;
	int	B = 5;
	ram[0x00] = 0x01001002;
	ram[0x01] = A;
	ram[0X02] = B;
	result = control_unit();

	printf("8bits CPU (Virtual), by MOHAMED ARRAF\n");
	printf("A + B = %hhd\n", result);
	return 0;
}
