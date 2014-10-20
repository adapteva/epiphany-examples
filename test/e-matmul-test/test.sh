#!/bin/bash



SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/../e-test/bin/e-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 0 $EXEPATH/bin/e-task.srec > e-matmul-test.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

