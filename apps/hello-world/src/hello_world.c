/*
  hello_world.c

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

// Last update: 2016 dec 19. (JLQ).

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <e-hal.h>

#define buff_sz   (4096)
#define buff_offset (0x01000000)

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	char emsg[buff_sz];
	int rc;

	e_set_loader_verbosity(H_D0);
	e_set_host_verbosity(H_D0);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	e_alloc(&emem, buff_offset, buff_sz);	
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Reset the workgroup
	e_reset_group(&dev);

	// Load the device program onto all the eCores
	e_load_group("e_hello_world.elf", &dev, 0, 0, platform.rows, platform.cols, E_FALSE);


	for (row=0; row<platform.rows; row++)
	{
		for (col=0; col<platform.cols; col++)
		{
		// clear shared buffer.
		memset(emsg, 0, buff_sz);
		e_write(&emem, 0, 0, 0x0000, emsg, buff_sz);

		// Print working core 
		coreid = (row + platform.row) * 64 + col + platform.col;
		printf("eCore 0x%03x (%2d,%2d): \n", coreid, row, col);

		// Start one core
		e_start(&dev, row, col);
		
		// Wait for core program execution to finish.
		usleep(10000);

		// Read message from shared buffer
		e_read(&emem, 0, 0, 0x0, emsg, buff_sz);

		// Print the message and close the workgroup.
		printf("%s\n", emsg);
				
		}
	}

	// Close the workgroup
	e_close(&dev);
	
	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_free(&emem);
	e_finalize();

	return 0;
}
