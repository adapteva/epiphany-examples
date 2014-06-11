#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

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
${CROSS_PREFIX}gcc int-test.c -o int-test.elf -I ${EINCS} -L ${ELIBS} -le-hal #-le-loader

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} e-int-test.master.c -o e-int-test.master.elf -le-lib
e-gcc -O0 -T ${ELDF} e-int-test.slave.c  -o e-int-test.slave.elf  -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec e-int-test.master.elf e-int-test.master.srec
e-objcopy --srec-forceS3 --output-target srec e-int-test.slave.elf  e-int-test.slave.srec

