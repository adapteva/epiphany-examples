#!/bin/bash


ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

export LD_LIBRARY_PATH=${ELIBS}

$EXEPATH/../e-test/bin/e-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 0 $EXEPATH/bin/e-task.srec > e-matmul-test.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

