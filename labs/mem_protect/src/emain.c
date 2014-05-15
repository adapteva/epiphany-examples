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
// core sets the mem protect registers, and tries to write to those 
// protected pages. The mpf_isr handler returns an address showing 
// which address location failed. A success/error message is sent 
// to the host according to the result.
//
// Aug-2013, XM.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <e-lib.h>

#define pagesize (0x1000)
#define offset (pagesize - sizeof(int))
#define mailbox ((unsigned *) 0x8f000000)

#define delay (300000)

typedef enum
{
	clear         = 0,
	write_success = 2,
	write_failed  = 3,
	finished      = 9,
}test_state;


void __attribute__((interrupt)) mpf_isr(int signum);


void mem_write(int page) {
	int *address;

	mailbox[1] = pagesize * page + offset;
	e_wait(E_CTIMER_0, delay);

	// write to the address
	address = (int *) mailbox[1];
	*address = 0xdeadbeef;
	e_wait(E_CTIMER_0, delay);
	if (mailbox[0] == clear)
		mailbox[0] = write_success;

	return;
}



int main(void)
{
	int i;

	//initialize
	e_irq_attach(E_MEM_FAULT, mpf_isr);

	//enable the interrupt
	e_irq_global_mask(E_FALSE);
	e_irq_mask(E_MEM_FAULT, E_FALSE);


	//set the mem protect
	int mem_pages = 0xff;
	e_reg_write(E_REG_MEMPROTECT, mem_pages);

	while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	for (i=0; i<8; i++)
	{
		//tell the host that write_failure is expected here
		mailbox[3] = write_failed;
		
		mailbox[2] = i;
		mem_write(i);
		e_wait(E_CTIMER_0, delay); 
		while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	}

	//set the mem protect
	mem_pages = 000;
	e_reg_write(E_REG_MEMPROTECT, mem_pages);
	for (i=0; i<8; i++)
	{
		//tell the host that write_succeed is expected here
		mailbox[3] = write_success;
		
		mailbox[2] = i;
		mem_write(i);
		e_wait(E_CTIMER_0, delay); 
		while (mailbox[0] != clear) { e_wait(E_CTIMER_0, delay); }
	}


	mailbox[2] = i;
	mailbox[0] = finished;
	e_reg_write(E_REG_MEMPROTECT, 0);
	
	return EXIT_SUCCESS;
}

