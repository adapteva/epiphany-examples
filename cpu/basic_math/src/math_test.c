/*
math_test.c

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

// This is the HOST side of the basic math example.
// The program initializes the Epiphany system, load program
// to each core on the chip and read results from the mailbox
// in each core.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define _BufSize   (4096)
#define _BufOffset (0x01000000)



int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i,j,m,n,k,flag;
	unsigned int mask0, mask1;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	char emsg[_BufSize];


	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	e_alloc(&emem, _BufOffset, _BufSize);	
	
    // Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);

	// Load the device program onto all the eCores
	// To get the verified values
	//e_load_group("e_math_test.srec", &dev, 0, 0, platform.rows,  platform.cols, E_FALSE);

	// To test 
	e_load_group("e_math_test1.srec", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);
	
	for (i=0; i<platform.rows ; i++)
	{	
		for (j=0; j<platform.cols; j++)
		{

				//Draw to a certain core
				row=i;
				col=j;
				coreid = (row + platform.row) * 64 + col + platform.col;
				//fprintf(stderr,"%3d: Message from eCore 0x%03x (%2d,%2d) : \n",(i*platform.cols+j),coreid,row,col);
				e_start(&dev, i, j);
				usleep(100000);
			
				// Wait for core program execution to finish
				// Read message from shared buffer
			
			
				e_read(&emem, 0, 0, 0x0, emsg, _BufSize);
				e_read(&dev, i, j, 0x6000, &flag, sizeof(flag));

				// Print the message and close the workgroup.
				if(flag == 14)
				{
				}else
				{
				  fprintf(stderr,"FAIL!\n");
				}
				//Only print out messages on core 0
				if(i==0 & j==0){
				  fprintf(stderr, "%s\n", emsg);
				}
		} 
	}

	// Close the workgroup
	e_close(&dev);
	
	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_free(&emem);
	e_finalize();

	return 0;
}
