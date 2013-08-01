#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/dma_message_a.c -o bin/dma_message_a.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_dma_message_a.c -o bin/e_dma_message_a.elf -le-lib 


# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_dma_message_a.elf bin/e_dma_message_a.srec



