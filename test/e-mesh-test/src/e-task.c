#include <stdlib.h>
#include <stdio.h>

#define WORD_SIZE       (4)
#define BUF_SIZE        (0x100)
#define ROWS            4
#define COLS            4
#define BANK2           (0x00004000)
#define START           (0x80800000)
//#define VERBOSE

//TODO: Fix for 64-core
//TODO: starting core and row/col should be inputs

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
  unsigned data;
  //Test Init
  coreID=e_test_init();
  row = (coreID >> 26) & 0x7;
  col = (coreID >> 20 )& 0x7;

  //Testing all cores
  for(i=0;i<ROWS;i++){
    for(j=0;j<COLS;j++){
      offset=START+(i<<26)+(j<<20)+BANK2+(row*ROWS+col)*BUF_SIZE;
      dummy = (unsigned *) (offset + BUF_SIZE-WORD_SIZE);
      if(1){
	if(!(offset==coreID)){

	  //Write PAT0
	  data=(PAT0 & 0xFFFF0000) | (row*ROWS+col);
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    (*(addr))= data;
	  }
	  //Memory Ordering Sync (to get around RAW races)
	  e_write_ack(dummy);  
	  //Verify PAT0
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    result=(*(addr));
	    if(result!=data){
	      status=0;
	    }
	  }	
	  //Write PAT1
	  data=( (PAT1 & 0x0000FFFF) | ((row*ROWS+col)<<16));
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    (*(addr))= data;	      
	  }
	  //Memory Ordering Sync (to get around RAW races)
	  e_write_ack(dummy);  
	  //Verify PAT0
	  for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	    addr=(unsigned *) (offset+k);
	    result=(*(addr));
	    if(result!= data){
	      status=0;
	    }
	  }
	}
      }
    }  
  }
  //Finish Test
  return e_test_finish(status);
}


