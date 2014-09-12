/*
e_demo.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>
               Xin Mao <maoxin99@gmail.com>

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

// This is the DEVICE side of epiphany resource manager.
// It first do the convolution calculation and after that
// It will transfer the results to another core by using
// dma_copy function.


#include <stdio.h>
#include <stdlib.h>
#include "e-lib.h"

int main(void)
{
	unsigned *a, *b, *c, *size;
	unsigned i, j;
	int      coreid;
	unsigned *src, *dest;
	unsigned row, col;

	coreid = e_get_coreid() ^ 0x0c3; // Copy to the opposite core in chip
	src    = (unsigned *) 0x6000;
	dest   = (void *) ((coreid<<20) | 0x6000);

	size = (unsigned *) 0x1e00;
	a    = (unsigned *) 0x2000;
	b    = (unsigned *) 0x4000;
	c    = (unsigned *) 0x6000;

	// Doing convolution to generate busy level
	for (i=0; i<(*size); i++)
	{
		for (j=0; j<=i; j++)
		{
			c[i] += a[i-j] * b[j];
		}
	}

	for(i=0; i<100000; i++)
	{
		e_dma_copy(dest, src, *size);
	}

	// clear the IMASK
	e_irq_mask(E_SYNC, E_FALSE);

	// enable the global interrupt
	e_irq_global_mask(E_FALSE);

	__asm__ __volatile__("idle");

	return EXIT_SUCCESS;
}

