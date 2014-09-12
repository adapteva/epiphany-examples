/*
  hello_world.c

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

// This is the host side of the ctimer example project.
//
// This program runs on the linux host and invokes the emain.c program.
// The program activits cores on the board one by one to do a simple
// matrix multiplication. The calculation is performed with three different
// versions: compiled by complier with level 3 optimization, with
// hardware_loop, without hardware_loop. A success/error message is
// printed on the terminal according to the results.
//
// The result shows that: the adapting of hardware_loop saves five cycles
// per iteration(saves 2500 cycles for 500 iterations).
//
// Aug-2013, XM.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>

#define sizeN (3)

int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	unsigned time[sizeN];
	int result[sizeN];

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	//open the workgroup
	rows = platform.rows;
	cols = platform.cols;
	e_open(&dev, 0, 0, rows, cols);

	//load the device program on the board
	e_load_group("emain.srec", &dev, 0, 0, rows, cols, E_FALSE);

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);

			//wait for core to execute the program
			usleep(200000);

			//check results
			e_read(&dev, 0, 0, 0x5100, &result[0], sizeN*sizeof(int));
			e_read(&dev, 0, 0, 0x5200, &time[0], sizeN*sizeof(unsigned));



			if ((result[1] == result[0]) && (result[1] == result[2]) && (time[1]<time[0]) && (time[1]<time[2]))
				fprintf(stderr, "\ntest hardware_loop passed!\n\n");
			else
			{
				fprintf(stderr, "\ntest hardware_loop failed!\n");
				fprintf(stderr, "result:\tauto =  %10d   hw =  %10d   sf =  %10d \n", result[0],result[1],result[2]);
				fprintf(stderr, "time:  \tauto = %5d cycles  hw = %5d cycles  sf = %5d cycles \n\n", time[0],time[1],time[2]);
			}

		}
	}


	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_close(&dev);
	e_free(&emem);
	e_finalize();



	return 0;
}

