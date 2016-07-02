#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

# Build run time dependencies
[ -f $EXEPATH/../e-test/bin/e-test.elf ] ||
    (cd $EXEPATH/../e-test && ./build.sh)

cd $EXEPATH

# Create the binaries directory
mkdir -p bin

# Build DEVICE side program
e-gcc -T ${ELDF} ../common/src/test_common.c src/e-task.c -o bin/e-task.elf -le-lib -lm -ffast-math

