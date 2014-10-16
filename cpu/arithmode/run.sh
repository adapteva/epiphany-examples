#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}



#dumping disassembly
e-objdump -D bin/e_task.elf > DUMP_CALLER
e-objdump -D bin/e_task_imode.elf > DUMP_INT
e-objdump -D bin/e_task_fmode.elf > DUMP_FLOAT

cd bin

#running program
sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./main.elf

