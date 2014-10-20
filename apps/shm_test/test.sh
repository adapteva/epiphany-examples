#!/bin/bash

set -e


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

cd $EXEPATH/bin
./shm_test.elf > shm_test.log
cd -

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi
