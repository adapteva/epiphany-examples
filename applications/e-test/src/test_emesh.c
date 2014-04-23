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

#define WORD_SIZE       (4)
#define BUF_SIZE        (0x10)
#define ROWS            4
#define COLS            4
#define BANK2           (0x00004000)
#define START           (0x80800000)
//#define VERBOSE

int main(int argc, char *argv[]){
  int      i,j,k;
  unsigned int offset;
  unsigned coreID,row,col; 

  unsigned *addr;
  unsigned *dummy;

  int status = 1;
  unsigned int result;

  unsigned PAT0 = 0x55555555;
  unsigned PAT1 = 0xAAAAAAAA;

  //Test Init
  coreID=e_test_init();
  row = (coreID >> 26) & 0x7;
  col = (coreID >> 20 )& 0x7;

  //Testing all cores
  for(i=0;i<ROWS;i++){
    for(j=0;j<COLS;j++){
      offset=START+(i<<26)+(j<<20)+BANK2+(row*ROWS+col)*BUF_SIZE;
#ifdef VERBOSE
	    printf("offset=%x, id=%x, i=%d, j=%d, row=%d, col=%d\n",offset,coreID,i,j,row,col);
#endif   
      dummy = (unsigned *) (offset + BUF_SIZE-WORD_SIZE);
      if(1){
	if(!(offset==coreID)){
	  //Write PAT0
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    (*(addr))= ( PAT0 | k );
#ifdef VERBOSE
	    printf("addr=%p, id=%x, i=%d, j=%d, row=%d, col=%d\n",addr,coreID,i,j,row,col);
#endif 
	  }
	  //Memory Ordering Sync (to get around RAW races)
	  e_write_ack(dummy);  
	  //Verify PAT0
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    result=(*(addr));
	    if(result!= ( PAT0  | k )){
	      status=0;
#ifdef VERBOSE
	      printf("FAIL: core=(%d,%d) addr=(0x%x) result=0x%x\n",i,j,k,result); 
#endif
	    }
	  }	
	  //Write PAT1
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    (*(addr))= ( PAT1 | k );
	  }
	  //Memory Ordering Sync (to get around RAW races)
	  e_write_ack(dummy);  
	  //Verify PAT0
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    result=(*(addr));
	    if(result!= ( PAT1  | k )){
	      status=0;
#ifdef VERBOSE
	      printf("FAIL: core=(%d,%d) addr=(0x%x) result=0x%x\n",i,j,k,result); 
#endif
	    }
	  }
	}
      }
    }  
  }
  //Finish Test
  return e_test_finish(status);
}


