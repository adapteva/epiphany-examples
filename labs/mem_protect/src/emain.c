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
// protected pages. The mpf_isr() handler sets the isr_flag if a write
// failed. That way mem_write() can determine if the write failed or
// succeeded. The result for each page is reported back to the host.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <e-lib.h>
#include <stdint.h>

#define pagesize (0x1000)
#define offset (pagesize - sizeof(int))

#define length(array) (sizeof(array)/sizeof(*(array)))

struct state {
	uint32_t expect_fail[8];
	uint32_t expect_pass[8];
	uint32_t isr_flag;
	uint32_t finished;
};



volatile struct state *mbox;

#define delay (100000)

typedef enum
{
	clear         = 0,
	write_success = 2,
	write_failed  = 3,
	finished      = 9,
}test_state;


void __attribute__((interrupt)) mpf_isr()
{
	mbox->isr_flag = 1;
}


void mem_write(int page, volatile uint32_t *result) {
	int i;
	volatile uint32_t *address;

	mbox->isr_flag = 0;
	// write to the address
	address = (volatile uint32_t *) (pagesize * page + offset);
	*address = 0xdeadbeef;

	e_wait(E_CTIMER_0, delay);

	if (mbox->isr_flag == 1)
		*result = write_failed;
	else
		*result = write_success;

	e_wait(E_CTIMER_0, delay);

	return;
}



int main(void)
{
	int i;

	mbox = (volatile struct state *) 0x8f000000;

	//initialize
	e_irq_attach(E_MEM_FAULT, mpf_isr);

	e_reg_write(E_REG_MEMPROTECT, 0);

	//enable the interrupt
	e_irq_global_mask(E_FALSE);
	e_irq_mask(E_MEM_FAULT, E_FALSE);


	// set the mem protect
	// mask all pages expect stack segment
	int mem_pages = 0x7f;
	e_reg_write(E_REG_MEMPROTECT, mem_pages);


	for (i=0; i<length(mbox->expect_fail)-1; i++)
	{
		mem_write(i, &mbox->expect_fail[i]);
	}

	//clear the mem protect
	mem_pages = 000;
	e_reg_write(E_REG_MEMPROTECT, mem_pages);

	for (i=0; i<length(mbox->expect_fail)-1; i++)
	{
		mem_write(i, &mbox->expect_pass[i]);
	}

	e_reg_write(E_REG_MEMPROTECT, 0);

	e_wait(E_CTIMER_0, delay);

	mbox->finished = 1;
	
	return EXIT_SUCCESS;
}

