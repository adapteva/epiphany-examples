#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ERM=../erm
ERMI=${ERM}/src
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/main.c ${ERM}/src/ermlib.c -o bin/main.elf -I ${ERMI} -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_demo.c -o bin/e_demo.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_demo.elf bin/e_demo.srec

