/*
e-reset.c

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
  unsigned int data,stage;
  int i,j;
  int row,col;
  
  if(argc < 2){
    usage();
    exit;
  }
  else{
    stage = atoi(argv[1]);
  }

  //Open device
  //e_set_loader_verbosity(H_D2);
  e_init(NULL);
  e_get_platform_info(&platform);
  e_open(&dev,0, 0, platform.rows, platform.cols);
  //Reset the system
  e_reset_system();
  
  //---------------------------------------
  //Shut down link (NORTH==0,2)
  if(stage>0){
    row=0;
    col=2;    
    ee_write_esys(E_SYS_CONFIG, 0x10000000);
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }
  //---------------------------------------
  //Shut down south link (SOUTH==7,2)
  if(stage>1){   
    col=2;
    if ((dev.type == E_E64G401)){
      row=7;
    }
    else{
      row=3;
    }
    ee_write_esys(E_SYS_CONFIG, 0x90000000);
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }
  //---------------------------------------
  //Shut down west link (WEST==2,0)
  if(stage>2){
    row=2;
    col=0;
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }
  //---------------------------------------
  if(stage>4){
    //Configuring clock divider(EAST==2,7)
    row=2;
    if ((dev.type == E_E64G401)){
      col=7;
    }
    else{
      col=3;
    }
    ee_write_esys(E_SYS_CONFIG, 0x50000000);
    //Change clock divider to solve FPGA receiver speed path
    data = 0x1;
    e_write(&dev, row, col, 0xf0300, &data, sizeof(int));
    //Up the current drive on the wait signal
    //data = 0x02000000;
    //e_write(&dev, 0, 0, 0xf0304, &data, sizeof(int));
    //Return to normal mode
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
    
  }
  if(stage>3){
    //Enable clock gating
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
  if(stage>5){
    //Enable timeout
    ee_write_esys(E_SYS_CONFIG, 0x00000001);
  }
  //-------------------------------------------------------
  e_close(&dev);
  e_finalize();  
  return 0;
}

void usage(){
  printf("Usage: e-init <stage>\n");
  printf("<stage>:\n");  
  printf(" 0 = Reset the Epiphany\n");
  printf(" 1 = #0 + power down north link\n");
  printf(" 2 = #1 + power down west link\n");
  printf(" 3 = #2 + power down south link\n");
  printf(" 4 = #3 + enable clock gating\n");
  printf(" 5 = #4 + set east tx link to divide by 4\n");
  printf(" 6 = #5 + enable FPGA read timeout\n");
}
