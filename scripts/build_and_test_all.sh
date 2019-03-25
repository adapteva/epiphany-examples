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

# Examples we don't want to cross compile (hard to get library dependencies)
#CROSS_BUILD_SKIP_REGEX="/apps/fft2d"

CROSS_BUILD_SKIP_REGEX=${CROSS_BUILD_SKIP_REGEX:-"\$a"} # HACK: valid regexp that matches nothing

# Examples expected to fail on non native (e.g. timing) (assume simulator)
CROSS_XFAIL_REGEX="/cpu/mutex|/test/test-elink-rx-remapping|/dma/dma_message_read|/dma/dma_slave"

# Problematic/unreliable/too slow examples we don't want to test goes here.
TEST_SKIP_REGEX=\
"/apps/eprime|/apps/erm|/apps/matmul-64|"\
"/test/e-loopback-test|/cpu/ctimer|/labs/|/io/|/test/e-standby-test|"\
"/apps/e-dump-regs"

[[ "x$(uname -m)" =~ xarm.* ]] && CROSS=no || CROSS=yes

if [ "x${CROSS}" = "xyes" ]; then
    echo Detected non-Parallella host

    # Tell build scripts to not cross compile
    if [ "x${CROSS_COMPILE}" != "x" ]; then
        echo "Using user-specified \$CROSS_COMPILE"
    else
        CROSS_COMPILE=""
        export CROSS_COMPILE

        echo Instructing build scripts to NOT cross-compile host programs

        # Is there a simulator we can use for the tests?
        if [ "x${RUNTEST}" != "x" ]; then
            echo "Using user-specified \$RUNTEST"
        else
            if which epiphany-elf-sim >/dev/null &&
                    gcc -Wl,-defsym=_start=0 -nostdlib -Wl,-L/opt/adapteva/esdk/tools/host/lib -le-hal -o /dev/null >/dev/null 2>&1; then
                RUNTEST="epiphany-elf-sim -p parallella16 --host "
                export RUNTEST
                echo Using simulator for tests
            else
                echo Could not find simulator, will only compile tests and not run them.
            fi
        fi
    fi

    echo
fi

tput_ () {
    # Fail silently, will only result in no colors.
    tput $@ 2>/dev/null || true
}

color () {
    color=$1
    str=$2

    # Only color if stdout is not terminal
    if [ -t 1 ]; then
        printf "$(tput_ setaf $color)%-12.12s$(tput_ sgr0)" "$str"
    else
        printf "%-12.12s" "$str"
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
    CROSS_XFAIL) # Expected to fail in sim
        color $yellow $str
        ;;
    CROSS_XPASS) # Expected to FAIL in sim
        color $red $str
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
        elif [ "x${CROSS}" = "xyes" ]; then
            if (echo $dir | grep -qE $CROSS_BUILD_SKIP_REGEX); then
                build_status="CROSS_SKIP"
            else
                ./build.sh >./build.log 2>&1 && build_status="OK" || build_status="FAIL"
            fi
        elif [ "x${nobuild}" = "x" ]; then
            ./build.sh >./build.log 2>&1 && build_status="OK" || build_status="FAIL"
        fi
        status "$build_status"
        sync

        if [ "x${CROSS}" = "xyes" -a "x${RUNTEST}" = "x" ]; then
            true
        elif [ -e "test.sh" ]; then
            test_script="./test.sh"
        elif [ -e "run.sh" ]; then
            test_script="./run.sh"
        else
            test_script=""
        fi

        test_status="SKIP"
        if ! [[ "x$(uname -m)" =~ xarm.* ]] && [ "x$RUNTEST" = "x" ]; then
            test_status="CROSS_SKIP"
        elif [ "x$build_status" = "xCROSS_SKIP" ]; then
            test_status="CROSS_SKIP"
        elif (echo $dir | grep -qE $BUILD_SKIP_REGEX); then
            test_status="SKIP"
        elif (echo $dir | grep -qE $TEST_SKIP_REGEX); then
            test_status="SKIP"
        elif [ "x" = "x$test_script" ]; then
            test_status="N/A"
        elif [ "x$CROSS" = "xyes" ] && (echo $dir | grep -qE $CROSS_XFAIL_REGEX); then
            $RUNTEST $test_script >./test.log 2>&1 && test_status="CROSS_XPASS" || test_status="CROSS_XFAIL"
        else
            $RUNTEST $test_script >./test.log 2>&1 && test_status="EXIT_OK" || test_status="EXIT_FAIL"
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
        if (echo $build_status | grep -qE ".*[^X]FAIL"); then
            false
        elif (echo $test_status | grep -qE ".*[^X]FAIL"); then
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
printf "%-36.36s%-12.12s%-12.12s\n" "Directory" "Build" "Test"
printf '=%.0s' {1..60}
printf '\n'

for f in $(find $LIMIT -name "build.sh" | sort ); do
    dir=$(dirname $f)
    printf "%-36.36s" $(echo $dir | sed s,$ROOT/*,,)
    test_example $dir || err="yes"
done

echo

case ${CROSS} in
no)
    if [ $LIMIT = $ROOT ]; then
        printf "\nPhase 2: Select repeated tests\n\n"
        printf "%-36.36s%-12.12s%-12.12s\n" "Directory" "Test" "Iterations"
        printf '=%.0s' {1..60}
        printf '\n'
        repeat "${ROOT}/apps/e-bandwidth-test"  100 || err="yes"
        repeat "${ROOT}/apps/matmul-16"         100 || err="yes"
        repeat "${ROOT}/dma/dma_slave"          100 || err="yes"
        repeat "${ROOT}/test/e-mem-test"         10 || err="yes"
    fi


#printf "\nPhase 3: Build and run all tests once (RANDOM ORDER)\n\n"
#printf "%-36.36s%-12.12s%-12.12s\n" "Directory" "Build" "Test"
#printf '=%.0s' {1..60}
#printf '\n'
#
#for f in $(find $LIMIT -name "build.sh" | shuf); do
#    dir=$(dirname $f)
#    printf "%-40.40s" $(echo $dir | sed s,$ROOT/*,,)
#    test_example $dir || err="yes"
#done

    ;;

yes)
    echo Cross compilation detected: Skipping repeated tests.

    ;;

esac

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

