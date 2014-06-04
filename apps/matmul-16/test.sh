#!/bin/bash


ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

LOG=$PWD/matmul-16.log

cd $EXEPATH/host/Release

sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./matmul.elf $@ ../../device/Release/e_matmul.srec > $LOG

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED!!!!"
else
    echo "$SCRIPT PASSED"
fi
