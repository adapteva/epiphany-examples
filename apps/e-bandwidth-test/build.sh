#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/e-bandwidth-test-host.c -o bin/e-bandwidth-test-host.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O3  -T ${ELDF} src/e-bandwidth-test-device.c -o bin/e-bandwidth-test-device.elf -le-lib -lm -ffast-math

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e-bandwidth-test-device.elf bin/e_bandwidth-test-device.srec
