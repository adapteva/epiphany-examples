/*
  emain_slave.c

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
// core attatches its interrupts handler with fibonacci functions. 
// A success/error message is sent to the host according to the result.
//
// Aug-2013, XM.


#include <stdio.h>
#include <stdlib.h>
#include <e-lib.h>

#define resultbase              (0x00005200)
#define signal      ((unsigned *)0x00005400)
#define start                   (0x00000000)
#define finish                  (0x00000001)



void swexception_isr(int signum);
void memfault_isr(int signum);
void timer0_isr(int signum);
void timer1_isr(int signum);
void dma0_isr(int signum);
void dma1_isr(int signum);
void user_isr(int signum);
unsigned fibonacci();

unsigned *result;


int main(void) 
{
	unsigned i;
	
	*signal = start;

	//attach the isr handlers
	e_irq_attach(E_SW_EXCEPTION, swexception_isr);
	e_irq_attach(E_MEM_FAULT, memfault_isr);
	e_irq_attach(E_TIMER0_INT, timer0_isr);
	e_irq_attach(E_TIMER1_INT, timer1_isr);
	e_irq_attach(E_DMA0_INT, dma0_isr);
	e_irq_attach(E_DMA1_INT, dma1_isr);
	e_irq_attach(E_USER_INT, user_isr);


	//enable interrupts
	e_irq_global_mask(E_FALSE);
	e_reg_write(E_REG_IMASK,0x0);

	//initialize the target mem
	result = (unsigned *)resultbase;
	for(i=0;i<7;i++)
	{
		result[i] = 0x0;
	}
	
	//wait for the finished signal from master
	while(*signal != finish) ;

	return EXIT_SUCCESS;
}



unsigned fibonacci()
{
	unsigned a,b,c;
	unsigned count;

	a = 1; b = 1;
	for(count=0;count<20;count++)
	{
		c = a+b;
		a = b;
		b = c;
	}
	
	return b;
}


void __attribute__((interrupt)) swexception_isr(int signum)
{
	result[0] = fibonacci();
	return;
}

void __attribute__((interrupt)) memfault_isr(int signum)
{
	result[1] = fibonacci();
	return;
}

void __attribute__((interrupt)) timer0_isr(int signum)
{
	result[2] = fibonacci();
	return;
}

void __attribute__((interrupt)) timer1_isr(int signum)
{
	result[3] = fibonacci();
	return;
}

void __attribute__((interrupt)) dma0_isr(int signum)
{
	result[4] = fibonacci();
	return;
}


void __attribute__((interrupt)) dma1_isr(int signum)
{
	result[5] = fibonacci();
	return;
}


void __attribute__((interrupt)) user_isr(int signum)
{
	result[6] = fibonacci();
	return;
}



