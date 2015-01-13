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

#define RAM_SIZE     (0x8000)
#define WORD_SIZE    (1)
#define REGS          64
void usage();

int main(int argc, char *argv[]){

  //----------------------------
  e_platform_t platform;
  e_epiphany_t dev;
  e_hal_diag_t e_verbose;
  unsigned int i,j,k,addr;
  unsigned int data;
  int status=1;//pass

  int row0,col0,rows,cols;
  int verbose=0;

  unsigned int high_pattern = 0xaaaaaaaa;
  unsigned int low_pattern  = 0x55555555;

  unsigned int result;

  if (argc < 5){
    usage();
    exit(1);
  }  
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
  }
  //Open
  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  

  printf("-------------------------------------------------------\n");  

  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {   
      printf("Running host register file test for core (%d,%d)\n", i,j);      
      for(k=0;k<REGS;k++){
	addr=0xF0000+k;
	//high pattern
	e_write(&dev, i, j, addr, &high_pattern,  sizeof(int));
	e_read(&dev, i, j, addr, &result, sizeof(int));
	printf("res=%x\n",result);
	if(result!=high_pattern){
	  status=0;
	}
	//low pattern
	e_write(&dev, i, j, addr, &low_pattern,  sizeof(int));
	e_read(&dev, i, j, addr, &result, sizeof(int));
	if(result!=low_pattern){
	  status=0;
	}
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();

  //Self Check
  if(status){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }   
}
//////////////////////////////////////////////////////////////////////////
void usage()
{
  printf("-----------------------------------------------\n");
  printf("Function: Runs exhaustive march-c memory test\n");
  printf("Usage:    e-regfile-test <row> <col> <rows> <cols> \n");
  printf("Example:  e-regfile-test 0 0 4 4 \n");
  printf("\n");
  printf("Options:\n");
  printf("  row     - target core start row coordinate\n");
  printf("  col     - target core start column coordinate\n");
  printf("  rows    - number of rows to test\n");
  printf("  cols    - number of columns to test\n");
  printf("-----------------------------------------------\n");
  return;
}
//////////////////////////////////////////////////////////////////////////

