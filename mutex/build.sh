#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/mutex_test.c -o bin/mutex_test.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_mutex_test0.c -o bin/e_mutex_test0.elf -le-lib 
e-gcc -O3 -T ${ELDF} src/e_mutex_test.c -o bin/e_mutex_test.elf -le-lib 

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_mutex_test0.elf bin/e_mutex_test0.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mutex_test.elf bin/e_mutex_test.srec


