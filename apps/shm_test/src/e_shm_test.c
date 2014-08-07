/*
  e_shm_test.c

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <e_lib.h>
#include <e_trace.h>

int main(void)
{
	const char		  ShmName[] = "test_shm"; 
	const char        Msg[] = "Hello World from core 0x%03x!";
	char              buf[256] = { 0 };
	e_coreid_t		  coreid;
	e_memseg_t     	  emem;
	unsigned          my_row;
	unsigned          my_col;
	int               retval = EXIT_SUCCESS;

	// Initialize the trace utility
	if ( E_OK != trace_init() ) {
		// what to do? Can't send a message to the arm side
	}

	trace_start();

	trace_write(T_SEVERITY_WARNING, T_EVENT_PROGRAM_START, T_BP_0, 'a');

	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);

	// Attach to the shm segment
	if ( E_OK != e_shm_attach(&emem, ShmName) ) {
		trace_write(T_SEVERITY_ERROR, T_EVENT_MARKER1, T_BP_1, 'b');
		retval = EXIT_FAILURE;
		goto done;
	}

	trace_write(T_SEVERITY_ERROR, T_EVENT_MARKER1, T_BP_2, 'c');

	snprintf(buf, sizeof(buf), Msg, coreid);
	
	if ( emem.size >= strlen(buf) + 1 ) {
		// Write the message (including the null terminating
		// character) to shared memory
		trace_write(T_SEVERITY_DEBUG, T_EVENT_MARKER1, T_BP_3, 'd');
		e_write((void*)&emem, buf, my_row, my_col, NULL, strlen(buf) + 1);
		trace_write(T_SEVERITY_DEBUG, T_EVENT_MARKER1, T_BP_4, 'e');
	} else {
		// Shared memory region is too small for the message
		retval = EXIT_FAILURE;
	}

 done:
	trace_write(T_SEVERITY_DEBUG, T_EVENT_PROGRAM_STOP, T_BP_5, 'f');
	trace_stop();

	return retval;
}
