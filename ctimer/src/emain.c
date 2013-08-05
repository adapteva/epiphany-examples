
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <e_lib.h>


#define membase 0x8f000000
#define _Ntaps 16
#define _Ndata (_Ntaps * 2)

// arrange Coefficients in descending order, i.e., Coeff_(N-1) first and Coeff_0 last
float coeffs[_Ntaps] =
	{0.01, 0.02, 0.04, 0.08,
	 0.16, 0.32, 0.64, 1.28,
	 1.28, 0.64, 0.32, 0.16,
	 0.08, 0.04, 0.02, 0.01};


float inp_data[_Ndata] SECTION(".data_bank3");
float out_o[_Ndata]    SECTION(".data_bank3");
float out_n[_Ndata]    SECTION(".data_bank3");


unsigned fir(float *a, float *b, unsigned t);
unsigned idle_test();
void init_data(float *a);

unsigned *result;
e_ctimer_config_t event_list[] = 
	{
		E_CTIMER_CLK, E_CTIMER_IDLE, E_CTIMER_IALU_INST, E_CTIMER_FPU_INST,	E_CTIMER_DUAL_INST, 
		E_CTIMER_E1_STALLS, E_CTIMER_RA_STALLS,	E_CTIMER_EXT_FETCH_STALLS, E_CTIMER_EXT_LOAD_STALLS
	};


int main(void) 
{
	int t;

	//initialize
	result = (unsigned *) (membase);
	init_data(inp_data);
	for (t=0;t<10;t++)
		result[t] = 0x0;

	//start counting events
	for (t=0;t<9;t++)
		result[t] = fir(inp_data, out_o, t);
		
	//enable the interruption
	e_irq_global_mask(E_FALSE);
	e_irq_mask(E_TIMER1_INT,E_FALSE);

	//call the idle function
	result[9] = idle_test();
	
	return EXIT_SUCCESS;
}






unsigned fir(float *a, float *b, unsigned t) 
{
	int wrp; // pointer to the delay line's current position.
	int rdp; // pointer to the I/O data's current position.
	int cp;  // pointer to the FIR coefficient's array.
	int dlp; // pointer to delay line position per-tap.

	float c[_Ntaps];      // FIR coefficients
	float dl[_Ntaps * 2]; // FIR delay line
	float fir[8];         // temp fir accumulators

	unsigned time, time_s, time_e;

	for (cp=0; cp<_Ntaps; cp++)
	{
		dl[cp] = dl[cp+_Ntaps] = 0.0;
		c[cp]  = coeffs[cp];
	}

	//count event
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	e_ctimer_start(E_CTIMER_0, event_list[t]);
	time_s = e_ctimer_get(E_CTIMER_0);

	//execute fir
	for (rdp=0; rdp<_Ndata; rdp+=_Ntaps)
	{
		for (wrp=0; wrp<_Ntaps; wrp+=8)
		{
			dl[wrp+0] = dl[wrp+_Ntaps+0] = a[rdp+wrp+0];
			dl[wrp+1] = dl[wrp+_Ntaps+1] = a[rdp+wrp+1];
			dl[wrp+2] = dl[wrp+_Ntaps+2] = a[rdp+wrp+2];
			dl[wrp+3] = dl[wrp+_Ntaps+3] = a[rdp+wrp+3];
			dl[wrp+4] = dl[wrp+_Ntaps+4] = a[rdp+wrp+4];
			dl[wrp+5] = dl[wrp+_Ntaps+5] = a[rdp+wrp+5];
			dl[wrp+6] = dl[wrp+_Ntaps+6] = a[rdp+wrp+6];
			dl[wrp+7] = dl[wrp+_Ntaps+7] = a[rdp+wrp+7];
			fir[0] = fir[1] = fir[2] = fir[3] = 0;
			fir[4] = fir[5] = fir[6] = fir[7] = 0;
			for (cp=0, dlp=(wrp+1); cp<_Ntaps; cp++, dlp++)
			{
				fir[0] += c[cp] * dl[dlp + 0];
				fir[1] += c[cp] * dl[dlp + 1];
				fir[2] += c[cp] * dl[dlp + 2];
				fir[3] += c[cp] * dl[dlp + 3];
				fir[4] += c[cp] * dl[dlp + 4];
				fir[5] += c[cp] * dl[dlp + 5];
				fir[6] += c[cp] * dl[dlp + 6];
				fir[7] += c[cp] * dl[dlp + 7];
			}
			b[rdp+wrp+0] = fir[0];
			b[rdp+wrp+1] = fir[1];
			b[rdp+wrp+2] = fir[2];
			b[rdp+wrp+3] = fir[3];
			b[rdp+wrp+4] = fir[4];
			b[rdp+wrp+5] = fir[5];
			b[rdp+wrp+6] = fir[6];
			b[rdp+wrp+7] = fir[7];
		}
	}
	
	time_e = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);
	time = time_s - time_e;

	return time;
}


unsigned idle_test()
{
	unsigned time, time_s, time_e;
	unsigned *ivt;
	
	//set the ctimer1 interrupt handler
	ivt = (unsigned *) 0x10;
	*ivt = 0x1d2;

	//set ctimer1 to generate the interrupt 
	//to awake core from idle
	e_ctimer_set(E_CTIMER_1, 100000);
	e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);

	//set ctiemr0 to count event IDLE
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	e_ctimer_start(E_CTIMER_0, E_CTIMER_IDLE);
	time_s = e_ctimer_get(E_CTIMER_0);

	//set the core into IDLE
	__asm__ __volatile__ ("idle");

	time_e = e_ctimer_get(E_CTIMER_0);
	e_ctimer_stop(E_CTIMER_0);
	time = time_s - time_e;

	return time;
}


void init_data(float *a)
{
	int i;

	for (i=0; i<_Ndata; i++)
	{
		a[i] = (i == 0) ? 1 : (i == _Ndata/2) ? 2 : 0;
	}

	return;
}


