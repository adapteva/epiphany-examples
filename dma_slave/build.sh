#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Build HOST side application
gcc dma_slave_test.c -o dma_slave_test.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} e_dma_slave_test.c -o e_dma_slave_test.elf -le-lib 
e-gcc -O0 -T ${ELDF} e_dma_slave_test1.c -o e_dma_slave_test1.elf -le-lib 

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec e_dma_slave_test.elf e_dma_slave_test.srec
e-objcopy --srec-forceS3 --output-target srec e_dma_slave_test1.elf e_dma_slave_test1.srec


