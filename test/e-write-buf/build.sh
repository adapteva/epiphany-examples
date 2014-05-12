#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/e-write-buf.c -o bin/e-write-buf.elf -I ${EINCS} -L ${ELIBS} -le-hal


