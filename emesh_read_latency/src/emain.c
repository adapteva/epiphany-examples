#include <stdio.h>
#include <stdlib.h>
#include <e-lib.h>


#define CTIMER               (0x000f0438)
#define targetbase           (0x00005100)
#define mailbox  ((unsigned *)0x00005200)
#define localbox ((unsigned *)0x00005300)




int main(void) {
	//initialize
	unsigned time_s, time_e, time;
	unsigned expect;
	unsigned *target;
	unsigned ctimer;
	int i, j, k;
	unsigned value;	
	
	
	unsigned row, col;

	row = e_group_config.core_row;
	col = e_group_config.core_col;
	ctimer = (unsigned)e_get_global_address(row, col, (void *)CTIMER);

	value = 0xdead;

	for (i=0;i<4;i++)
	{	
		for (j=0;j<4;j++)
		{	

			target = (unsigned *)e_get_global_address(i, j, (void *)targetbase);
			
			//initialize the target
			for (k=0;k<10;k++)
			{
				target[0] = 0xdeadbeef;
				localbox[0] = 0x0;
			}
	
	
			//record the offset
			e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
			e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_s):"r"(ctimer):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_e):"r"(ctimer):);
			e_ctimer_stop(E_CTIMER_0);
			time = time_s - time_e;


			//record the ctimer	
			e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
			e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_s):"r"(ctimer):);

			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);

			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_e):"r"(ctimer):);
			e_ctimer_stop(E_CTIMER_0);
			
			time = time_s - time_e - time;
			expect = ((abs(row - i) + abs(col - j)) * 3 + 18)*10;
			
			//update the mailbox
			mailbox[(i*4+j)] = time;
			mailbox[(i*4+j+16)] = expect;
			
		}
	}
	
	return EXIT_SUCCESS;
}


