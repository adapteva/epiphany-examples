#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")




#dumping disassembly
e-objdump -D $EXEPATH/bin/e_task.elf > DUMP_CALLER
e-objdump -D $EXEPATH/bin/e_task_imode.elf > DUMP_INT
e-objdump -D $EXEPATH/bin/e_task_fmode.elf > DUMP_FLOAT

#running program
cd $EXEPATH/bin
./main.elf

