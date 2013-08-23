/*
main.c
Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>
               Xin Mao <maoxin99@gmail.com> 
               

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

// This is the HOST side of the epiphany resource manager example.
// The program asks for a group of cores to run the device program.
// In the e_reserve function, the inputs are the group size that 
// the host is asking for and the searching mode. It gives back 
// the coordinate of the first core of the available group. After
// the program ends, the e_release function will release the resources
// so other host program can use later.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/file.h> 
#include <time.h>
#include <e-hal.h>
#include "ermlib.h"


e_epiphany_t dev;

int main(int argc, char *argv[])
{
	e_platform_t platform;
	unsigned row, col;
	unsigned success;
	unsigned board_rows, board_cols;
	unsigned ROWS, COLS;
	unsigned MODE;
	unsigned limit;
	unsigned k;
	unsigned i, j;
	unsigned a[3000], b[3000], c[3000];
	
	e_init(NULL);
	e_get_platform_info(&platform);
	board_rows = platform.rows;
	board_cols = platform.cols;	
	e_finalize();

	MODE = atoi(argv[1]);
	limit = atoi(argv[2]);
	
	for(k=0;k<limit;k++)
	{
		a[k] = k;
		b[k] = k;
		c[k] = 0;
	}
	
	while(1)
	{
		srand(time(NULL));
		ROWS = rand()%2+1;
		COLS = rand()%2+1;
		
		e_init(NULL);
		
		// Resource manager
		success = e_reserve(MODE, ROWS, COLS, &row, &col);
	 
		fprintf(stderr,"Finished e_reserve.\n");
	
		if(success == E_OK)
		{
			// Open workgroup
			fprintf(stderr, "\tStart open group!\n");
			e_open(&dev, row, col, ROWS, COLS);
	
		
			// Load the program
			fprintf(stderr, "\tStart load program!\n");
			e_load_group("e_demo.srec", &dev, 0, 0, ROWS, COLS, E_FALSE);
			
			for(i=0; i<ROWS; i++)
			{
				for(j=0; j<COLS; j++)
				{
					e_write(&dev, i, j, 0x1e00, &limit, sizeof(limit));

					e_write(&dev, i, j, 0x2000, a, sizeof(unsigned)*limit);
					e_write(&dev, i, j, 0x4000, b, sizeof(unsigned)*limit);
				}
			}
			
			e_start_group(&dev);

			usleep(300000);
			//fprintf(stderr, "start copy back\n");
			//e_read(&dev, 0, 0, 0x6000, &c[0], sizeof(unsigned)*limit);
			
			// Release the resource
			fprintf(stderr, "\tStart release group!\n");
			e_release(ROWS, COLS, row, col);
	
			fprintf(stderr, "Release the group successfully!\n");
		}else
			fprintf(stderr, "Failed to reserve work group!\n");
		
	
		e_close(&dev);
		e_finalize();
	}
	return 0;
}

