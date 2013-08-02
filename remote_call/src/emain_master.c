#include <stdlib.h>
#include <e-lib.h>


#define E_WAND_INT             (8)
#define irq_num                (7)
#define host                   (0xdeadbeef)
#define mail_address           (0x00005000)
#define resultbase             (0x00005200)
#define delay                  (0x00500000)
#define expect_result          (0x0000452f)
#define signalbase             (0x00005400)
#define finish                 (0x00000001)


int main(void) 
{
	unsigned i, j, k, row, col, num;
	unsigned *result, *mailbox;
	unsigned *signal;
	
	//mask interrupts
	e_irq_global_mask(E_FALSE);

	e_reg_write(E_REG_IMASK,0x0);
	
	row = e_group_config.core_row;
	col = e_group_config.core_col;

	//initialize mailbox
	mailbox = (unsigned *)mail_address;
	e_wait(E_CTIMER_0, 0x00040000);
	for (i=0;i<16;i++)
	{
		mailbox[i] = 0x0;
	}

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if((i==row) && (j==col))
			{
				mailbox[(i*4+j)] = host;
			}
			else
			{	
				result = e_get_global_address(i,j,(void *)resultbase);
				signal = e_get_global_address(i,j,(void *)signalbase);

				//insert interrupt
				e_wait(E_CTIMER_0, delay);
				
				e_irq_set(i,j,E_SW_EXCEPTION);
				e_irq_set(i,j,E_MEM_FAULT);
				e_irq_set(i,j,E_TIMER0_INT);
				e_irq_set(i,j,E_TIMER1_INT);
				e_irq_set(i,j,E_DMA0_INT);
				e_irq_set(i,j,E_DMA1_INT);
				e_irq_set(i,j,E_USER_INT);

				e_wait(E_CTIMER_0, delay);

				//send slave the finish signal
				*signal = finish;
				//check the result
				for(k=0;k<irq_num;k++)
				{
					if(result[k] != expect_result)
						mailbox[(i*4+j)] = mailbox[(i*4+j)] | (0x1<<k);
				}
			}
		}
	}


	while(1);
	
	return EXIT_SUCCESS;
}







