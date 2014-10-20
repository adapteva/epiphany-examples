#!/bin/bash

set -e

EROW0=${EROW0:-0}
ECOL0=${ECOL0:-0}
EROWS=${EROWS:-4}
ECOLS=${ECOLS:-4}


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

$EXEPATH/bin/e-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 0  >> e-test.log

if [ $? -ne 0 ] 
then
    echo "$1 FAILED"
else
    echo "$1 PASSED"
fi

