/*

Copyright (C) 2015 Adapteva, Inc.
Contributed by Ola Jeppsson <support@adapteva.com>

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

/*
 * e_alloc() resource leak regression test.
 *
 * Fixed in this commit:
 * "e-hal: e_free: Fix resource leak"
 * be28d22caf3e09aa5813ec3a946279af2ee5191c
 */

#include <stdlib.h>
#include <stdio.h>
#include <e-hal.h>

#define EMEM_SIZE (0x02000000)

int main()
{
	int i, ret, retval = 0;
	e_mem_t emem;

	//Initalize Epiphany device
	e_init(NULL);

	/* Trigger bug */
	for (i = 0; i < 1000; i++) {
		ret = e_alloc(&emem, 0, EMEM_SIZE);

		if (ret != E_OK) {
			printf("ERROR: e_alloc() failed after %d iterations\n",
			       i);
			retval = 1;
			break;
		}

		e_free(&emem);
	}

	e_finalize();

	return retval;
}
