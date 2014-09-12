/*
e_math_test1.c

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

// This is the device side of the basic math example.
// This program is to use the hardcode values of each
// operation to test each core.This program tests basic
// math operations as follows: Addition, Substraction,
// Multiplication, Division, Modular, sin, cos, square root,
// ceil, floor, log10, ln, power, ldexp.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "e_lib.h"

#define pi (3.141592653);

char outbuf[4096] SECTION("shared_dram");
int main(void)
{
	e_coreid_t coreid;
	unsigned int i;
	unsigned int num;
	unsigned int time_p;
	unsigned int time_c;
	unsigned int time_compare;
	unsigned int list[14] = {4, 3, 3, 144953, 68892, 24971, 52908, 96970, 19531, 17676, 10459, 10458, 357432, 36235};
	unsigned int index = 0;
	unsigned *mailbox;
	float volatile af;
	float volatile bf;
	float volatile cf;
	float volatile df;
	float volatile ref;
	unsigned int temp;

	float volatile in_sin;
	float volatile in_cos;
	float volatile in_sqt;
	float volatile in_ceil;
	float volatile in_log;
	float re_f0, re_f1, re_f2, re_f3, re_f4, re_f5;

	af = 3.5f;
	bf = 2.8f;
	cf = 8.0f;
	df = 3.0f;
	in_sin = (float) pi;
	in_sin = in_sin / 6 ;
	in_cos = (float) pi;
	in_cos = in_cos / 3 ;
	in_sqt = 0.25f;
	in_ceil = 2.5f;
	in_log = 100.0f;
	mailbox = (unsigned *)0x6000;
	mailbox[0] = 0;

	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();
	sprintf(outbuf, "");

	// Get time waste on functions
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);
	time_compare = time_p - time_c ;

	// Addition
	// E_CTIMER0
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

		ref = bf + af ;


	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (!((ref > 6.299f)&&(ref < 6.301f)) )
	{
		sprintf(outbuf, "\n Addition is wrong!\n");
	}else
	{
	  index++;
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for addition is %d.\n", temp);
	}

	// Subtraction
	// E_CTIMER0
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

		ref =  af - bf;


	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if ( !((ref > 0.699f)&&(ref < 0.701f)))
	{
		sprintf(outbuf+strlen(outbuf), "\n Subtraction is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf) , "\nPASS for substraction!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for subtraction is %d.\n", temp);
	}

	// Mul
	//E_CTIMER0
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

		ref = af * bf;

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (!((ref > 9.799f)&&(ref < 9.801f)))
	{
		sprintf(outbuf+strlen(outbuf), "\n Multiplication is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for multiplication!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for multiplication is %d.\n", temp);
	}

	// Div
	//E_CTIMER0
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

		ref = ( af / bf);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (!((ref > 1.2499f)&&(ref < 1.2501f)))
	{
		sprintf(outbuf+strlen(outbuf), "\n Division is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for division!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for division is %d.\n", temp);
	}

	// Mod
	// E_CTIMER0
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

		ref = fmodf(cf,df);


	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (!((ref > 1.99f)&&(ref < 2.01f)))
	{
		sprintf(outbuf+strlen(outbuf), "\n Mod is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for mod!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"fmodf()\" is %d.\n", temp);
	}

	// Sin
	// E_CTIMER0
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f0 = sinf(in_sin);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if ( !((re_f0 > 0.499) && (re_f0 < 0.501)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Sin is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for sin!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"sinf()\" is %d.\n", temp);
	}

	// Cos
	// E_CTIMER0
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f1 = cosf(in_cos);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (  !((re_f1 > 0.499) && (re_f1 < 0.501)))
	{
		sprintf(outbuf+strlen(outbuf), "\n Cos is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for cos!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"cosf()\" is %d.\n", temp);
	}


	// Sqrt
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f2 = sqrtf(in_sqt);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if ( !((re_f2 > 0.499) && (re_f2 < 0.501)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Sqrt is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for square root!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"sqrtf()\" is %d.\n", temp);
	}

	// Ceil
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f3 = ceilf(in_ceil);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if ( !((re_f3 > 2.99) && (re_f3 < 3.01)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Ceil is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for ceil!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"ceilf()\" is %d.\n", temp);
	}

	// Floor
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f5 = floorf(in_ceil);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if ( !((re_f5 > 1.99f) && (re_f5 < 2.01f))  )
	{
		sprintf(outbuf+strlen(outbuf), "\n Floor is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for floor!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"floorf()\" is %d.\n", temp);
	}


	// Log10
	e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_1);

	re_f4 = log10f(df);

	time_c = e_ctimer_get(E_CTIMER_1);
	e_ctimer_stop(E_CTIMER_1);

	temp = time_p - time_c - time_compare;

	if ( !((re_f4 > 0.477f) && (re_f4 < 0.478f)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Log10 is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for log10!\n");
	  sprintf(outbuf +strlen(outbuf) , "\nThe clock cycle count for \"log10f()\" is %d.\n", temp);
	}


	// Ln
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f4 = logf(in_log);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	temp = time_p - time_c - time_compare;

	if (!((re_f4 > 4.6f) && (re_f4 < 4.61f)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Ln is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for ln!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"logf()\" is %d.\n", temp);
	}

	// Power
	e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_1);

	re_f4 = powf(cf,df);

	time_c = e_ctimer_get(E_CTIMER_1);
	e_ctimer_stop(E_CTIMER_1);

	temp = time_p - time_c - time_compare;

	if (!( (re_f4 > 511.99f) && (re_f4 < 512.01f) ))
	{
		sprintf(outbuf+strlen(outbuf), "\n Power is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for power!\n");
	  sprintf(outbuf +strlen(outbuf), "\nThe clock cycle count for \"powf()\" is %d.\n", temp);
	}

	// Ldexp
	e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_1);

	re_f4 = ldexpf(cf,df);

	time_c = e_ctimer_get(E_CTIMER_1);
	e_ctimer_stop(E_CTIMER_1);

	temp = time_p - time_c - time_compare;

	if ( !((re_f4 > 63.99f) && (re_f4 < 64.01f)) )
	{
		sprintf(outbuf+strlen(outbuf), "\n Ldexp is wrong!\n");
	}else
	{
	  index++;//sprintf(outbuf +strlen(outbuf), "\nPASS for ldexp!\n");
	  sprintf(outbuf +strlen(outbuf)  , "\nThe clock cycle count for \"ldexpf()\" is %d.\n", temp);
	}

	mailbox[0] = index;

	return EXIT_SUCCESS;

}
