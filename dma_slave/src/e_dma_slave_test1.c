/*
e_dma_slave_test1.c

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
// slave mode. This is the program for the transmitter core.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

// Function to check if the transfer is correct
unsigned checkbuffer(unsigned *po, unsigned a, unsigned num);		

int main(void)
{
	unsigned k,i,j;
	unsigned *auto0, *p, *mst;
	unsigned tran;	
	unsigned *r_row, *r_col;
	unsigned *dst, *dst1, *dst2;
	unsigned *mailbox;
	unsigned index;
	e_dma_desc_t dma_desc;
	
	// Number of transactions
	tran = 128;
	
	// Define the mailbox
	mailbox = (unsigned *)0x6100;
	mailbox[0] = 0x00000000;
	
	// Define the auto0 register address in receiver core
	r_row = (unsigned *)0x6500;
	r_col = (unsigned *)0x6504;
	
	// Address of the dma auto register
	p = (unsigned *)0xf0514;
	//p = (unsigned *)0xf0518;
	
	
	auto0 = (unsigned *) e_get_global_address(*r_row, *r_col, p);
	
	p = (unsigned *)0x2000;
	dst = (unsigned *) e_get_global_address(*r_row, *r_col, p);
	
	p = (unsigned *)0x2400;
	dst1 = (unsigned *) e_get_global_address(*r_row, *r_col, p);
	
	p = (unsigned *)0x2800;
	dst2 = (unsigned *) e_get_global_address(*r_row, *r_col, p);
	
	p = (unsigned *)0x4000;
	mst = (unsigned *) e_get_global_address(*r_row, *r_col, p);
	
	// Send the signal to receiver and let it start dma engine
	mst[0] = 0xdeadbeef;
	
	// Start writing to the auto register of slave core
	for(j=0; j<8; j++)
	{
		for(i=0; i<tran; i++)
		{
			e_wait(E_REG_CTIMER0 , 100000);
			auto0[0] += 4 ;
			
		}
		e_wait(E_REG_CTIMER0, 100000);
	}
	
	// Check the destination buffer value
	index = checkbuffer(dst,  (unsigned)0x00000004, tran*3);

	if(index == 0)
	{
		mailbox[0] = 0xffffffff;
	}else
	{
		mailbox[0] = 0x00000000;
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
		a = a + 4;
	}	
	return flag;
}
