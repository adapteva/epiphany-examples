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
${CROSS_COMPILE}gcc src/interrupt_test.c -o bin/interrupt_test.elf  ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_nested_test.c -o bin/e_nested_test.elf -le-lib 





e-objcopy --srec-forceS3 --output-target srec bin/e_nested_test.elf bin/e_nested_test.srec
