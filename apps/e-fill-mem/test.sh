#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

e-reset

$EXEPATH/bin/e-fill-mem.elf $EROW0 $ECOL0 $EROWS $ECOLS 8192 0x12345678 > e-fill-mem.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

