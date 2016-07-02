#!/bin/bash
COUNTER=0
while [ true ]
 do
   echo LOOP# $COUNTER
  ./test.sh || break
  let COUNTER=COUNTER+1
done
