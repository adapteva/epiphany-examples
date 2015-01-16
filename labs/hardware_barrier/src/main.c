/*
  main.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Xin Mao <maoxin99@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This is the host side of the Hardware Barrier example project.
//
// This program runs on the linux host and invokes the emain.c program.
// The program activits the entire board and have them do the counting
// simultaneously. The hardware barrier is inserted in each counting to 
// syncronize the counting. The result of counting is printed on the 
// terminal. A success/error message is printed on the terminal according
// to the performance of the counting.
//
// Jul-2013, XM.



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>

#define _MAX_CORES 64
#define _BufSize   (4 * _MAX_CORES)
#define _BufOffset (0x01000000)

int main(int argc, char *argv[])
{
	unsigned rows, cols, ncores, coreid, i, j;
	int result[_MAX_CORES];
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	int fault;

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	e_alloc(&emem, _BufOffset, _BufSize);

	//open the workgroup
	rows   = platform.rows;
	cols   = platform.cols;
	e_open(&dev, 0, 0, rows, cols);

	//load the device program on the board
	e_load_group("emain.srec", &dev, 0, 0, rows, cols, E_FALSE);
	e_start_group(&dev);

	ncores = rows * cols;
	printf("num-cores = %4d\n", ncores);
	fault = 0x0;
	for (i=0; i<100; i++)
	{
		//read the results and print them on the terminal
		usleep(100000);
		e_read(&emem, 0, 0, 0x0, &result, ncores*sizeof(int));
		
		for (j=0; j<ncores; j++)
		{
			if (result[j] != result[0])
				fault++;
		}
		
		printf("[%3d] ", i);
		for (j=0;j<ncores;j++)
			printf("%04x ", result[j]);
		printf("\n");
	}


	//print the success/error message duel to the number of fault
	if (fault == 0)
		printf("\ntest #20: Hardware Barrier Passed!\n");
	else
		printf("\ntest #20: Hardware Barrier Failed! Fault is 0x%08x!\n", fault);

	e_close(&dev);
	e_free(&emem);
	e_finalize();

	return fault != 0;
}

