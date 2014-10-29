#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
LOG=$PWD/emesh_bandwidth_all2one.log

cd $EXEPATH/bin

./mesh_one.elf > $LOG

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval




