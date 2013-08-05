#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <e-lib.h>

#define pagesize (0x1000)
#define offset (pagesize - sizeof(int))
#define mailbox ((unsigned *) 0x8f000000)

#define delay (300000)

typedef enum
{
	clear         = 0,
	write_success = 2,
	write_failed  = 3,
	finished      = 9,
}test_state;


void __attribute__((interrupt)) mpf_isr(int signum);


void mem_write(int page) {
	int *address;

	mailbox[1] = pagesize * page + offset;
	e_wait(E_CTIMER_0, delay);

	// write to the address
	address = (int *) mailbox[1];
	*address = 0xdeadbeef;
	e_wait(E_CTIMER_0, delay);
	if (mailbox[0] == clear)
		mailbox[0] = write_success;

	return;
}



int main(void)
{
	int i;

	//initialize
	e_irq_attach(E_MEM_FAULT, mpf_isr);

	//enable the interrupt
	e_irq_global_mask(E_FALSE);
	e_irq_mask(E_MEM_FAULT, E_FALSE);


	//set the mem protect
	int mem_pages = 0xff;
	e_reg_write(E_REG_MEM_PROTECT, mem_pages);

	while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	for (i=0; i<8; i++)
	{
		//tell the host that write_failure is expected here
		mailbox[3] = write_failed;
		
		mailbox[2] = i;
		mem_write(i);
		e_wait(E_CTIMER_0, delay); 
		while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	}

	//set the mem protect
	mem_pages = 000;
	e_reg_write(E_REG_MEM_PROTECT, mem_pages);
	for (i=0; i<8; i++)
	{
		//tell the host that write_succeed is expected here
		mailbox[3] = write_success;
		
		mailbox[2] = i;
		mem_write(i);
		e_wait(E_CTIMER_0, delay); 
		while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	}


	mailbox[2] = i;
	mailbox[0] = finished;
	e_reg_write(E_REG_MEM_PROTECT, 0);
	
	return EXIT_SUCCESS;
}

