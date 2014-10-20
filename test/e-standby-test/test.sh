#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

TEST="e-standby-test"

cd $EXEPATH/bin
./$TEST.elf > $TEST.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

