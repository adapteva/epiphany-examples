/*
  e-lowpower-mode.c
  
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void)
{
  e_coreid_t coreid;
  unsigned *addr;
  unsigned int led_state;


  //Get Value of LED State
  addr=(unsigned *) (0x6000);
  led_state =  (*(addr));

  //Set "direct north routing mode to access north IO registers
  e_reg_write(E_REG_CONFIG, (unsigned) 0x00001000); 

  //Write to north IO registers
  addr=(unsigned *) (0x80AF030c);
  (*(addr)) = 0x03FFFFFF;

  addr=(unsigned *) (0x80AF0318);    
  (*(addr)) = ~led_state & 0x00000001;
  //(*(addr)) = 0x00000000;

  //Set config register back to normal
  e_reg_write(E_REG_CONFIG, (unsigned) 0x00400000); 

}
