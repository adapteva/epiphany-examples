#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>
#include <e-loader.h>

#define RAM_SIZE     (0x8000)
#define WORD_SIZE    (1)

void usage();
int my_reset_system();

int main(int argc, char *argv[]){

  //----------------------------
  e_platform_t platform;
  e_epiphany_t dev;
  e_loader_diag_t e_verbose;
  unsigned int i,j,k,addr;
  unsigned int data;
  int status=1;//pass

  int row0,col0,rows,cols;
  int verbose=0;

  char result,data_in,expected;
  char high_pattern = 0xff;
  char low_pattern  = 0x00;
  

  if (argc < 5){
    usage();
    exit(1);
  }  
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
  }
  //Open
  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  //e_set_host_verbosity(H_D3);

  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {   
      if(verbose){
	printf("Running host march-C read/write test for core (%d,%d)\n", i,j);      }
      //M0: UP{w0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr= k;
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, k, &data_in, sizeof(data_in));
      }
      //M1: UP{r0,w1,r1,w0,r0,w1}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr= k;
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R1
	expected=high_pattern;	  
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M2: UP{r1,w0,w1}		
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(k);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M2: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result);
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M3: DOWN{R1,W0,W1,W0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M3: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 	
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M4:DOWN{R0,W1,W0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M4: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M5:DOWN{R0,W1,R1}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M5: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M5: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
      }
      //M6:DOWN{R1,W0,R0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M6: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M6: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();

  //Self Check
  if(status){
    printf("TEST \"e-mem-test\" PASSED\n");
    return EXIT_SUCCESS;
  }
  else{
    printf("TEST \"e-mem-test\" FAILED\n");
    return EXIT_FAILURE;
  }   
}
//////////////////////////////////////////////////////////////////////////
void usage()
{
  printf("-----------------------------------------------\n");
  printf("Function: Runs exhaustive march-c memory test\n");
  printf("Usage:    e-mem-test <row> <col> <rows> <cols> <v>\n");
  printf("Example:  e-mem-test 0 0 4 4 1\n");
  printf("\n");
  printf("Options:\n");
  printf("  row     - target core start row coordinate\n");
  printf("  col     - target core start column coordinate\n");
  printf("  rows    - number of rows to test\n");
  printf("  cols    - number of columns to test\n");
  printf("  v       - 1=verbose mode, 0=quiet\n");
  printf("-----------------------------------------------\n");
  return;
}
//////////////////////////////////////////////////////////////////////////
