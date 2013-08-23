#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/register-test.c -o bin/register-test.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -T ${ELDF} src/e-register-test2.c -o bin/e-register-test2.elf -le-lib
e-gcc -T ${ELDF} src/e-register-test3.c -o bin/e-register-test3.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e-register-test2.elf bin/e-register-test2.srec
e-objcopy --srec-forceS3 --output-target srec bin/e-register-test3.elf bin/e-register-test3.srec

