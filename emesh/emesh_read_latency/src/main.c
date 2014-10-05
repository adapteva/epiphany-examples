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
// The program activits the entire board. Each core reads from all the
// other fifteen cores and records the number of cycles spend for the
// read. A success/error message is printed on the terminal according
// to the comparison between the result and the expecting result.
//
// Jul-2013, XM.


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>

#define _BufSize   (4096)
#define _BufOffset (0x01000000)

int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j, k;
	unsigned fault, result[32];
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;

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
	rows = platform.rows;
	cols = platform.cols;
	e_open(&dev, 0, 0, rows, cols);


	//load the device program on the board
	e_load_group("emain.srec", &dev, 0, 0, rows, cols, E_FALSE);


	for (i=0;i<rows;i++)
	{
		for (j=0;j<cols;j++)
		{

			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);
			usleep(100000);
			e_read(&dev, i, j, 0x5200, &result[0], 32*sizeof(unsigned));

			//chech result
			fault = 0x0;
			for (k=0;k<16;k++)
			{
				if ((result[k]<result[k]*0.9)||(result[k]>=result[k]*1.1))
				{
					fault++;
					fprintf(stderr, "Read from core (%2d,%2d) spent %d cycles, expecting %d cycles!\n",(k/4),(k%4),result[k],result[k+16]);
				}
			}


			if (fault == 0x0)
				fprintf(stderr, "\ntest emesh_read_latency passed!\n\n");
			else
			{
				fprintf(stderr, "\ntest emesh_read_latency failed!\n");
			}

		}
	}

	e_close(&dev);
	e_free(&emem);
	e_finalize();



	return 0;
}

