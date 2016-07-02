/*
mesh_traffic.c

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

// This is the HOST side of the mesh traffic example.
// In this test, we program the config register to count 
// different mesh events.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>
#define mas_row (1)
#define mas_col (1)


int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, k, m, q, mode, signal;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned desired, real;
	unsigned time[16][13];
	row = mas_row;
	col = mas_col;
	signal = 0xdeadbeef;
	
	// For dma copy, define the number of desired transaction
	desired = 0x3c00;
	
	srand(1);
	
	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Let other cores know the core id of the specific core
	for(i=0; i<platform.rows; i++)
	{
		for(j=0; j<platform.cols; j++)
		{
			if((i!=mas_row)|(j!=mas_col))
			{
				e_write(&dev, i, j, 0x5000, &row, sizeof(row));
				e_write(&dev, i, j, 0x5004, &col, sizeof(col));
			}		
		}
	}
	
	// Load device program onto the receiver
		
	e_load("e_mesh_receiver.elf",&dev, mas_row, mas_col, E_TRUE);
	
	
	// Load device program onto the transmitter
	for (i=0; i<(platform.rows); i++)
	{
		for(j=0; j<(platform.cols); j++)
		{
			if((i!=mas_row)|(j!=mas_col))
			{
				e_load("e_mesh_transmitter.elf",&dev, i, j, E_TRUE);
			}
		}
	}
		
	// Wait for all cores to initialize
	
	usleep(1000);
	
	for(q=0; q<13; q++)
	{
		// Select the mesh event
		mode = q;	
		
		// Tell each core the mesh event
		for (i=0; i<(platform.rows); i++)
		{
			for(j=0; j<(platform.cols); j++)
			{
				e_write(&dev, i, j, 0x5400, &mode, sizeof(mode));
			}
		}
	
		usleep(10000);
		
		// Send the start signal to receiver core
		e_write(&dev, mas_row, mas_col, 0x5100, &signal, sizeof(signal));
		
		usleep(500000);
		
	 }
	 
	 // Read from mailbox of all the cores
	 for(i=0; i<platform.rows; i++)
	 {
	 	for(j=0; j<platform.cols; j++)
	 	{
	 		for(k=0; k<13; k++)
	 		{
	 			e_read(&dev, i, j, (0x6000+k*4), &time[i*platform.cols+j][k], sizeof(real));
	 		}
		}
	 }
	 
	// Test if the results make sense
	real = time[mas_row*platform.cols+mas_col][8]+time[mas_row*platform.cols+mas_col][9]+
		  time[mas_row*platform.cols+mas_col][10]+time[mas_row*platform.cols+mas_col][11]-
		  time[(mas_row-1)*platform.cols+mas_col][0]-time[(mas_row)*platform.cols+mas_col-1][0]-
		  time[(mas_row)*platform.cols+mas_col+1][0]-time[(mas_row+1)*platform.cols+mas_col][0];
		  
	if ((real < (unsigned)desired * 1.1) && (real > (unsigned)desired * 0.1))
	{
		fprintf(stderr, "PASS for verification!\n");	
	}else
	{
		fprintf(stderr, "FAIL for verification!\n");
	}

	// Close the workgroup
	e_close(&dev);
	
	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}
	
