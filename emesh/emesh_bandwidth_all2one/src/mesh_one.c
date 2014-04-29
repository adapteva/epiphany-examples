/*
mesh_one.c

Copyright (C) 2012 Adapteva, Inc.
Contributed by Wenlin Song <wsong@wpi.edu>

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

// This is the HOST side of the e_mesh bandwidth example
// In this test, the host selects one core to be the target 
// core and make all other cores write to this core. Then
// use the C_TIMER_0 register to measure the time of transfering


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define mas_row (0)
#define mas_col (0)

int main(int argc, char *argv[])
{
  unsigned row, col, coreid, i, j, m, n, k;
  e_platform_t platform;
  e_epiphany_t dev;
  unsigned time;
  unsigned signal = 0xdeadbeef;
  unsigned clr = 0x00000000;
  unsigned master_row, master_col;
  master_row = mas_row;
  master_col = mas_col;
  float result;
  srand(1);
  
  
  // initialize system, read platform params from
  // default HDF. Then, reset the platform and
  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);
  
  // Open a workgroup
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  
  // Load the device program onto target core
  e_load("e_mesh_one.srec", &dev, mas_row, mas_col, E_TRUE);
  
  // Let other cores know the core id of the target core
  for(i=0; i<platform.rows; i++){
    for(j=0; j<platform.cols; j++){
      e_write(&dev, i, j, 0x6000, &master_row, sizeof(master_row));
      e_write(&dev, i, j, 0x6004, &master_col, sizeof(master_col));
    }
  }
  
  // Load the device program onto all cores except for target core
  for (i=0; i<platform.rows; i++){
    for(j=0; j<platform.cols; j++){
      if((i!=mas_row)|(j!=mas_col)){
	e_load("e_mesh_one1.srec",&dev, i, j, E_TRUE);
      }
    }
  }
  
  usleep(10000);
  
  // Sent the signal to start transfer
  
  e_write(&dev, mas_row, mas_col, 0x6100, &signal, sizeof(signal));
  
  // Wait for cores to run
  usleep(1000000);
  
  // Read message from target core
  e_read(&dev, mas_row, mas_col, 0x5000, &time, sizeof(time));

  // Close the workgroup
  e_close(&dev); 
  e_finalize();
  
  // Calculate the bandwidth
  result = (120*585938)/(time);
  
  printf("The bandwidth of all-to-one is %.2fMB/s!\n", result);		  
  if(result > 1000){
    return EXIT_SUCCESS;    
  }
  else{
    return EXIT_FAILURE;    
  }
}
