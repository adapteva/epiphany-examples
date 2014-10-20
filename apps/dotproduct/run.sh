#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


e-reset

cd $EXEPATH/bin; ./main.elf 0 0 4 4 0x12345678

#> dotproduct.log

if [ $? -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi
