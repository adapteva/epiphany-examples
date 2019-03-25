#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include <e-loader.h>

void e_check_test(void *dev, unsigned row, unsigned col, int *status, int verbose);
void usage();

int main(int argc, char *argv[]){

  e_loader_diag_t e_verbose;
  e_platform_t platform;
  e_epiphany_t dev;
  int row0,col0,rows,cols,para;
  char elfFile[4096];
  int status=1;//pass
  int i,j;
  int verbose=0;

  if (argc < 7){
    usage();
    exit(EXIT_FAILURE);
  }  
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
    para    = atoi(argv[5]);
    strcpy(elfFile, argv[6]);

    //Initalize Epiphany device
    e_init(NULL);

    e_reset_system();
    e_get_platform_info(&platform);                          
    //e_set_host_verbosity(H_D3);
    e_open(&dev, 0, 0, platform.rows, platform.cols); //open all cores
    
    //Load program one at a time, checking one a time
    if(para){
      printf("Running in parallel\n");
      for (i=row0; i<(row0+rows); i++) {
	for (j=col0; j<(col0+cols); j++) {   
	  e_load_group(elfFile, &dev, i, j, 1, 1, E_TRUE);
	}
      }    
    }  
    else{
      e_load_group(elfFile, &dev, row0, col0, rows, cols, E_TRUE);
    }

    usleep(100000);

    //Checking the test
    for (i=row0; i<(row0+rows); i++) {
      for (j=col0; j<(col0+cols); j++) {   
	e_check_test(&dev, i, j, &status,verbose);
      }
    }


    //Close down Epiphany device
    e_close(&dev);
    e_finalize();
  }

  //self check
  if(status){
    printf("TEST %s PASSED\n", elfFile);
    return EXIT_SUCCESS;
  }
  else{
    printf("TEST %s FAILED\n", elfFile);
    return EXIT_FAILURE;
  }    
}
/**********************************************************************/
void e_check_test( void *dev, 
		   unsigned row, 
		   unsigned col, 
		   int *status,
		   int verbose
		  ){

  unsigned int result;
  int wait=1;

  while(1){
    e_read(dev,row, col, 0x24, &result, sizeof(unsigned));
    if(result==0xDEADBEEF){
      printf("Core (%d,%d) FAILED\n",row,col);
      *status=0;
      break;
    }
    else if(result==0x12345678){
      unsigned clr= ( unsigned ) 0x0;
      e_write(dev,row, col, 0x24, &clr, sizeof(clr));
      if(verbose){
	  printf("Core (%d,%d) PASSED\n",row,col);
	}
      break;
    }
    else{
      if(wait){
	usleep(10000);      
	if(verbose){
	  printf("Core (%d,%d) WAITING...\n",row,col);
	}
	wait=0;
      }
      else{
	if(verbose){
	  printf("Core (%d,%d) FAILED\n",row,col);
	}
	*status=0;
	break;
      }
    }
  }		  
}
/**********************************************************************/
void usage(){
 printf("-----------------------------------------------\n");
  printf("Function: Runs exhaustive march-c memory test\n");
  printf("Usage:    e-test <row> <col> <rows> <cols> <elf>\n");
  printf("Example:  e-test 0 0 4 4 1 my.elf\n");
  printf("\n");
  printf("Options:\n");
  printf("  row     - target core start row coordinate\n");
  printf("  col     - target core start column coordinate\n");
  printf("  rows    - number of rows to test\n");
  printf("  cols    - number of columns to test\n");
  printf("  para    - run test in parallel\n");
  printf("  elf     - path to elf file\n");
  printf("-----------------------------------------------\n");
}
