#!/bin/bash

set -e

if [[ "$1" == "-d" ]]; then
	Config='Debug'
else
	Config='Release'
fi


cd host/${Config}

./matmul.elf $@ ../../device/${Config}/e_matmul.srec

cd ../../

