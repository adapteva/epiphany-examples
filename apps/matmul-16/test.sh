#!/bin/bash



SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

LOG=$PWD/matmul-16.log

cd $EXEPATH/host/Release

./matmul.elf $@ ../../device/Release/e_matmul.elf > $LOG

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED!!!!"
else
    echo "$SCRIPT PASSED"
fi

exit $retval
