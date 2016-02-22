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
// The program activits cores on the board one by one to run varies
// assembly command. A success/error message is printed on the terminal 
// according to the results.
//
// Aug-2013, XM.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e-hal.h>

#define _BufSize   (4096)
#define _BufOffset (0x01000000)

int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	char emsg[_BufSize];

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_get_platform_info(&platform);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	e_alloc(&emem, _BufOffset, _BufSize);

	//open the workgroup
	rows = platform.rows;
	cols = platform.cols;
	e_open(&dev, 0, 0, rows, cols);

	
	//load the device program on the board
	e_load_group("emain.elf", &dev, 0, 0, rows, cols, E_FALSE);




	for (i=0; i<platform.rows; i++){
		for (j=0; j<platform.cols; j++){


			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);

			//wait for core to execute the program
			usleep(200000);


			//print the message
			e_read(&emem, 0, 0, 0x0, emsg, _BufSize);
			fprintf(stderr, "%s\n", emsg);
			
		}	
	}	


	e_close(&dev);
	e_free(&emem);
	e_finalize();

	

	return 0;
}

