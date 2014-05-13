#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} $EXEPATH/bin/e-test.elf 0 0 $EROWS $ECOLS $1  >> e-test.log

if [ $? -ne 0 ] 
then
    echo "$1 FAILED"
else
    echo "$1 PASSED"
fi

