/*
e_dma_slave_test.c

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

// This is the DEVICE side of the DMA slave mode example.
// The host may load this program to any eCore. In this program
// the transmitter will send data to the receiver core under dma
// slave mode. This is the program for the recevier core.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"


int main(void)
{
	unsigned k,i,j;
	unsigned *dst, *dst1, *dst2, *mst;
	unsigned tran;
	e_dma_desc_t dma_desc;
	tran = 128;

	dst = (unsigned *)0x2000;
	dst1 = (unsigned *)0x2400;
	dst2 = (unsigned *)0x2800;
	mst = (unsigned *)0x4000;

	// Initialize the buffer in receiver core
	for (i=0; i<tran*5; i++)
	{
		dst[i]  = 0x00000000;
	}

	mst[0] = 0x00000000;

	// Initialize the auto dma register in receiver core
	e_reg_write(E_REG_DMA0AUTODMA0, 0x00000000);

	// Prepare for the descriptor slave dma

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_WORD), 0x0000,
	0x0000, 0x0004,
	0x0400, 0x0001,
	0x0000, 0x0000,
	0x0000,(void *)dst, &dma_desc);

	// Wait for the signal to start dma transfering

	while(*mst == 0x00000000){};

	e_dma_start(&dma_desc, E_DMA_0);

	e_dma_wait(E_DMA_0);

	return 0;
}


