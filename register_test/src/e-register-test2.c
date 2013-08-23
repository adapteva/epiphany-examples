/*
e-register-test2.c

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

// This is the DEVICE side of the register example.
// This is the program for cores to clear the system
// registers and then set system registers and then clear
// system registers. 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

char outbuf[1024] SECTION("shared_dram");
unsigned int clear_value(unsigned int prev,unsigned int mask, unsigned int r_name, unsigned int j);
unsigned int set_value(unsigned int prev,unsigned int mask, unsigned int r_name, unsigned int j);

char reg_name[17][20];
unsigned *global_index;

int main(void)
{
	e_coreid_t coreid;
	unsigned int *po;
	unsigned int mask0;
	unsigned int mask1;
	unsigned int reg;
	unsigned int *auto0;
	unsigned int p_value;
	unsigned int flag0, flag1, flag2;
 	unsigned *pass;
	unsigned int temp, veri, i, k;

	pass = (unsigned *)0x5000;
	global_index = (unsigned *)0x80802000;
	po=(unsigned *) 0x6000;
	po[0] = (unsigned)0x00000000;
	po[1] = (unsigned)0x00000000;
	po[2] = (unsigned)0x00000000;
	veri = 0;
	k = 0;
	
	strcpy(reg_name[0], "IRET");
	strcpy(reg_name[1], "IMASK");
	strcpy(reg_name[2], "IPEND");
	strcpy(reg_name[3], "DMA0CONFIG");
	strcpy(reg_name[4], "DMA0STRIDE");	
	strcpy(reg_name[5], "DMA0COUNT");
	strcpy(reg_name[6], "DMA0SRCADDR");
	strcpy(reg_name[7], "DMA0DSTADDR");
	strcpy(reg_name[8], "DMA0STATUS");
	strcpy(reg_name[9], "DMA1CONFIG");
	strcpy(reg_name[10], "DMA1STRIDE");
	strcpy(reg_name[11], "DMA1COUNT");
	strcpy(reg_name[12], "DMA1SRCADDR");
	strcpy(reg_name[13], "DMA1DSTADDR");
	strcpy(reg_name[14], "DMA1STATUS");	
	strcpy(reg_name[15], "CTIMER0");
	strcpy(reg_name[16], "CTIMER1");
		
	while(1)
	{	
		po[0] = (unsigned)0x00000000;
		po[1] = (unsigned)0x00000000;
		po[2] = (unsigned)0x00000000;
		pass[0] = (unsigned)0x00000000;
		while( (po[1]==(unsigned)0x00000000) | (po[2]==(unsigned)0x00000000) )
		{};
		veri = veri + 1;
		mask0 = po[0];		
		mask1 = po[1];
		reg = po[2];
	
		sprintf(outbuf,"");
		
		// Test for DMA0AUTODMA0 and DMA0AUTODMA1 in a pair 
		if(reg == 0xf0514)
		{
			// Store the previous value of the register	
			p_value = e_reg_read(reg);	
	
			// Clear the register
			auto0 = (unsigned *) e_get_global_address(e_group_config.core_row, e_group_config.core_col, (void *) 0xf0514);
			auto0[0] = (unsigned)0x0000000000000000;
			if((auto0[0] == 0)&&(auto0[1] == 0))
			{
				flag0 = 1;
			}else
			{
				flag0 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA0AUTODMA0.", 
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0x00000000);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA0AUTODMA1\n", auto0[1], 0x00000000);
				global_index[0] = global_index[0] + 1;
			}
			// Set to 1
			auto0[0] = 0xffffffffffffffff;
			
			if((auto0[0] == 0xffffffff)&&(auto0[1] == 0xffffffff)) 
			{
				flag1 = 1;
			}else
			{
				flag1 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA0AUTODMA0.",
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0xffffffff);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA0AUTODMA1\n", auto0[1], 0xffffffff);
				global_index[0] = global_index[0] + 1;
			}
			
			// Clear the register
			auto0[0] = (unsigned)0x0000000000000000;
			
			if((auto0[0] == 0)&&(auto0[1] == 0))
			{
				flag2 = 1;
			}else
			{
				flag2 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA0AUTODMA0.", 
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0x00000000);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA0AUTODMA1\n", auto0[1], 0x00000000);
				global_index[0] = global_index[0] + 1;
			}
			
			// Set the register to the previous value
			e_reg_write(reg, p_value);
				
			if ((flag0 == 1) && (flag1 == 1) && (flag2 == 1))
			{
				pass[0] = 0xdeadbeef;
			}
			
		} 
		else if(reg == 0xf0534)
		{
			// Store the previous value of the register	
			p_value = e_reg_read(reg);
			
			auto0 = (unsigned *) e_get_global_address(e_group_config.core_row, e_group_config.core_col, (void *) 0xf0534);	
	
			// Clear the register
			auto0[0] = 0x0000000000000000;
			
			
			if((auto0[0] == 0)&&(auto0[1] == 0))
			{
				flag0 = 1;
			}else
			{
				flag0 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA1AUTODMA0.", 
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0x00000000);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA1AUTODMA1\n", auto0[1], 0x00000000);
				global_index[0] = global_index[0] + 1;
			}
			// Set to 1
			auto0[0] = 0xffffffffffffffff;
			
			if((auto0[0] == 0xffffffff)&&(auto0[1] == 0xffffffff))
			{
				flag1 = 1;
			}else
			{
				flag1 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA10AUTODMA0.", 
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0xffffffff);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA1AUTODMA1\n", auto0[1], 0xffffffff);
				global_index[0] = global_index[0] + 1;
			}
			
			// Clear the register
			auto0[0] = 0x0000000000000000;
			
			if((auto0[0] == 0)&&(auto0[1] == 0))
			{
				flag2 = 1;
			}else
			{
				flag2 = 0;
				sprintf(outbuf+strlen(outbuf) ,"CORE(%d,%d): We get 0x%08x instead of 0x%08x, DMA1AUTODMA0.", 
				e_group_config.core_row, e_group_config.core_col, auto0[0], 0x00000000);
				sprintf(outbuf+strlen(outbuf) ,"We get 0x%08x instead of 0x%08x, DMA1AUTODMA1\n", auto0[1], 0x00000000);
				global_index[0] = global_index[0] + 1;
			}
			
			// Set the register to the previous value
			e_reg_write(reg, p_value);
				
			if ((flag0 == 1) && (flag1 == 1) && (flag2 == 1))
			{
				pass[0] = 0xdeadbeef;
			}
			
		}
		else
		{
			// Store the previous value of the register	
			p_value = e_reg_read(reg);	
	
			// Test the register	
			// Clear the register by setting to 0
			flag0 = clear_value(p_value, mask0, reg, k);

			// Set the register by setting to 1

			flag1 = set_value(p_value, mask1, reg, k);
 	
			// Clear the register bt setting to 0
	
			flag2 = clear_value(p_value, mask0, reg, k);

			// Set the register to the previous value
			e_reg_write(reg, p_value);

				
			if ((flag0 == 1) && (flag1 == 1) && (flag2 == 1))
			{
				pass[0] = 0xdeadbeef;
			}
			
			k = k+1;
		
		}
	}
	
		return EXIT_SUCCESS;
		
}
	

	//define the function clear_value and set_value	
	unsigned int clear_value(unsigned int prev, unsigned int mask, unsigned int r_name, unsigned int j)
	{
		unsigned int curr;
		unsigned int tem;
		unsigned int flag;
		curr = prev & mask; 
		e_reg_write(r_name, curr);
		tem = e_reg_read(r_name);
		if (tem == curr) //change here for verifying
		{	
			flag = 1;
		}else
		{
			flag = 0;
			sprintf(outbuf+strlen(outbuf) ,"Core(%d,%d): We get 0x%08x instead of 0x%08x, %s\n", 
			e_group_config.core_row, e_group_config.core_col, tem, curr, reg_name[j]);
			global_index[0] = global_index[0] + 1;
		}
		return flag;
	}
	
	unsigned int set_value(unsigned int prev, unsigned int mask, unsigned int r_name, unsigned int j)
	{
		unsigned int curr;
		unsigned int tem;
		unsigned int flag;
		curr = prev | mask; 
		e_reg_write(r_name, curr);
		tem = e_reg_read(r_name);
		if (tem == curr)
		{	
			flag = 1;
		}else
		{
			flag = 0;
			sprintf(outbuf+strlen(outbuf) ,"Core(%d,%d): We get 0x%08x instead of 0x%08x, %s\n", 
			e_group_config.core_row, e_group_config.core_col, tem, curr, reg_name[j]);
			global_index[0] = global_index[0] + 1;
		}
		return flag;
	}	
	
	
