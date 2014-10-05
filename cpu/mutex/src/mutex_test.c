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

#include <e-hal.h>


int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned clk_max = 21618997;
	unsigned clk_min = 17688270;
	unsigned num;
	unsigned counter = 0;
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
	e_load("e_mutex_test0.srec", &dev, 0, 0, E_TRUE);

	usleep(10000);
	// Load the device program onto all the other eCores
	e_load_group("e_mutex_test.srec", &dev, 0, 1, 1, 3, E_TRUE);
	e_load_group("e_mutex_test.srec", &dev, 1, 0, 3, 4, E_TRUE);

	usleep(100000);

	// Wait for core program execution to finish
	// Read message from shared buffer

	e_read(&dev, 0, 0, 0x6200, &num, sizeof(num));
	e_read(&dev, 0, 0, 0x6300, &counter, sizeof(counter));

	// Print the message
	fprintf(stderr, "The counter now is %d!\n", counter);
	fprintf(stderr, "The clock cycle is %d!\n", num);

	if((num < clk_max)&&(num > clk_min))
	{
		fprintf(stderr, "PASS!\n");
	}else
	{
		fprintf(stderr, "FAIL!\n");
	}

	// Close the workgroup
	e_close(&dev);

	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}

