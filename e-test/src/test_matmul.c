/*
  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Andreas Olofsson <andreas@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <stdio.h>

#define N 16
#define A(i,j) a_mat[j*N+i]
#define B(i,j) b_mat[j*N+i]
#define C(i,j) c_mat[j*N+i]

float  a_mat[N*N] __attribute__ ((section (".data_bank1")));//result matrix
float  b_mat[N*N] __attribute__ ((section (".data_bank2")));//result matrix
float  c_mat[N*N] __attribute__ ((section (".data_bank3")));//result matrix

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
    status=0;         //fail
  }
  //Finish Test
  return e_test_finish(status);
}

