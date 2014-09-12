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
#include <e_test.h>
#include <e_regs.h>

#define word_size       (4)
#define buffer_size     (0x1000) //1000
#define LINKS           3

int main(int argc, char *argv[]){
  int      k, i, j;
  unsigned row = 0;
  unsigned col = 0;

  unsigned int offset = 0x4000;

  int fail    = 0;
  int verbose = 1;

  unsigned  buffers[LINKS];

  //Addresses outside the chip to write
  //assuming that chip sits at 0x82400000
  buffers[0] = 0x42400000; //north
  buffers[1] = 0x92400000; //south
  buffers[2] = 0x83400000; //east

  const char *test_name = "loopback";

  unsigned int result;

  unsigned PAT0 = 0x55555555;
  unsigned PAT1 = 0xAAAAAAAA;

  volatile unsigned *addr;
  volatile unsigned *local_addr;

  e_test_init(&row, &col);

  //North-Link
  addr=(unsigned *) (0x82400000 + 0x002F0328); //on-chip IO register address
  e_sysreg_write(E_CONFIG, (unsigned) 0x1000); //"direct north routing mode"
  (*(addr)) = 0x00004000;                      //write to north IO transmit reg
  e_sysreg_write(E_CONFIG, (unsigned) 0x0000); //clear out direct routing mode

  //South-Link
  addr = (unsigned *) (0x82400000 + 0x0C2F0328);
  e_sysreg_write(E_CONFIG, (unsigned) 0x1000);
  (*(addr)) = 0x00004000;
  e_sysreg_write(E_CONFIG, (unsigned) 0x0000);

  //East-Link
  addr = (unsigned *) (0x82400000 + 0x083F0328);
  e_sysreg_write(E_CONFIG, (unsigned) 0x5000);
  (*(addr)) = 0x00004000;
  e_sysreg_write(E_CONFIG, (unsigned) 0x0000);

  //West-Link (not used for test platform)
  //addr=(unsigned *) (0x82400000+0x080F0328;
  //e_sysreg_write(E_CONFIG, (unsigned) 0x5000);
  //(*(addr))                   =0x00004000;
  //e_sysreg_write(E_CONFIG, (unsigned) 0x0000);

  //Testing link loopback
  for(i=0; i<LINKS; i++){
    for(k=0; k<buffer_size; k=k+sizeof(int)){
      addr = (unsigned *) (buffers[i] + offset + k);   //writing to global address through link
      local_addr = (unsigned *) (offset+k);            //reading from local 0x000xxxxx memory
      (*(addr)) = PAT0;
      for(j=0; j<500; j++){
	__asm__ __volatile__ ("NOP;"); //wait loop to make sure data has returned
      }
      result = (*(local_addr));
      if(result != PAT0){
	fail = 1;
	if(verbose > 0){
	  printf("FAIL-PAT0: core=(%d,%d) addr=(%p) result=0x%x\n",row,col,addr,result);
	}
      }
      /*PAT1*/
      (*(addr)) = PAT1;
      for(j=0; j<500; j++){
	__asm__ __volatile__ ("NOP;");
      }
      result = (*(local_addr));
      if(result != PAT1){
	fail = 1;
	if(verbose > 0){
	  printf("FAIL-PAT1: core=(%d,%d) addr=(%p) result=0x%x\n",row,col,addr,result);
	}
      }
    }
  }
  //Finish Test
  return e_test_finish(test_name,fail,row,col);
}

