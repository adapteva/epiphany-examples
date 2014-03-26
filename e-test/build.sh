#!/bin/bash
ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/internal.ldf

#Cleaning the bin directory
rm bin/*.elf
rm bin/*.srec

#Host side program
gcc src/e-test.c -o bin/e-test.elf -I ${EINCS} -L ${ELIBS} -le-hal

#Loop for creating device side programs
for TEST in test_memory_simple test_memory_march test_memory_dram test_matmul test_emesh
do
e-gcc -T ${ELDF} src/${TEST}.c src/test_common.c -o bin/$TEST.elf
e-objcopy --srec-forceS3 --output-target srec bin/${TEST}.elf bin/${TEST}.srec
done


