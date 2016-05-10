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
// a message identifying it.srec to the shared external
// memory buffer.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

extern int _PROG_SIZE_FOR_CORE_;
extern int _HEAP_SIZE_FOR_CORE_;
extern int _HALF_BANK_SIZE_;
extern int _NUM_ROWS_IN_CHIP_;
extern int _NUM_COLS_IN_CHIP_;
extern int _FIRST_CORE_ROW_;
extern int _FIRST_CORE_COL_;
extern int _CORE_ROW_;
extern int _CORE_COL_;
extern int _CORE_NUM_;
extern int _SHARED_DRAM_;

int main(void) {
	const char		  ShmName[] = "hello_shm"; 
	const char        Msg[] = "Hello World from core 0x%03x: %#x %#x %#x %#x %#x %#x %#x %#x %#x %#x %#x";
	char              buf[256] = { 0 };
	e_coreid_t		  coreid;
	e_memseg_t   	  emem;
	unsigned          my_row;
	unsigned          my_col;


	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);

	if ( E_OK != e_shm_attach(&emem, ShmName) ) {
		return EXIT_FAILURE;
	}

	// Attach to the shm segment
	snprintf(buf, sizeof(buf), Msg,
		coreid,
		&_PROG_SIZE_FOR_CORE_,
		&_HEAP_SIZE_FOR_CORE_,
		&_HALF_BANK_SIZE_,
		&_NUM_ROWS_IN_CHIP_,
		&_NUM_COLS_IN_CHIP_,
		&_FIRST_CORE_ROW_,
		&_FIRST_CORE_COL_,
		&_CORE_ROW_,
		&_CORE_COL_,
		&_CORE_NUM_,
		&_SHARED_DRAM_);

	if ( emem.size >= strlen(buf) + 1 ) {
		// Write the message (including the null terminating
		// character) to shared memory
		e_write((void*)&emem, buf, my_row, my_col, NULL, strlen(buf) + 1);
	} else {
		// Shared memory region is too small for the message
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
