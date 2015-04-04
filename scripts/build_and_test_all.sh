#!/bin/bash
set -e

trap "kill $BASHPID" SIGINT SIGTERM

SCRIPT=$(readlink -e $0)
EXEPATH=$(dirname $SCRIPT)
ROOT=$(readlink -e $EXEPATH/..)

if [[ $PWD =~ .*$ROOT.* ]]; then
    LIMIT=$PWD
else
    LIMIT=$ROOT
fi

cd $ROOT

# Examples we don't even want to build (nor test) goes here
BUILD_SKIP_REGEX="/archive/|/test/e-test|/test/e-matmul-test"

# Problematic/unreliable/too slow examples we don't want to test goes here.
TEST_SKIP_REGEX=\
"/apps/eprime|/apps/erm|/apps/matmul-64|/labs/hardware_loops|"\
"/test/e-loopback-test|/cpu/ctimer|/labs/|/io/|/test/e-standby-test"

tput_ () {
    # Fail silently, will only result in no colors.
    tput $@ 2>/dev/null || true
}

color () {
    color=$1
    str=$2

    # Only color if stdout is not terminal
    if [ -t 1 ]; then
        printf "$(tput_ setaf $color)%-10.10s$(tput_ sgr0)" "$str"
    else
        printf "%-10.10s" "$str"
    fi
}


status () {
    str=$1

    red=1
    green=2
    yellow=3
    default=65536 # Any high number seem to work

    case $str in
    OK|PASS)
        color $green $str
        ;;
    EXIT_OK) # Some tests fail silently
        color $yellow $str
        ;;
    SKIP) # Some tests are broken atm
        color $yellow $str
        ;;
    *FAIL)
        color $red $str
        ;;
    N/A) # Could not find test to run
        color $yellow $str
        ;;
    *)
        color $yellow $str
    esac
}

# cd into directory; build; test
test_example () {
    test_script=""
    build_status=""
    test_status=""

    dir=${1}
    nobuild=${2} # Anything but empty string
    (
        cd $dir

        build_status="SKIP"
        if (echo $dir | grep -qE $BUILD_SKIP_REGEX); then
            true
        elif [ "x${nobuild}" = "x" ]; then
            ./build.sh >./build.log 2>&1 && build_status="OK" || build_status="FAIL"
        fi
        status "$build_status"
        sync

        if ! [[ "x$(uname -m)" =~ xarm.* ]]; then
            true
        elif [ -e "test.sh" ]; then
            test_script="./test.sh"
        elif [ -e "run.sh" ]; then
            test_script="./run.sh"
        else
            test_script=""
        fi

        test_status="SKIP"
        if ! [[ "x$(uname -m)" =~ xarm.* ]]; then
            test_status="CROSS_SKIP"
        elif (echo $dir | grep -qE $BUILD_SKIP_REGEX); then
            test_status="SKIP"
        elif (echo $dir | grep -qE $TEST_SKIP_REGEX); then
            test_status="SKIP"
        elif [ "x" = "x$test_script" ]; then
            test_status="N/A"
        else
            $test_script >./test.log 2>&1 && test_status="EXIT_OK" || test_status="EXIT_FAIL"
        fi

        if [ "xEXIT_OK" = "x$test_status" -a -f EXPECTED.txt ]; then
            expected_md5=$(md5sum EXPECTED.txt | cut -f1 -d" ")
            test_md5=$(md5sum test.log | cut -f1 -d" ")
            if [ "x$expected_md5" = "x$test_md5" ]; then
                test_status="PASS"
            else
                test_status="FAIL"
            fi
        fi

        status $test_status

        printf "\n"
        if (echo $build_status | grep -qE ".*FAIL"); then
            false
        elif (echo $test_status | grep -qE ".*FAIL"); then
            false
        else
            true
        fi
    )
}


repeat () {
    local err dir iterations
    dir=$1
    iterations=$2

    err=

    printf "%s (%s iterations)\n" $(echo $dir | sed s,$ROOT/*,,) "$iterations"

    for i in `seq 1 $iterations`; do
        if test_example $dir nobuild 2>&1 1>/dev/null; then
            printf "."
            [ $[i%40] -eq 0 ] && printf "\n"
            continue
        fi

        err=yes
        break
    done
    printf "\n%-40.40s" " "

    if [ "x$err" = "xyes" ]; then
        status "FAIL"
        printf "${i}\n"
        false
    else
        status "PASS"
        printf "${i}\n"
        true
    fi

}



err=no

printf "Phase 1: Build and run all tests once\n\n"
printf "%-40.40s%-10.10s%-10.10s\n" "Directory" "Build" "Test"
printf '=%.0s' {1..60}
printf '\n'

for f in $(find $LIMIT -name "build.sh" | sort ); do
    dir=$(dirname $f)
    printf "%-40.40s" $(echo $dir | sed s,$ROOT/*,,)
    test_example $dir || err="yes"
done


if [ $LIMIT = $ROOT ]; then
    printf "\nPhase 2: Select repeated tests\n\n"
    printf "%-40.40s%-10.10s%-10.10s\n" "Directory" "Test" "Iterations"
    printf '=%.0s' {1..60}
    printf '\n'
    repeat "${ROOT}/apps/e-bandwidth-test"  100 || err="yes"
    repeat "${ROOT}/apps/matmul-16"         100 || err="yes"
    repeat "${ROOT}/dma/dma_slave"          100 || err="yes"
    repeat "${ROOT}/test/e-mem-test"         10 || err="yes"
fi


#printf "\nPhase 3: Build and run all tests once (RANDOM ORDER)\n\n"
#printf "%-40.40s%-10.10s%-10.10s\n" "Directory" "Build" "Test"
#printf '=%.0s' {1..60}
#printf '\n'
#
#for f in $(find $LIMIT -name "build.sh" | shuf); do
#    dir=$(dirname $f)
#    printf "%-40.40s" $(echo $dir | sed s,$ROOT/*,,)
#    test_example $dir || err="yes"
#done


printf "\n\n"
printf '=%.0s' {1..60}
printf "\nSTATUS: "

if [ "x${err}" = "xyes" ]; then
    status "FAIL"
    printf "\n\n"
    exit 1
else
    status "PASS"
    printf "\n\n"
    exit 0
fi

