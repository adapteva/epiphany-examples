#include <stdlib.h>
#include <stdio.h>

#define WORD_SIZE       (4)
#define BUF_SIZE        (0x100)
#define START           (0x8e000000)
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
  offset=START+BUF_SIZE*(row*32+col);//hack!

#ifdef VERBOSE
  printf("offset=%x, id=%x, row=%d, col=%d\n",offset,coreID,row,col);
#endif   
  
  if(1){
      //Write PAT0
      for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	addr=(unsigned *) (offset+k);
	(*(addr))= ( PAT0 | k );
	//printf("WPAT0 k=%d, addr=%p, id=%x,row=%d, col=%d\n",k,addr,coreID,row,col);
      }
      //Memory Ordering Sync (to get around RAW races)
      dummy = (unsigned *) (offset + BUF_SIZE-WORD_SIZE);
      e_write_ack(dummy);  
      //Verify PAT0
      for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	addr=(unsigned *) (offset+k);
	result=(*(addr));
	if(result!= ( PAT0  | k )){
	  status=0;
#ifdef VERBOSE
	  printf("FAIL: addr=(0x%p) result=0x%x\n",addr,result); 
#endif
	}
      }	
      //Write PAT1
      for(k=0; k<(BUF_SIZE-WORD_SIZE); k=k+WORD_SIZE){
	addr=(unsigned *) (offset+k);
	(*(addr))= ( PAT1 | k );
	//printf("WPAT1 k=%d, addr=%p, id=%x,row=%d, col=%d\n",k,addr,coreID,row,col);
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
	  printf("FAIL: addr=(0x%p) result=0x%x\n",addr,result); 
#endif
	}
      }
  }
  //Finish Test
  return e_test_finish(status);
}

