#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/interrupt_test.c -o bin/interrupt_test.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_interrupt_test.c -o bin/e_interrupt_test.elf -mfp-mode=round-nearest -le-lib 


# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_interrupt_test.elf bin/e_interrupt_test.srec



