#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/math_test.c -o bin/math_test.elf -I ${EINCS} -L ${ELIBS} -le-hal -lm

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_math_test.c -o bin/e_math_test.elf -mfp-mode=round-nearest -le-lib -lm #-ffast-math
e-gcc -O3 -T ${ELDF} src/e_math_test1.c -o bin/e_math_test1.elf -mfp-mode=round-nearest -le-lib -lm #-ffast-math

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_math_test.elf bin/e_math_test.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_math_test1.elf bin/e_math_test1.srec


