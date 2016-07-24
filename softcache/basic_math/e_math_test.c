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
#include <stdint.h>

#include "e_lib.h"

#define pi (3.141592653);

char outbuf[4096] SECTION("shared_dram");

int main(void)
{
	unsigned int i;
	unsigned int num;
	unsigned int time_p;
	unsigned int time_c;
	unsigned int time_compare;
	unsigned int index = 0;
        unsigned int count = 0;
	uint32_t *mailbox;
	volatile uint32_t *done;
	float volatile af;
	float volatile bf;
	float volatile cf;
	float volatile df;
	float volatile ref;
	unsigned int temp = 0;

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
	done = (uint32_t *)0x5ffc;
	mailbox = (uint32_t *)0x6000;
	mailbox[0] = 0;

	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	// Get time waste on functions
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);
	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);
	time_compare = time_p - time_c ;

        // Mod
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

          ref = fmodf(cf,df);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((ref > 1.99f)&&(ref < 2.01f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Sin
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f0 = sinf(in_sin);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f0 > 0.499) && (re_f0 < 0.501))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Cos
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f1 = cosf(in_cos);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f1 > 0.499) && (re_f1 < 0.501))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Sqrt
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

          re_f2 = sqrtf(in_sqt);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f2 > 0.499) && (re_f2 < 0.501))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Ceil
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

          re_f3 = ceilf(in_ceil);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f3 > 2.99) && (re_f3 < 3.01))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Floor
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

          re_f5 = floorf(in_ceil);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f5 > 1.99f) && (re_f5 < 2.01f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Log10
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f4 = log10f(df);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

          if ((re_f4 > 0.477f) && (re_f4 < 0.478f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Ln
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f4 = logf(in_log);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

	  if ((re_f4 > 4.6f) && (re_f4 < 4.61f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Power
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f4 = powf(cf,df);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

	  if ((re_f4 > 511.99f) && (re_f4 < 512.01f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	// Ldexp
        for (i = 0; i < 4; i++) {
          e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
          e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
          time_p = e_ctimer_get(E_CTIMER_0);

	  re_f4 = ldexpf(cf,df);

          time_c = e_ctimer_get(E_CTIMER_0);
          e_ctimer_stop(E_CTIMER_0);

          temp = time_p - time_c - time_compare;

	  if ((re_f4 > 63.99f) && (re_f4 < 64.01f))
          {
            mailbox[index] = temp;
            index++;
          }
        }

	e_ctimer_stop(E_CTIMER_0);

        mailbox[index] = index;

	*done = 1;

	return EXIT_SUCCESS;

}
