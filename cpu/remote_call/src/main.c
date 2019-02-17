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
// This is the host side of the Remote Call example project.
//
// This program runs on the linux host and invokes the emain_master.c 
// and emain_slave.c program. The program activits the entire board and
// choose on core as the master to remote call all other 15 cores. The
// slaves cores attatch their interrupts handler with fibonacci functions.
// The master remote generats interruptions on slaves, then check if the 
// slaves return expected results. A success/error message is printed on 
// the terminal according to the results.
//
// This isn't a complete version. the program now can only run for one 
// iteration, it failed to run for the second iteration.
//
// Aug-2013, XM.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include <e-loader.h>


#define host (0xdeadbeef)

int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j, k, fault;
	int zero = 0, done;
	unsigned result[16];
	e_platform_t platform;
	e_epiphany_t dev;


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

	
	//start the test
	for (i=0;i<1;i++)
	{
		//for (j=3;j>1;j--)
		for (j=0;j<1;j++)
		{




			//use core(i,j) to remote call all other cores
			e_load_group("emain_slave.elf", &dev, 0, 0, rows, cols, E_FALSE);
			e_load("emain_master.elf", &dev, i, j, E_FALSE);

			// set done flag to zero
			e_write(&dev, i, j, 0x5800, &zero, sizeof(zero));

			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			usleep(10000);
			e_start_group(&dev);

			// Wait for completion
			do {
				usleep(10000);
				e_read(&dev, i, j, 0x5800, &done, sizeof(done));
			} while (!done);

			//check the result
			fault = 0x0;
			e_read(&dev, i, j, 0x5000, &result[0], 16*sizeof(int));
			for (k=0;k<16;k++)
			{
				if (k == (i*4+j))
				{
					if (result[k] != host)
					{
						fprintf(stderr, "Result from Core(%2d,%2d) is 0x%08x! Expecting 0x%08x! This one is the host!\n",(k/4), (k%4), result[k], host);
						fault++;
					}
				}
				else
				{
					if (result[k] != 0x0)
					{
						fprintf(stderr, "Result from Core(%2d,%2d) is 0x%08x! Expecting 0x%08x! This one is   a slave!\n",(k/4), (k%4), result[k], 0x0);
						fault++;
					}
				}
			}
	
			if (fault == 0)
				fprintf(stderr, "\ntest Remote Call passed!\n\n");
			else
				fprintf(stderr, "\ntest Remote Call failed! Number of faults is %d!\n\n", fault);

		
		}
	}

			e_close(&dev);
			e_finalize();	


	return fault ? EXIT_FAILURE : EXIT_SUCCESS;
}

