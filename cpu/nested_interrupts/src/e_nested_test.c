/*
e_nested_test.c
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
// to 1 will make the core go into the interrupt. Also attaching interrupt 
// handler, in the interrupt handler, make this interrupt be interrupted by
// both interrupt with higher priority and lower priority.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

char outbuf[8192] SECTION("shared_dram");
void inter_higher0_isr();
void inter_lower0_isr();
void inter_higher1_isr();
void inter_lower1_isr();
void inter_higher2_isr();
void inter_lower2_isr();
void inter_higher3_isr();
void inter_lower3_isr();
void inter_higher4_isr();
void inter_lower4_isr();
void inter_higher5_isr();
void inter_lower5_isr();
void inter_higher6_isr();
void inter_lower6_isr();
void inter_higher7_isr();
void inter_lower7_isr();
void inter_higher8_isr();
void inter_lower8_isr();
void inter_higher9_isr();
void inter_lower9_isr();
void inter_higher10_isr();
void inter_lower10_isr();
void inter_higher11_isr();
void inter_lower11_isr();
void inter_higher12_isr();
void inter_lower12_isr();
void inter_higher13_isr();
void inter_lower13_isr();

int main(void)
{
	unsigned *po;
	unsigned name_irq[8] = {E_SW_EXCEPTION, E_MEM_FAULT, E_TIMER0_INT, E_TIMER1_INT, E_MESSAGE_INT, E_DMA0_INT, E_DMA1_INT, E_USER_INT};
	unsigned st[8] = {0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000200};
	char event [8][20];
	unsigned k;
	strcpy (event[0], "E_SW_EXCEPTION");
	strcpy (event[1], "E_MEM_FAULT");
	strcpy (event[2], "E_TIMER0_INT");
	strcpy (event[3], "E_TIMER1_INT");
	strcpy (event[4], "E_MESSAGE_INT");
	strcpy (event[5], "E_DMA0_INT");
	strcpy (event[6], "E_DMA1_INT");
	strcpy (event[7], "E_USER_INT");
	
		// Clear the outbuffer
		sprintf(outbuf,"");
		
		// Test the E_SW_EXCEPTION interrupt

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[0], inter_higher0_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[0], event[1]);	

			// Clear the IMASK
			e_irq_mask(name_irq[0], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[1], inter_lower0_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[1], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[0]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");			
	
			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[0]);
			e_reg_write(E_REG_ILATCL, st[1]);
			e_irq_mask(name_irq[0], E_TRUE);
			e_irq_mask(name_irq[1], E_TRUE);
			e_irq_global_mask(E_TRUE);	


		// Test the E_USER_INT interrupt
		
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[7], inter_lower1_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[7], event[6]);	

			// Clear the IMASK
			e_irq_mask(name_irq[7], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[6], inter_higher1_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[6], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[7]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[7]);
			e_reg_write(E_REG_ILATCL, st[6]);
			e_irq_mask(name_irq[7], E_TRUE);
			e_irq_mask(name_irq[6], E_TRUE);
			e_irq_global_mask(E_TRUE);	

		// Test the E_MEM_FAULT
			
			// Interrupted by E_SW_EXPECTION			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[1], inter_lower2_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[1], event[0]);	

			// Clear the IMASK
			e_irq_mask(name_irq[1], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[0], inter_higher2_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[0], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[1]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[1]);
			e_reg_write(E_REG_ILATCL, st[0]);
			e_irq_mask(name_irq[1], E_TRUE);
			e_irq_mask(name_irq[0], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_TIMER0_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[1], inter_higher3_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[1], event[2]);	

			// Clear the IMASK
			e_irq_mask(name_irq[1], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[2], inter_lower3_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[2], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[1]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[1]);
			e_reg_write(E_REG_ILATCL, st[2]);
			e_irq_mask(name_irq[1], E_TRUE);
			e_irq_mask(name_irq[2], E_TRUE);
			e_irq_global_mask(E_TRUE);
	
		// Test the E_TIMER0_INT
			
			// Interrupted by E_MEM_FAULT			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[2], inter_lower4_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[2], event[1]);	

			// Clear the IMASK
			e_irq_mask(name_irq[2], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[1], inter_higher4_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[1], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[2]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[2]);
			e_reg_write(E_REG_ILATCL, st[1]);
			e_irq_mask(name_irq[2], E_TRUE);
			e_irq_mask(name_irq[1], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_TIMER1_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[2], inter_higher5_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[2], event[3]);	

			// Clear the IMASK
			e_irq_mask(name_irq[2], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[3], inter_lower5_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[3], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[2]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[2]);
			e_reg_write(E_REG_ILATCL, st[3]);
			e_irq_mask(name_irq[2], E_TRUE);
			e_irq_mask(name_irq[3], E_TRUE);
			e_irq_global_mask(E_TRUE);

		// Test the E_TIMER1_INT
			
			// Interrupted by E_TIMER0_INT			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[3], inter_lower6_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[3], event[2]);	

			// Clear the IMASK
			e_irq_mask(name_irq[3], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[2], inter_higher6_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[2], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[3]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[3]);
			e_reg_write(E_REG_ILATCL, st[2]);
			e_irq_mask(name_irq[3], E_TRUE);
			e_irq_mask(name_irq[2], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_MESSAGE_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[3], inter_higher7_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[3], event[4]);	

			// Clear the IMASK
			e_irq_mask(name_irq[3], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[4], inter_lower7_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[4], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[3]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[3]);
			e_reg_write(E_REG_ILATCL, st[4]);
			e_irq_mask(name_irq[3], E_TRUE);
			e_irq_mask(name_irq[4], E_TRUE);
			e_irq_global_mask(E_TRUE);

		// Test the E_MESSAGE_INT
			
			// Interrupted by E_TIMER1_INT			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[4], inter_lower8_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[4], event[3]);	

			// Clear the IMASK
			e_irq_mask(name_irq[4], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[3], inter_higher8_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[3], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[4]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[4]);
			e_reg_write(E_REG_ILATCL, st[3]);
			e_irq_mask(name_irq[4], E_TRUE);
			e_irq_mask(name_irq[3], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_DMA0_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[4], inter_higher9_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[4], event[5]);	

			// Clear the IMASK
			e_irq_mask(name_irq[4], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[5], inter_lower9_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[5], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[4]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[4]);
			e_reg_write(E_REG_ILATCL, st[5]);
			e_irq_mask(name_irq[4], E_TRUE);
			e_irq_mask(name_irq[5], E_TRUE);
			e_irq_global_mask(E_TRUE);

		// Test the E_DMA0_INT
			
			// Interrupted by E_MESSAGE_INT			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[5], inter_lower10_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[5], event[4]);	

			// Clear the IMASK
			e_irq_mask(name_irq[5], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[4], inter_higher10_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[4], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[5]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[5]);
			e_reg_write(E_REG_ILATCL, st[4]);
			e_irq_mask(name_irq[5], E_TRUE);
			e_irq_mask(name_irq[4], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_DMA0_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[5], inter_higher11_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[5], event[6]);	

			// Clear the IMASK
			e_irq_mask(name_irq[5], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[6], inter_lower11_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[6], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[5]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[5]);
			e_reg_write(E_REG_ILATCL, st[6]);
			e_irq_mask(name_irq[5], E_TRUE);
			e_irq_mask(name_irq[6], E_TRUE);
			e_irq_global_mask(E_TRUE);

		// Test the E_DMA1_INT
			
			// Interrupted by E_DMA0_INT			

			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[6], inter_lower12_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[6], event[5]);	

			// Clear the IMASK
			e_irq_mask(name_irq[6], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with higher priority
			e_irq_attach(name_irq[5], inter_higher12_isr);

			// Clear the IMASK of a higher priority interrupt
			e_irq_mask(name_irq[5], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[6]);
			
			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[6]);
			e_reg_write(E_REG_ILATCL, st[5]);
			e_irq_mask(name_irq[6], E_TRUE);
			e_irq_mask(name_irq[5], E_TRUE);
			e_irq_global_mask(E_TRUE);

			// Interrupted by the E_USER_INT
			
			// Attach the ISR with this interrupt
			e_irq_attach(name_irq[6], inter_higher13_isr);

			// print out the name of the interrupt
			sprintf(outbuf + strlen(outbuf), "%s is interrupted by %s: \n", event[6], event[7]);	

			// Clear the IMASK
			e_irq_mask(name_irq[6], E_FALSE);
	
			// Enable the global interrupt
			e_irq_global_mask(E_FALSE);

			// Attach the ISR with interrupt with lower priority
			e_irq_attach(name_irq[7], inter_lower13_isr);

			// Clear the IMASK of a lower priority interrupt
			e_irq_mask(name_irq[7], E_FALSE);		

			// Set the ILAT
			e_reg_write(E_REG_ILATST, st[6]);

			// See if it returns to main after interrupt
			sprintf(outbuf + strlen(outbuf), "I return to the main!\n\n");

			// Clear the ILAT and the Set the IMASK
			e_reg_write(E_REG_ILATCL, st[6]);
			e_reg_write(E_REG_ILATCL, st[7]);
			e_irq_mask(name_irq[6], E_TRUE);
			e_irq_mask(name_irq[7], E_TRUE);
			e_irq_global_mask(E_TRUE);
	return 0;
}

void __attribute__((interrupt)) inter_higher0_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000004);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_SW_EXPECTION with higher priority!\n");
	return;
}	

void __attribute__((interrupt)) inter_lower0_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MEM_FAULT with lower priority!\n\n");

	return;
}

void __attribute__((interrupt)) inter_higher1_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA1_INT with higher priority!\n");

	return;
}	

void __attribute__((interrupt)) inter_lower1_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000080);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_USER_INT with lower priority!\n\n");
	return;
}	

void __attribute__((interrupt)) inter_higher2_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_SW_EXPECTION with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower2_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000002);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MEM_FAULT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher3_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000008);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MEM_FAULT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower3_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER0_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher4_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MEM_FAULT with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower4_isr()
{
	// Set the ILAT for the interrupt with higher priority
	e_reg_write(E_REG_ILATST, 0x00000004);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER0_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher5_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000010);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER0_INT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower5_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER1_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher6_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER0_INT with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower6_isr()
{
	// Set the ILAT for the interrupt with higher priority
	e_reg_write(E_REG_ILATST, 0x00000008);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER1_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher7_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000020);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER1_INT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower7_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MESSAGE_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher8_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_TIMER1_INT with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower8_isr()
{
	// Set the ILAT for the interrupt with higher priority
	e_reg_write(E_REG_ILATST, 0x00000010);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MESSAGE_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher9_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000040);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MESSAGE_INT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower9_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA0_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher10_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_MESSAGE_INT with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower10_isr()
{
	// Set the ILAT for the interrupt with higher priority
	e_reg_write(E_REG_ILATST, 0x00000020);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA0_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher11_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000080);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA0_INT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower11_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA1_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher12_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA0_INT with higher priority!\n");
}

void __attribute__((interrupt)) inter_lower12_isr()
{
	// Set the ILAT for the interrupt with higher priority
	e_reg_write(E_REG_ILATST, 0x00000040);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA1_INT with lower priority!\n\n");
	return;
}

void __attribute__((interrupt)) inter_higher13_isr()
{
	// Set the ILAT for the interrupt with lower priority
	e_reg_write(E_REG_ILATST, 0x00000200);
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_DMA1_INT with higher priority!\n");
	return;	
}

void __attribute__((interrupt)) inter_lower13_isr()
{
	sprintf(outbuf + strlen(outbuf),"I am interrupt E_USER_INT with lower priority!\n\n");
	return;
}
