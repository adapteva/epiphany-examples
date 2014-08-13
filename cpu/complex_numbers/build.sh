#!/bin/bash

set -e

# Create the binaries directory
mkdir -p bin/

#Building Kernel
e-gcc src/complex_numbers.c -o bin/complex_numbers.elf -lm -le-lib -std=c99

