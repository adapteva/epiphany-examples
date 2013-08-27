#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h> 
#include <e-hal.h>
#include "erm.h"


typedef enum{
	REGULAR = 0,
	ALIGNED = 1,
	RANDOM  = 2,
} mode_type;


char *filename_work = "/home/linaro/epiphany.status";

e_epiphany_t dev;

unsigned platform_rows = 4; // TODO: make generic
unsigned platform_cols = 4;

void get_epiphany_status(unsigned platform_rows, unsigned platform_cols, FILE *file, unsigned *p_status);
void modify_epiphany_status(unsigned platform_rows, unsigned platform_cols, FILE *file, unsigned rows, unsigned cols, unsigned row, unsigned col, unsigned value);
int check_available(unsigned platform_rows, unsigned platform_cols, unsigned *status, unsigned row, unsigned col, unsigned rows, unsigned cols);
int find_group(unsigned platform_rows, unsigned platform_cols, unsigned mode, unsigned rows, unsigned cols, unsigned *p_row, unsigned *p_col, unsigned *status);


int e_reserve(unsigned search_mode, unsigned rows, unsigned cols, unsigned *p_row, unsigned *p_col)
{
	FILE *file;
	unsigned status[platform_rows*platform_cols];
	unsigned i, j, index, m, k, try_again;
	unsigned result;
	int brk;
	
	while (1)
	{
		try_again = 1;
		file = fopen(filename_work, "r+");
	
		// Lock the epiphany.status file
		result = flock(fileno(file), LOCK_EX);
	
		if (result == 0) //-----only do the following when lock successfully
		{
			// Get access to the current epiphany.status file 
			get_epiphany_status(platform_rows, platform_cols, file, &status[0]);
	
			// Method-1, try to find an available work group, then break
			brk = find_group(platform_rows, platform_cols, search_mode, rows, cols, p_row, p_col, &status[0]);
			
			//-----if find, brk=E_OK; if can't find, brk=E_ERR;
			if (brk == E_OK)
			{
				fprintf(stderr, "Find idle %d*%d group successfully!\n", rows, cols);
				fprintf(stderr, "Assigning group beginning with (row:%d,col:%d)!\n", *p_row, *p_col); 
				
				// Modify work.status file, set busy(1)
				modify_epiphany_status(platform_rows, platform_cols, file, rows, cols, *p_row, *p_col, 1);
			
				// Flush the file
				fflush(file);
	
				// Unlock the file
				flock(fileno(file), LOCK_UN);
				
				try_again = 0;
			} else
			{
				fprintf(stderr, "Failed to find idle %d*%d group!\n", rows, cols);
				
				// Unlock the file
				flock(fileno(file), LOCK_UN);
			}
		}
		
		fclose(file);

		
		if (try_again == 0)
			break;
		
		usleep(1000);
	}
		
	return E_OK; // success;
}


int find_group(unsigned platform_rows, unsigned platform_cols, unsigned mode, unsigned rows, unsigned cols, unsigned *p_row, unsigned *p_col, unsigned *status)
{
	unsigned i, j, k;
	int find;
	
	switch (mode) 
	{
		case REGULAR:
			for (i=0; i<(platform_rows - rows + 1); i++)
			{
				for (j=0; j<(platform_cols - cols + 1); j++)
				{
					if ((find = check_available(platform_rows, platform_cols, status, i, j, rows, cols)) == E_OK)
					{
						*p_row = i;
						*p_col = j;
						break;
					}
				}
				if (find==E_OK)
				{
					break;
				}
			}
			break;
		
		case ALIGNED:
			for (i=0; i<(platform_rows - rows + 1); i+=rows) // adjusting row and col aligned
			{
				for (j=0; j<(platform_cols - cols + 1); j+=cols)
				{
					if ((find = check_available(platform_rows, platform_cols, status, i, j, rows, cols)) == E_OK)
					{
						*p_row = i;
						*p_col = j;
						break;
					}
				}
				if (find==E_OK)
				{
					break;
				}
			}
			break;
					
		case RANDOM:
			srand(time(NULL));

			for (k=0; k<platform_rows*platform_cols; k++)
			{
				// randomly generate "row" and "col"
				i = rand()%(platform_rows-rows+1);
				j = rand()%(platform_cols-cols+1);
				
				// check if is available
				if ((find = check_available(platform_rows, platform_cols, status, i, j, rows, cols)) == E_OK)
				{
					*p_row = i;
					*p_col = j;
					break;
				}
			}
			break;
			
		default:
			fprintf(stderr, "ERROR: Invalid search mode!\n");
	}

	return find;
}



int check_available(unsigned platform_rows, unsigned platform_cols, unsigned *status, unsigned row, unsigned col, unsigned rows, unsigned cols)
{
	int available = E_OK;
	unsigned i, j;
	
	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (status[(row+i)*platform_cols + (col + j)] == 1)
			{
				available = E_ERR;
				break;
			}
		}
		if (available == E_ERR)
			break;
	}
	
	return available;
}


void get_epiphany_status(unsigned platform_rows, unsigned platform_cols, FILE *file, unsigned *p_status)
{
	unsigned i, j;
	char line[50];
	
	for (i=0; i<platform_rows; i++)
	{
		for (j=0; j<platform_cols; j++)
		{
			fseek(file, 12*(i*platform_cols + j) + 10, SEEK_SET);
			fgets(line, 50, file);
  		sscanf(line, "%d", (p_status + i*platform_cols + j));
		}
	}

	return;
}

void modify_epiphany_status(unsigned platform_rows, unsigned platform_cols, FILE *file, unsigned rows, unsigned cols, unsigned row, unsigned col, unsigned value)
{
	
	unsigned i, j;
	
	for (i=row; i<(row+rows); i++)
	{
		for (j=col; j<(col+cols); j++)
		{
			fseek(file, 12*(i*platform_cols + j)+10, SEEK_SET);
			fprintf(file, "%d", value);
		}
	}
	
	return;
}


int e_release(unsigned rows, unsigned cols, unsigned row, unsigned col)
{
	FILE *file;
	unsigned i, j;
	unsigned result;
	
	file = fopen(filename_work, "r+");
	
	// Lock the epiphany.status file
	while ((result = flock(fileno(file), LOCK_EX)) != 0) {};
	
	// Modifying the epiphany.status file, set idle(0)
	modify_epiphany_status(platform_rows, platform_cols, file, rows, cols, row, col, 0);
	
	// Flush the file
	fflush(file);
	
	// Unlock the file
	flock(fileno(file), LOCK_UN);
	
	fclose(file);
	
	return E_OK;
}
