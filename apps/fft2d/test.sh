#!/bin/bash


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

LOG=$PWD/fft2d.log

cd $EXEPATH/host/Release

./fft2d_host.elf $EXEPATH/lenna.jpg > $LOG

if [ $? -ne 0 ] 
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi
