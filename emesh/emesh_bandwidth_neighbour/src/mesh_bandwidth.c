/*
mesh_bandwidth.c

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

// This is the HOST side of the e_mesh bandwidth example
// In this test, the host selects one core to be the target 
// core and make it write to and read from the 4 neighbour
// cores. And use C_TIMER_0 register to record the clock
// cycles. At the end, to get the bandwidth, divide the size 
// of transfering data by the time. 



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned time_write;
	unsigned time_read;
	float result1;
	float result2;
	srand(1);
	

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	e_init(NULL);
	e_get_platform_info(&platform);
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Load the device program onto core (0,0)
	e_load_group("e_mesh_bandwidth_near.elf", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

	// Launch to each core in every loop
	for (i=0; i<platform.rows; i++)
	{	
		for(j=0; j<platform.cols; j++)
		{
			row=i;
			col=j;
			coreid = (row + platform.row) * 64 + col + platform.col;
			fprintf(stderr,"%3d: Message from eCore 0x%03x (%2d,%2d) : \n",(row*platform.cols+col),coreid,row,col);
			// Start device
			e_start(&dev, row, col);
			
			// Wait for core program execution to finish
			usleep(500000);
			
			// Read message from shared buffer
				
			e_read(&dev, row, col, 0x5000, &time_write, sizeof(time_write));
			e_read(&dev, row, col, 0x5004, &time_read, sizeof(time_read));
			
			// Calculate the bandwidth
			result1 = (32*585937.5)/time_write;
			result2 = (32*585937.5)/time_read;
			
			// Print the message and close the workgroup
			fprintf(stderr, "%.2fMB\n%.2fMB\n", result1, result2);
			
		}
	}

	// Close the workgroup
	e_close(&dev);
	
	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}
