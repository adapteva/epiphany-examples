/*
mesh_traffic.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Andreas Olofsson <andreas@adapteva.com>

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

// This is the HOST side of the mesh traffic example.
// In this test, we program the config register to count
// different mesh events.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, k, m, q, mode, signal;
	e_platform_t platform;
	e_epiphany_t dev;
	unsigned desired, real;
	unsigned time[16][2];
	time[0][0]=0;
	time[0][1]=0;

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);

	// Load device program
	e_load("e_loadstore.srec",&dev, 0, 0, E_TRUE);

	//Waiting for finish
	usleep(500000);

	// Read back timer values
	e_read(&dev, 0, 0, (0x6000), &time[0][0], sizeof(real));
	e_read(&dev, 0, 0, (0x6004), &time[0][1], sizeof(real));

	//Print Results
	float ratio= 100*((float)time[0][1]/(float)time[0][0]-1.0f);
	printf("Local  STR loop cycles=%d\n", time[0][0]);
	printf("Remote STR loop cycles=%d\n", time[0][1]);
	printf("Performance hit       =%f%%\n", ratio);

	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}

