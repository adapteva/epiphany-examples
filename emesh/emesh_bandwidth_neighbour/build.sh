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
${CROSS_PREFIX}gcc src/mesh_bandwidth.c -o bin/mesh_bandwidth.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread


# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_mesh_bandwidth_near.c -o bin/e_mesh_bandwidth_near.elf -le-lib 


# Convert ebinary to SREC file



