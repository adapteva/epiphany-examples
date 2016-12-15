#!/bin/bash
set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

cd bin

./main.elf

