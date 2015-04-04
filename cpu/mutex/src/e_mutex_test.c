/*
e_mutex_test.c

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

// This is the DEVICE side of other cores except core(0,0)
// They start at the same time and try to grab the key to 
// add 1 to the counter.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "e_lib.h"


int main(void)
{
	e_mutex_t *mutex_p;
	unsigned counter;
	unsigned *p;
	unsigned *p1;
	volatile uint32_t *host_flag;

	mutex_p = (void *)0x00004000;
	p = (void *)0x00006000;
	p1 = (void *)0x00006200;

	/* Host go flag */
	host_flag = (volatile uint32_t *) 0x80806400;

	/* Wait for host go flag before starting */
	while (!(*host_flag)) ;

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
