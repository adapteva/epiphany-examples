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


void usage();

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;

  unsigned int row, col;
  unsigned int data;
  int row0,col0,rows,cols;
  int i,j;
 
  if(argc < 2){
    usage();
    return EXIT_FAILURE;
  }
  else{
    row0  = atoi(argv[1]);
    col0  = atoi(argv[2]);
    rows  = atoi(argv[3]);
    cols  = atoi(argv[4]);
  }

  //Open
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  //e_set_loader_verbosity(L_D3);
  //Put Code here
  printf("CORE  CONFIG      STATUS      PC          CTIMER0     CTIMER1     DMA0STATUS  DMA1STATUS  DEBUG   IRET    IMASK    ILAT    IPEND\n");
  printf("--------------------------------------------------------------------------------------------------------------------------------------------\n");
  for (i=row0; i<rows; i++) {
    for (j=col0; j<cols; j++) {     
      printf("%02d%02d  ", i,j);
      
      e_read(&dev, i, j, 0xf0400, &data, sizeof(unsigned));//config 
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf0404, &data, sizeof(unsigned));//status 
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf0408, &data, sizeof(unsigned));//pc 
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf0438, &data, sizeof(unsigned));//ctimer0
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf043C, &data, sizeof(unsigned));//ctimer1
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf051C, &data, sizeof(unsigned));//dam0status
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf053C, &data, sizeof(unsigned));//dma1status
      printf("0x%08x  ",data);

      e_read(&dev, i, j, 0xf040C, &data, sizeof(unsigned));//debug
      printf("0x%04x  ",data);

      e_read(&dev, i, j, 0xf0420, &data, sizeof(unsigned));//iret 
      printf("0x%04x  ",data);

      e_read(&dev, i, j, 0xf0424, &data, sizeof(unsigned));//imask 
      printf("0x%04x  ",data);

      e_read(&dev, i, j, 0xf0428, &data, sizeof(unsigned));//ilat
      printf("0x%04x  ",data);

      e_read(&dev, i, j, 0xf0434, &data, sizeof(unsigned));//ipend 
      printf("0x%04x  ",data);
      printf("\n");
    }
  }

  //Close
  e_close(&dev);
  e_finalize();
  
  return EXIT_SUCCESS;
}

void usage()
{
  printf("---------------------------------------------------\n");
  printf("Function: Dumps important register values to STDIO\n");
  printf("Usage:    e-dump-regs <row> <col> <rows> <cols>\n");
  printf("Example:  e-dump-regs 0 0 4 4\n");
  printf("\n");
  printf("Options:\n");
  printf("  row   - target core row coordinate\n");
  printf("  col   - target core column coordinate\n");
  printf("  rows  - number of rows to dump\n");
  printf("  cols  - number of columns to dump\n");
  printf("---------------------------------------------------\n");
  return;

}
