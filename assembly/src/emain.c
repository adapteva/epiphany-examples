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
// core runs varies assembly command. A success/error message is sent 
// to the host according to the result.
//
// Aug-2013, XM.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <e_lib.h>

#define e_status 0x000f0404


char outbuf[4096] SECTION("shared_dram");



void test_add();
void test_and();
void test_asr();
void test_bitr();
void test_eor();
void test_fabs();
void test_fadd();
void test_fmadd();
void test_fmul();
void test_fmsub();
void test_fsub();
void test_gid_and_gie();
void test_iadd();
void test_imadd();
void test_imsub();
void test_imul();
void test_isub();
void test_sub();
void test_jr();
void test_lsl();
void test_lsr();
void test_orr();
void test_fix();
void test_float();
void test_ldr_str();
void test_mov();
void test_movs();
void test_testset();

unsigned fault;

int main(void) 
{
	//initialize
	sprintf(outbuf, "\t");
	fault = 0x0;

	//call the test function
	test_add();
	test_and();
	test_asr();
	test_bitr(); 
	test_eor();
	test_fabs();
	test_fadd();
	test_fmadd();
	test_fmul();
	test_fmsub();
	test_fsub();
	test_gid_and_gie();
	test_iadd();
	test_imadd();
	test_imsub();
	test_imul();
	test_isub();
	test_sub();
	test_jr();
	test_lsl();
	test_lsr();
	test_orr();
	test_fix();
	test_float();
	test_ldr_str();
	test_mov();
	test_movs();
	test_testset();

	//check the results
	if (fault == 0)
		sprintf(outbuf + strlen(outbuf),"\ntest Assembly passed!\n");
	else
		sprintf(outbuf + strlen(outbuf),"\ntest Assembly failed! Number of fault is %d!\n", fault);

	return EXIT_SUCCESS;
}

void test_add() 
{
	int a, b, c;
	a = 40; b = -60; c = 0;
	
	//add
	__asm__ __volatile__ ("add %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	//check result
	if (c != -20)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ADD failed!\n\t");
		fault++;
	}


	//add(displacement)
	c = 0;
	__asm__ __volatile__ ("add %0, %1, #-60": "=r"(c): "r"(a));
	
	//check result
	if (c != -20)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ADD(displacement) failed!\n\t");
		fault++;
	}

	return;
}

void test_and() 
{
	unsigned a, b, c;
	a = 0xe3; b = 0x79; c = 0x0;
	
	__asm__ __volatile__ ("and %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != 0x61)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction AND failed!\n\t");
		fault++;
	}

	return;
}

void test_asr() 
{
	unsigned a, b, c;
	a = 0xf0; b = 0x4; c = 0x0;
	
	//asr
	__asm__ __volatile__ ("asr %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != 0xf)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ASR failed!\n\t");
		fault++;
	}

	//asr(displacement)
	__asm__ __volatile__ ("asr %0, %1,#4 ": "=r"(c): "r"(a));
	
	//check result
	if (c != 0xf)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ASR(displacement) failed!\n\t");
		fault++;
	}

	return;
}

void test_bitr() 
{
	unsigned a, b;
	a = 0x13; b = 0x0;
	__asm__ __volatile__ ("bitr %0, %1": "=r"(b): "r"(a));
	if (b != 0xc8000000)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction BITR failed!\n\t");
		fault++;
	}

	return;
}

void test_eor() 
{
	unsigned a, b, c;
	a = 0x9999; b = 0x2222; c = 0x0;
	
	__asm__ __volatile__ ("eor %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != 0xbbbb)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction EOR failed!\n\t");
		fault++;
	}

	return;
}

void test_fadd() 
{
	float a, b, c;
	a = 4.6f; b = 5.4f; c = 0.0f;
	
	__asm__ __volatile__ ("fadd %0, %1, %2":"=r"(c):"r"(a),"r"(b):);
	
	//check result
	if ((c<10.0f*0.9) || (c>10.0f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FADD failed!\n\t");
		fault++;
	}

	return;
}

void test_fabs() 
{
	float a, b;
	a = -4.6f; b = 0.0f;
	
	__asm__ __volatile__ ("fabs %0, %1":"=r"(b):"r"(a):);
	
	//check result
	if ((b<4.6f*0.9) || (b>4.6f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FABS failed!\n\t");
		fault++;
	}
	
	return;
}

void test_fmadd() 
{
	float a, b, c;
	a = 4.0f; b = 6.0f; c = 3.0f;
	
	__asm__ __volatile__ ("fmadd %0, %1, %2":"+r"(c):"r"(a),"r"(b):);

	//check result
	if ((c<27.0f*0.9) || (c>27.0f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FMADD failed!\n\t");
		fault++;
	}
	
	return;
}

void test_fmul() 
{
	float a, b, c;
	a = 4.6f; b = 5.4f; c = 3.0f;
	
	__asm__ __volatile__ ("fmul %0, %1, %2":"=r"(c):"r"(a),"r"(b):);
	
	//check result
	if ((c<24.84f*0.9) || (c>24.84f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FMUL failed!\n\t");
		fault++;
	}
	
	return;
}

void test_fmsub() 
{
	float a, b, c;
	a = 4.6f; b = 5.4f; c = 3.0f;
	
	__asm__ __volatile__ ("fmsub %0, %1, %2":"+r"(c):"r"(a),"r"(b):);
	
	//check result
	if ((c>-21.84f*0.9) || (c<-21.84f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FMSUB failed!\n\t");
		fault++;
	}
	
	return;
}

void test_fsub() 
{
	float a, b, c;
	a = 5.4f; b =4.6f; c = 3.0f;
	
	__asm__ __volatile__ ("fsub %0, %1, %2":"=r"(c):"r"(a),"r"(b):);
	
	//check result
	if ((c<0.8f*0.9) || (c>0.8f*1.1))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FSUB failed!\n\t");
		fault++;
	}
	
	return;
}

void test_gid_and_gie() 
{
	unsigned c, set1, set2, set3;
	c = 0x0; set1 = 0x0; set2 = 0x0; set3 = 0x0;
	
	//disable for the 1st time
	__asm__ __volatile__ ("GID");
	
	//check result
	c = e_reg_read(E_REG_STATUS);
	if((c&0x2) != 0x2) 
	{
		sprintf(outbuf + strlen(outbuf),"Instruction GID failed(1st time)!\n\t");
		fault++;
	}

	//enalbe
	__asm__ __volatile__ ("GIE");
	
	//check result
	c = e_reg_read(E_REG_STATUS);
	if((c&0x2) != 0x0) {
		sprintf(outbuf + strlen(outbuf),"Instruction GIE failed!\n\t");
		fault++;
	}

	//disable for the 2nd time
	__asm__ __volatile__ ("GID");
	
	//check result
	c = e_reg_read(E_REG_STATUS);
	if((c&0x2) != 0x2) {
		sprintf(outbuf + strlen(outbuf),"Instruction GID failed(2nd time)!\n\t");
		fault++;
	}

	return;

}


void test_iadd() 
{
	int a, b, c;
	a = -40; b = -60; c = 0;
	
	//change calculate mode
	e_reg_write(E_REG_CONFIG, 0x80000);
	
	__asm__ __volatile__ ("iadd %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != -100)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction IADD failed!\n\t");
		fault++;
	}

	e_reg_write(E_REG_CONFIG, 0x0);
	return;
}

void test_imadd() 
{
	int a, b, c;
	a=4; b=6; c=3;
	
	//change calculate mode
	e_reg_write(E_REG_CONFIG, 0x80000);
	
	__asm__ __volatile__ ("imadd %0, %1, %2": "+r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != 27)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction IMADD failed!\n\t");
		fault++;
	}

	e_reg_write(E_REG_CONFIG, 0x0);
	return;

}

void test_imsub() 
{
	int a, b, c;
	a=4; b=6; c=3;
	
	//change calculate mode
	e_reg_write(E_REG_CONFIG, 0x80000);
	
	__asm__ __volatile__ ("imsub %0, %1, %2": "+r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != -21)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction IMSUB failed!\n\t");
		fault++;
	}

	e_reg_write(E_REG_CONFIG, 0x0);
	return;

}

void test_imul() 
{
	int a, b, c;
	a=4; b=6; c=0;
	
	//change calculate mode
	e_reg_write(E_REG_CONFIG, 0x80000);
	
	__asm__ __volatile__ ("imul %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != 24)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction IMUL failed!\n\t");
		fault++;
	}

	e_reg_write(E_REG_CONFIG, 0x0);
	return;

}

void test_isub() 
{
	int a, b, c;
	a=4; b=6; c=0;
	
	//change calculate mode
	e_reg_write(E_REG_CONFIG, 0x80000);
	
	__asm__ __volatile__ ("isub %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != -2)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ISUB failed!\n\t");
		fault++;
	}

	e_reg_write(E_REG_CONFIG, 0x0);
	return;

}


void test_sub() 
{
	int a, b, c;
	a = 40; b = 60; c = 0;

	//sub
	__asm__ __volatile__ ("sub %0, %1, %2": "=r"(c): "r"(a), "r"(b));
	
	//check result
	if (c != -20)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction SUB failed!\n\t");
		fault++;
	}

	//sub(displacement)
	__asm__ __volatile__ ("sub %0, %1, 60": "=r"(c): "r"(a));
	
	//check result
	if (c != -20)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction SUB(displacement) failed!\n\t");
		fault++;
	}

	return;
}

void test_jr() 
{
	unsigned *lab, i, jr_count, jr_sp_count;
	jr_count = 0; jr_sp_count = 0;
	lab = &&mylab;
	
	//jump
	__asm__ __volatile__("jr %0"::"r"(lab):);
	
	//if jump failed, jr_sp_count will accumulated to 2
	jr_sp_count++;

mylab:	
	jr_sp_count++;
	
	//check result
	if (jr_sp_count != 1)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction JR failed!\n\t");
		fault++;
	}

	return;
}



void test_lsl() 
{
	unsigned a, b, c;
	a = 0x12345678;
	b = 0x10000004;
	c = 0x0;

	__asm__ __volatile__ ("lsl %0, %1, %2": "=r"(c): "r"(a), "r"(b));

	//check result
	if (c != 0x23456780)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LSL failed!\n\t");
		fault++;
	}

	return;
}

void test_lsr() 
{
	unsigned a, b, c;
	a = 0x12345678;
	b = 0x10000004;
	c = 0x0;

	__asm__ __volatile__ ("lsr %0, %1, %2": "=r"(c): "r"(a), "r"(b));

	//check result
	if (c != 0x01234567)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LSR failed!\n\t");
		fault++;
	}

	return;
}


void test_orr() 
{
	unsigned a, b, c;
	a = 0x12345678;
	b = 0x30400304;
	c = 0x0;

	__asm__ __volatile__ ("orr %0, %1, %2": "=r"(c): "r"(a), "r"(b));

	//check result
	if (c != 0x3274577c)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction ORR failed!\n\t");
		fault++;
	}

	return;
}



void test_fix() 
{
	float a;
	int b;
	a = -134.567f;
	
	__asm__ __volatile__("fix %0, %1":"=r"(b):"r"(a):);

	//check result
	if (b != -135)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FIX failed!\n\t");
		fault++;
	}	

	return;
}

void test_float() 
{
	float b;
	int a;
	a = -135;
	
	__asm__ __volatile__("float %0, %1":"=r"(b):"r"(a):);

	//check result
	if ((b<-135.0f*1.1)||(b>-135.0f*0.9))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction FLOAT failed!\n\t");
		fault++;
	}

	return;
}

void test_ldr_str() 
{
	unsigned a,b,c,d,r;
	unsigned *p0;
	a = 0x1234;
	b = 0x4000;
	c = 0x200;
	d = 0x4200;
	p0 =(unsigned *) 0x4200;//p0 point to d
	//test index
	*p0 = 0x20;
	
	__asm__ __volatile__("str %0,[%1,%2]"::"r"(a),"r"(b),"r"(c):);
	__asm__ __volatile__("ldr %0, [%1, %2]":"=r"(r):"r"(b),"r"(c):);

	//check result
	if (r != a)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LDR & STR(index) failed!\n\t");
		fault++;
	}		

	//test displacement
	*p0 = 0x40;
	
	__asm__ __volatile__("str %0,[%1,#0x80]"::"r"(a),"r"(b):);
	__asm__ __volatile__("ldr %0, [%1, #0x80]":"=r"(r):"r"(b):);	
	
	//check result
	if (r != a)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LDR & STR(displacement) failed!\n\t");
		fault++;
	}		

	//test ldr post
	p0 = (unsigned *) 0x4000;
	*p0 = 0x60;
	
	__asm__ __volatile__("ldr %0, [%1], %2":"=r"(r),"+r"(b):"r"(c):);	

	//check result
	if ((r!= 0x60) || (b!= 0x4200))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LDR(post) failed!\n\t");
		fault++;
	}	

	//test ldr post+dis
	p0 = (unsigned *) 0x4000;
	b = 0x4000;
	*p0 = 0x80;
	__asm__ __volatile__("ldr %0, [%1], #0x80":"=r"(r),"+r"(b):"r"(c):);
	
	//check result
	if ((r!= 0x80) || (b!= 0x4200))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction LDR(post+displacement) failed!\n\t");
		fault++;
	}		


	//test str post
	p0 = (unsigned *) 0x4000;
	b = 0x4000;
	*p0 = 0x0;
	__asm__ __volatile__("str %1,[%0],%2":"+r"(b):"r"(a),"r"(c):);
	
	//check result
	if ((*p0 != a) & (b != 0x4200))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction STR(post) failed!\n\t");
		fault++;
	}		


	//test str post_dis
	p0 = (unsigned *) 0x4000;
	b = 0x4000;
	*p0 = 0x0;
	__asm__ __volatile__("str %1,[%0],#0x80":"+r"(b):"r"(a):);
	
	//check result
	if ((*p0 != a) & (b != 0x4200))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction STR(post+displacement) failed!\n\t");
		fault++;
	}		

	return;
}

void test_mov() 
{
	int a, b, c;
	a = 0x12345678;
	b = 0x123;
	c = 0x12345678;
	
	//test mov
	__asm__ __volatile__("mov %0, %1":"=r"(b):"r"(a):);

	//check result
	if (b != a)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction MOV failed!\n\t");
		fault++;
	}		

	//test mov(immediate)
	__asm__ __volatile__("mov %0, #0x345":"=r"(c)::);

	//check result
	if (c != 0x345)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction MOV(immediate) failed!\n\t");
		fault++;
	}		

	//test movt
	__asm__ __volatile__("movt %0, #0x345":"=r"(c)::);

	//check result
	if (c != 0x03450345)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction MOVT failed!\n\t");
		fault++;
	}	

	return;

}

void test_movs() 
{
	unsigned a, b, c;

	//test movts
	b = 0x00080000;
	
	__asm__ __volatile__("movts ctimer0, %0"::"r"(b):);

	//check result
	a = e_reg_read(E_REG_CTIMER0);
	if (a != b)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction MOVTS failed!\n\t");
		fault++;
	}	

	//test movfs
	__asm__ __volatile__("movfs %0, ctimer0":"=r"(c)::);
	
	//check result
	if (c != a)
	{
		sprintf(outbuf + strlen(outbuf),"Instruction MOVFS failed!\n\t");
		fault++;
	}	
	
	

	return;

}


void test_testset() 
{
	unsigned a, b, c;
	unsigned *pt;
	
	a = 0x1234;
	b = (unsigned)e_get_global_address(e_group_config.core_row, e_group_config.core_col, (void *)0x6000);
	c = 0x200;
	pt = (unsigned *) 0x6200;

	//test when target is zero
	*pt = 0x0;
	
	__asm__ __volatile__("TESTSET %0,[%1,%2]":"+r"(a):"r"(b),"r"(c):);
	
	//check result
	if ((a != 0x0)||(*pt != 0x1234))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction TESTSET(target is zero) failed!\n\t");
		fault++;
	}	

	//test when target is not zero
	a = 0x1234;
	*pt = 0x100;
	
	__asm__ __volatile__("TESTSET %0,[%1,%2]":"=r"(a):"r"(b),"r"(c):);
	
	//check result
	if ((a != 0x100)||(*pt != 0x100))
	{
		sprintf(outbuf + strlen(outbuf),"Instruction TESTSET(target is not zero) failed!\n\t");
		fault++;
	}	

	return;
}







