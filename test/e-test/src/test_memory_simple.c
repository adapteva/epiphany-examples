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
#include <string.h>
#include <unistd.h>

#define WORD_SIZE       (4)            //testing 32 bit integers
#define RAM_START       (0x2000)       //start after program
#define RAM_END         (0x2010)       //only test bank 1 and 2, half bank 3

int main(int argc, char *argv[]){

  int      k;
  int      status = 1 ;  //1 is pass, 0 is fail
  unsigned int coreID; 

  unsigned int     *addr;
  unsigned int data_in;
  unsigned int result;
  unsigned int expected;
  
  //Test Init
  coreID=e_test_init();

  //Testing local memory
  for(k=RAM_START;k<(RAM_END);k=(k+WORD_SIZE)){
    addr=( unsigned int * ) k;
    data_in= (unsigned ) k;	  
    //Writing in data
    (*(addr))=data_in;    
    //Reading back data
    result=(*(addr));
    if(result!=data_in){
      status=0;//fail
    }
    //Inverting data
    data_in=data_in^0xffff;
    //Writing in data
    (*(addr))=data_in;
    //Reading back data
    result=(*(addr));
    if(result!=data_in){
      status=0;//fail
    }
  }
  //Finish Test
  return e_test_finish(status);
}

