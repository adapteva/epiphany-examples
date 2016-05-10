#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

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
${CROSS_COMPILE}gcc src/math_test.c -o bin/math_test.elf ${EINCS} ${ELIBS} -le-hal -lm -le-loader -lpthread

# Build DEVICE side program
e-gcc -O3 -T ${ELDF} src/e_math_test.c -o bin/e_math_test.elf -mfp-mode=round-nearest -le-lib -lm -ffast-math
e-gcc -O3 -T ${ELDF} src/e_math_test1.c -o bin/e_math_test1.elf -mfp-mode=round-nearest -le-lib -lm -ffast-math



e-objcopy --srec-forceS3 --output-target srec bin/e_math_test1.elf bin/e_math_test1.srec
e-objcopy --srec-forceS3 --output-target srec bin/e_math_test.elf bin/e_math_test.srec
