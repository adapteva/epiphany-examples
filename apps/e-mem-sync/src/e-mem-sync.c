/*
e-toggle-led.c

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

int main(int argc, char *argv[])
{
  e_platform_t platform;
  e_epiphany_t dev;
  e_mem_t emem;
  
  unsigned int row, col;
  unsigned int sync_data,poll_data;
  int i,j;
 
  // initialize system, read platform params from
  // default HDF. Then, reset the platform and
  // get the actual system parameters.

  e_set_host_verbosity(H_D0);
  e_init(NULL);
  e_get_platform_info(&platform);
  
  // Allocate a buffer in shared external memory
  // for message passing from eCore to host.
  e_alloc(&emem, 0x0, 128);	
  
  // Open a workgroup
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //Read data from sync location
  e_read(&dev, 0, 0, 0x6000, &sync_data, sizeof(sync_data));
  //Inverting data
  sync_data=sync_data ^ 0xFFFFFF;
  //Write inverted data back to location
  e_write(&dev, 0, 0, 0x6000, &sync_data, sizeof(sync_data));
  
  while(1){
    e_read(&dev, 0, 0, 0x6000, &poll_data, sizeof(sync_data));
    if(sync_data==poll_data){
      printf("All reads/writes to core 0,0 have completed at this point!\n");
      break;
    }
  }
  
  // Close the workgroup
  e_close(&dev);
  
  // Release the allocated buffer and finalize the
  e_free(&emem);
  e_finalize();
  
  return EXIT_SUCCESS;
}

 
