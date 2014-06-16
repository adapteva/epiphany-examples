/*
e_math_example.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>
Contributed by Pierre Langlois <pierre.langlois@embecosm.com>

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

#include <math.h>
#include "e_lib.h"

int main(void)
{
	float volatile af;
	float volatile bf;
	float volatile cf;
	float volatile df;
	float re_f0, re_f1;
	unsigned int time_p;
	unsigned int time_c;
	unsigned int time_compare;
	unsigned int time1, time2;

	af = 3.5f;
	bf = 2.8f;

  // Make the cache manager load the functions into internal memory
	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	e_ctimer_set(E_CTIMER_0,  E_CTIMER_MAX) ;
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);
	time_compare = time_p - time_c ;

  // Fist measurement
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f0 = fmodf(af,bf);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	time1 = time_p - time_c - time_compare;

  // Second measurement
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	time_p = e_ctimer_get(E_CTIMER_0);

	re_f1 = fmodf(af,bf);

	time_c = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);

	time2 = time_p - time_c - time_compare;

	return 0;

}
