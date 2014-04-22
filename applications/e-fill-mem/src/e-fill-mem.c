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
#define N          RAM_SIZE/4
void usage();

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;

  unsigned int buffer[N];
  int i,j,k;
  int row0,col0,rows,cols;
  unsigned int pattern;

  if(argc < 2){
    usage();
    return 1;
  }
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
    sscanf(argv[5], "%x", &pattern);
  }
  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);


  //Create array
  for (i=0; i<N; i++){
    buffer[i] = (pattern & 0xFFFF0000)+4*i;
    //printf("%x %x\n",i,buffer[i]);
  }

  //write in buffer
  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {           
      e_write(&dev, i, j, 0x0, &buffer, N*sizeof(int));
    }
  }
  //Close
  e_close(&dev);
  e_finalize();
  
  return 0;
}
void usage(){
  printf("Usage: e-fill-mem <row-start> <col-start> <rows> <cols> 0x<pat> <inc>\n");
  printf("\n");
  printf("<row-start> starting row\n");
  printf("<row-start> starting column\n");
  printf("<rows>      total rows to write to\n");
  printf("<cols>      total cols to write to\n");
  printf("<pat>       32-bit hex pattern starting with 0x\n");
  printf("<inc>       1-->mask <pat> and add byte addess to lower 16 bits\n");
  printf("            0-->write <pat> as is\n");
}
