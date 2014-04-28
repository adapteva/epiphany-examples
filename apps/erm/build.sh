#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include

# Create the binaries directory
mkdir -p bin/

# Build HOST application
gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal

