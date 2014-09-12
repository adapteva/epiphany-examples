/*
e-register-test3.c

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
unsigned int clear_value(unsigned int prev,unsigned int mask, unsigned int *p);
unsigned int set_value(unsigned int prev,unsigned int mask, unsigned int *p);

unsigned *global_index;

int main(void)
{
	e_coreid_t coreid;
	unsigned int *po;
	unsigned int reg;
	unsigned int p_value;
	unsigned int flag3, flag4, flag5;
	unsigned int temp;
	unsigned int k;
	unsigned *pass;
	pass = (unsigned *)7000;
	po= (unsigned *) 0x600c;
	po[0] = 0x00000000;
	global_index = (unsigned *)0x80802000;

	while(1)
	{
		pass[0] = (unsigned)0x00000000;
		while(po[0] == (unsigned) 0x00000000)
		{};

		sprintf(outbuf,"");
		// Store the previous value of the register
		p_value = e_reg_read(*po);
		// Disable interrupts

		e_irq_global_mask(E_TRUE);

		//For register EILAT
			// Clear the EILAT
			e_reg_write(E_REG_ILATCL, (unsigned)0xffffffff);
			temp = e_reg_read(*po);
			if (temp == (unsigned)0x00000000)
			{
				flag3=1;
			}
			else
			{
				flag3=0;
				sprintf(outbuf,"We get 0x%08x instead of 0x%08x, ILAT\n", temp, 0x00000000);
				global_index[0] = global_index[0] + 1;
			}

			// Set the EILAT
			e_reg_write(E_REG_ILATST, (unsigned)0xffffffff);
			temp = 	e_reg_read(*po);
			if (temp == (unsigned)0x000003ff)
			{
				flag4=1;
			}else
			{
				flag4=0;
				sprintf(outbuf + strlen(outbuf),"We get 0x%08x instead of 0x%08x, ILAT\n", temp, 0x000003ff);
				global_index[0] = global_index[0] + 1;
			}

			// Clear the EILAT

			e_reg_write(E_REG_ILATCL, (unsigned)0xffffffff);
			temp = e_reg_read(*po);
			if (temp == (unsigned)0x00000000)
			{
				flag5=1;
			}else{
				flag5=0;
				sprintf(outbuf + strlen(outbuf),"We get 0x%08x instead of 0x%08x, ILAT\n", temp, 0x00000000);
				global_index[0] = global_index[0] + 1;
			}

			if ((flag3 == 1) && (flag4 == 1) && (flag5 == 1))
			{
				pass[0] = 0xdeadbeef;
			}

			// Set the register to the previous value
			e_reg_write(*po, p_value);
			po[0] = (unsigned) 0x00000000;

			// Enable the interrupts
			e_irq_global_mask(E_FALSE);
	}

			return EXIT_SUCCESS;
}

