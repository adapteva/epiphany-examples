/*
Copyright (C) 2013 Adapteva, Inc.
Contributed by Yaniv Sapir <support@adapteva.com>
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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "e-hal.h"

#define _BUF_SZ (8*1024)
#define eMHz 600
#define aMHz 667

typedef struct {
	e_bool_t load;
	e_bool_t reset_target;
	e_bool_t run_target;
	e_loader_diag_t verbose;
	char srecFile[4096];
} args_t;

args_t ar = {E_TRUE, E_TRUE, E_FALSE, L_D0, "bin/e_startup_test.srec"};

e_epiphany_t Epiphany, *pEpiphany;
e_mem_t      ERAM,     *pERAM;

struct {
	int      fail;
	unsigned core;
	unsigned coreid;
	unsigned addr;
	unsigned wr_val;
	unsigned rd_val;
} res;

int SRAM_speed(); // Speed of host access to SRAM
int ERAM_speed(); // Speed of host access to eDRAM
int DRAM_speed(); // Speed of host access to DRAM (userspace)
int EPI_speed();  // Speeds of Epiphany access to SRAM and ERAM

extern e_platform_t e_platform;

FILE *fd;

int main(int argc, char *argv[])
{
  int result, fail;
  
  fd = stderr;
  
  pEpiphany = &Epiphany;
  pERAM     = &ERAM;
  
  e_set_host_verbosity(H_D0);
  e_init(NULL);
  e_reset_system();

  // prepare ERAM
  if (e_alloc(pERAM, 0x00000000, e_platform.emem[0].size))
    {
      fprintf(stderr, "\nERROR: Can't allocate Epiphany DRAM!\n\n");
      exit(1);
    }
  e_set_host_verbosity(H_D0);
  
  if (e_open(pEpiphany, 0, 0, e_platform.rows, e_platform.cols))
    {
      fprintf(stderr, "\nERROR: Can't establish connection to Epiphany device!\n\n");
      exit(1);
    }
  
  
  fail = 0;
  
  
  
  //////////////////////////////
  // Test Host-Device throughput
  SRAM_speed();
  ERAM_speed();
  DRAM_speed();
  
  /////////////////////////////
  // Test eCore-ERAM throughput
  result = EPI_speed();

  //Finalize
  e_close(pEpiphany);
  e_free(pERAM);
  e_finalize();
  
  /////////////////////////////
  //For now, always pass
  return EXIT_SUCCESS;
}


typedef struct timeval timeval_t;
timeval_t timer[2];
/*****************************************************************************/
#define SRAM_BUF_SZ_KB 32
#define SRAM_BUF_SZ    (SRAM_BUF_SZ_KB * 1024)
char    sbuf[SRAM_BUF_SZ];
int SRAM_speed(){
  double tdiff, rate;
  
  gettimeofday(&timer[0], NULL);
  e_write(pEpiphany, 0, 0, (off_t) 0, sbuf, SRAM_BUF_SZ);
  gettimeofday(&timer[1], NULL);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000000.0);
  rate  = (double) SRAM_BUF_SZ_KB / 1024.0 / tdiff;
  
  printf("ARM Host    --> eCore(0,0) write spead       = %7.2f MB/s\n", rate);

  gettimeofday(&timer[0], NULL);
  e_read(pEpiphany, 0, 0, (off_t) 0, sbuf, SRAM_BUF_SZ);
  gettimeofday(&timer[1], NULL);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000000.0);
  rate  = (double) SRAM_BUF_SZ_KB / 1024.0 / tdiff;
  
  printf("ARM Host    --> eCore(0,0) read spead        = %7.2f MB/s\n", rate);
  return 0;
}

/*****************************************************************************/
#define ERAM_BUF_SZ_MB 16
#define ERAM_BUF_SZ    (ERAM_BUF_SZ_MB * (1024*1024))
char    ebuf[ERAM_BUF_SZ];
__suseconds_t d;
int ERAM_speed(){
  double tdiff, rate;
  
  gettimeofday(&timer[0], NULL);
  e_write(pERAM, 0, 0, (off_t) 0, ebuf, ERAM_BUF_SZ);
  gettimeofday(&timer[1], NULL);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000000.0);
  rate  = (double) ERAM_BUF_SZ_MB / tdiff;
  
  printf("ARM Host    --> ERAM write speed             = %7.2f MB/s\n", rate);
  
  gettimeofday(&timer[0], NULL);
  e_read(pERAM, 0, 0, (off_t) 0, ebuf, ERAM_BUF_SZ);
  gettimeofday(&timer[1], NULL);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000000.0);
  rate  = (double) ERAM_BUF_SZ_MB / tdiff;
  
  printf("ARM Host    <-- ERAM read speed              = %7.2f MB/s\n", rate);
  return 0;
}


#define DRAM_BUF_SZ_MB 16
#define DRAM_BUF_SZ    (DRAM_BUF_SZ_MB * (1024*1024))
char    dbufr[DRAM_BUF_SZ];
char    dbufw[DRAM_BUF_SZ];
int DRAM_speed(){

  double tdiff, rate;
  gettimeofday(&timer[0], NULL);
  memcpy(dbufw, dbufr, DRAM_BUF_SZ);
  gettimeofday(&timer[1], NULL);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_usec - timer[0].tv_usec) / 1000000.0);
  rate  = (double) DRAM_BUF_SZ_MB / tdiff;
  
  printf("ARM Host    <-> DRAM: Copy speed             = %7.2f MB/s\n", rate);
  //printf("eCore (0,0) --> eCore(1,0) write speed (DMA) = %7.2f MB/s\n", rate);
  return 0;
}


int EPI_speed(){
  unsigned int clocks;
  double       rate;
  int          result;
  int row = 0;
  int col = 0;

  //Run program
  e_load_group(ar.srecFile, pEpiphany, row, col,1,1, E_TRUE);

  //Lazy way of waiting till finished
  sleep(2);

  //Calculate rates
  e_read(pEpiphany, row, col, 0x7000, &clocks, sizeof(clocks));
  rate = (double) _BUF_SZ / (double) clocks * (eMHz * 1000000) / (1024*1024);
  printf("eCore (0,0) --> eCore(1,0) write speed (DMA) = %7.2f MB/s\n", rate);
  
  e_read(pEpiphany, row, col, 0x7004, &clocks, sizeof(clocks));
  rate = (double) _BUF_SZ / (double) clocks * (eMHz * 1000000) / (1024*1024);
  printf("eCore (0,0) <-- eCore(1,0) read speed (DMA)  = %7.2f MB/s\n", rate);
  
  e_read(pEpiphany, row, col, 0x7008, &clocks, sizeof(clocks));
  rate = (double) _BUF_SZ / (double) clocks * (eMHz * 1000000) / (1024*1024);
  printf("eCore (0,0) --> ERAM write speed (DMA)       = %7.2f MB/s\n", rate);
  
  e_read(pEpiphany, row, col, 0x700c, &clocks, sizeof(clocks));
  rate = (double) _BUF_SZ / (double) clocks * (eMHz * 1000000) / (1024*1024);
  printf("eCore (0,0) <-- ERAM read speed (DMA)        = %7.2f MB/s\n", rate);
  
  e_read(pEpiphany, row, col, 0x7010, &result, sizeof(result));
  
  return result;
}


