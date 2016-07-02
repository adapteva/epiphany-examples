#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

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
${CROSS_COMPILE}gcc -O3 -Wall src/e-bandwidth-test-host.c -o bin/e-bandwidth-test-host.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread -lrt

# Build DEVICE side program
e-gcc -O3  -T ${ELDF} src/e-bandwidth-test-device.c -o bin/e-bandwidth-test-device.elf -le-lib -lm -ffast-math
