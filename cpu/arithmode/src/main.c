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
#include <e-hal.h>
#include "common.h"

int main(int argc, char *argv[]){
  e_platform_t platform;
  e_epiphany_t dev;

  int a[N], b[N], c[CORES];
  int done[CORES],all_done;
  int sop;
  int i,j;
  unsigned clr;
  clr = (unsigned)0x00000000;

  //Initalize Epiphany device
  e_init(NULL);                      
  e_reset_system();                                      //reset Epiphany
  e_get_platform_info(&platform);                          
  e_open(&dev, 0, 0, platform.rows, platform.cols); //open all cores

  //Load program to cores and run
  e_load_group("e_task.elf", &dev, 0, 0, platform.rows, platform.cols, E_TRUE);
    
  //Close down Epiphany device
  e_close(&dev);
  e_finalize();
  
  return 0;
}

