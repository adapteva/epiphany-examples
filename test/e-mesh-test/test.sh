#!/bin/bash

# Default values if none are provided
EROW0=${EROW0:-0}
ECOL0=${ECOL0:-0}
EROWS=${EROWS:-4}
ECOLS=${ECOLS:-4}

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/../e-test/bin/e-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 0 $EXEPATH/bin/e-task.elf > e-matmul-test.log

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval

