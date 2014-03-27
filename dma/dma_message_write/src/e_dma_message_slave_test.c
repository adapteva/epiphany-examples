/*
e_dma_message_test.c

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

// This is the DEVICE side of the DMA message example.
// This is the receiver which will go into an interrupt 
// after dma transfer.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

void int_isr(int signum);
unsigned *mailbox, *mailbox1;

int main(void)
{
	mailbox = (unsigned *)0x00006000;
	mailbox1 = (unsigned *)0x00006100;

	// Initialize the mailbox
	mailbox[0]  = 0x00000000;
	mailbox1[0] = 0x00000000;

	// Preparing for the interrupt
	// Attach the ISR with this interrupt
	e_irq_attach(E_MESSAGE_INT, int_isr);

	// Clear the IMASK
	e_irq_mask(E_MESSAGE_INT, E_FALSE);
	
	// Enable the global interrupt
	e_irq_global_mask(E_FALSE);
	
	// Making slave_core into idle and wait for the finishing transfer interrupt
	__asm__ __volatile__("idle");

	// Showing coming back from the interrupt
	mailbox1[0] = 0xffffffff;

	// Set the IMASK
	e_irq_mask(E_MESSAGE_INT, E_TRUE);
	
	// Disable the global interrupt
	e_irq_global_mask(E_TRUE);
	
	return 0;
}

void __attribute__((interrupt)) int_isr(int signum)
{
	mailbox[0] = 0xdeadbeef;
	return;
}	

	
	 
