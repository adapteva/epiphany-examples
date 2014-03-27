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

   e-gcc -Wall -O3 -std=c99 \
         -mlong-calls \
         -mfp-mode=round-nearest \
         -ffp-contract=fast \
         -funroll-loops \
         -T ${EPIPHANY_HOME}/bsps/current/legacy.ldf \
         -o bin/emain.elf \
         src/emain.c -le-lib




# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/emain.elf bin/emain.srec

