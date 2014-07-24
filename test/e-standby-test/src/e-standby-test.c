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

void usage();
int my_reset_system();

int main(int argc, char *argv[]){

  //----------------------------
  e_platform_t platform;
  e_epiphany_t dev;
  e_loader_diag_t e_verbose;
  unsigned int i,j,k,addr;
  unsigned int data;
  int status=1;//pass

  int row0,col0,rows,cols;
  int verbose=0;


 
  //Open
  e_init(NULL);

  //Power down function
  my_reset_system();
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
 
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
int my_reset_system()
{
  unsigned int row,col,i,j,data;
  e_epiphany_t dev;
  e_platform_t platform;


  e_init(NULL);
  e_get_platform_info(&platform);
  ee_write_esys(E_SYS_RESET, 0);//reset
  usleep(200000);
  
  //Open all cores
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //shut down north link
  if(1){
      row=0;
      col=2;

      ee_write_esys(E_SYS_CONFIG, 0x10000000);
      data = 0x000000FFF;
      e_write(&dev, row, col, 0xf0304, &data, sizeof(int));  
      data = 0x000000FFF;
      e_write(&dev, row, col, 0xf0308, &data, sizeof(int));  
      ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }
  
  //Shut down west link (WEST==2,0)
  if(1){
    row=2;
    col=0;
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));      
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

  //Shut down south link (SOUTH==7,2)
  if(1){
    if ((dev.type == E_E64G401)){
      row=7;
      col=2;
    }
    else{
      row=3;
      col=2;
    }

    ee_write_esys(E_SYS_CONFIG, 0x90000000);    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));      
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

   //Change elink clock divider (temporary workaround due to FPGA timing issue)
  if(1){
    //east link register is in a different place in e64
    if ((dev.type == E_E64G401)){
      row=2;
      col=7;
    }
    else{
      row=2;
      col=3;
    }
    //Writing to the east ELINK transmit config register
    ee_write_esys(E_SYS_CONFIG, 0x50000000);
    data = 0x1;
    e_write(&dev, row, col, 0xf0300, &data, sizeof(int));
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }  

 //Reset chip one more time (west side))
  if(0){
    row=2;
    col=0;
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);    
    data = 0x000000001;
    e_write(&dev, row, col, 0xf0324, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

  //Enable Clock Gating
  if(1){
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {
  	//eCore clock gating
	data=0x00400000;
	e_write(&dev, i, j, 0xf0400, &data, sizeof(data));
	//eMesh clock gating
	data=0x00000002;
	e_write(&dev, i, j, 0xf0700, &data, sizeof(data));
      }
    }
  }  

 
  //Close down device
  e_close(&dev);
  return E_OK;
}
