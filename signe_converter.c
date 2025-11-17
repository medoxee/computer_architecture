#include <stdio.h>
#include <stdint.h>

/*
 * 	IN THE NAME OF ALLAH,
 * History: 17/11/2025 (21:56)
 * Description: A C program that converts positive number
 *  to negative in signed 8 bits (range is -128 to 127).
 *  To convert a signed 8 bits integer to its negative value,
 *  we do the Two's comlement process (Google it).
 *  Example: 99 --> -99
 *  Note: in ligne where scanf is used, %hhd (h: short) is the format specifier of int8_t type
 */

int8_t	two_complement(int8_t	positive_nbr)
{
	int8_t	value;

	// shift all 8 bits of positive_nbr (0 to 1 and 1 to 0)
	value = ~(positive_nbr);
	// add 1
	value++;
	return value;
	
}
int	main(void)
{
	int	value_int_type; // used in input validation
	int8_t	positive_nbr; // example: 1 = 00000001 (MSD for + signe)
	int8_t	negative_nbr; // example: -1 = 11111111 (MSD for - signe)
	
	printf("Enter a number between 1 and 127: ");
	if ((scanf("%d", &value_int_type) != 1) || (value_int_type > 127))
	{
		// exit if scanf fails
		printf("Program failed during scanf process!\n");
		return 1;
	}
	positive_nbr = (int8_t)value_int_type; // type casting
	negative_nbr = two_complement(positive_nbr);
	printf("the negative represention of %hhd is %hhd\n", positive_nbr, negative_nbr);
	return 0;
}
