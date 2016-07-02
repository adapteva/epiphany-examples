#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf
ELDF_INTERNAL=${ESDK}/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

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

mkdir -p bin

# Build HOST side application
${CROSS_COMPILE}gcc src/hello_world.c -o bin/hello_world.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Test fast.ldf (also device side program)
e-gcc -T ${ELDF} src/e_hello_world.c -o bin/e_hello_world.elf -le-lib

# Remove .shared_dram section to create empty segment
e-objcopy -R .shared_dram bin/e_hello_world.elf 2>&1 | grep -q "warning: Empty loadable segment detected" && exit 0
echo "ERROR, could not create empty segment"
exit 1
