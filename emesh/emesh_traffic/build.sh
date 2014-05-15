#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

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
${CROSS_PREFIX}gcc src/mesh_traffic.c -o bin/mesh_traffic.elf -I ${EINCS} -L ${ELIBS} -le-hal -le-loader

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_mesh_transmitter.c -o bin/e_mesh_transmitter.elf -le-lib
e-gcc -O0 -T ${ELDF} src/e_mesh_receiver.c -o bin/e_mesh_receiver.elf -le-lib

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_transmitter.elf bin/e_mesh_transmitter.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_receiver.elf bin/e_mesh_receiver.srec


