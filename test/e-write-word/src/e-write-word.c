/*
Copyright (C) 2013 Adapteva, Inc.
Contributed by Andreas Olofsson <support@adapteva.com>

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

void usage();

int main(int argc, char *argv[]){

  e_epiphany_t dev;
  unsigned int data_in;

  //Open
  e_init(NULL);
  e_reset_system();
  e_open(&dev, 0, 0, 1, 1);
  e_write(&dev, 0, 0, 0, &data_in, sizeof(data_in));
  e_close(&dev);
  e_finalize();
  return 0;
}
