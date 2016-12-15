/*
  emain.c

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

#include <stdint.h>

#define RESULT_ADDR     0x8e000000
#define VALID_ADDR		0x8e0f0000
#define INVALID_ADDR	0x7e0f0000

int main(void) {
	uint32_t val, tmp, old;
	volatile uint32_t *result = (uint32_t *) RESULT_ADDR;
	volatile uint32_t *valid = (uint32_t *) VALID_ADDR;
	volatile uint32_t *invalid = (uint32_t *) INVALID_ADDR;

	old = *valid;

	/* random number */
	val = 0xed0d4a75;

	*valid = val;

	/* Wait for write to propagate */
	while (*valid != val)
		;

	tmp = *invalid;

	/* Restore old value (just in case) */
	*valid = old;


	/* We expect the invalid address to alias to the valid one when the FPGA
	 * elink RX is configured in remapping mode. In Parabuntu 2016.11, the
	 * epiphany driver incorrectly configures RX to use MMU mode without
	 * filling in the entire MMU table. This makes the invalid address alias to
	 * 0x000f0000  */

	if (tmp != val) {
		/* 2 indicates error */
		*result = 2;
		return 1;
	}

	/* invalid address was translated into valid adress by FPGA elink RX */
	*result = 1;
	return 0;
}
