#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

#define RAM_SIZE   (0x8000)

int main(int argc, char *argv[]){

  unsigned int read_buffer[RAM_SIZE/4];
  e_epiphany_t dev; 
  int k;

  //Open
  e_init(NULL);
  e_reset_system();
  e_open(&dev, 0, 0, 1, 1);
  e_read(&dev, 0, 0, 0x0, &read_buffer, RAM_SIZE);
  e_close(&dev);
  e_finalize();  
  printf("TEST \"e-read-buf\" PASSED\n");
  return EXIT_SUCCESS;
}
