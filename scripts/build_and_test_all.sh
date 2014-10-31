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

TEST_SKIP_REGEX=\
"/archive/|/apps/eprime|/apps/erm|/apps/matmul-64|/labs/hardware_loops|"\
"/test/e-loopback-test|/test/e-test"

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
    (
        cd $dir

        ./build.sh >./build.log 2>&1 && build_status="OK" || build_status="FAIL"
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
    )
}


printf "%-40.40s%-10.10s%-10.10s\n" "Directory" "Build" "Test"
printf '=%.0s' {1..60}
printf '\n'

for f in $(find $LIMIT -name "build.sh" | sort ); do
    dir=$(dirname $f)
    printf "%-40.40s" $(echo $dir | sed s,$ROOT/*,,)
    test_example $dir
done


