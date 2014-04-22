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

  
  if(argc < 2){
    usage();
    exit;
  }
  else{
    stage = atoi(argv[1]);
  }

  //Open device
  e_init(NULL);
  e_get_platform_info(&platform);
  //Reset the system
  e_reset_system();

  
  
  //---------------------------------------
  //Shut down link (NORTH==0,2)
  if(stage>0){
    e_open(&dev,0, 2, 1, 1);
    ee_write_esys(E_SYS_CONFIG, 0x10000000);
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
    e_close(&dev);
  }
  //---------------------------------------
  //Shut down south link (SOUTH==7,2)
  if(stage>1){
    e_open(&dev,7, 2, 1, 1);
    ee_write_esys(E_SYS_CONFIG, 0x90000000);
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
    e_close(&dev);
  }
  //---------------------------------------
  //Shut down link (WEST==2,0)
  if(stage>2){
    e_open(&dev,2, 0, 1, 1);
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0304, &data, sizeof(int));  
    
    data = 0x000000FFF;
    e_write(&dev, 0, 0, 0xf0308, &data, sizeof(int));  
    
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
    e_close(&dev);
  }
  //---------------------------------------
  if(stage>3){
    //Change clock divider (EAST==2,7)
    e_open(&dev,2, 7, 1, 1);
    ee_write_esys(E_SYS_CONFIG, 0x50000000);
    data = 0x1;
    e_write(&dev, 0, 0, 0xf0300, &data, sizeof(int));
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
    e_close(&dev);
  }
  if(stage>4){
    //Enable timeout
    ee_write_esys(E_SYS_CONFIG, 0x00000001);
  }
  //-------------------------------------------------------
  e_finalize();  
  return 0;
}

void usage(){
  printf("Usage: e-init <stage>\n");
  printf("<stage>:\n");  
  printf(" 1 = reset & (power-down north)\n");
  printf(" 2 = #1 + power-down west\n");
  printf(" 3 = #2 + power-down south\n");
  printf(" 4 = #3 + set east tx link to divide by 4\n");
  printf(" 5 = #4 + enable FPGA read timeout\n");
}
