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
  volatile unsigned *addr;

  /////////////////////////////////////////////////
  //North-Link
  e_reg_write(E_REG_CONFIG, (unsigned) 0x00001000); 

  addr=(unsigned *) (0x80AF0308);
  (*(addr)) = 0x00000FFF;
  addr=(unsigned *) (0x80AF0304);
  (*(addr)) = 0x00000FFF;

  e_reg_write(E_REG_CONFIG, (unsigned) 0x00400000); 

  /////////////////////////////////////////////////
  //South-Link
  e_reg_write(E_REG_CONFIG, (unsigned) 0x00009000);

  addr=(unsigned *) (0x8CAF0308);
  (*(addr)) = 0x00000FFF;
  addr=(unsigned *) (0x8CAF0304);
  (*(addr)) = 0x00000FFF;

  e_reg_write(E_REG_CONFIG, (unsigned) 0x00400000);
  
  /////////////////////////////////////////////////
  //West-Link
  e_reg_write(E_REG_CONFIG, (unsigned) 0x0000d000);

  addr=(unsigned *) (0x888F0308);
  (*(addr)) = 0x00000FFF;
  addr=(unsigned *) (0x888F0304);
  (*(addr)) = 0x00000FFF;

  e_reg_write(E_REG_CONFIG, (unsigned) 0x00400000);

}
