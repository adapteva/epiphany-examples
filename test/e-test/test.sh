#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} $EXEPATH/bin/e-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 0  >> e-test.log

if [ $? -ne 0 ] 
then
    echo "$1 FAILED"
else
    echo "$1 PASSED"
fi

