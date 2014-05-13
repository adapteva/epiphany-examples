#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/mesh_bandwidth.c -o bin/mesh_bandwidth.elf -I ${EINCS} -L ${ELIBS} -le-hal


# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_mesh_bandwidth_near.c -o bin/e_mesh_bandwidth_near.elf -le-lib 


# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bandwidth_near.elf bin/e_mesh_bandwidth_near.srec



