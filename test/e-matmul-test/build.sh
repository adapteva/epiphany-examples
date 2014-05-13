#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin

# Build DEVICE side program
e-gcc -T ${ELDF} ../common/src/test_common.c src/e-task.c -o bin/e-task.elf -le-lib -lm -ffast-math

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e-task.elf bin/e-task.srec
