/*
e_mutex_test0.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program, see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.
*/

// This is the DEVICE side of the mutex example.
// This is the program for core (0,0) which stores the 
// the mutex. In this program, it initializes the mutex and
// initialize the counter.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <e-lib.h>

int main(void)
{
	e_mutex_t *mutex;
	volatile unsigned *po;
	unsigned time_p;
	unsigned time_c;
	int i;
	unsigned *box;
	unsigned *box1;
	volatile uint32_t *host_flag;

	// Define the address of mutex
	mutex = (int *)0x00004000;

	// Define the address of counter
	po = (int *)0x00006000;

	// Define the mailbox
	box = (int *)0x00006200;
	box1 = (int *)0x00006300;

	/* Host go flag */
	host_flag = (volatile uint32_t *) 0x6400;

	// Initialize the counter to 0
	*po = 0;

	// Initialize the mutex in core (0,0)
	e_mutex_init(0, 0, mutex, MUTEXATTR_NULL);

	/* Wait for host go flag before starting */
	while (!(*host_flag)) ;

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
