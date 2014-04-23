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
int  my_reset_system();
void usage();

int main(int argc, char *argv[]){

  unsigned int stage;

  if (argc < 2){
    usage();
    exit(1);
  }  
  else{
    stage = atoi(argv[1]);
  }

  //----------------------------
  e_platform_t platform;
  e_epiphany_t dev;
  unsigned int result;
  unsigned int i,j,k;
  unsigned int rows,cols;
  unsigned int data;
  int status=1;//pass
  unsigned addr;
  unsigned int read_data;

  unsigned int read_buffer[RAM_SIZE/4];
  unsigned int write_55_buffer[RAM_SIZE/4];
  unsigned int write_aa_buffer[RAM_SIZE/4];

  //Open
  e_loader_diag_t verbose;
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  //e_set_loader_verbosity(L_D3);
  rows=platform.rows;
  cols=platform.cols;

  printf("-------------------------------------------------------\n");
  //##############################
  //1. Simple 32 Bit Memory Test
  //##############################
  if(stage==0 || stage==1 ){
    my_reset_system();
    printf("***Running simple memory test for all cores***\n");

    e_load_group("bin/test_memory_simple.srec", &dev, 0, 0, rows, cols, E_TRUE);
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //2. March Memory Test
  //##############################
  if(stage==0 || stage==2 ){
    my_reset_system();
    printf("***Running march-C memory test for all cores***\n");  
    //Running test, all in parallel
    for (i=0; i<rows; i=i+4) {
      for (j=0; j<cols; j=j+4) {   
	e_load_group("bin/test_memory_march.srec", &dev, i, j, 1, 1, E_TRUE); 
      }
    }
    //Checking results one by one
    for (i=0; i<rows; i=i+4){
      for (j=0; j<cols; j=j+4){   
	e_check_test(&dev, i, j, &status);  
      }
    }
  }
  //##############################
  //3. Read/Write Test from Host
  //##############################
  if(stage==0 || stage==3 ){
    my_reset_system();    
    printf("***Running host read/write test for all cores***\n");  

    //Create write values
    for(k=0;k<RAM_SIZE/4;k++){
      write_55_buffer[k]=0x55555555;
    }
    for(k=0;k<RAM_SIZE/4;k++){
      write_aa_buffer[k]=0xaaaaaaaa;
    }
    for (i=0; i<rows; i++) {
      for (j=0; j<cols; j++) {   
	//Write 555 buffer
	e_write(&dev, i, j, 0x0, &write_55_buffer, RAM_SIZE);
	//Read back one word at a time to overcome 64-core row 1-3 issue
	//Note! Temporary
	for(k=0;k<RAM_SIZE/4;k++){
	  addr=4*k;
	  e_read(&dev, i, j, addr, &read_data, sizeof(int));
          read_buffer[k]=read_data;
	}
	//Comparing result
	for(k=0;k<RAM_SIZE/4;k=k+1){
	  if(read_buffer[k]!=write_55_buffer[k]){
	    printf("FAIL (%d,%d) k=%d data=%x\n",i,j,k,read_buffer[k]);
	    status=0;
	  }
	}
	//Write AAA buffer
	e_write(&dev, i, j, 0x0, &write_aa_buffer, RAM_SIZE);
	for(k=0;k<RAM_SIZE/4;k++){
	  addr=4*k;
	  e_read(&dev, i, j, addr, &read_data, sizeof(int));
          read_buffer[k]=read_data;
	}
	for(k=0;k<RAM_SIZE/4;k=k+1){
	  if(read_buffer[k]!=write_aa_buffer[k]){
	    printf("FAIL (%d,%d) k=%d data=%x\n",i,j,k,read_buffer[k]);
	    status=0;
	  }
	}
      }
    }
  }
  //#################################
  //4. DRAM Read/Write Test from Core
  //#################################
  if(stage==0 || stage==4 ){
    my_reset_system();
    printf("***Running DRAM read/write test for all cores***\n");
    
    //Testing row 0
    e_load_group("bin/test_memory_dram.srec", &dev, 0, 0, 1, cols, E_TRUE);
    //Skipping row 1-2 for e64
    if ((dev.type == E_E64G401)){
      printf("(Note: skipping row 1-2 for E64G401)\n");
    }
    else{
      e_load_group("bin/test_memory_dram.srec", &dev, 1, 0, 2, cols, E_TRUE);
    }
    e_load_group("bin/test_memory_dram.srec", &dev, 3, 0, (rows-3), cols, E_TRUE);
    
    //Check (0,0)--(0,7)
    for (i=0; i<1; i++) {
      for (j=0; j<cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
    //Check (1,0)--(2,7) only if ! E64
    if (!(dev.type == E_E64G401)){
      for (i=1; i<3; i++) {
	for (j=0; j<cols; j++) {           
	  e_check_test(&dev, i, j, &status);
	}
      }
    }
    //Checking (3,0)-->(rows,cols)
    for (i=3; i<rows; i++) {
      for (j=0; j<cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //5. EMESH Test
  //##############################
  if(stage==0 || stage==5 ){
    my_reset_system();
    printf("***Running emesh test for all cores***\n");

    e_load_group("bin/test_emesh.srec", &dev, 0, 0, rows, cols, E_TRUE);
    for (i=0; i<rows; i++) {
      for (j=0; j<cols; j++) {           
	e_check_test(&dev, i, j, &status);
      }
    }
  }
  //##############################
  //6. Simple Per Core Matmul Test
  //##############################
  if(stage==0 || stage==6 ){
    my_reset_system();
    printf("***Running simple matmul test for all cores***\n");

    e_load_group("bin/test_matmul.srec", &dev, 0, 0, rows, cols, E_TRUE);
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
  char chip[32];
  if(status>0){
    //Hack, can't make sense of hal data structure...
    if ((dev.type == E_E64G401)){
      strcpy(chip,"E64G401");
    }
    else{
      strcpy(chip,"E16G301");
    }
    printf("GOOD: %s PASSED SELF-TEST!\n",chip);
  }
  else{
    printf("BAD: CHIP FAILED!!!!\n");
  }
  printf("-------------------------------------------------------\n");    

  //Close
  e_close(&dev);
  e_finalize();
  return ~status;
}
//////////////////////////////////////////////////////////////////////////
void e_check_test(void *dev, unsigned row, unsigned col, int *status){
  unsigned int result;
  int wait=1;
  while(1){
    e_read(dev,row, col, 0x24, &result, sizeof(unsigned));
    if(result==0xDEADBEEF){
      printf("FAILED for core (%d,%d)\n",row,col);
      *status=0;
      break;
    }
    else if(result==0x12345678){
      //printf("PASSED for core (%d,%d)\n",row,col);
      unsigned clr= ( unsigned ) 0x0;
      e_write(dev,row, col, 0x24, &clr, sizeof(clr));
      break;
    }
    else{
      if(wait){
	usleep(1000000);      
	printf("waiting for core (%d,%d)\n",row,col);
	wait=0;
      }
      else{
	printf("FAILED for core (%d,%d)\n",row,col);
	*status=0;
	break;
      }
    }
  }		  
}

int my_reset_system()
{

  e_epiphany_t dev;
  e_platform_t platform;
  e_init(NULL);
  e_get_platform_info(&platform);
  ee_write_esys(E_SYS_RESET, 0);
  usleep(200000);
  
  //Change elink clock divider (temporary workaround due to FPGA timing issue)
  
  unsigned int data,col;

  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Setting transaction control mode in FPGA
  ee_write_esys(E_SYS_CONFIG, 0x50000000);
  data = 0x1;

  //east link register is in a different place in e64
  if ((dev.type == E_E64G401)){
    col=7;
  }
  else{
    col=3;
  }
  //Writing to the ELINK transmit config register
  e_write(&dev, 2, col, 0xf0300, &data, sizeof(int));
  ee_write_esys(E_SYS_CONFIG, 0x00000000);
  e_close(&dev);
  return E_OK;
}

void usage(){
  printf("Usage: e-init <stage>\n");
  printf("<stage>:\n");  
  printf(" 0 = run all tests)\n");
  printf(" 1 = simple memory test\n");
  printf(" 2 = march-c memory test\n");
  printf(" 3 = host read/write test\n");
  printf(" 4 = dram test\n");
  printf(" 5 = emesh test\n");
  printf(" 6 = matmul test\n");
}
