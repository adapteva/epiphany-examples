#!/bin/bash

###TEST
echo "Building ../test/e-reset"
../test/e-reset/build.sh

echo "Building ../test/e-test"
../test/e-test/build.sh

echo "Building ../test/e-extmem-test"
../test/e-extmem-test/build.sh

echo "Building ../test/e-mem-test"
../test/e-mem-test/build.sh

echo "Building ../test/e-write-word"
../test/e-write-word/build.sh

echo "Building ../test/e-main" 
../test/e-main/build.sh

echo "Building ../test/e-read-buf"
../test/e-read-buf/build.sh

echo "Building ../test/e-matmul-test"
../test/e-matmul-test/build.sh

echo "Building ../test/e-read-word"
../test/e-read-word/build.sh

echo "Building ../test/e-write-buf"
../test/e-write-buf/build.sh

###APPS
echo "Building ../apps/dotproduct"
../apps/dotproduct/build.sh

echo "Building ../apps/e-fill-mem" 
../apps/e-fill-mem/build.sh

echo "Building ../apps/matmul-64"
../apps/matmul-64/build.sh

echo "Building ../apps/matmul-16"
../apps/matmul-16/build.sh

echo "Building ../apps/e-bandwidth-test"
../apps/e-bandwidth-test/build.sh

echo "Building ../apps/e-dump-mem"
../apps/e-dump-mem/build.sh

echo "Building ../apps/e-mem-sync"
../apps/e-mem-sync/build.sh

echo "Building ../apps/fft2d"
../apps/fft2d/build.sh

echo "Building ../apps/e-dump-regs"
../apps/e-dump-regs/build.sh

echo "Building ../apps/e-toggle-led"
../apps/e-toggle-led/build.sh

###EMESH  
echo "Building ../emesh/emesh_bandwidth_all2one"
../emesh/emesh_bandwidth_all2one/build.sh

echo "Building ../emesh/emesh_bandwidth_neighbour"
../emesh/emesh_bandwidth_neighbour/build.sh

#echo "Building ../emesh/emesh_traffic"
#../emesh/emesh_traffic/build.sh-->broken FIX!!!!x

echo "Building ../emesh/emesh_bandwidth_bisection"
../emesh/emesh_bandwidth_bisection/build.sh

echo "Building ../emesh/emesh_read_latency"
../emesh/emesh_read_latency/build.sh

###
