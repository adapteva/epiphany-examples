#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
#ELDF=${ESDK}/bsps/current/internal.ldf
#ELDF=${ESDK}/bsps/current/legacy.ldf
ELDF=src/int-code-ext-stack.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
OPT=0
e-gcc -T ${ELDF} src/emain.c src/mpf_isr.s -O$OPT -o bin/emain.elf -le-lib
#e-gcc src/emain.c -S -O$OPT -o bin/emain.O$OPT.s

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/emain.elf bin/emain.srec

