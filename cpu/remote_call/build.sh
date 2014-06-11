#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

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
${CROSS_PREFIX}gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal #-le-loader

# Build DEVICE side program
e-gcc -T ${ELDF} src/emain_master.c -o bin/emain_master.elf -le-lib
#
e-gcc -T ${ELDF} src/emain_slave.c -o bin/emain_slave.elf -le-lib



# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/emain_slave.elf bin/emain_slave.srec
e-objcopy --srec-forceS3 --output-target srec bin/emain_master.elf bin/emain_master.srec


