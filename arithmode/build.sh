#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin/

# Build HOST side application
gcc src/main.c -o bin/main.elf -I ${EINCS} -L ${ELIBS} -le-hal

# Build DEVICE side program
e-gcc -Os -T ${ELDF} src/e_task.c -o bin/e_task.elf -le-lib -lm -ffast-math


# Build DEVICE side program
e-gcc -Os -T ${ELDF} src/e_task.c -o bin/e_task.elf -le-lib -lm -ffast-math

e-gcc -Os -mfp-mode=int -T ${ELDF} src/e_task.c -o bin/e_task_imode.elf -le-lib -lm -ffast-math

e-gcc -Os -mfp-mode=round-nearest -T ${ELDF} src/e_task.c -o bin/e_task_fmode.elf -le-lib -lm -ffast-math

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_task.elf bin/e_task.srec
