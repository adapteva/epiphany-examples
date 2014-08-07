/*

Copyright (C) 2012 Adapteva, Inc.
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

/*

The purpose of the program is to demonstrate simple parallel programming 
example for the Parallella demonstrating how to do fork-join 
parallel processing using the Epiphany library. Similar type programs can be 
constructed with OpenCL, OpenMP, and many other parallel programming frameworks.


The main.c program distributes N tasks to N Epiphany cores in parallel and
aggregates the result after all cores have completed to produce the final 
result.
   
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include <e-loader.h>
#include "common.h"

void e_check_test(void *dev, unsigned row, unsigned col, int *status);

int main(int argc, char *argv[]){
  e_loader_diag_t e_verbose;
  e_platform_t platform;
  e_epiphany_t dev;
  int status=1;//pass
  char elfFile[4096];
  //e_set_loader_verbosity(L_D3);

  //Gets ELF file name from command line
  strcpy(elfFile, argv[1]);

  //Initalize Epiphany device
  e_init(NULL);                      
  e_reset_system();
  e_get_platform_info(&platform);                          
  e_open(&dev, 0, 0, 1, 1); //open core 0,0


  //Load program to cores and run
  e_load_group(elfFile, &dev, 0, 0, 1, 1, E_TRUE);
  e_check_test(&dev, 0, 0, &status);

  
  //Close down Epiphany device
  e_close(&dev);
  e_finalize();
  
  //self check
  if(status){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }   

}
/**********************************************************************/
void e_check_test( void *dev, 
		   unsigned row, 
		   unsigned col, 
		   int *status 
		  ){

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
      unsigned clr= ( unsigned ) 0x0;
      e_write(dev,row, col, 0x24, &clr, sizeof(clr));
      break;
    }
    else{
      if(wait){
	usleep(1000000);      
	printf("...waiting for core (%d,%d)\n",row,col);
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
