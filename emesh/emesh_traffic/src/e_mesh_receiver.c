/*
e_mesh_receiver.c

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

// This is the DEVICE side. This is the receiver which will receive 
// data from all the other cores at the same time. In this program,
// after programing the config register, the CTIMER_0 will measure
// the mesh event during the transfer.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"
#define E_CTIMER_MESH_0 (0xf)
#define E_CTIMER_MESH_1 (0xe)

// Different mesh counter types for E_CTIMER_MESH_0 and E_CTIMER_MESH_1
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
	unsigned time_c1, time_p1;
	unsigned time1;
	unsigned i,j,q,m,n,k;
	unsigned *commander;
	unsigned *counter;
	unsigned *mailbox,*mode;
	unsigned *n_row, *n_col;
	unsigned *neighbour0, *neighbour1, *neighbour2, *neighbour3, *p;
	
	commander = (unsigned *)0x5100;
	counter = (unsigned *)0x5300;
	mailbox = (unsigned *)0x6000;
	mode = (unsigned *)0x5400;
	n_row = (unsigned *)0x5008;
	n_col = (unsigned *)0x500c;
	p = (unsigned *)0x5100;
	
	// Broadcast to neighbours
	e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour0 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);
	neighbour1 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_PREV_CORE, E_COL_WRAP, n_row, n_col);
	neighbour2 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, n_row, n_col);
	neighbour3 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	
	// Get core information
	k = e_group_config.core_row * e_group_config.group_cols + e_group_config.core_col;
	
	while(1)
	{
		// Clear the counter of finishing transfering
		for(m=0; m<(e_group_config.group_rows*e_group_config.group_cols); m++)
		{
			counter[m] = 0;
		}
	
		// Initialize the commander and counter
		counter[k] = 1;
		
		//Clear the mode box
		mode[0] = 0xdeadbeef;	

		// Clear the start commander 
		commander[0] = 0x00000000;
		
		// Wait for the mesh event
		while(mode[0] == 0xdeadbeef)
		{};
		
		q = mode[0];
		
		mesh_reg = e_reg_read(E_REG_MESHCFG);
		mesh_reg_modify = mesh_reg & 0xffffff0f;
		mesh_reg_modify = mesh_reg_modify |mesh_type[1][q]; 
		e_reg_write(E_REG_MESHCFG, mesh_reg_modify);
		
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
		
		
		while((counter[0]&counter[1]&counter[2]&counter[3]&counter[4]&counter[5]&counter[6]
	     &counter[7]&counter[8]&counter[9]&counter[10]&counter[11]&counter[12]&counter[13]
	     &counter[14]&counter[15]) == 0) {};
	     
		time_c = e_ctimer_get(E_CTIMER_0);
		
		time = time_p - time_c;		
		mailbox[(*mode)] = time;
		
		// Load the original value to E_REG_MESHCFG system register
		e_reg_write(E_REG_MESHCFG, mesh_reg);
		
		if(mode[0] == 12)
		{
			break;
		}
	}
	return 0;
}
