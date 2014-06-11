#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ERM=../erm
ERMI=${ERM}/src
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

CROSS_PREFIX=
case $(uname -p) in
    arm*)
        # Use native arm compiler (no cross prefix)
        CROSS_PREFIX=
        ;;
       *)
        # Use cross compiler
        CROSS_PREFIX="arm-linux-gnueabihf-"
        ;;
esac

# Build HOST side application
${CROSS_PREFIX}gcc src/main.c ${ERM}/src/ermlib.c -o bin/main.elf -I ${ERMI} -I ${EINCS} -L ${ELIBS} -le-hal #-le-loader

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_demo.c -o bin/e_demo.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_demo.elf bin/e_demo.srec

