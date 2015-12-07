/*
mutex_test.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program, see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.
*/

// This is the HOST side of the mutex example.
// The program initializes the Epiphany system,put
// mutex key in core (0,0) and make other cores try to
// get the key to add 1 to the counter.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <e-hal.h>


int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	/* Assume 600 Mhz clock frequency. */
	unsigned clk_max = 5600; /* 10 stddev */
	unsigned clk_min = 1500;
	unsigned num;
	unsigned counter = 0;
	const uint32_t one = 1;
	const uint32_t zero = 0;
	int err = 0;
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.

	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);

	// Load the device program onto core (0,0)
	e_load("e_mutex_test0.elf", &dev, 0, 0, E_FALSE);

	// Load the device program onto all the other eCores
	e_load_group("e_mutex_test.elf", &dev, 0, 1, 1, 3, E_FALSE);
	e_load_group("e_mutex_test.elf", &dev, 1, 0, 3, 4, E_FALSE);

	usleep(1000);

	/* Clear the go flag */
	e_write(&dev, 0, 0, 0x6400, &zero, sizeof(zero));

	usleep(1000);

	/* Start all cores */
	e_start_group(&dev);

	/* Give core0 plenty of time to initialize mutex */
	usleep(10000);

	/* Go! */
	e_write(&dev, 0, 0, 0x6400, &one, sizeof(one));

	// Wait for core program execution to finish
	usleep(10000);

	/* Read results from core0 */
	e_read(&dev, 0, 0, 0x6200, &num, sizeof(num));
	e_read(&dev, 0, 0, 0x6300, &counter, sizeof(counter));

	/* Clear go flag */
	e_write(&dev, 0, 0, 0x6400, &zero, sizeof(zero));

	// Print the message
	fprintf(stderr, "The counter now is %d\n", counter);
	fprintf(stderr, "The clock cycle is %d\n", num);

	if((num < clk_max)&&(num > clk_min))
	{
		fprintf(stderr, "Clock: PASS\n");
	} else {
		fprintf(stderr, "Clock: FAIL\n");
		err = 1;
	}

	if (counter == 16) {
		fprintf(stderr, "Counter: PASS\n");
	} else {
		fprintf(stderr, "Counter: FAIL\n");
		err = 1;
	}


	// Close the workgroup
	e_close(&dev);

	// Finalize the e-platform connection.
	e_finalize();

	return err;
}

