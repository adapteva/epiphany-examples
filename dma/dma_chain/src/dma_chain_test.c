/*
dma_chain_test.c

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

// This is the HOST side of the dma chain example.
// The program initializes the Epiphany system, load program
// to each core on the chip and read results from the mailbox
// in each core.


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
	unsigned flag  = 0x00000000;
	unsigned flag1 = 0x00000000;
	unsigned flag2 = 0x00000000;
	unsigned flag3 = 0x00000000;
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	//e_set_host_verbosity(H_D2);
	//e_set_host_verbosity(H_D1);
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);


	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	//e_alloc(&emem, _BufOffset, _BufSize);	
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	
	// Load the device program onto core (0,0)
	e_load_group("e_dma_chain_test.srec", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

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
			e_read(&dev, i, j, 0x6100, &flag1, sizeof(flag1));
			e_read(&dev, i, j, 0x6104, &flag2, sizeof(flag2));
			e_read(&dev, i, j, 0x6108, &flag3, sizeof(flag3));

			// Print the message and close the workgroup.
			if(flag == 0xffffffff)
			{
				fprintf(stderr, "PASS for word size!\n");
			}else
			{
				fprintf(stderr, "Fail for word size!\n");
				err = 1;
			}
	
			if(flag1 == 0xffffffff)
			{
				fprintf(stderr, "PASS for doubleword size!\n");
			}else
			{
				fprintf(stderr, "Fail for doubleword size!\n");
				err = 1;
			}
			
			if(flag2 == 0xffffffff)
			{
				fprintf(stderr, "PASS for halfword size!\n");
			}else
			{
				fprintf(stderr, "Fail for halfword size!\n");
				err = 1;
			}
		
			if(flag3 == 0xffffffff)
			{
				fprintf(stderr, "PASS for byte size!\n");
			}else
			{
				fprintf(stderr, "Fail for byte size!\n");
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
