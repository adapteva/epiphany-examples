#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin

# Build DEVICE side program
e-gcc -T ${ELDF} ../common/src/test_common.c src/e-task.c -o bin/e-task.elf -le-lib -lm -ffast-math

e-objcopy --srec-forceS3 --output-target srec bin/e-task.elf bin/e-task.srec
