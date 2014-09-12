/*
dma_slave_test.c

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

// This is the HOST side of the DMA slave mode example.
// The program initializes the Epiphany system,
// selects an eCore to be transmitter and another to be
//the receiver and then loads and launches the device
//program on that eCore. It then reads the mailbox in
//transmitter core for the core's output message.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define r_row (1)
#define r_col (1)
#define t_row (2)
#define t_col (1)


int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned flag;
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.

	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);


	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	//e_alloc(&emem, _BufOffset, _BufSize);

    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);


	// Load the device program onto receiver core (1,1) and transmitter core (2,1)
	e_load("e_dma_slave_test.srec", &dev, r_row, r_col, E_FALSE);
	e_load("e_dma_slave_test1.srec", &dev, t_row, t_col, E_FALSE);

	// Start the receiver core

	row=r_row;
	col=r_col;
	coreid = (row + platform.row) * 64 + col + platform.col;
	fprintf(stderr,"%3d: Message from eCore 0x%03x (%2d,%2d) : \n",(row*platform.cols+col),coreid,row,col);
	// Start device
	e_start(&dev, r_row, r_col);
	usleep(10000);

	// Tell transmitter the coordinate of receiver core
	e_write(&dev, t_row, t_col, 0x6500, &row, sizeof(row));
	e_write(&dev, t_row, t_col, 0x6504, &col, sizeof(col));

	usleep(1000);

	// Start the transmitter core
	e_start(&dev, t_row, t_col);

	// Wait for core program execution to finish
	usleep(300000);

	// Read message from the mailbox in transmitter core
	e_read(&dev, t_row, t_col, 0x6100, &flag, sizeof(flag));

	// Check if the result is right and print the message.
	if(flag==(unsigned)0xffffffff)
	{
		fprintf(stderr, "PASS!\n");
	}else
	{
		fprintf(stderr,"Fail!\n");
	}

	// Close the workgroup
	e_close(&dev);

	// Finalize the
	// e-platform connection.
	e_finalize();

	return 0;
}
