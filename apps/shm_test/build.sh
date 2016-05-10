#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

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

if [ ! -d ./bin ]; then
	mkdir ./bin
fi

# Build HOST side application
${CROSS_COMPILE}gcc src/shm_test.c -g -O0 -o bin/shm_test.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -T ${ELDF} src/e_shm_test.c -o bin/e_shm_test.elf -le-lib


e-objcopy --srec-forceS3 --output-target srec bin/e_shm_test.elf bin/e_shm_test.srec
