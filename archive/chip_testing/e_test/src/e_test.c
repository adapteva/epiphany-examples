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
#include <stdio.h>
#include <stdlib.h>
#ifdef __epiphany__
#include <e_regs.h>
#endif


int e_test_finish(const char * test_name, int status, unsigned row,unsigned col ){
  int *pass = (int*) 0x0;//overwrite the sync ivt entry on exit
  if(status==0){
    *pass = 0x12345678;
    return EXIT_SUCCESS;
  }
  else{
    printf("TEST \"%s\" on core (%d,%d) FAILED\n",test_name,row,col);
    *pass = 0xDEADBEEF;
    return EXIT_FAILURE;
  }
}

void e_test_init(unsigned *row, unsigned *col){
#ifdef __epiphany__
  unsigned coreID;
  int *pass = (int*) 0x0;
  *pass = 0xDEADBEEF;//initialize as fail
  coreID=e_reg_read(E_REG_COREID);
  *row = (coreID >> 6) & 0x3f;
  *col = coreID & 0x3f;
#endif
}

void e_write_ack(unsigned *addr){
  unsigned probe_data;
  //printf("addr=%p\n",addr);
  probe_data = (*(addr));              //read old data
  probe_data = probe_data ^ 0xFFFFFFFF;//toggle old data
  (*(addr)) = probe_data;              //write new toggled data
  while(probe_data != *(addr)){       //keep reading until match is met  
  }
}




