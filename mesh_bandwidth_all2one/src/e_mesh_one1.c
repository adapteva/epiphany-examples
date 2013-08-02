/*
e_mesh_one1.c

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
// This program will be loaded to all the cores except target
// core. They wait for the signal to start transfering. When
// transaction finishes, they will send the signal immediately


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
	unsigned *counter;
	unsigned *master, *slave, *p;
	unsigned *row, *col;
	unsigned *n_row, *n_col;
	unsigned *neighbour0, *neighbour1, *neighbour2, *neighbour3;

	master = (unsigned *)0x2000;
	row = (unsigned *)0x6000;
	col = (unsigned *)0x6004;
	p =(unsigned *) 0x2000;
	slave = (unsigned *) e_get_global_address(*row, *col, p);
	commander = (unsigned *)0x6100;
	p = (unsigned *) 0x6300;
	counter = (unsigned *) e_get_global_address(*row, *col, p);
	n_row = (unsigned *)0x5000;
	n_col = (unsigned *)0x5100;
	tran = 2048;
	
	// Clear the start commander 
	commander[0] = 0x00000000;
	
	// Core number 
	k = (e_group_config.core_row)*e_group_config.group_cols + (e_group_config.core_col);
	
	// Get the neighbour addresses 
	p = (unsigned *)0x6100;
	e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour0 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour1 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_PREV_CORE, E_COL_WRAP, n_row, n_col);
	neighbour2 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, n_row, n_col);
	neighbour3 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	// Initialize master and slave
	for(i=0; i<tran; i++)
	{
		master[i] = 0xdeadbee0;
		slave[i] = 0x00000000;
	}
			
	// Waiting for the signal to start transfering
	while(commander[0] != 0xdeadbeef)
	{};
	
	// Broadcast the start signal to all neighbours
	neighbour0[0] = 0xdeadbeef;
	neighbour1[0] = 0xdeadbeef;
	neighbour2[0] = 0xdeadbeef;
	neighbour3[0] = 0xdeadbeef;
	
	// Write to the corresponding core
	e_dma_copy(slave, master, 0x2000);	
	
	while(1)
	{
		if(slave[2047] == 0xdeadbee0)
		{
			counter[k] = 1;
			break;
		}
	}
		
	return 0;
}
