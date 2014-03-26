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

#define RAM_SIZE (0x8000)

void e_check_test(void *dev, unsigned row, unsigned col, int *status);

int main(int argc, char *argv[]){

  e_platform_t platform;
  e_epiphany_t dev;
  unsigned int result;
  unsigned int i,j,k;
  unsigned int rows,cols;
  unsigned int data;
  int status=1;//pass

  unsigned int read_buffer[RAM_SIZE/4];
  unsigned int write_55_buffer[RAM_SIZE/4];
  unsigned int write_aa_buffer[RAM_SIZE/4];

  //Open
  e_loader_diag_t verbose;
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  //e_set_loader_verbosity(L_D3);

  printf("-------------------------------------------------------\n");
  //##############################
  //1. Simple 32 Bit Memory Test
  //##############################
  if(1){
    e_reset_system();
    printf("***Running Simple Memory Test***\n");
    i=0;
    j=0;
    rows=platform.rows;
    cols=platform.cols;
    e_load_group("bin/test_memory_simple.srec", &dev, i, j, rows, cols, E_TRUE);
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //2. March Memory Test
  //##############################
  if(1){
    e_reset_system();
    printf("***Running March-C Memory Test***\n");  
    //Loading all four quadrants (speedup)
    e_load_group("bin/test_memory_march.srec", &dev, 0, 0, 1, 1, E_TRUE);  
    e_load_group("bin/test_memory_march.srec", &dev, 0, 2, 1, 1, E_TRUE);  
    e_load_group("bin/test_memory_march.srec", &dev, 2, 0, 1, 1, E_TRUE);  
    e_load_group("bin/test_memory_march.srec", &dev, 2, 2, 1, 1, E_TRUE);  
    //Checking all four quadrants
    e_check_test(&dev, 0, 0, &status);  
    e_check_test(&dev, 0, 2, &status);  
    e_check_test(&dev, 2, 0, &status);  
    e_check_test(&dev, 2, 2, &status);  
  }
  //##############################
  //3. Read/Write Test from Host
  //##############################
  if(1){
    //Create write values
    for(k=0;k<RAM_SIZE/4;k++){
      write_55_buffer[k]=0x55555555;
    }
    for(k=0;k<RAM_SIZE/4;k++){
      write_aa_buffer[k]=0xaaaaaaaa;
    }
    e_reset_system();
    printf("***Running Host Read/Write Test***\n");  
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {   
	e_write(&dev, i, j, 0x0, &write_55_buffer, RAM_SIZE);
	e_read(&dev, i, j,  0x0, &read_buffer, RAM_SIZE);	
	for(k=0;k<RAM_SIZE/4;k=k+1){
	  if(read_buffer[k]!=write_55_buffer[k]){
	    printf("FAIL (%d,%d) k=%d data=%x\n",i,j,k,read_buffer[k]);
	    status=0;
	  }
	}
	e_write(&dev, i, j, 0x0, &write_aa_buffer, RAM_SIZE);
	e_read(&dev, i, j, 0x0, &read_buffer, RAM_SIZE);
	for(k=0;k<RAM_SIZE/4;k=k+1){
	  if(read_buffer[k]!=write_aa_buffer[k]){
	    printf("FAIL (%d,%d) k=%d data=%x\n",i,j,k,read_buffer[k]);
	    status=0;
	  }
	}
      }
    }
  }
  //##############################
  //4. DRAM Read/Write Test from Core
  //##############################
  if(1){
    e_reset_system();
    printf("***Running DRAM Read/Write Test***\n");
    i=0;
    j=0;
    rows=platform.rows;
    cols=platform.cols;
    e_load_group("bin/test_memory_dram.srec", &dev, i, j, rows, cols, E_TRUE);
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }

  //##############################
  //5. EMESH Test
  //##############################
  if(1){
    e_reset_system();
    printf("***Running Emesh Test***\n");
    i=0;
    j=0;
    rows=platform.rows;
    cols=platform.cols;
    e_load_group("bin/test_emesh.srec", &dev, i, j, rows, cols, E_TRUE);
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //6. Simple Per Core Matmul Test
  //##############################
  if(1){
    e_reset_system();
    printf("***Running Simple Matmul Test***\n");
    i=0;
    j=0;
    rows=platform.rows;
    cols=platform.cols;
    e_load_group("bin/test_matmul.srec", &dev, i, j, rows, cols, E_TRUE);
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //Max Power Test
  //##############################
  //1.Convoluation kernel
  //2.DMA channel 0 on chip
  //3.DMA channel 1 off chip

  //##############################
  //Final Check
  //##############################
  printf("-------------------------------------------------------\n");
  if(status>0){
    printf("GOOD: CHIP PASSED SELF-TEST!\n");
  }
  else{
    printf("BAD: CHIP FAILED!!!!\n");
  }
  printf("-------------------------------------------------------\n");    

  //Close
  e_close(&dev);
  e_finalize();
  return 0;
}
//////////////////////////////////////////////////////////////////////////
void e_check_test(void *dev, unsigned row, unsigned col, int *status){
  unsigned int result;
  while(1){
    e_read(dev,row, col, 0x24, &result, sizeof(unsigned));
    if(result==0xDEADBEEF){
      printf("FAILED for core (%d,%d)\n",row,col);
      status=0;
      break;
    }
    else if(result==0x12345678){
      //printf("PASSED\n");
      unsigned clr= ( unsigned ) 0x0;
      e_write(dev,row, col, 0x24, &clr, sizeof(clr));
      break;
    }
    else{
      usleep(1000000);
      //printf("waiting for core (%d,%d)\n",row,col);
    }
  }		  
}
