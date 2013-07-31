#include <stdio.h>
#include <stdlib.h>
#include <e-lib.h>

#define E_WAND_INT (0x8)


void wand_isr(int signum);

unsigned state;
unsigned *result;


int main(void) {
	//initialize
	int i;
	unsigned row, col, delay, num;
	unsigned *ivt;
	
	e_irq_global_mask(E_FALSE);
	e_irq_attach(E_WAND_INT, wand_isr);
	e_irq_mask(E_WAND_INT, E_FALSE);
	
	row     = e_group_config.core_row;
	col     = e_group_config.core_col;
	num     = row * e_group_config.group_cols + col;
	result  = (unsigned *) (0x8f000000 + 0x4*num);
	delay   = 0x10000 * num + 0x10000;
	*result = 0xdeadbeef;
	
	for(i=0; i<0x10000; i++)
	{
		*result = i;
		e_wait(E_CTIMER_0, delay);

		__asm__ __volatile__("wand");
		__asm__ __volatile__("idle");

		// clear wand bit
		state = e_reg_read(E_REG_STATUS);
		state = state & (~0x8);
		e_reg_write(E_REG_FSTATUS, state);
	}
	
	return EXIT_SUCCESS;
}


#if 1
void __attribute__((interrupt)) wand_isr(int signum)
{
//	e_wait(E_CTIMER_1, 0x100);
	
	return;
}
#endif


