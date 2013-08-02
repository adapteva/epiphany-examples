#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

char outbuf[4096] SECTION("shared_dram");

int main(void)
{
	e_mutex_t *mutex;
	volatile unsigned *po;
	unsigned time_p;
	unsigned time_c;
	int i;
	unsigned *box;
	unsigned *box1;
	
	// Define the address of mutex
	mutex = (int *)0x00004000;
	
	// Define the address of counter
	po = (int *)0x00006000;
	
	// Define the mailbox
	box = (int *)0x00006200;
	box1 = (int *)0x00006300;
	
	// Initialize the counter to 0
	*po = 0;
			
	// Initialize the mutex in core (0,0)
	e_mutex_init(0, 0, mutex, MUTEXATTR_NULL);
	
	// Start counting 
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);

	// Wait to get the key
	e_mutex_lock(0, 0, mutex);
		
	// Add 1 to counter
	po[0] = po[0] + 1;
	
	// Release the key
	e_mutex_unlock(0, 0, mutex);

	while(po[0] != 16) {};
	
	// Return the value of counter
	time_c = e_ctimer_get(E_CTIMER_0);

	*box = time_p - time_c;
	*box1 = po[0];
	
return 0;
}
