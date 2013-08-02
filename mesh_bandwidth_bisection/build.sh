#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/mesh_bi.c -o bin/mesh_bi.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -O0 -T ${ELDF} src/e_mesh_bi00.c -o bin/e_mesh_bi00.elf -le-lib 
e-gcc -O0 -T ${ELDF} src/e_mesh_bi.c -o bin/e_mesh_bi.elf -le-lib
e-gcc -O0 -T ${ELDF} src/e_mesh_bi1.c -o bin/e_mesh_bi1.elf -le-lib

    
# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi00.elf bin/e_mesh_bi00.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi.elf bin/e_mesh_bi.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_mesh_bi1.elf bin/e_mesh_bi1.srec


