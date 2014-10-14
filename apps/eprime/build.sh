#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

gcc -O3 -o test_host src/test_host.c src/isprime.c -lm

# Build HOST side application
gcc src/prime.c -o prime.elf -I ${EINCS} -L ${ELIBS} -le-hal -le-loader

# Build DEVICE side program
e-gcc -O2 -T ${ELDF} src/isprime.c src/e_prime.c -o e_prime.elf -le-lib -lm

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec e_prime.elf e_prime.srec
