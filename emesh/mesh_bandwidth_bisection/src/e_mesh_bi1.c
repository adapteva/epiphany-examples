/*
e_mesh_bi1.c

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

// This is the DEVICE side of the e_mesh bandwidth example.
// Wait for the start signal to transfer data to the related
// core on the other side of the chip.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void)
{
	unsigned time_c, time_p;
	unsigned time;
	unsigned tran,k,i,j,q,h,m,n;
	unsigned *commander;
	unsigned *n_row, *n_col, *p, *nei_row, *nei_col;
	unsigned *neighbour, *neighbour0;
	unsigned *master;
	unsigned *counter;
	
	// Define the mailbox
	master = (unsigned *)0x2000;
	n_row = (unsigned *)0x6000;
	n_col = (unsigned *)0x6004;
	neighbour0 = (unsigned *)0x6008;
	nei_row = (unsigned *) 0x600c;
	nei_col = (unsigned *) 0x6010;
	p =(unsigned *) 0x2000;
	commander = (unsigned *)0x6100;
	counter = (unsigned *)0x80806300;
	tran = 2048;
	
	// Get the neighbour global address
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	k = (*n_row)*e_group_config.group_cols + (*n_col);
	
	commander[0] = 0x00000000;
	counter[0] = 0;
	
	// Broadcast to the next core
	p = (unsigned *)0x6100;
	e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, nei_row, nei_col);
	neighbour0 = (unsigned *) e_get_global_address(*nei_row, *nei_col, p) ;
	
	// Initialize master and slave
	for(i=0; i<tran; i++)
	{
		master[i] = 0xdeadbee2;
		neighbour[i] = 0x00000000;
	}
			
	// Waiting for the signal to start transfering
	while(commander[0] != (unsigned)0xdeadbeef)
	{};
	
	// Broadcast the signal to neighbour
	neighbour0[0] = 0xdeadbeef;
	
	// Write to all neighbour cores
	e_dma_copy(neighbour, master, 0x2000);	
	
	while(1)
	{
		if(neighbour[2047] == 0xdeadbee2)
		{
			counter[e_group_config.core_row * (e_group_config.group_cols/2) + e_group_config.core_col] = 1;
			break;
		}
	}
		
	return 0;
}
