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

// This is the host side of the Mem Protection example project.
//
// This program runs on the linux host and invokes the emain.c program.
// The program activits the entire board, sets the mem protect registers,
// and tries to write to those protected pages. The mpf_isr handler returns
// an address showing which address location failed. A success/error message
//  is printed on the terminal according to the results.
//
// Aug-2013, XM.


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>

#define _BufSize      (10 * sizeof(int))
#define _BufOffset    (0x01000000)
#define mail_offset   (0x0)

typedef enum
{
	clear         = 0,
	write_success = 2,
	write_failed  = 3,
	finished      = 9,
}test_state;

unsigned address;
unsigned mbox[10];




int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j, fault;
	e_bool_t endloop;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t      emem;

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
	e_set_host_verbosity(H_D0);

	for (i=0; i<rows; i++){
		for (j=0; j<cols; j++)
		{
			mbox[0] = clear;
			e_write(&emem, 0, 0, mail_offset, &mbox[0], sizeof(unsigned));
			usleep(100000);

			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);

			fault = 0x0;
			endloop = E_FALSE;
			while (!endloop)
			{
				switch (mbox[0])
				{
					case clear:
						break;

					case write_success:
						if (mbox[0] != mbox[3])
						{
							fprintf(stderr, "Unexpected performance:\t");
							fprintf(stderr, "Write to unprotected memory 0x%04x succeeded. Expecting failure!\n", mbox[1]);
							fault++;
						}
						mbox[0] = clear;
						e_write(&emem, 0, 0, mail_offset, &mbox[0], sizeof(unsigned));
						break;

					case write_failed:
						if (mbox[0] != mbox[3])
						{
							fprintf(stderr, "Unexpected performance:\t");
							fprintf(stderr, "Write to   protected memory 0x%04x    failed. Expecting succeed!\n", mbox[1]);
							fault++;
						}
						mbox[0] = clear;
						e_write(&emem, 0, 0, mail_offset, &mbox[0], sizeof(unsigned));
						break;

					case finished:
						endloop = E_TRUE;
						continue;

					default:
						fprintf(stderr, "Warnning: Unexpected mailbox value (%d)!\n", mbox[0]);
						break;
				}
				usleep(100000);
				e_read(&emem, 0, 0, mail_offset, &mbox[0], 4*sizeof(unsigned));
			}

			if (fault == 0)
				fprintf(stderr, "\ntest14: Mem_Protect passed!\n\n\n");
			else
				fprintf(stderr, "\ntest14: Mem_Protect Failed!\t\t\t\tFault = %d!\n\n\n", fault);
		}	
	}	

	e_close(&dev);
	e_free(&emem);
	e_finalize();

	return 0;
}

