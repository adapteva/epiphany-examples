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
${CROSS_COMPILE}gcc src/e-toggle-led.c -o bin/e-toggle-led.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -T ${ELDF} src/device-e-toggle-led.c -o bin/device-e-toggle-led.elf -le-lib



