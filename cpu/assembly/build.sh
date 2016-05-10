#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

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
e-gcc -T ${ELDF} src/emain.c -o bin/emain.elf -le-lib






e-objcopy --srec-forceS3 --output-target srec bin/emain.elf bin/emain.srec
