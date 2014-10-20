#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
LOG=$PWD/emesh_bandwidth_all2one.log

cd $EXEPATH

$EXEPATH/bin/mesh_one.elf > $LOG

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi




