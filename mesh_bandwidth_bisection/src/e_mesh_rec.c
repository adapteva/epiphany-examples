#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void)
{
	unsigned time_c, time_p;
	unsigned time;
	unsigned i,j,q,h,m,n,k;
	unsigned *commander;
	unsigned *counter;
	unsigned *mailbox;
	unsigned *n_row, *n_col, *p, *p1;
	n_row = (unsigned *)0x6000;
	n_col = (unsigned *)0x6004;
	p = (unsigned *)0x6100;
	p1 = (unsigned *)0x6300;
	
	// Define the mailbox
	e_neighbor_id(E_PREV_CORE, E_ROW_WRAP, n_row, n_col);
	commander = (unsigned *) e_get_global_address(*n_row, *n_col, p) ;
	counter = (unsigned *) e_get_global_address(*n_row, *n_col, p1) ;
	mailbox = (unsigned *)0x5000;
	// Get core information
	k = e_group_config.core_row * e_group_config.group_cols + e_group_config.core_col;
	
	// Set the ctimer
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX) ;
			
	// Waiting for the signal to start transfering
	while(commander[0] != (unsigned)0xdeadbeef)
	{};
	
	// Start the ctimer and select the time type
	time_p = e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	
	while(1)
	{
		if(counter[0]!= 0)
		{
			time_c = e_ctimer_get(E_CTIMER_0);
			break;
		}	
	}
	
	time = time_p - time_c;	
	mailbox[0] = time;
	
	return 0;
}
