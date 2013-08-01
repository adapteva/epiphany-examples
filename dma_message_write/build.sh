#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/dma_message_test.c -o bin/dma_message_test.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_dma_message_test.c -o bin/e_dma_message_test.elf -le-lib 
e-gcc -O3 -T ${ELDF} src/e_dma_message_slave_test.c -o bin/e_dma_message_slave_test.elf -le-lib 

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_dma_message_test.elf bin/e_dma_message_test.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_dma_message_slave_test.elf bin/e_dma_message_slave_test.srec


