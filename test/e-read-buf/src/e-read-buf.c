/*
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

#define RAM_SIZE   (0x8000)

int main(int argc, char *argv[]){

  unsigned int read_buffer[RAM_SIZE/4];
  e_epiphany_t dev; 
  int k;

  //Open
  e_init(NULL);
  e_reset_system();
  e_open(&dev, 0, 0, 1, 1);
  e_read(&dev, 0, 0, 0x0, &read_buffer, RAM_SIZE);
  for(k=0;k<RAM_SIZE/4;k++){
    printf("0x%08x   0x%08x\n",k*4,read_buffer[k]);
  }
  e_close(&dev);
  e_finalize();  
  return 0;
}
