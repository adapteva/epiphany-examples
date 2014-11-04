#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


cd $EXEPATH/bin

java -jar ~/Desktop/EpiphanyMonitor2.jar &

./main.elf

