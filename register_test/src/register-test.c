/*
register-test.c

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

// This is the HOST side of the register example.
// The program initializes the Epiphany system and load program
// onto the core. The host sends signal to cores to let them start
// and read the mailbox after execution.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define _BufSize   (1024)
#define _BufOffset (0x01000000)


int main(int argc, char *argv[])
{
	unsigned row, col, coreid, i,j,m,n,k,l,x;
	unsigned int mask0, mask1;
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	char emsg[_BufSize];
	unsigned int reg_list[] = {E_REG_IRET, E_REG_IMASK, E_REG_IPEND, E_REG_DMA0CONFIG,
								E_REG_DMA0STRIDE, E_REG_DMA0COUNT,  E_REG_DMA0SRCADDR, E_REG_DMA0DSTADDR, E_REG_DMA0AUTODMA0,    						       			          	E_REG_DMA0STATUS,E_REG_DMA1CONFIG, E_REG_DMA1STRIDE,  E_REG_DMA1COUNT, E_REG_DMA1SRCADDR,   
							  E_REG_DMA1DSTADDR, E_REG_DMA1AUTODMA0, E_REG_DMA1STATUS, E_REG_CTIMER0, E_REG_CTIMER1};
	unsigned int reg_name[3];
	unsigned int *pa;
	unsigned int reg_lat;
	unsigned int list_num;
	list_num = (sizeof(reg_list))/(sizeof(reg_list[0]));
	unsigned int flag;
	unsigned int index;
	unsigned clr;
	unsigned sum = 0x00000000;
	reg_lat = E_REG_ILAT;
	pa = &reg_name[0];
	index = 1;
	clr = (unsigned)0x00000000;
	
	srand(1);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	e_alloc(&emem, _BufOffset, _BufSize);	
	
    	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Load the device program onto all the eCores
	e_load_group("e-register-test2.srec", &dev, 0, 0, platform.rows, platform.cols, E_TRUE);
	
	// Let all cores initialize
	usleep(10000);
	
	// Select a register
	for(k=0; k<list_num; k++)
	{

		*(pa+2) = reg_list[k];
		
		switch (*(pa+2))
		{
		case E_REG_STATUS     : mask0 = (unsigned)0x0000080f; mask1 = (unsigned)0xfffff7f0; break;
		case E_REG_DMA0CONFIG : mask0 = (unsigned)0x0000ff89; mask1 = (unsigned)0xffff0076; break;
		case E_REG_DMA1CONFIG : mask0 = (unsigned)0x0000ff89; mask1 = (unsigned)0xffff0076; break;
		case E_REG_DMA0STATUS : mask0 = (unsigned)0x0000ffff; mask1 = (unsigned)0xffff0000; break;
		case E_REG_DMA1STATUS : mask0 = (unsigned)0x0000ffff; mask1 = (unsigned)0xffff0000; break;
		case E_REG_IMASK      : mask0 = (unsigned)0xfffffc00; mask1 = (unsigned)0x000003ff; break;
		case E_REG_IPEND	    : mask0 = (unsigned)0xfffffc00; mask1 = (unsigned)0x000003ff; break;	   
		default               : mask0 = (unsigned)0x00000000; mask1 = (unsigned)0xffffffff; break;
		}
		
		*pa = mask0;
		*(pa+1) = mask1;
		
		// Launch a core

		for (i=0; i<platform.rows; i++)
		{
			for (j=0; j<platform.cols; j++)
			{
			
				// Initialize the global index which will show if there are errors
				e_write(&dev, 0, 0, 0x2000, &clr, sizeof(clr));
		
				flag = 0;
				
				//Draw to a certain core
				row=i;
				col=j;
				coreid = (row + platform.row) * 64 + col + platform.col;
				
				// Write 2 masks and register name to the local address	
				e_write(&dev, i, j, 0x6000, pa, sizeof(reg_name));
				
				usleep(10000);
			
				// Wait for core program execution to finish
				// Read message from shared buffer
			
			
				e_read(&emem, 0, 0, 0x0, emsg, _BufSize);
				
				e_read(&dev, 0, 0, 0x2000, &flag,sizeof(flag));
				
				// Print the message and close the workgroup.
				if(flag != 0x00000000)
				{
					fprintf(stderr, "%s\n", emsg);
				}
				sum +=flag;
			}
		}
	}

	// Close the workgroup
	e_close(&dev);
	
	
	// Test E_ILAT register	
	// Open a workgroup
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	
	// Reset the workgroup
	for (m=0; m<platform.rows; m++)
	{	for(n=0; n<platform.cols;n++)
		{	
			e_reset_core(&dev, m, n);
		}
	}
	
	// Load the device program onto all the eCores
	e_load_group("e-register-test3.srec", &dev, 0, 0,  platform.rows,  platform.cols, E_TRUE);
	
	
	for (i=0; i< platform.rows; i++)
	{
		for (j=0; j< platform.cols; j++)
		{
		
			// Initialize the global index which will show if there are errors
			e_write(&dev, 0, 0, 0x2000, &clr, sizeof(clr));
		
			flag = 0;
	
			//Draw to a certain core
			row=i;
			col=j;
			coreid = (row + platform.row) * 64 + col + platform.col;
			
			e_write(&dev, i, j, 0x600c, &reg_lat, sizeof(reg_lat));
			usleep(10000);
			
			// Wait for core program execution to finish
			// Read message from shared buffer
			
			
			e_read(&emem, 0, 0, 0x0, emsg, _BufSize);
							
			e_read(&dev, 0, 0, 0x2000, &flag,sizeof(flag));

			// Print the message and close the workgroup.
			
			if(flag != 0x00000000)
			{
				fprintf(stderr, "%s\n", emsg);
			}
			sum +=flag;		
		}
	}

	
	if(sum == 0x00000000)
	{
		fprintf(stderr,"PASS!\n");
	}else
	{
		fprintf(stderr,"Fail! There are %d mistakes!\n", sum);
	}
				
	// Close the workgroup
	e_close(&dev);
	
	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_free(&emem);
	e_finalize();

	return 0;
}

 
