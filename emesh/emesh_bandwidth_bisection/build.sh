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
${CROSS_COMPILE}gcc src/mesh_bi.c -o bin/mesh_bi.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_mesh_bi00.c -o bin/e_mesh_bi00.elf -le-lib 
e-gcc -O0 -T ${ELDF} src/e_mesh_bi.c -o bin/e_mesh_bi.elf -le-lib
e-gcc -O0 -T ${ELDF} src/e_mesh_bi1.c -o bin/e_mesh_bi1.elf -le-lib

    


e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi.elf bin/e_mesh_bi.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi1.elf bin/e_mesh_bi1.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi00.elf bin/e_mesh_bi00.srec
