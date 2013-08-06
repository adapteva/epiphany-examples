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

// This is the host side of the ctimer example project.
//
// This program runs on the linux host and invokes the emain.c program.
// The program activits cores on the board one by one, sets the core to 
// run a fir function and records numbers of cycles spend on different
// events. After that, the core goes into "idle" and been awaked by 
// ctimer_1. A success/error message is printed on the terminal according 
// to the results.
//
// Aug-2013, XM.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>

#define _BufSize   (0x100)
#define _BufOffset (0x01000000)


void check();


//set up the expected result list
unsigned expect[] = {565000, 0, 752, 512, 184, 1904, 126500, 510000, 120500, 86300};
unsigned result[10];
char event[10][20];

int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j;
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

	//set up the event list table
	strcpy(event[0], "CLK");
	strcpy(event[1], "IDLE");
	strcpy(event[2], "IALU_INST");
	strcpy(event[3], "FPU_INST");
	strcpy(event[4], "DUAL_INST");
	strcpy(event[5], "E1_STALLS");
	strcpy(event[6], "RA_STALLS");
	strcpy(event[7], "EXT_FETCH_STALLS");
	strcpy(event[8], "EXT_LOAD_STALLS");
	strcpy(event[9], "IALU_INST");

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{

			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);

			//wait for core to execute the program
			usleep(100000);

			e_read(&emem, 0, 0, 0x0, &result, sizeof(unsigned)*10);

			check();
		}	
	}	

	e_close(&dev);
	e_free(&emem);
	e_finalize();

	return 0;
}


void check()
{
	int t, fault;
	
	fault = 0;
	
	for (t=0; t<10; t++)
	{
		if ((result[t]>expect[t]*1.05) || (result[t]<expect[t]*0.95))
		{
			fprintf(stderr, "test02 CTimer failed! cycles spent on event %-18s is %-6d cycles! \
			Expecting %-6d cycles!\n", event[t], result[t], expect[t]);
			fault++;
		}
	}

	if (fault == 0)
		fprintf(stderr, "\ntest02 CTimer passed!\n\n");
	else
		fprintf(stderr, "\ntest02 CTimer failed! Number of faults is %d!\n\n", fault);
	
	return;
}

