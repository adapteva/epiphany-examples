#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

e-reset

$EXEPATH/bin/e-dump-regs.elf $EROW0 $ECOL0 $EROWS $ECOLS > e-dump-regs.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi
