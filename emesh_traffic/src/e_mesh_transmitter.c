/*
e_mesh_transmitter.c

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

// This is the DEVICE side. This is the transmitter which will transfer 
// data to receiver core. This device program will be loaded to all the 
// cores except the receiver core. In this situation, there will be wait
// cycles occuring at transmitter core.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

#define E_CTIMER_MESH_0 (0xf)
#define E_CTIMER_MESH_1 (0xe)

unsigned mesh_type[2][13] = {{0x00000200, 0x00000300, 0x00000400, 0x00000500,
                              0x00000600, 0x00000700, 0x00000800, 0x00000900,
                              0x00000a00, 0x00000b00, 0x00000c00, 0x00000d00, 0x00000e00},
                             {0x00000020, 0x00000030, 0x00000040, 0x00000050,
                              0x00000060, 0x00000070, 0x00000080, 0x00000990,
                              0x000000a0, 0x000000b0, 0x000000c0, 0x000000d0, 0x000000e0}};


int main(void)
{
	unsigned mesh_reg;
	unsigned mesh_reg_modify;
	unsigned time_c, time_p;
	unsigned time;
	unsigned tran,k,i,j,h,m,n,q;
	unsigned *mailbox, *mode;
	unsigned *commander;
	unsigned *counter;
	unsigned *master, *slave, *p;
	unsigned *row, *col;
	unsigned *n_row, *n_col;
	unsigned *neighbour0, *neighbour1, *neighbour2, *neighbour3;
	
	row = (unsigned *)0x5000;
	col = (unsigned *)0x5004;
	n_row = (unsigned *)0x5008;
	n_col = (unsigned *)0x500c;
	master = (unsigned *)0x2000;
	p =(unsigned *) 0x2000;
	slave = (unsigned *) e_get_global_address(*row, *col, p);
	commander = (unsigned *)0x5100;
	p = (unsigned *) 0x5300;
	counter = (unsigned *) e_get_global_address(*row, *col, p);
	mailbox = (unsigned *)0x6000;
	mode = (unsigned *)0x5400;
	tran = 2048;
	
	// Core number 
	k = (e_group_config.core_row)*e_group_config.group_cols + (e_group_config.core_col);
	
	// Broadcast to all the other neighbours
	p = (unsigned *)0x5100;
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
		master[i] = 0xdeadbee9;
		slave[i] = 0x00000000;
	}
	
	while(1)
	{
		//Clear the mode box
		mode[0] = 0xdeadbeef;	

		// Clear the start commander 
		commander[0] = 0x00000000;
		
		// Wait for the mesh event
		while(mode[0] == 0xdeadbeef)
		{};
		
		q = mode[0];
		mesh_reg = e_reg_read(E_REG_MESH_CONFIG);
		mesh_reg_modify = mesh_reg & 0xffffff0f;
		mesh_reg_modify = mesh_reg_modify |mesh_type[1][q]; 
		e_reg_write(E_REG_MESH_CONFIG, mesh_reg_modify);
		
		// Set the ctimer
		e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;		
		
		// Waiting for the signal to start transfering
		while(commander[0] != 0xdeadbeef)
		{};
		
		// Start the ctimer and select the time type
		time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_MESH_0);
	
		// Broadcast to all the other neighbours
		neighbour0[0] = 0xdeadbeef;
		neighbour1[0] = 0xdeadbeef;
		neighbour2[0] = 0xdeadbeef;
		neighbour3[0] = 0xdeadbeef;
		
		e_dma_copy(slave, master, 0x2000);	
		
		// Wait for transfer finishing
		while(slave[2047] != 0xdeadbee9 )
		{};
		counter[k] = 1;
	
		// Get the time now
		time_c = e_ctimer_get(E_CTIMER_0);
	
		time = time_p - time_c;		
		
		// Output the result
		mailbox[q] = time;
		
		// Load the original value of E_REG_MESH_CONFIG
		e_reg_write(E_REG_MESH_CONFIG, mesh_reg);
	
		// Check if all the mesh events have been through
		if(q == 12)
		{
			break;
		}
	}
	return 0;
}
