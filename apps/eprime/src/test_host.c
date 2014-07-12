/*
  Copyright (C) 2014 Adapteva, Inc.
  Contributed by Matt Thompson <mthompson@hexwave.com>

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

#include <stdio.h>
#include <stdlib.h>

int is_prime(unsigned long number);

int main(void)
{
        int row,col;
        int iter = 1000000;
        int i;
	unsigned long pcount = 0;

        for(i=0;i<iter;i++)
        {
                for(row=0;row<4;row++)
                {
                        for(col=0;col<4;col++)
                        {
				int p;
                                int number;
                                number = 2 + ((2*row*4) + (2*col+1));

                                number += i * (2*16);
                                //printf("(%d,%d) %d\n", row,col, number);

				p = is_prime(number);
				if(p)
					pcount++;
                        }
                }
        }

	printf("Primes %lu\n", pcount);
}

