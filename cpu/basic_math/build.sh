#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
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
${CROSS_PREFIX}gcc src/math_test.c -o bin/math_test.elf ${EINCS} ${ELIBS} -le-hal -lm -le-loader -lpthread

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_math_test.c -o bin/e_math_test.elf -mfp-mode=round-nearest -le-lib -lm -ffast-math
e-gcc -O3 -T ${ELDF} src/e_math_test1.c -o bin/e_math_test1.elf -mfp-mode=round-nearest -le-lib -lm -ffast-math

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_math_test.elf bin/e_math_test.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_math_test1.elf bin/e_math_test1.srec


