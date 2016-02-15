#!/bin/bash

set -e

have_mem_arg=$(e-run --help 2>&1 | grep -q -- "--e-core-mem"; echo $?)
arg=
if [ $have_mem_arg -eq 0 ]; then
    arg="--e-core-mem 1M"
fi

e-run --e-coreid 0x808 --e-rows 1 --e-cols 1 ${arg} bin/complex_numbers.elf
