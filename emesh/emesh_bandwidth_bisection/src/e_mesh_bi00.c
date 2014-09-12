/*
e_mesh_bi00.c

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
// This core will measure clock cycles of transfer and broadcast
// start signal.

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
	unsigned *n_row, *n_col, *p;
	unsigned *mailbox;
	unsigned *neighbour;
	unsigned *master;
	unsigned *counter;
	unsigned *neighbour0, *neighbour1;
	unsigned *nei_row, *nei_col;

	// Define the mailbox
	neighbour0 = (unsigned *)0x6500;
	neighbour1 = (unsigned *)0x6600;
	master = (unsigned *)0x2000;
	n_row = (unsigned *)0x6000;
	n_col = (unsigned *)0x6004;
	nei_row = (unsigned *)0x6008;
	nei_col = (unsigned *)0x600c;
	p =(unsigned *) 0x2000;
	commander = (unsigned *)0x6100;
	counter = (unsigned *)0x6300;
	mailbox = (unsigned *)0x5000;
	tran = 2048;

	// Clear the start commander and initialize the mutex and the counter
	commander[0] = 0x00000000;

	for(i=0; i<(e_group_config.group_rows*e_group_config.group_cols/2); i++)
	{
		counter[i] = 0;
	}

	k = e_group_config.core_row*e_group_config.group_cols + e_group_config.core_col;

	// Get the neighbour global address
	e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;

	// Broadcast to the neighbours
	p = (unsigned *)0x6100;
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, nei_row, nei_col);
	neighbour0 = (unsigned *) e_get_global_address(*nei_row, *nei_col, p) ;

	e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, nei_row, nei_col);
	neighbour1 = (unsigned *) e_get_global_address(*nei_row, *nei_col, p) ;

	// Initialize master and slave
	for(i=0; i<tran; i++)
	{
		master[i] = 0xdeadbee0;
		neighbour[i] = 0x00000000;
	}

	// Set the ctimer
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;

	// Waiting for the signal to start transfering
	while(commander[0] != 0xdeadbeef)
	{};

	// Broadcast the signal to the neighbours
	neighbour0[0] = 0xdeadbeef;
	neighbour1[0] = 0xdeadbeef;

	// Start the ctimer and select the time type
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);

	// Write to the corresponding core
	e_dma_copy(neighbour, master, 0x2000);

	while(1)
	{
		if(neighbour[2047] == 0xdeadbee0)
		{
			counter[e_group_config.core_row * (e_group_config.group_cols/2) + e_group_config.core_col] = 1;
			break;
		}
	}

	while(1)
	{
		if(counter[0]&counter[1]&counter[2]&counter[3]&counter[4]&counter[5]&counter[6]&counter[7]!= 0)
		//if(counter[0]!=0)
		{
			time_c = e_ctimer_get(E_CTIMER_0);
			break;
		}
	}

	time = time_p - time_c;

	mailbox[0] = time;
	return 0;
}
