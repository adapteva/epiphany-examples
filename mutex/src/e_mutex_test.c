#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"


int main(void)
{
	e_mutex_t *mutex_p;
	unsigned counter;
	unsigned *p;
	unsigned *p1;
	mutex_p = (void *)0x00004000;
	p = (void *)0x00006000;
	p1 = (void *)0x00006200;

	// Wait to get the key
	e_mutex_lock(0, 0, mutex_p);
	
	// After get the key, add 1 to the counter
	counter = *((unsigned *) 0x80806000);
	
	counter = counter + 1;
	
	*((unsigned *) 0x80806000) = counter;
	
	// Release the key
	e_mutex_unlock(0, 0, mutex_p);
	
	return 0;
}
