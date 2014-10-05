#include <stdlib.h>

unsigned e_test_init(){

  int *pass = (int*)0x24;      //use user interrupt entry for now
  *pass   = 0x00000000;        //initialize as "started"

  //Resetting mask register
  __asm__ __volatile__ ("MOVTS IMASK, %0" : : "r" (0x0));

  //Get the coreID using assembly
  register unsigned coreid_in_reg asm("r0");
  __asm__ __volatile__ ("MOVFS %0, COREID" : : "r" (coreid_in_reg));
  coreid_in_reg = coreid_in_reg << 20;
  return (coreid_in_reg);
}

int e_test_finish(int status){

  int *pass = (int*)0x24;    //overwrite the sync ivt entry on exit
  if(status==1){
    *pass = 0x12345678;
  }
  else{
    *pass = 0xDEADBEEF;
  }
  while(1);
}

void e_write_ack(unsigned *addr){
  unsigned probe_data;
  probe_data = (*(addr));              //read old data
  probe_data = probe_data ^ 0xFFFFFFFF;//toggle old data
  (*(addr)) = probe_data;              //write new toggled data
  while(probe_data != *(addr)){        //keep reading until match is met
  }
}
