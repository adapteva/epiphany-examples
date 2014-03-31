#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

#Building Kernel
e-gcc src/complex_numbers.c -o bin/complex_numbers.elf -lm -le-lib -std=c99

