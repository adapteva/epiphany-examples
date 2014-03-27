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

/*---------------------------------------------------------------
REFERENCE: "Using March Tests to Test SRAM", by AD J Van De Good
*/

#include <stdlib.h>
#include <stdio.h>
#include <e_test.h>
#include <e_regs.h>

#define word_size       (1)
#define ram_size        (0x8000)

int main(int argc, char *argv[]){

  const char *test_name = "memory_test_march-g";

  int      k,i,j;
  unsigned row     = 0;
  unsigned col     = 0;
  unsigned rows    = 4;
  unsigned cols    = 4;
  int      fail    = 0;
  int      verbose = 1;
  unsigned start   = 0x82400000;  
  unsigned offset  = start;

  unsigned coreID;
  char *addr;
    
  unsigned int data_in,result,expected;
  unsigned int high_pattern = 0xff;
  unsigned int low_pattern  = 0x00;
  
  
  //Test Init
  e_test_init(&row, &col);
  coreID = e_sysreg_read(E_COREID);
  coreID = coreID<<20;

  //Testing all cores
  for(i=0;i<rows;i++){
    for(j=0;j<cols;j++){
      offset=start+(i<<26)+(j<<20);
      if(!(offset==coreID)){
	//M0: UP{w0}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+k);
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	}
	//M1: UP{r0,w1,r1,w0,r0,w1}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+k);
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //R1
	  expected=high_pattern;	  
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	}
	//M2: UP{r1,w0,w1}		
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+k);
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M2: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
	    } 
	    fail=1;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	}
	//M3: DOWN{R1,W0,W1,W0}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *)(offset+(ram_size-k-word_size));
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M3: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 		
	    }
	    fail=1;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  
	}
	//M4:DOWN{R0,W1,W0}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+(ram_size-k-word_size));
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M4: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	}
	//M5:DOWN{R0,W1,R1}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+(ram_size-k-word_size));
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M5: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M5: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	}
	//M6:DOWN{R1,W0,R0}
	for(k=0;k<(ram_size);k=k+word_size){
	  addr=(char *) (offset+(ram_size-k-word_size));
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M6: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
	    if(verbose>0){
	      printf("FAIL-PAT-M6: addr=%p expected=0x%x result=0x%x\n",addr,expected,result); 
	    }
	    fail=1;
	  }
	}

      }
    }
  }
  //Finish Test
  return e_test_finish(test_name,fail,row,col);
}

