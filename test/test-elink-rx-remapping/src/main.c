/*
  main.c

  Copyright (C) 2016 Adapteva, Inc.
  Contributed by Ola Jeppsson <ola@adapteva.com>

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#include <e-hal.h>

int main(int argc, char *argv[])
{
	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t emem;
	const uint32_t zero = 0;
	uint32_t result = 0;

	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);

	if (e_alloc(&emem, 0, 128) != E_OK)
		exit(EXIT_FAILURE);

	e_write(&emem, 0, 0, 0, &zero, sizeof(zero));

	e_open(&dev, 0, 0, 1, 1);
	if (e_load("emain.elf", &dev, 0, 0, E_TRUE) != E_OK)
		exit(EXIT_FAILURE);

	do {
		e_read(&emem, 0, 0, 0, &result, sizeof(result));
	} while (!result);

	if (result != 1) {
		fprintf(stderr,
"Test failed. elink RX remapping configured incorrectly. Faulty programs can\n"
"likely access the system's first 1MB memory region. Go patch your kernel!\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "Test passed\n");
	return 0;
}
