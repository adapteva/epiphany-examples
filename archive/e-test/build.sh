#!/bin/bash
ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/internal.ldf

mkdir -p bin

#Cleaning the bin directory
rm -f bin/* &> /dev/null

#Host side program
gcc src/e-test.c -o bin/e-test.elf ${EINCS} ${ELIBS} -le-hal -lpthread

#Loop for creating device side programs
for TEST in test_memory_simple test_memory_march test_memory_dram test_matmul test_emesh test_elink
do
e-gcc -T ${ELDF} src/${TEST}.c src/test_common.c -o bin/$TEST.elf
e-objcopy --srec-forceS3 --output-target srec bin/${TEST}.elf bin/${TEST}.srec
done


