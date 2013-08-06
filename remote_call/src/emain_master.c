/*
  emain_master.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Xin Mao <maoxin99@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This is the device side of the Hardware Barrier example project.
// The host may load this program to any eCore. When launched, the 
// core remote call all other 15 cores. A success/error message is 
// sent to the host according to the result.
//
// Aug-2013, XM.


#include <stdlib.h>
#include <e-lib.h>


#define E_WAND_INT             (8)
#define irq_num                (7)
#define host                   (0xdeadbeef)
#define mail_address           (0x00005000)
#define resultbase             (0x00005200)
#define delay                  (0x00500000)
#define expect_result          (0x0000452f)
#define signalbase             (0x00005400)
#define finish                 (0x00000001)


int main(void) 
{
	unsigned i, j, k, row, col, num;
	unsigned *result, *mailbox;
	unsigned *signal;
	
	//mask interrupts
	e_irq_global_mask(E_FALSE);

	e_reg_write(E_REG_IMASK,0x0);
	
	row = e_group_config.core_row;
	col = e_group_config.core_col;

	//initialize mailbox
	mailbox = (unsigned *)mail_address;
	e_wait(E_CTIMER_0, 0x00040000);
	for (i=0;i<16;i++)
	{
		mailbox[i] = 0x0;
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if((i==row) && (j==col))
			{
				mailbox[(i*4+j)] = host;
			}
			else
			{	
				result = e_get_global_address(i,j,(void *)resultbase);
				signal = e_get_global_address(i,j,(void *)signalbase);

				//insert interrupt
				e_wait(E_CTIMER_0, delay);
				
				e_irq_set(i,j,E_SW_EXCEPTION);
				e_irq_set(i,j,E_MEM_FAULT);
				e_irq_set(i,j,E_TIMER0_INT);
				e_irq_set(i,j,E_TIMER1_INT);
				e_irq_set(i,j,E_DMA0_INT);
				e_irq_set(i,j,E_DMA1_INT);
				e_irq_set(i,j,E_USER_INT);

				e_wait(E_CTIMER_0, delay);

				//send slave the finish signal
				*signal = finish;
				//check the result
				for(k=0;k<irq_num;k++)
				{
					if(result[k] != expect_result)
						mailbox[(i*4+j)] = mailbox[(i*4+j)] | (0x1<<k);
				}
			}
		}
	}


	while(1);
	
	return EXIT_SUCCESS;
}







