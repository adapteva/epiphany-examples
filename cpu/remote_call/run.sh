#!/bin/bash

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

set -e


cd $EXEPATH/bin

./main.elf

