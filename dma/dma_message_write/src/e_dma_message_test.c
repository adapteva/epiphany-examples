/*
e_dma_message_test.c

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

// This is the DEVICE side of the DMA message example.
// This is the transmitter which will send data to receiver
// in dma message mode.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void)
{
	unsigned k,i,j;
	e_dma_desc_t dma_desc;
	e_dma_desc_t dma_desc1;
	unsigned *dst, *src, *dst1, *src1, *dst2, *src2;
	unsigned *e_dst;
	unsigned tran;
	unsigned index[3];
	unsigned slave_core;
	unsigned *p;
	tran = 128;
	p = 0x0000;

	for(i=0; i<e_group_config.group_rows; i++)
	{
		for(j=0; j<e_group_config.group_cols; j++)
		{
			if((i!=e_group_config.core_row)|(j!=e_group_config.core_col))
			{
				// Test the write message mode
				// Get global address of the slave core
				slave_core = (unsigned) e_get_global_address(i, j, p);

				// Initialize the buffer address for dma chain test
				src = (int *)0x2000;
				dst = (int *)(slave_core + (unsigned)0x2000);
				src1 = (int *)0x2300;
				dst1 = (int *)(slave_core + (unsigned)0x2500);
				src2 = (int *)0x2600;
				dst2 = (int *)(slave_core + (unsigned)0x2a00);


 				// Initialize the source, destination buffer
				for (k=0; k<tran; k++)
				{
					src[k]  = 0xaaaaaaaa;
					src1[k] = 0xbbbbbbbb;
					src2[k] = 0xcccccccc;
				}

				for (k=0; k<tran*6; k++)
				{
					dst[k] = 0x00000000;
				}

				// Prepare for the descriptor for 2d dma

				e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_WORD|E_DMA_MSGMODE), 0x0000,
				0x0004, 0x0004,
				0x0080, 0x0003,
				0x0104 , 0x0304,
				(void *)src,(void *)dst, &dma_desc);

				// Start transaction
				e_dma_start(&dma_desc, E_DMA_0);

				// Wait
				e_wait(E_CTIMER_0, 2000);

			}
		}
	}

	return 0;
}


