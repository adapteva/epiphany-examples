/*
The MIT License (MIT)

Copyright (c) 2013 Adapteva, Inc

Contributor Andreas Olofsson <andreas@adapteva.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <e_test.h>

#define N 16
#define A(i,j) a_mat[j*N+i]
#define B(i,j) b_mat[j*N+i]
#define C(i,j) c_mat[j*N+i]

float  a_mat[N*N] __attribute__ ((section (".data_bank1")));//result matrix
float  b_mat[N*N] __attribute__ ((section (".data_bank2")));//result matrix
float  c_mat[N*N] __attribute__ ((section (".data_bank3")));//result matrix

int main(){

  int      i,j,k;
  float    sum  = 0.f;
  int      fail = 0;
  unsigned row  = 0;
  unsigned col  = 0;

  const char *test_name = "simple_matmul";
  
  //Test Init
  e_test_init(&row, &col);
  
  //Fill input matrices with a constant
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      A(i,j) = 1.0f;
      B(i,j) = 1.0f;
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
  if(sum!=4096.0f){
    fail=1;
    printf("FAIL: core=(%d,%d) expected=4096.0000 result=%f\n",row,col,sum); 
  }
  return e_test_finish(test_name,fail,row,col);
}


