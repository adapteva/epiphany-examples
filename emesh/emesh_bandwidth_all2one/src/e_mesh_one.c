/*
e_mesh_one.c

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

// This is the DEVICE side of the e_mesh bandwidth example.
// This core is the target core in which the clock cycles will
// be measured. This core will broadcast the signal to all other 
// cores to start transfering at same time.

#include "e_lib.h"

int main(void){

  unsigned time_c, time_p;
  unsigned time;
  unsigned i,j,q,h,m,n,k;
  unsigned *commander;
  unsigned *counter;
  unsigned *mailbox;
  unsigned *n_row, *n_col;
  unsigned *neighbour0, *neighbour1, *neighbour2, *neighbour3, *p;
  
  commander = (unsigned *)0x6100;
  counter   = (unsigned *)0x6300;
  mailbox   = (unsigned *)0x5000;
  n_row     = (unsigned *)0x5100;
  n_col     = (unsigned *)0x5200;
  p         = (unsigned *)0x6100;
  
  // Broadcast to all neighbours
  e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
  neighbour0 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
  
  e_neighbor_id(E_NEXT_CORE, E_ROW_WRAP, n_row, n_col);
  neighbour1 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
  
  e_neighbor_id(E_PREV_CORE, E_COL_WRAP, n_row, n_col);
  neighbour2 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
  
  e_neighbor_id(E_NEXT_CORE, E_COL_WRAP, n_row, n_col);
  neighbour3 = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
  
  // Get core number
  k = e_group_config.core_row * e_group_config.group_cols + e_group_config.core_col;
  
  // Clear the counter of finishing transfering
  for(i=0; i<(e_group_config.group_rows*e_group_config.group_cols); i++){
    counter[i] = 0;
  }
  
  // Initialize the commander and counter for the target core
  counter[k] = 1;
  commander[0] = 0x00000000;
  
  // Set the ctimer
  e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
  
  // Waiting for the signal of starting transfering
  while(commander[0] != (unsigned)0xdeadbeef)
    {};
  
  // Broadcast the signal to neighbours
  neighbour0[0] = 0xdeadbeef;
  neighbour1[0] = 0xdeadbeef;
  neighbour2[0] = 0xdeadbeef;
  neighbour3[0] = 0xdeadbeef;
  
  // Start the ctimer and select the time type
  time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
  
  //BAD! Hard coded for 16 cores! Fix
  while(1){
    if((counter[0]&counter[1]&counter[2]&counter[3]&counter[4]&counter[5]
	&counter[6]&counter[7]&counter[8]&counter[9]&counter[10]&counter[11]
	&counter[12]&counter[13]&counter[14]&counter[15]) != 0){
      time_c = e_ctimer_get(E_CTIMER_0);
      break;
    }	
  }
  
  time = time_p - time_c;	
  mailbox[0] = time;  
  return 0;
}
