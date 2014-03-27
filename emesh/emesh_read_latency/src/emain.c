/*
  emain.c

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
// core reads from all the other fifteen cores and records the number 
// of cycles spend for the read. A success/error message is sent to 
// the host according to the result.
//
// Aug-2013, XM.


#include <stdio.h>
#include <stdlib.h>
#include <e-lib.h>


#define CTIMER               (0x000f0438)
#define targetbase           (0x00005100)
#define mailbox  ((unsigned *)0x00005200)
#define localbox ((unsigned *)0x00005300)




int main(void) {
	//initialize
	unsigned time_s, time_e, time;
	unsigned expect;
	unsigned *target;
	unsigned ctimer;
	int i, j, k;
	unsigned value;	
	
	
	unsigned row, col;

	row = e_group_config.core_row;
	col = e_group_config.core_col;
	ctimer = (unsigned)e_get_global_address(row, col, (void *)CTIMER);

	value = 0xdead;

	for (i=0;i<4;i++)
	{	
		for (j=0;j<4;j++)
		{	

			target = (unsigned *)e_get_global_address(i, j, (void *)targetbase);
			
			//initialize the target
			for (k=0;k<10;k++)
			{
				target[0] = 0xdeadbeef;
				localbox[0] = 0x0;
			}
	
	
			//record the offset
			e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
			e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_s):"r"(ctimer):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_e):"r"(ctimer):);
			e_ctimer_stop(E_CTIMER_0);
			time = time_s - time_e;


			//record the ctimer	
			e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
			e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_s):"r"(ctimer):);

			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);
			__asm__ __volatile__("ldr %0, [%1]":"=r"(localbox[0]):"r"(&target[0]):);

			__asm__ __volatile__("ldr %0, [%1]":"=r"(time_e):"r"(ctimer):);
			e_ctimer_stop(E_CTIMER_0);
			
			time = time_s - time_e - time;
			expect = ((abs(row - i) + abs(col - j)) * 3 + 18)*10;
			
			//update the mailbox
			mailbox[(i*4+j)] = time;
			mailbox[(i*4+j+16)] = expect;
			
		}
	}
	
	return EXIT_SUCCESS;
}


