#!/bin/bash

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

e-reset

export LD_LIBRARY_PATH=${ELIBS}
$EXEPATH/bin/e-dump-mem.elf $EROW0 $ECOL0 $EROWS $ECOLS > e-dump-mem.log

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi




