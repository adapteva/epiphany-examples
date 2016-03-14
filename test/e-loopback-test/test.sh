#!/bin/bash



SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/../e-test/bin/e-test.elf 0 0 1 1 0 $EXEPATH/bin/e-task.elf > e-loopback-test.log

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval

