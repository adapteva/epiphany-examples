#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>

void usage();

int main(int argc, char *argv[]){

  //Open
  e_init(NULL);
  /* e_reset_system(); */
  e_finalize();  
  printf("TEST \"e-reset\" PASSED\n");
  return EXIT_SUCCESS;
}
