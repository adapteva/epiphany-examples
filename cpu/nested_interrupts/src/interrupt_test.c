/*
interrupt_test.c

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

// This is the HOST side of the interrupt example.
// The program initializes the Epiphany system, load program
// to each core on the chip and read results from the mailbox
// in each core.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define _BufSize   (8192)
#define _BufOffset (0x01000000)
#define _SeqLen    (4)
#define _SeqLen_row  (4)
#define _SeqLen_col  (4)



int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	char emsg[_BufSize];
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
	e_alloc(&emem, _BufOffset, _BufSize);	
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Reset the workgroup
	for (m=0; m<platform.rows; m++)
	{	for(n=0; n<platform.cols;n++)
		{	
			ee_reset_core(&dev, m, n);
		}
	}
	
	// Load the device program onto all the eCores
	e_load_group("e_nested_test.elf", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);

	// Select one core to work 
	for (i=0; i<platform.rows; i++)
	{
		for (j=0; j<platform.cols; j++)
		{
			// Draw to a certain core
			row=i;
			col=j;
			coreid = (row + platform.row) * 64 + col + platform.col;
			fprintf(stderr,"%d: Message from eCore 0x%03x (%2d,%2d): \n",(i*platform.cols+j),coreid,row,col);
		
			e_start(&dev, i, j);			
			usleep(1000000);
			
			// Wait for core program execution to finish
			// Read message from shared buffer
				
			e_read(&emem, 0, 0, 0x0, &emsg, _BufSize);

			// Print the message and close the workgroup.
			fprintf(stderr, "%s\n", emsg);
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

