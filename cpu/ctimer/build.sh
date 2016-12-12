#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
#ELDF=${ESDK}/bsps/current/legacy.ldf
ELDF=${ESDK}/bsps/current/legacy.ldf

# Create the binaries directory
mkdir -p bin/

if [ -z "${CROSS_COMPILE+xxx}" ]; then
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_COMPILE=
		;;
	   *)
		# Use cross compiler
		CROSS_COMPILE="arm-linux-gnueabihf-"
		;;
esac
fi

# Build HOST side application
${CROSS_COMPILE}gcc src/main.c -o bin/main.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program

   e-gcc -g -Wall -O3 -std=c99 \
         -mlong-calls \
         -mfp-mode=round-nearest \
         -ffp-contract=fast \
         -funroll-loops \
         -T ${ELDF} \
         -o bin/emain.elf \
         src/emain.c -le-lib





