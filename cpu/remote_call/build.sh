#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -T ${ELDF} src/emain_master.c -o bin/emain_master.elf -le-lib
#
e-gcc -T ${ELDF} src/emain_slave.c -o bin/emain_slave.elf -le-lib



# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/emain_slave.elf bin/emain_slave.srec
e-objcopy --srec-forceS3 --output-target srec bin/emain_master.elf bin/emain_master.srec


