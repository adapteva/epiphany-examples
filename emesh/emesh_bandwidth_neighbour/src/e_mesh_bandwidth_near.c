/*
e_mesh_bandwidth_near.c

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
// This core will write to and read from the four neighbour
// cores. C_TIMER_0 register is used to record clock cycles.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void)
{
	unsigned time_c, time_p;
	unsigned time;
	unsigned tran,k,i,j,q,h,m,n;
	unsigned *box;
	unsigned *n_row, *n_col, *p;
	unsigned *neighbour_n;
	unsigned *neighbour_s;
	unsigned *neighbour_w;
	unsigned *neighbour_e;
	unsigned *master;

	// Define the mailbox
	master = (unsigned *)0x2000;
	box = (unsigned *) 0x5000;
	n_row = (unsigned *)0x6000;
	n_col = (unsigned *)0x6004;
	tran = 2048;
	p =(unsigned *) 0x2000;

	// Get the neighbour global address
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour_e = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;

	e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour_w = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;

	e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, n_row, n_col);
	neighbour_s = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;

	e_neighbor_id(E_PREV_CORE, E_COL_WRAP, n_row, n_col);
	neighbour_n = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;

	// Test the writing bandwidth
	// Initialize master and slave
	for(i=0; i<tran; i++)
	{
		master[i] = 0xdeadbeef;
		neighbour_e[i] = 0x00000000;
		neighbour_w[i] = 0x00000000;
		neighbour_s[i] = 0x00000000;
		neighbour_n[i] = 0x00000000;
	}


	// Set the ctimer
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;

	// Start the ctimer and select the time type
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);

	// Write to all neighbour cores
	e_dma_copy(neighbour_e, master, 0x2000);
	e_dma_copy(neighbour_w, master, 0x2000);
	e_dma_copy(neighbour_s, master, 0x2000);
	e_dma_copy(neighbour_n, master, 0x2000);

	// Get the time now
	time_c = e_ctimer_get(E_CTIMER_0);

	time = time_p - time_c;

	// Output the result
	box[0] = time;

	// Test the reading bandwidth
	// Initialize master and slave
	for(i=0; i<tran; i++)
	{
		master[i] = 0x00000000;
		neighbour_e[i] = 0xdeadbee1;
		neighbour_w[i] = 0xdeadbee2;
		neighbour_s[i] = 0xdeadbee3;
		neighbour_n[i] = 0xdeadbee4;
	}


	// Set the ctimer
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;

	// Start the ctimer and select the time type
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);

	// Read from all neighbour cores
	e_dma_copy(master, neighbour_e, 0x2000);
	e_dma_copy(master, neighbour_w, 0x2000);
	e_dma_copy(master, neighbour_s, 0x2000);
	e_dma_copy(master, neighbour_n, 0x2000);

	// Get the time now
	time_c = e_ctimer_get(E_CTIMER_0);

	time = time_p - time_c;

	// Output the result
	box[1] = time;
	return 0;
}
