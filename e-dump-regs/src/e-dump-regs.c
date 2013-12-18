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

#define _BufSize   (1024)
#define _BufOffset (0x01000000)

void usage();

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;
  char emsg[_BufSize];
  

  unsigned int row, col;
  unsigned int data, led_state;
  int i,j;
 
  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Put Code here
  printf("CORE\tCONFIG\t\tSTATUS\t\tPC\t\tDEBUG\t\tIRET\t\tIMASK\t\tILAT\t\tIPEND\n");
  printf("------------------------------------------------------------------");
  printf("------------------------------------------------------------------\n");
  for (i=0; i<platform.rows; i++) {
    for (j=0; j<platform.cols; j++) {     
      printf("(%02d,%02d)\t", i,j);

      e_read(&dev, i, j, 0xf0400, &data, sizeof(unsigned));//config 
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0404, &data, sizeof(unsigned));//status 
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0408, &data, sizeof(unsigned));//pc 
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf040C, &data, sizeof(unsigned));//debug
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0420, &data, sizeof(unsigned));//iret 
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0424, &data, sizeof(unsigned));//imask 
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0428, &data, sizeof(unsigned));//ilat
      printf("0x%08x\t",data);

      e_read(&dev, i, j, 0xf0434, &data, sizeof(unsigned));//ipend 
      printf("0x%08x\t",data);
      printf("\n");
    }
  }

  //Close
  e_close(&dev);
  e_finalize();
  
  return 0;
}
