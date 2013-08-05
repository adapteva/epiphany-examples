/*
e_dma_chain_test.c

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

// This is the DEVICE side. This core will get the neighbour address
// and transfer data to it by using dma. This core initializes 3
// descriptors and use dma chain to transfer data.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

unsigned checkbuffer(unsigned *po, unsigned a, unsigned num);

	
int main(void)
{
	unsigned k,i,j;
	e_dma_desc_t dma_desc[3];
	unsigned *dst, *src, *dst1, *src1, *dst2, *src2;
	unsigned tran;
	unsigned index[3];
	unsigned neighbour_core;
	unsigned *n_row, *n_col, *p;
	unsigned *mailbox, *mailbox1, *mailbox2, *mailbox3;
	n_row = (unsigned *) 0x00006000;
	n_col = (unsigned *) 0x00006004;
	tran = 128;
	p = 0x0000;
	
	// Get the core id of the neighbour core
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);

	neighbour_core = (unsigned) e_get_global_address(*n_row, *n_col, p);

	// Define the mailbox
	mailbox = (unsigned *)0x2250;
	mailbox1 = (unsigned *)0x6100;
	mailbox2 = (unsigned *)0x6104;
	mailbox3 = (unsigned *)0x6108;

	// Initialize the buffer address for dma chain test
	src = (int *)0x2000;
	dst = (int *)(neighbour_core + (unsigned)0x2000);
	src1 = (int *)0x2300;
	dst1 = (int *)(neighbour_core + (unsigned)0x2500);
	src2 = (int *)0x2600;
	dst2 = (int *)(neighbour_core + (unsigned)0x2a00);

	// Test for word size
 	// Initialize the source and destination buffer
	for (i=0; i<tran; i++)
	{ 
		src[i] = 0xaaaaaaaa;
		dst[i] = 0x00000000;
		src1[i] = 0xbbbbbbbb;
		dst1[i] = 0x00000000;
		src2[i] = 0xcccccccc;
		dst2[i] = 0x00000000;
	}
	

	// Prepare for the descriptor for dma chain

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_WORD),0x0000,
	0x0004,0x0004,
	0x0080,0x0001,
	0x0000,0x0000,
	(void *)src2,(void *)dst2, &dma_desc[2]);	

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_WORD|E_DMA_CHAIN),&dma_desc[2],
	0x0004,0x0004,
	0x0080,0x0001,
	0x0000,0x0000,
	(void *)src1,(void *)dst1, &dma_desc[1]);
	
	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_WORD|E_DMA_CHAIN), &dma_desc[1],
	0x0004, 0x0004,
	0x0080, 0x0001,
	0x0000 , 0x0000,
	(void *)src,(void *)dst, &dma_desc[0]);
	
	// Start transaction
	e_dma_start(&dma_desc[0], E_DMA_0);
		
	// Wait 
	e_dma_wait(E_DMA_0);	
	
	// Check the destination buffer value
	index[0] = checkbuffer(dst,  (unsigned)0xaaaaaaaa, tran);
	index[1] = checkbuffer(dst1, (unsigned)0xbbbbbbbb, tran);
	index[2] = checkbuffer(dst2, (unsigned)0xcccccccc, tran);

	if((index[0]|index[1]|index[2]) == 0)
	{
		mailbox[0] = 0xffffffff;
	}else
	{
		mailbox[0] = 0x0000000;
	}

	// Test for double word size
 	// Initialize the source and destination buffer
	for (i=0; i<tran; i++)
	{ 
		src[i] = 0xaaaaaaaa;
		dst[i] = 0x00000000;
		src1[i] = 0xbbbbbbbb;
		dst1[i] = 0x00000000;
		src2[i] = 0xcccccccc;
		dst2[i] = 0x00000000;
	}
	

	// Prepare for the descriptor for dma chain

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_DWORD),0x0000,
	0x0008,0x0008,
	0x0040,0x0001,
	0x0000,0x0000,
	(void *)src2,(void *)dst2, &dma_desc[2]);	

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_DWORD|E_DMA_CHAIN),&dma_desc[2],
	0x0008,0x0008,
	0x0040,0x0001,
	0x0000,0x0000,
	(void *)src1,(void *)dst1, &dma_desc[1]);
	
	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_DWORD|E_DMA_CHAIN), &dma_desc[1],
	0x0008, 0x0008,
	0x0040, 0x0001,
	0x0000 , 0x0000,
	(void *)src,(void *)dst, &dma_desc[0]);
	
	// Start transaction
	e_dma_start(&dma_desc[0], E_DMA_0);
	
	// Wait 
	e_dma_wait(E_DMA_0);	

	// Check the destination buffer value
	index[0] = checkbuffer(dst,  (unsigned)0xaaaaaaaa, tran);
	index[1] = checkbuffer(dst1, (unsigned)0xbbbbbbbb, tran);
	index[2] = checkbuffer(dst2, (unsigned)0xcccccccc, tran);

	if((index[0]|index[1]|index[2]) == 0)
	{
		mailbox1[0] = 0xffffffff;
	}else
	{
		mailbox1[0] = 0x0000000;
	}

	// Test for half word size
 	// Initialize the source and destination buffer
	for (i=0; i<tran; i++)
	{ 
		src[i] = 0xaaaaaaaa;
		dst[i] = 0x00000000;
		src1[i] = 0xbbbbbbbb;
		dst1[i] = 0x00000000;
		src2[i] = 0xcccccccc;
		dst2[i] = 0x00000000;
	}
	

	// Prepare for the descriptor for dma chain

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_HWORD),0x0000,
	0x0002,0x0002,
	0x0100,0x0001,
	0x0000,0x0000,
	(void *)src2,(void *)dst2, &dma_desc[2]);	

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_HWORD|E_DMA_CHAIN),&dma_desc[2],
	0x0002,0x0002,
	0x0100,0x0001,
	0x0000,0x0000,
	(void *)src1,(void *)dst1, &dma_desc[1]);
	
	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_HWORD|E_DMA_CHAIN), &dma_desc[1],
	0x0002, 0x0002,
	0x0100, 0x0001,
	0x0000 , 0x0000,
	(void *)src,(void *)dst, &dma_desc[0]);
	
	// Start transaction
	e_dma_start(&dma_desc[0], E_DMA_0);

	// Wait 
	e_dma_wait(E_DMA_0);	
	
	// Check the destination buffer value
	index[0] = checkbuffer(dst,  (unsigned)0xaaaaaaaa, tran);
	index[1] = checkbuffer(dst1, (unsigned)0xbbbbbbbb, tran);
	index[2] = checkbuffer(dst2, (unsigned)0xcccccccc, tran);

	if((index[0]|index[1]|index[2]) == 0)
	{
		mailbox2[0] = 0xffffffff;
	}else
	{
		mailbox2[0] = 0x0000000;
	}

	// Test for byte size
 	// Initialize the source and destination buffer
	for (i=0; i<tran; i++)
	{ 
		src[i] = 0xaaaaaaaa;
		dst[i] = 0x00000000;
		src1[i] = 0xbbbbbbbb;
		dst1[i] = 0x00000000;
		src2[i] = 0xcccccccc;
		dst2[i] = 0x00000000;
	}
	

	// Prepare for the descriptor for dma chain

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_BYTE),0x0000,
	0x0001,0x0001,
	0x0200,0x0001,
	0x0000,0x0000,
	(void *)src2,(void *)dst2, &dma_desc[2]);	

	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_BYTE|E_DMA_CHAIN),&dma_desc[2],
	0x0001,0x0001,
	0x0200,0x0001,
	0x0000,0x0000,
	(void *)src1,(void *)dst1, &dma_desc[1]);
	
	e_dma_set_desc(E_DMA_0,(E_DMA_ENABLE|E_DMA_MASTER|E_DMA_BYTE|E_DMA_CHAIN), &dma_desc[1],
	0x0001, 0x0001,
	0x0200, 0x0001,
	0x0000 , 0x0000,
	(void *)src,(void *)dst, &dma_desc[0]);
	
	// Start transaction
	e_dma_start(&dma_desc[0], E_DMA_0);

	// Wait 
	e_dma_wait(E_DMA_0);	
	
	// Check the destination buffer value
	index[0] = checkbuffer(dst,  (unsigned)0xaaaaaaaa, tran);
	index[1] = checkbuffer(dst1, (unsigned)0xbbbbbbbb, tran);
	index[2] = checkbuffer(dst2, (unsigned)0xcccccccc, tran);

	if((index[0]|index[1]|index[2]) == 0)
	{
		mailbox3[0] = 0xffffffff;
	}else
	{
		mailbox3[0] = 0x0000000;
	}
	return 0;
}

unsigned checkbuffer(unsigned *po, unsigned a, unsigned num)
{
	unsigned k;
	unsigned flag;
	flag = 0;
	for(k=0; k<num; k++)
	{
		if(po[k] != a)
		{
			flag = 1;
		}
	}	
	return flag;
}
