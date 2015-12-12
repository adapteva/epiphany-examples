#!/bin/bash
SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

cd ${EXEPATH}
./math_example

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval
