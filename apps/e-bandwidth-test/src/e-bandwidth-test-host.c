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
#include <time.h>

#include <e-hal.h>
#include <e-loader.h>

#define _BUF_SZ (8*1024)
#define eMHz 600
#define aMHz 667

typedef struct {
	e_bool_t load;
	e_bool_t reset_target;
	e_bool_t run_target;
	e_loader_diag_t verbose;
	char elf_file[4096];
} args_t;

args_t ar = {E_TRUE, E_TRUE, E_FALSE, L_D0, "bin/e-bandwidth-test-device.elf"};

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
  int fail;

  fd = stderr;

  pEpiphany = &Epiphany;
  pERAM     = &ERAM;

  if (argc > 1)
    strcpy(ar.elf_file, argv[1]);

  e_set_host_verbosity(H_D0);

  if ( E_OK != e_init(NULL) ) {
      fprintf(stderr, "\nERROR: epiphinay initialization failed!\n\n");
      exit(1);
  }

  if (E_OK != e_reset_system() ) {
      fprintf(stderr, "\nWARNING: epiphinay system rest failed!\n\n");
  }

  // prepare ERAM
  if (E_OK != e_alloc(pERAM, 0x00000000, e_platform.emem[0].size))
  {
      fprintf(stderr, "\nERROR: Can't allocate Epiphany DRAM!\n\n");
      exit(1);
  }
  e_set_host_verbosity(H_D0);
  
  if (E_OK != e_open(pEpiphany, 0, 0, e_platform.rows, e_platform.cols))
  {
      fprintf(stderr, "\nERROR: Can't establish connection to Epiphany device!\n\n");
      exit(1);
  }


  fail = 0;


  printf("------------------------------------------------------------\n");
  //////////////////////////////
  // Test Host-Device throughput
  fail += SRAM_speed();
  fail += ERAM_speed();
  fail += DRAM_speed();

  /////////////////////////////
  // Test eCore-ERAM throughput
  fail += EPI_speed();

  //Finalize
  e_close(pEpiphany);
  e_free(pERAM);
  e_finalize();

  printf("------------------------------------------------------------\n");
  if(!fail){
    printf( "TEST \"e-bandwidth-test\" PASSED\n");
    return EXIT_SUCCESS;
  }
  else{
    printf( "TEST \"e-bandwidth-test\" FAILED\n");
    return EXIT_FAILURE;
  }
}


struct timespec timer[2];
/*****************************************************************************/
#define SRAM_BUF_SZ_KB 32
#define SRAM_BUF_SZ    (SRAM_BUF_SZ_KB * 1024)
char    sbuf[SRAM_BUF_SZ];
int SRAM_speed(){
  double tdiff, rate;
  int err = 0, i;

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[0]);
  /* Do a couple of iterations to get more stable results */
  for (i = 0; i < 512; i++)
    e_write(pEpiphany, 0, 0, (off_t) 0, sbuf, SRAM_BUF_SZ);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[1]);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000000.0);
  rate  = (double) SRAM_BUF_SZ_KB / (1024.0/512.0) / tdiff;

  printf("ARM Host    --> eCore(0,0) write speed       = %7.2f MB/s", rate);

  if (rate < 30.0) {
    printf(" (TOO SLOW)");
    err  = 1;
  }
  putchar('\n');

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[0]);
  /* Do a couple of iterations to get more stable results */
  for (i = 0; i < 8; i++)
    e_read(pEpiphany, 0, 0, (off_t) 0, sbuf, SRAM_BUF_SZ);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[1]);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000000.0);
  rate  = (double) SRAM_BUF_SZ_KB / (1024.0/8.0) / tdiff;

  printf("ARM Host    --> eCore(0,0) read speed        = %7.2f MB/s", rate);

  if (rate < 4.0) {
    printf(" (TOO SLOW)");
    err  = 1;
  }
  putchar('\n');

  return err;
}

/*****************************************************************************/
#define ERAM_BUF_SZ_MB 16
#define ERAM_BUF_SZ    (ERAM_BUF_SZ_MB * (1024*1024))
char    ebuf[ERAM_BUF_SZ];
__suseconds_t d;
int ERAM_speed(){
  double tdiff, rate;
  
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[0]);
  e_write(pERAM, 0, 0, (off_t) 0, ebuf, ERAM_BUF_SZ);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[1]);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000000.0);
  rate  = (double) ERAM_BUF_SZ_MB / tdiff;
  
  printf("ARM Host    --> ERAM write speed             = %7.2f MB/s\n", rate);
  
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[0]);
  e_read(pERAM, 0, 0, (off_t) 0, ebuf, ERAM_BUF_SZ);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[1]);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000000.0);
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
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[0]);
  memcpy(dbufw, dbufr, DRAM_BUF_SZ);
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[1]);
  tdiff = ((double) (timer[1].tv_sec - timer[0].tv_sec)) + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000000.0);
  rate  = (double) DRAM_BUF_SZ_MB / tdiff;

  printf("ARM Host    <-> DRAM: Copy speed             = %7.2f MB/s\n", rate);
  return 0;
}


int EPI_speed(){
  unsigned int clocks;
  double       rate;
  int          result;
  int row = 0;
  int col = 0;

  //Run program
  e_load_group(ar.elf_file, pEpiphany, row, col,1,1, E_TRUE);

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

  /* Return anything but 0 on error */
  return !(result == 5);
}


