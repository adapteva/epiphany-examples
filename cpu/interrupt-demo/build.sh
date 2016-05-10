#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf

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
${CROSS_COMPILE}gcc int-test.c -o int-test.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} e-int-test.master.c -o e-int-test.master.elf -le-lib
e-gcc -O0 -T ${ELDF} e-int-test.slave.c  -o e-int-test.slave.elf  -le-lib


e-objcopy --srec-forceS3 --output-target srec e-int-test.slave.elf  e-int-test.slave.srec
e-objcopy --srec-forceS3 --output-target srec e-int-test.master.elf e-int-test.master.srec
