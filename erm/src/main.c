/*
  main.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Xin Mao <maoxin99@gmail.com>
                 Wenlin Song <wsong@wpi.edu>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/

// This is the epiphany resource manager(erm) project.
//
// This program runs on the linux host before running any other program
// that contains e_reserve() and e_release() function. The program has
// two mode, Mode with monitor(0) and mode without monitor(1).
// The program first initializes the board and set up the epiphany.status,
// ecore.status and emesh.status files. Then in the erm_monitor() function, the 
// program repeatedly update the ecore.status and emesh.status files.

//
// Aug-2013, XM.



#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <e-hal.h>


int erm_init(unsigned *rowSize, unsigned *colSize);
int erm_monitor(unsigned rowSize, unsigned colSize);
int update_record(unsigned rows, unsigned cols, unsigned *ecore, unsigned *emesh, unsigned *mod);
int update_file(unsigned rows, unsigned cols, unsigned *ecore, unsigned *emesh);
int set_timer(unsigned row, unsigned col);

e_epiphany_t dev;

typedef enum{
	with_monitor    = 0,
	without_monitor = 1,
}MODE;


int main()
{
	unsigned rowSize, colSize;
	unsigned mode;
	
	//e_set_host_verbosity(H_D2);
	
	fprintf(stderr, "\nPlease enter the erm working mode(number between 0 to 1).\n");
	scanf("%d", &mode);	
	while(mode>=2)
	{
		fprintf(stderr, "Invalid input of mode, please re-enter a number between 0 to 1.\n");
		scanf("%d", &mode);
	}
	

	switch (mode)
	{
		case with_monitor:
			fprintf(stderr, "mode: with monitor.\n", mode);
			usleep(1000000);
			erm_init(&rowSize, &colSize);
			fprintf(stderr, "Initialized\n\n");
			erm_monitor(rowSize, colSize);
			break;
		case without_monitor:
			fprintf(stderr, "mode: without monitor.\n", mode);
			usleep(1000000);
			erm_init(&rowSize, &colSize);
			fprintf(stderr, "Initialized\n\n");
			break;
		default:
			break;	
	}
	

	return 0;
}

int erm_init(unsigned *rowSize, unsigned *colSize)
{
	e_platform_t platform;
	//e_epiphany_t dev;
	unsigned i, j;
	FILE *file;
	char *filename_0 = "/home/linaro/epiphany.status";
	char *filename_1 = "/home/linaro/ecore.status";
	char *filename_2 = "/home/linaro/emesh.status";
		
	
	//set up the board
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	
	//open workgroup
	*rowSize = platform.rows;
	*colSize = platform.cols;
	e_open(&dev, 0, 0, *rowSize, *colSize);
	
	//set up epiphany.status file
	file = fopen(filename_0,"w+");
	
	for (i=0;i<*rowSize;i++)
	{
		for (j=0;j<*colSize;j++)
		{
			fprintf(file,"%04d %04d %d\n", i, j, 0x0); /*row, col, busy*/
		}
	}
	fclose(file);


	//set up ecore.status file
	file = fopen(filename_1,"w+");
	
	for (i=0;i<*rowSize;i++)
	{
		for (j=0;j<*colSize;j++)
		{
			fprintf(file,"%04d %04d %02d\n", i, j, 0x0); /*row, col, busy*/
		}
	}
	fclose(file);


	//set up emesh.status file
	file = fopen(filename_2,"w+");
	
	for (i=0;i<*rowSize;i++)
	{
		for (j=0;j<*colSize;j++)
		{
			fprintf(file,"%04d %04d %02d\n", i, j, 0x0); /*row, col, busy*/
		}
	}
	fclose(file);
	
	return 0;
}


int erm_monitor(unsigned rowSize, unsigned colSize)
{
	unsigned ecore_record[rowSize*colSize];
	unsigned emesh_record[rowSize*colSize];
	unsigned i, j;
	unsigned mod;
	
	//initialize ecore_record, emesh_record and ctiemr1
	for (i=0;i<rowSize;i++)
	{
		for (j=0;j<colSize;j++)
		{
			ecore_record[i*4+j] = 0;
			emesh_record[i*4+j] = 0;
			set_timer(i, j);
		}
	}
	
	
	i = 0x0;
	mod = 0x0;
	while (1)
	{
		//update the ecore_record and emesh_record
		update_record(rowSize, colSize, &ecore_record[0], &emesh_record[0], &mod);
		usleep(800);//sample every 8 ms
		
		//update status files
		i++;
		if (i >= 128)//around every 1 sec update files once
		{
			update_file(rowSize, colSize, &ecore_record[0], &emesh_record[0]);
			i = 0;
			fprintf(stderr, "Updated files!\n");
		}
		
	}	

	return 0;
}


int set_timer(unsigned row, unsigned col)
{
	unsigned maxtime, starttimer, emesh;
	
	maxtime    = 0xffffffff;
	starttimer = 0x00000f00;//set ctimer1 to count emesh event
	//starttimer = 0x00000100;
	emesh      = 0x000000e0;//set emesh to count access cycles
	
	
	e_write(&dev, row, col, 0xf043c, &maxtime   , sizeof(maxtime));   //set ctimer1
	e_write(&dev, row, col, 0xf0700, &emesh     , sizeof(emesh));     //set emesh event
	e_write(&dev, row, col, 0xf0400, &starttimer, sizeof(starttimer));//start ctimer	
	
	return 0;
}


int update_record(unsigned rows, unsigned cols, unsigned *ecore, unsigned *emesh, unsigned *mod)
{
	unsigned i, j, emesh_value, maxtime, crt_mod;
	unsigned state[rows*cols];
	
	
	maxtime     = 0xffffffff;
	crt_mod = 0x000000a0 + (((*mod)%5)<<4);
	(*mod)++;


	for (i=0;i<rows;i++)
	{
		for (j=0;j<cols;j++)
		{
			e_read (&dev, i, j, 0xf0404, &state[i*cols+j], sizeof(unsigned)); //-----read status register
			e_read (&dev, i, j, 0xf043c, &emesh_value    , sizeof(unsigned)); //-----read counting value from ctimer1
			
			
			state[i*cols+j]  = state[i*cols+j]&0x00000001; //check if the core is actived
			ecore[i*cols+j] += state[i*cols+j];
			emesh[i*cols+j] += (maxtime - emesh_value);
			//fprintf(stderr, "emesh[%d] = %d!\n", (i*cols+j), emesh[i*cols+j]);
			
			e_write(&dev, i, j, 0xf0700, &crt_mod, sizeof(maxtime));
			e_write(&dev, i, j, 0xf043c, &maxtime, sizeof(maxtime));  //----reset the counting value in ctiemr1 to maxtime
		}
	}	
	//fprintf(stderr, "emesh[%d] = %d!\n", (0), emesh[0]);

	return 0;
}


int update_file(unsigned rows, unsigned cols, unsigned *ecore, unsigned *emesh)
{
	FILE *file;
	char *filename_1 = "/home/linaro/ecore.status";
	char *filename_2 = "/home/linaro/emesh.status";	
	unsigned i, j;
	
	
//-----insert a test-------show the result of epiphany.status and ecore.status
	char *filename_0 = "/home/linaro/epiphany.status";
	unsigned work[rows*cols], idle[rows*cols];
	char line[100];
	unsigned r, c;
	
	while ((file = fopen(filename_0,"r+")) == NULL) {;}

	
	for (i=0;i<rows;i++)
	{
		for (j=0;j<cols;j++)
		{
			fgets(line, 100, file);
			sscanf(line, "%d %d %d", &r, &c, &work[i*cols+j]);			
		}
	}
	fclose(file);
	fprintf(stderr, "\n\nCurrent status is:\t(row, col, busy, ecore, emesh)\n");
//end of test	
	
	

	
	//----update ecore.status file
	while ((file = fopen(filename_1,"r+")) == NULL) {;}

	
	for (i=0;i<rows;i++)
	{
		for (j=0;j<cols;j++)
		{
			//determin the level
			ecore[i*cols+j] = ecore[i*cols+j]>>2;
			if (ecore[i*cols+j]>31)
				ecore[i*cols+j] = 31;
			
			fseek(file, (((i)*cols + (j))*13 + 10), SEEK_SET);
			fprintf(file,"%02d\n", ecore[i*cols+j]);
			
//---insert a test----
			idle[i*cols+j] = ecore[i*cols+j];
			//fprintf(stderr, "\t%d %d %d %02d\n", i, j, work[i*cols+j], ecore[i*cols+j]);			
						
			ecore[i*cols+j] = 0x0;//clear the ecore
		}
	}
	fclose(file);



	//----update emesh.status file
	while ((file = fopen(filename_2,"r+")) == NULL) {;}
	
	for (i=0;i<rows;i++)
	{
		for (j=0;j<cols;j++)
		{
			//determin the level
			//-------------the level of emesh still need to adjust
			emesh[i*cols+j] = emesh[i*cols+j]>>18;
			if (emesh[i*cols+j]>31)
				emesh[i*cols+j] = 31;
				
			fseek(file, (((i)*cols + (j))*13 + 10), SEEK_SET);
			fprintf(file,"%02d\n", emesh[i*cols+j]);
//---insert a test-----//
			fprintf(stderr, "\t%d %d %d %02d %02d\n", i, j, work[i*cols+j], idle[i*cols+j], emesh[i*cols+j]);	
			
			emesh[i*cols+j] = 0x0;//clear the emesh
			
		}
	}
	fclose(file);

	return 0;
}



