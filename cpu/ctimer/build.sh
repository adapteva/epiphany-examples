#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

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
${CROSS_PREFIX}gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal #-le-loader

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

