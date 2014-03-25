/*
  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Andreas Olofsson <andreas@adapteva.com>

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e_test.h>

#define word_size       (4)
#define buffer_size     (0x2000)
#define CORES           16
#define N 16
#define A(i,j) a_mat[j*N+i]
#define B(i,j) b_mat[j*N+i]
#define C(i,j) c_mat[j*N+i]

float  a_mat[N*N] __attribute__ ((section (".data_bank1")));//result matrix
float  b_mat[N*N] __attribute__ ((section (".data_bank2")));//result matrix
float  c_mat[N*N] __attribute__ ((section (".data_bank3")));//result matrix

int main(int argc, char *argv[]){
  int      i,j,k;
  unsigned row = 0;
  unsigned col = 0;
  float    sum  = 0.f;
 
  unsigned int offset;
  
  unsigned *addr;
  unsigned *dummy;

  int fail = 0;
  int verbose = 1;
  int buffers[CORES+1];

  buffers[0] =0x82400000;//core (0,0)
  buffers[1] =0x82500000;//core (0,1)
  buffers[2] =0x82600000;//core (0,2)
  buffers[3] =0x82700000;//core (0,3)
  buffers[4] =0x86400000;//core (1,0)
  buffers[5] =0x86500000;//etc
  buffers[6] =0x86600000;
  buffers[7] =0x86700000;
  buffers[8] =0x8a400000;
  buffers[9] =0x8a500000;
  buffers[10]=0x8a600000;
  buffers[11]=0x8a700000;
  buffers[12]=0x8e400000;
  buffers[13]=0x8e500000;
  buffers[14]=0x8e600000;
  buffers[15]=0x8e700000;
  buffers[16]=0x80100000;//DRAM

  const char *test_name = "emesh";

  unsigned int result;

  unsigned PAT0 = 0x55555555;
  unsigned PAT1 = 0xAAAAAAAA;

  //Test Init
  e_test_init(&row, &col);
  
  //Communicate with all cores on chip and DRAM
  for(i=0; i<(CORES+1); i++){
    offset = buffers[i] + 0x2000;
    dummy = (unsigned *) (offset+buffer_size);
    //Write/Read Buffer
    for(k=0; k<(buffer_size); k=k+word_size){
      addr=(unsigned *) (offset+k);
      /*PAT0*/
      (*(addr))=PAT0;
      //call below b/c architecture does not guarantee RAW for external access
      e_write_ack(dummy);                         
      result=(*(addr));
      if(result!=PAT0){
	fail=1;
	if(verbose>0){
	  printf("FAIL-PAT0: core=(%d,%d) addr=(0x%x) result=0x%x\n",row,col,k,result); 
	}
      }
      /*PAT1*/
      (*(addr))=PAT1;
      e_write_ack(dummy); 
      result=(*(addr));
      if(result!=PAT1){
	fail=1;
	if(verbose>0){	    
	  printf("FAIL-PAT1: core=(%d,%d) addr=(0x%x) result=0x%x\n",row,col,k,result); 
	}
      }
    }
  }

  //Fill input matrices with a constant
  for (i=0; i<N; i++){
     for (j=0; j<N; j++){
      A(i,j) = 1.0f;
      B(i,j) = 1.0f;
       }
    }

  //Run matrix multiplication
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      C(i,j) = 0;
      for (k=0; k<N; k++){
         C(i,j) += A(i,k)*B(k,j);
      }
    }
  }

  //Sum up the C matrix
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      sum+=C(i,j);
    }
  }
  
  //Compare to expected result
  if(sum!=4096.0f){
    fail=1;
    printf("FAIL: core=(%d,%d) expected=4096.0000 result=%f\n",row,col,sum); 
  }
  
  //Finish Test
  return e_test_finish(test_name,fail,row,col);
}

