/*
The MIT License (MIT)

Copyright (c) 2013 Adapteva, Inc

Contributor Andreas Olofsson <andreas@adapteva.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e_test.h>

#define word_size       (4)
#define buffer_size     (0x2000)
#define CORES           16

int main(int argc, char *argv[]){
  int      k,i;
  unsigned row = 0;
  unsigned col = 0;

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
  //Finish Test
  return e_test_finish(test_name,fail,row,col);
}

