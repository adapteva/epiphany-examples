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
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <e-hal.h>

#define _BufOffset    (0x01000000)

#define length(array) (sizeof(array)/sizeof(*(array)))

typedef enum
{
	clear         = 0,
	write_success = 2,
	write_failed  = 3,
	finished      = 9
} test_state;

unsigned address;

struct state {
	uint32_t expect_fail[8];
	uint32_t expect_pass[8];
	uint32_t isr_flag;
	uint32_t finished;
};

struct state mbox;



int main(int argc, char *argv[])
{
	unsigned rows, cols, coreid, i, j, k, fault, rc=0;
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
	e_alloc(&emem, _BufOffset, sizeof(mbox));

	//open the workgroup
	rows = platform.rows;
	cols = platform.cols;
	e_open(&dev, 0, 0, rows, cols);

	//load the device program on the board
	e_load_group("emain.srec", &dev, 0, 0, rows, cols, E_FALSE);
	e_set_host_verbosity(H_D0);

	for (i=0; i<rows; i++) {
		for (j=0; j<cols; j++) {
			fault=0;
			memset((void *) &mbox, 0, sizeof(mbox));
			e_write(&emem, 0, 0, 0, &mbox, sizeof(mbox));

			coreid = (i + platform.row) * 64 + j + platform.col;
			fprintf(stderr, "Message from eCore 0x%03x (%2d,%2d): \n", coreid, i, j);
			e_start(&dev, i, j);

			while (1) {
				e_read(&emem, 0, 0, 0, &mbox, sizeof(mbox));
				if (mbox.finished)
					break;
				usleep(100000);
			}

			for (k = 0; k < length(mbox.expect_fail)-1; k++) {
				if (mbox.expect_fail[k] != write_failed) {
					printf("Expected WRITE_FAIL,\tgot WRITE_SUCCESS,\tpage: %d\n", k);
					fault++;
				}
			}

			for (k = 0; k < length(mbox.expect_pass)-1 ; k++) {
				if (mbox.expect_pass[k] != write_success) {
					printf("Expected WRITE_SUCCESS,\tgot WRITE_FAIL,\tpage: %d\n", k);
					fault++;
				}
			}


			if (fault == 0)
				fprintf(stderr, "PASSED\n\n");
			else {
				fprintf(stderr, "FAILED\n\n");
				rc = 1;
			}
		}
	}

	e_close(&dev);
	e_free(&emem);
	e_finalize();

	return rc;
}

