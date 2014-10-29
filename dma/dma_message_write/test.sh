#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

cd $EXEPATH/bin
./dma_message_test.elf > ./dma_message_write.log

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval
