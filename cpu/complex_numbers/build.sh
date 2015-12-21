#!/bin/bash

set -e

# Create the binaries directory
mkdir -p bin/

#Building Kernel
# Pretend we have entire memory region
e-gcc -Os -Wl,--section-start=.init=0x200 -Ttext=0x300 -Tdata=0xb000 -Wl,--defsym=__stack=0xefff0 src/complex_numbers.c -o bin/complex_numbers.elf -lm -le-lib -std=c99

