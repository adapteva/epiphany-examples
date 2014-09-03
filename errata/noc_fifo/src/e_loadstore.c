/*
Copyright (C) 2012 Adapteva, Inc.
Contributed by Andreas Olofsson <andreas@adapteva.com>

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
  unsigned time, time_c, time_p;
  
  unsigned i,j;
  unsigned volatile *mailbox,*buffer;
  unsigned volatile tmp;
  mailbox = (unsigned *)0x6000;

  mailbox[0]=0;
  mailbox[1]=0;
  for(i=0;i<2;i++){	
    //Writing to local memory vs neighbor core
    if(i<1){
      buffer = (unsigned *) 0x7000;
    }
    else{
      buffer = (unsigned *) 0x80907000;
    }
    // Set the ctimer
    e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;		 
    
    // Start the ctimer
    time_p = e_ctimer_start(E_CTIMER_0, 0x1);
    
    for(j=0;j<10;j++){
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678; 
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
      *buffer= 0x12345678;      
    }
    
    // Get the time
    time_c = e_ctimer_get(E_CTIMER_0);
    time = time_p - time_c;		
    
    // Output the result in mail box
    mailbox[i] = time;
  }
  
  return 0;
}
