/*
e-dump-regs.c

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

#define RAM_SIZE   (0x8000)

void usage();

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;

  unsigned int read_buffer[RAM_SIZE/4];
  int i,j,k;
  
  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Put Code here
  printf("(ROW,COL)   ADDRESS   DATA\n");
  printf("-----------------------------\n");
  for (i=0; i<platform.rows; i++) {
    for (j=0; j<platform.cols; j++) {           
      e_read(&dev, i, j, 0x0, &read_buffer, RAM_SIZE);
      for(k=0;k<RAM_SIZE/4;k++){
	printf("(%2d,%2d)     0x%08x   0x%08x\n",i,j,k*4,read_buffer[k]);
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();
  
  return 0;
}
