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
  unsigned int read_data;
  unsigned int i,j;
  int row0,col0,rows,cols,slow;
  unsigned addr,k;

  if(argc < 2){
    usage();
    return EXIT_FAILURE;
  }
  else{
    row0  = atoi(argv[1]);
    col0  = atoi(argv[2]);
    rows  = atoi(argv[3]);
    cols  = atoi(argv[4]);
    slow  = 0;//atoi(argv[5]);
  }
  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Put Code here
  printf("(ROW,COL)   ADDRESS   DATA\n");
  printf("-----------------------------\n");
  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {           
      if(slow>0){
	for(k=0;k<RAM_SIZE/4;k++){
	  addr=4*k;
	  e_read(&dev, i, j, addr, &read_data, sizeof(int));
	  read_buffer[k]=read_data;
	  //printf("addr=%x data=%x\n",addr,read_data);
	}
      }
      else{
	e_read(&dev, i, j, 0x0, &read_buffer, RAM_SIZE);
      }
      for(k=0;k<RAM_SIZE/4;k++){
	printf("(%2d,%2d)     0x%08x   0x%08x\n",i,j,k*4,read_buffer[k]);
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();
  
  //Always return sucess if it runs to completion
  return EXIT_SUCCESS;
}
void usage()
{
  printf("-----------------------------------------------\n");
  printf("Function: Dumps SRAM locations to STDIO\n");
  printf("\n");
  printf("Usage:    e-dump-mem <row> <col> <rows> <cols>\n");
  printf("\n");
  printf("Options:\n");
  printf("  row     - target core row coordinate\n");
  printf("  col     - target core column coordinate\n");
  printf("  rows    - number of rows to dump\n");
  printf("  cols    - number of columns to dump\n");
  printf("\n");
  printf("Example:  e-dump-mem 0 0 4 4\n");
  printf("-----------------------------------------------\n");
  return;

}
