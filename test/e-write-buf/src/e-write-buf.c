#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>


#define RAM_SIZE   (0x8000)
#define N          RAM_SIZE/4

int main(int argc, char *argv[]){

  e_epiphany_t dev; 
  unsigned int write_buffer[N];
  int i;

  for (i=0; i<N; i++){
    write_buffer[i] = 0x12345678;
  }

  //Open
  e_init(NULL);
  e_reset_system();
  e_open(&dev, 0, 0, 1, 1);  
  e_write(&dev, 0, 0, 0, &write_buffer, N*sizeof(int));
  e_close(&dev);
  e_finalize();  
  printf("TEST \"e-write-buf\" PASSED\n");
  return EXIT_SUCCESS;
}
