#include <stdio.h>
#include <stdlib.h>
#include <e_lib.h>

#define N 1026
#define ctype E_CTIMER_CLK

int A[N], B[N];
extern int A[N], B[N];
void init_array();
int comloop();
int hwloop(int);



int sfloop(int);

int main(void) {
	int *result;
	unsigned *time;
	unsigned time_s, time_e;
	unsigned ctimer;
	int row, col;

	init_array();
	result = (unsigned *)0x5100;
	time   = (unsigned *)0x5200;
	//sprintf(outbuf,"\t");

	row = e_group_config.core_row;
	col = e_group_config.core_col;
	ctimer = (unsigned)e_get_global_address(row,col,(void *)0xf0438);

	result[4] = 0x0;

	//test the comloop
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	e_ctimer_start(E_CTIMER_0, ctype);
	__asm__ __volatile__("ldr %0, [%1]":"=r"(time_s):"r"(ctimer):);


	result[0] = comloop();


	__asm__ __volatile__("ldr %0, [%1]":"=r"(time_e):"r"(ctimer):);
	e_ctimer_stop(E_CTIMER_0);
	time[0] = time_s - time_e;


	//test the hwloop
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	time_s = e_ctimer_start(E_CTIMER_0, ctype);
	result[1] = hwloop(1024);
	time_e = e_ctimer_stop(E_CTIMER_0);
	time[1] = time_s - time_e;

	time_s = e_ctimer_start(E_CTIMER_0, ctype);
	
	hwloop(2048);
	time_e = e_ctimer_stop(E_CTIMER_0);
	time[1] = (time_s - time_e) - time[1];


	//test the sfloop
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	time_s = e_ctimer_start(E_CTIMER_0, ctype);
	result[2] = sfloop(1024);
	time_e = e_ctimer_stop(E_CTIMER_0);
	time[2] = time_s - time_e;

	time_s = e_ctimer_start(E_CTIMER_0, ctype);

	sfloop(2048);
	time_e = e_ctimer_stop(E_CTIMER_0);
	time[2] = (time_s - time_e) - time[2];

	

	return EXIT_SUCCESS;
}


void init_array() {
	int _i;
	for(_i=0;_i<N;_i++)
	{
		A[_i] = _i;
		B[_i] = _i;
	}
	return;
}

int comloop() {
	int _i, sum;
	sum = 0;
	for(_i=0;_i<1024;_i++)
	{
		sum += A[_i]*B[_i]; 
	}

	return sum;
}
