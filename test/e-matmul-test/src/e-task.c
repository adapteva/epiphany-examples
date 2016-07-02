#include <stdlib.h>
#include <stdio.h>

#define N 16
#define A(i,j) a_mat[j*N+i]
#define B(i,j) b_mat[j*N+i]
#define C(i,j) c_mat[j*N+i]

float  a_mat[N*N] __attribute__ ((section (".data_bank1")));//result matrix
float  b_mat[N*N] __attribute__ ((section (".data_bank2")));//result matrix
float  c_mat[N*N] __attribute__ ((section (".data_bank3")));//result matrix

float sfrand(void);

static unsigned int mirand =1;

int main(int argc, char *argv[]){
  int      i,j,k;
  float    sum  = 0.f;  
  int      status = 1;
  unsigned coreID;

  //Test Init
  coreID=e_test_init();

  //Fill input matrices with a constant
  for (i=0; i<N; i++){
     for (j=0; j<N; j++){
       A(i,j) = sfrand();
       B(i,j) = sfrand();
     }
  }

  //Run matrix multiplication
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      C(i,j) = 0;
      for (k=0; k<N; k++){
         C(i,j) += A(i,k)*B(k,j);
      }
    }
  }

  //Sum up the C matrix
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      sum+=C(i,j);
    }
  }
  
  //Compare to expected result
  // printf("sum=%f\n",sum);

  //sum!=-9.114673f
  if(sum!=-9.114673f){//testing for bit exact sum from Epiphany golden chip(s)
    status=0;         //fail
  }
  //Finish Test
  return e_test_finish(status);
}

//Weird and cool random floating point generator from:
//www.rgba.org/articles/sfrand/sfrand.htm
float sfrand( void){
  unsigned int a;
  mirand *=16807;
  a = (mirand & 0x007fffff) | 0x40000000;
  return ( *((float*)&a) - 3.0f );
}

