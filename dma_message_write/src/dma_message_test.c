/*
dma_message_test.c

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

// This is the HOST side of the DMA message example.
// The program initializes the Epiphany system,
// selects an eCore to be transmitter and another to be
// the receiver and then loads and launches the device
// program on that eCore. In message mode, after tranfer, 
// it will make receiver go into an interrupt. This test
// generate the situation and output some message in the
// interrupt.



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>
#define mas_row (1)
#define mas_col (2)

int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i, j, m, n, k;
	unsigned mail0, mail1;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	unsigned flag1 = 0x00000000;
	unsigned flag2 = 0x00000000;
	unsigned test = 0x00000000;
	mail0 = mas_row;
	mail1 = mas_col;
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Load the device program onto all slave cores  
	for(i=0; i<platform.rows; i++)
	{
		for(j=0; j<platform.cols; j++)
		{
			if((i!=mas_row)|(j!=mas_col))
			{
				e_load("e_dma_message_slave_test.srec", &dev, i, j, E_TRUE);
			}
		}
	} 
	usleep(100000);	
	
	// Send the coordinate of the transmitter to the receiver
	e_write(&dev, mas_row, mas_col, 0x6500, &mail0, sizeof(mail0));
	e_write(&dev, mas_row, mas_col, 0x6600, &mail1, sizeof(mail1));
	e_load("e_dma_message_test.srec", &dev, mas_row, mas_col, E_TRUE);
	
	// Wait for core program execution to finish
	usleep(1000000);
		
	// Results from every slave core
	coreid = (mas_row + platform.row) * 64 + mas_col + platform.col;
	fprintf(stderr,"Now the master core is 0x%03x (%2d,%2d)!\n",coreid, mas_row, mas_col);
	
	for (i=0; i<platform.rows; i++)
	{	
		for(j=0; j<platform.cols; j++)
		{
			if((i!=mas_row)|(j!=mas_col))
			{
				row=i;
				col=j;
				coreid = (row + platform.row) * 64 + col + platform.col;
				fprintf(stderr,"%d: Message from eCore 0x%03x (%2d,%2d) : \n",(row*platform.cols+col),coreid,row,col);
			
				// Read message from slave
				e_read(&dev, row, col, 0x6000, &flag1, sizeof(flag1));
				e_read(&dev, row, col, 0x6100, &flag2, sizeof(flag2));

				// Print the message and close the workgroup.
				if(flag1 == (unsigned) 0xdeadbeef)
				{
					fprintf(stderr, "PASS!\n");
				}else
				{
					fprintf(stderr, "FAIL!\n");
				}
			}
		}
	}

	
	// Close the workgroup
	e_close(&dev);
	
	// Finalize the e-platform connection.
	e_finalize();

	return 0;
}
