#include <stdlib.h>
#include <stdio.h>
#include <e_lib.h>

#define LINKS     2
#define N        (0x1000) //1000

int main(int argc, char *argv[]){

  int      k, i, j;
  unsigned row = 0;
  unsigned col = 0;
  int status   = 1;

  unsigned int offset = 0x4000;
  
  int fail    = 0;
  int verbose = 1;


  unsigned  buffers[LINKS];

  //Addresses outside the chip to write
  //assuming that chip sits at 0x82400000
  buffers[0] = 0x40800000; //north
  buffers[1] = 0x90800000; //south

  unsigned int result;
  unsigned coreID;

  unsigned PAT0 = 0x55555555;
  unsigned PAT1 = 0xAAAAAAAA;

  volatile unsigned *addr;
  volatile unsigned *local_addr;

  //Test Init
  coreID=e_test_init();

  //Configure North-Link
  addr=(unsigned *) (0x80800000 + 0x002F0328); //on-chip IO register address
  e_reg_write(E_REG_CONFIG, (unsigned) 0x1000); //"direct north routing mode"
  (*(addr)) = 0x00004000;                      //enable loopback mode
  e_reg_write(E_REG_CONFIG, (unsigned) 0x0000); //clear out direct routing mode

  //Configure South-Link
  addr = (unsigned *) (0x80800000 + 0x0C2F0328);
  e_reg_write(E_REG_CONFIG, (unsigned) 0x1000);
  (*(addr)) = 0x00004000;                      //enable loopback mode
  e_reg_write(E_REG_CONFIG, (unsigned) 0x0000);
    
  //Testing link loopback
  for(i=0; i<LINKS; i++){
    for(k=0; k<N; k=k+sizeof(int)){
      addr = (unsigned *) (buffers[i] + offset + k);   //writing to global address through link
      local_addr = (unsigned *) (offset+k);            //reading from local 0x000xxxxx memory
      (*(addr)) = PAT0;
      for(j=0; j<500; j++){
	__asm__ __volatile__ ("NOP;"); //wait loop to make sure data has returned
      }
      result = (*(local_addr));
      if(result != PAT0){
	status = 0;
      }
      /*PAT1*/
      (*(addr)) = PAT1;
      for(j=0; j<500; j++){
	__asm__ __volatile__ ("NOP;");
      }
      result = (*(local_addr));
      if(result != PAT1){
	status = 0;
      }
    }
  }
  //Finish Test
  return e_test_finish(status);
}


