#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf
#ELDF=src/int-code-ext-stack.ldf

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
${CROSS_COMPILE}gcc src/main.c -o bin/main.elf  ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
OPT=0
e-gcc -T ${ELDF} src/emain.c -O$OPT -o bin/emain.elf -le-lib
#e-gcc src/emain.c -S -O$OPT -o bin/emain.O$OPT.s


