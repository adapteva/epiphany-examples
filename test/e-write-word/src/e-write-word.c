#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <e-hal.h>

void usage();

int main(int argc, char *argv[]){

  e_epiphany_t dev; 
  unsigned int data_in;

  //Open
  e_init(NULL);
  e_reset_system();
  e_open(&dev, 0, 0, 1, 1);
  e_write(&dev, 0, 0, 0, &data_in, sizeof(data_in));
  e_close(&dev);
  e_finalize();  
  printf("TEST \"e-write-word\" PASSED\n");
  return EXIT_SUCCESS;
}
