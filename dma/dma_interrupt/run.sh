#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


cd $EXEPATH/bin


./dma_int_test.elf

