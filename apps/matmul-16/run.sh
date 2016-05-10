#!/bin/bash



SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

LOG=$PWD/matmul_demo.log

cd $EXEPATH/host/Release

./matmul.elf $@ ../../device/Release/e_matmul.srec
