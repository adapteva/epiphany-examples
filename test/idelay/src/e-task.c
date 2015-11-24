
/*A bare minimum self test function function */
#include <e-lib.h>
#include "common.h"

int main(void){

  int *dram   = (int*)0x8e000000;  
  int *local  = (int*)0x4000;  
  int *result = (int*)0x6000;  
  int *pass   = (int*)0x8e000000;    //overwrite the sync ivt entry on exit
  int i;

  //CREATE DATA
  for (i=0;i<N;i++){
    *(local+i) =0x55555555;
    *(result+i)=0xDEADBEEF;
  }

  //WRITE TO DRAM  
  e_dma_copy(dram, local, N*sizeof(int));

  //READ FROM DRAM
  e_dma_copy(result,dram, N*sizeof(int));

  int check=1;
  for (i=0;i<N;i++){
    if(*(result+4*i)!=0x55555555){
      check=0;
    }
  }
  if(check){
    //PRINT SUCCESS IF PASSED
    *pass = 0x12345678;
  }
  else{
    *pass = 0xFFFFFFFF;//-1
  }
  //while(1);
}
