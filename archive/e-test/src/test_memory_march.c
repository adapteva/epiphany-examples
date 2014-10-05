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

/*---------------------------------------------------------------
REFERENCE: "Using March Tests to Test SRAM", by AD J Van De Good
*/

#include <stdlib.h>
#include <stdio.h>

#define WORD_SIZE       (1)
#define RAM_SIZE        (0x8000)
//#define VERBOSE

int main(int argc, char *argv[]){

  unsigned int   k,i,j;
  unsigned rows    = 4;
  unsigned cols    = 4;
  int      status  = 1;
  unsigned offset;
  unsigned coreID;
  char *addr;

  unsigned int data_in,result,expected;
  unsigned int high_pattern = 0xff;
  unsigned int low_pattern  = 0x00;

  //Test Init
  coreID=e_test_init();
  //Testing all cores
  for(i=0;i<rows;i++){
    for(j=0;j<cols;j++){
      offset=coreID+(i<<26)+(j<<20);
      //printf("testing core (%d,%d) %x\n",i,j,offset);
      if(!(offset==coreID)){
	//M0: UP{w0}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+k);
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	}
	//M1: UP{r0,w1,r1,w0,r0,w1}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+k);
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	    printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	    printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	      printf("FAIL-PAT-M1: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	      status=0;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	}
	//M2: UP{r1,w0,w1}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+k);
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	    printf("FAIL-PAT-M2: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	}
	//M3: DOWN{R1,W0,W1,W0}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *)(offset+(RAM_SIZE-k-WORD_SIZE));
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	    printf("FAIL-PAT-M3: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
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
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+(RAM_SIZE-k-WORD_SIZE));
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	    printf("FAIL-PAT-M4: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	}
	//M5:DOWN{R0,W1,R1}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+(RAM_SIZE-k-WORD_SIZE));
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	      printf("FAIL-PAT-M5: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W1
	  data_in=high_pattern;
	  (*(addr))=data_in;
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	      printf("FAIL-PAT-M5: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	}
	//M6:DOWN{R1,W0,R0}
	for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	  addr=(char *) (offset+(RAM_SIZE-k-WORD_SIZE));
	  //R1
	  expected=high_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	      printf("FAIL-PAT-M6: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	  //W0
	  data_in=low_pattern;
	  (*(addr))=data_in;
	  //R0
	  expected=low_pattern;
	  result=(*(addr));
	  if(result!=expected){
#ifdef VERBOSE
	      printf("FAIL-PAT-M6: addr=%p expected=0x%x result=0x%x\n",addr,expected,result);
#endif
	    status=0;
	  }
	}
      }
    }
  }
  //Finish Test
  return e_test_finish(status);
}

