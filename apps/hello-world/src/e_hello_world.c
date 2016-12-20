/*
  e_hello_world.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yaniv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This is the DEVICE side of the Hello World example.
// The host may load this program to any eCore. When
// launched, the program queries the CoreID and prints
// a message identifying itself to the shared external
// memory buffer.

// Last update: 2016 dec 19. (JLQ).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

#define buff_sz   (4096)

char outbuf[buff_sz] SECTION("shared_dram");

int main(void) {
	e_coreid_t	coreid;

	coreid = e_get_coreid();
	
	sprintf(outbuf, "Hello World from core 0x%03x! \n", coreid);
	
	return EXIT_SUCCESS;
}

