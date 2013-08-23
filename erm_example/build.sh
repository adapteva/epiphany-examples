#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/main.c src/ermlib.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal 

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_demo.c -o bin/e_demo.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_demo.elf bin/e_demo.srec

