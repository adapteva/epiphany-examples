#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf

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
${CROSS_COMPILE}gcc src/main.c -o bin/main.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-gcc -Os -T ${ELDF} src/e_task.c -o bin/e_task.elf -le-lib -lm -ffast-math


# Build DEVICE side program
e-gcc -Os -T ${ELDF} src/e_task.c -o bin/e_task.elf -le-lib -lm -ffast-math

e-gcc -Os -mfp-mode=int -T ${ELDF} src/e_task.c -o bin/e_task_imode.elf -le-lib -lm -ffast-math

e-gcc -Os -mfp-mode=round-nearest -T ${ELDF} src/e_task.c -o bin/e_task_fmode.elf -le-lib -lm -ffast-math

