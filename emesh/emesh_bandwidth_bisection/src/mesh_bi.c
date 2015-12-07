/*
mesh_bi.c

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

// This is the HOST side of the e_mesh bandwidth example
// In this test, the left half section on the chip transfer
// data to the right half section on the chip simultaneously.
// Use CTIMER_0 register to measure the time on core (0,0).


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
	unsigned time;
	unsigned signal = 0xdeadbeef;
	unsigned clr = 0x00000000;
	float result1;
	srand(1);
	

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Load the device program onto core (0,0)
	e_load("e_mesh_bi00.elf", &dev, 0, 0, E_TRUE);
	usleep(1000);
	e_load_group("e_mesh_bi.elf", &dev, 1, 0, (platform.rows-1), 1, E_TRUE);
	e_load_group("e_mesh_bi1.elf", &dev, 0, 1, platform.rows, 1, E_TRUE);
	
	usleep(10000);
	
	// Sent the signal to start transfer
	e_write(&dev, 0, 0, 0x6100, &signal, sizeof(signal));	
	
	usleep(1000000);
			
	// Read message from shared buffer

	e_read(&dev, 0, 0, 0x5000, &time, sizeof(time));
		
	// Calculate the bandwidth
	result1 = (37500000)/time;
	
	// Print the message and close the workgroup
	fprintf(stderr, "0x%08x!\n", time);
	
	fprintf(stderr, "The bandwidth of bisection is %.2fMB/s!\n", result1);		
	
	// Close the workgroup
	e_close(&dev);
	
	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}
