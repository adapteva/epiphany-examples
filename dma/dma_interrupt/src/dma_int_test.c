/*
dma_int_test.c

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

// This is the HOST side of the DMA interrupt example.
// The program initializes the Epiphany system,
// selects an eCore to sending data to neighbour core
// by using dma. After start, the core goes into idle.
// After transfering in interrupt mode, the core will 
// wake up and come into the interrupt.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	int err = 0;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned flag;
	unsigned flag1;
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);


    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	
	// Load the device program onto core (0,0)
	e_load_group("e_dma_int_test.srec", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

	// Launch to each core
	for (i=0; i<platform.rows; i++)
	{	
		for(j=0; j<platform.cols; j++)
		{
			row=i;
			col=j;
			coreid = (row + platform.row) * 64 + col + platform.col;
			fprintf(stderr,"%3d: Message from eCore 0x%03x (%2d,%2d) : \n",(row*platform.cols+col),coreid,row,col);
			
			// Start device
			e_start(&dev, i, j);
			
			// Wait for core program execution to finish
			usleep(500000);
			
			// Read message from shared buffer	
			e_read(&dev, i, j, 0x2250, &flag, sizeof(flag));
			e_read(&dev, i, j, 0x3000, &flag1, sizeof(flag1));
			
			// Print the message and close the workgroup.
			if((flag==(unsigned)0xdeadbeef)&&(flag1==(unsigned)0xdeadbeef))
			{
				fprintf(stderr, "PASS!\n");
			}else
			{
				fprintf(stderr,"Fail!\nThe interrupt output is 0x%08x!\nThe return output is 0x%08x!\n",flag,flag1);
				err = 1;
			}
		}
	}

	// Close the workgroup
	e_close(&dev);
	
	// Finalize the
	// e-platform connection.
	e_finalize();

	return err;
}
