#!/bin/bash


EROW0=${EROW0:-0}
ECOL0=${ECOL0:-0}
EROWS=${EROWS:-4}
ECOLS=${ECOLS:-4}


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/bin/e-mem-test.elf $EROW0 $ECOL0 $EROWS $ECOLS 1

