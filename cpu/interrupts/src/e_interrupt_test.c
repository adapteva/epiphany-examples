/*
e_interrupt_test.c
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

// This is the DEVICE side. By setting the specific bit of ILAT register
// to 1 will make the core go into the interrupt. By attaching interrupt
// handler, make the core to print out "I am interrupt!"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

char outbuf[4096] SECTION("shared_dram");
void timer_isr(void);

int main(void)
{
	e_coreid_t coreid;
	unsigned name_irq[8] = {E_SW_EXCEPTION, E_MEM_FAULT, E_TIMER0_INT, E_TIMER1_INT, E_MESSAGE_INT, E_DMA0_INT, E_DMA1_INT, E_USER_INT};
	unsigned *po;
	char event [8][20];
	strcpy (event[0], "E_SW_EXCEPTION");
	strcpy (event[1], "E_MEM_FAULT");
	strcpy (event[2], "E_TIMER0_INT");
	strcpy (event[3], "E_TIMER1_INT");
	strcpy (event[4], "E_MESSAGE_INT");
	strcpy (event[5], "E_DMA0_INT");
	strcpy (event[6], "E_DMA1_INT");
	strcpy (event[7], "E_USER_INT");
	po = 0x00006000;

	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();

	// Test E_SW_EXCEPTION interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[0], timer_isr);
/*
		// Set the ivt entry address
		ivt  = 0x4;

		// Set the relative branch offset.
		*ivt = (((unsigned) timer_isr - (unsigned) ivt) >> 1) << 8;

		// Add the instruction opcode.
		*ivt = *ivt | B_OPCODE;
*/
		// print out the name of the interrupt
		sprintf(outbuf, "Interrupt %s: \n", event[0]);

		// Clear the IMASK
		e_irq_mask(name_irq[0], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		//e_reg_write(E_REG_ILATST, (unsigned)0x00000002);
		e_irq_set(po[0] ,po[1], name_irq[0]);

		// Clear the ILAT and the Set the IMASK
		//e_reg_write(E_REG_ILATCL, (unsigned)0x00000002);
		e_irq_clear(po[0], po[1], name_irq[0]);
		e_irq_mask(name_irq[0], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_MEM_FAULT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[1], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[1]);

		// Clear the IMASK
		e_irq_mask(name_irq[1], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000004);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000004);
		e_irq_mask(name_irq[1], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_TIMER0_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[2], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[2]);

		// Clear the IMASK
		e_irq_mask(name_irq[2], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000008);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000008);
		e_irq_mask(name_irq[2], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_TIMER1_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[3], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[3]);

		// Clear the IMASK
		e_irq_mask(name_irq[3], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000010);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000010);
		e_irq_mask(name_irq[3], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_MESSAGE_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[4], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[4]);

		// Clear the IMASK
		e_irq_mask(name_irq[4], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000020);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000020);
		e_irq_mask(name_irq[4], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_DMA0_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[5], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[5]);

		// Clear the IMASK
		e_irq_mask(name_irq[5], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000040);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000040);
		e_irq_mask(name_irq[5], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_DMA1_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[6], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[6]);

		// Clear the IMASK
		e_irq_mask(name_irq[6], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000080);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000080);
		e_irq_mask(name_irq[6], E_TRUE);
		e_irq_global_mask(E_TRUE);

	// Test E_USER_INT interrupt
		// Attach the ISR with this interrupt
		e_irq_attach(name_irq[7], timer_isr);

		// print out the name of the interrupt
		sprintf(outbuf+strlen(outbuf), "Interrupt %s: \n", event[7]);

		// Clear the IMASK
		e_irq_mask(name_irq[7], E_FALSE);

		// Enable the global interrupt
		e_irq_global_mask(E_FALSE);

		// Set the ILAT
		e_reg_write(E_REG_ILATST, (unsigned)0x00000200);

		// Clear the ILAT and the Set the IMASK
		e_reg_write(E_REG_ILATCL, (unsigned)0x00000200);
		e_irq_mask(name_irq[7], E_TRUE);
		e_irq_global_mask(E_TRUE);

	return 0;
}

void __attribute__((interrupt)) timer_isr(void)
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt !\n");

	return;
}
