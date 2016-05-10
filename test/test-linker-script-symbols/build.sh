#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf
ELDF_INTERNAL=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

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

mkdir -p bin

# Build HOST side application
${CROSS_COMPILE}gcc src/hello_world.c -o bin/hello_world.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Test fast.ldf (also device side program)
e-gcc -T ${ELDF} src/e_hello_world.c -o bin/e_hello_world.elf -le-lib

# Test internal.ldf (compile only)
e-gcc -T ${ELDF_INTERNAL} src/symbols_internal.c -o bin/symbols_internal.elf

e-objcopy --srec-forceS3 --output-target srec bin/e_hello_world.elf bin/e_hello_world.srec
e-objcopy --srec-forceS3 --output-target srec bin/symbols_internal.elf bin/symbols_internal.srec

