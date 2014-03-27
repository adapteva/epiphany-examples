/*
dma_message_a.c

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

// This is the HOST side of the DMA message example.
// The program initializes the Epiphany system,
// selects an eCore to read data from external memory.
// In message mode, after sending the request, the core
// will be busy until the last data arrives. 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>
#define mas_row (2)
#define mas_col (2)
#define _BufSize (512)
#define _BufOffset (0x01000000)

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	unsigned flag1 = 0x00000000;
	unsigned flag2 = 0x00000000;
	unsigned test = 0x00000000;
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
	
	
	// Load the device program 
	e_load("e_dma_message_a.srec", &dev, mas_row, mas_col, E_TRUE);

	// Wait for core program execution to finish
	usleep(1000000);
		
	// Results from every slave core
	row=mas_row;
	col=mas_col;
	coreid = (row + platform.row) * 64 + col + platform.col;
	fprintf(stderr,"%d: Message from eCore 0x%03x (%2d,%2d) : \n",(row*platform.cols+col),coreid,row,col);
			
	// Read message
	e_read(&dev, row, col, 0x6000, &flag1, sizeof(flag1));
	e_read(&dev, row, col, 0x6004, &flag2, sizeof(flag2));

	// Print the message and close the workgroup.
	if((flag1 == (unsigned) 0x87654321)&&(flag2 != (unsigned) 0x87654321))
	{
		fprintf(stderr, "PASS!\n");
	}else
	{
		fprintf(stderr, "FAIL!\n");
	}
		
	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_free(&emem);
	
	// Close the workgroup
	e_close(&dev);
	
	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}
