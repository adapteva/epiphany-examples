/*
  shm_test.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yaniv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This is the HOST side of the Hello World example.
// The program initializes the Epiphany system,
// randomly draws an eCore and then loads and launches
// the device program on that eCore. It then reads the
// shared external memory buffer for the core's output
// message.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <e-hal.h>
#include <a_trace.h>

const unsigned ShmSize = 128;
const char ShmName[] = "test_shm";
const unsigned SeqLen = 10;
const char HostMsg[] = "Host Initialized. You should not see this!";

int main(int argc, char *argv[])
{
	unsigned     row, col, coreid, i, e_status;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t      mbuf;
	int          retval = EXIT_SUCCESS; 
	srand((unsigned int)time(NULL));

	e_set_loader_verbosity(L_D0);
	e_set_host_verbosity(H_D0);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	if ( E_OK != e_init(NULL) ) {
		fprintf(stderr, "Epiphany HAL initialization failed\n");
		return EXIT_FAILURE;
	}

	if ( E_OK != e_reset_system() ) {
		fprintf(stderr, "Epiphany system reset failed\n");
		retval = EXIT_FAILURE;
		goto err_out3;
	}

	fprintf(stderr, "Getting platform info\n");
	if ( E_OK != e_get_platform_info(&platform) ) {
		fprintf(stderr, "Failed to get Epiphany platform info\n");
		retval = EXIT_FAILURE;
		goto err_out3;
	}
	fprintf(stderr, "Platform version: %s, HAL version 0x%08x\n",
			platform.version, platform.hal_ver);
	
	// Allocate a few buffers that won't be touched. 
	if ( E_OK != e_shm_alloc(&mbuf, "shm_1", 4096) ) {
		fprintf(stderr, "Failed to allocate shared memory. Error is %s\n",
				strerror(errno));
		retval = EXIT_FAILURE;
		goto err_out3;
	}

	// Allocate a few buffers that won't be touched. 
	if ( E_OK != e_shm_alloc(&mbuf, "shm_2", 4096) ) {
		fprintf(stderr, "Failed to allocate shared memory. Error is %s\n",
				strerror(errno));
		retval = EXIT_FAILURE;
		goto err_out2;
	}

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	if ( E_OK != e_shm_alloc(&mbuf, ShmName, ShmSize) ) {
		fprintf(stderr, "Failed to allocate shared memory. Error is %s\n",
				strerror(errno));
		retval = EXIT_FAILURE;
		goto err_out1;
	}

	// Scribble on memory from host side
	e_write(&mbuf, 0, 0, 0, (const void*)HostMsg, sizeof(HostMsg));

	// Dump the shm table - we should see three valid regions
	{
		int i = 0;
		const e_shmtable_t *tbl = e_shm_get_shmtable();

		printf("Shared Memory Regions:\n");
		printf("------------------------\n");
		if ( tbl ) {
			for ( i = 0; i < MAX_SHM_REGIONS; ++i ) {
				if ( tbl->regions[i].valid ) {
					printf("region %d: name = %s, paddr = %p, length=%d\n",
						   i, tbl->regions[i].shm_seg.name,
						   tbl->regions[i].shm_seg.paddr,
						   tbl->regions[i].shm_seg.size);
				}
			}
		}
		printf("------------------------\n");
	}

	for ( i = 0; i < SeqLen; ++i )
	{
		char buf[ShmSize];

		// Draw a random core
		row = rand() % platform.rows;
		col = rand() % platform.cols;

		coreid = (row + platform.row) * 16 + col + platform.col;
		fprintf(stderr, "%3d: Message from eCore 0x%03x (%2d,%2d): ", i, coreid, row, col);

		// Open the single-core workgroup and reset the core, in
		// case a previous process is running. Note that we used
		// core coordinates relative to the workgroup.
		e_open(&dev, row, col, 1, 1);
		e_reset_group(&dev);

		// Load the device program onto the selected eCore
		// and launch after loading.
		if ( E_OK != e_load("./e_shm_test.srec", &dev, 0, 0, E_TRUE) ) {
			fprintf(stderr, "Failed to load e_shm_test.srec\n");
			retval = EXIT_FAILURE;
			goto err_out;
		}

		// Wait for core program execution to finish, then
		// read message from shared buffer.
		usleep(10000);

		e_read(&mbuf, 0, 0, 0, buf, ShmSize);

 		// Print the message and close the workgroup.
		printf("\"%s\"\n", buf);
		e_close(&dev);
	}

	// Release the allocated buffer and finalize the
	// e-platform connection.
 err_out:
	e_shm_release(ShmName);
 err_out2:
	e_shm_release("shm_2");
 err_out1:
	e_shm_release("shm_1");
 err_out3:
	e_finalize();

	return retval;
}

