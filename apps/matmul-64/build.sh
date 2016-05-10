#!/bin/bash

set -e

if [[ "`arch`" == "x86_64" ]]; then
	echo "Setting path to ARM tools."
	GNU_PATH='a-'
else
	GNU_PATH=''
fi

Proj='matmul'
if [[ "$1" == "-d" ]]; then
	Config='Debug'
else
	Config='Release'
fi
BUILD_DEVICE='yes'
BUILD_HOST='yes'

MK_CLEAN='yesXX'
MK_ALL='yes'


if [[ "${BUILD_DEVICE}" == "yes" ]]; then

	pushd device/${Config} >& /dev/null
	if [[ "${MK_CLEAN}" == "yes" ]]; then
		echo "*** Cleaning device/${Config}"
		make clean
	fi
	if [[ "${MK_ALL}" == "yes" ]]; then
		make --warn-undefined-variables BuildConfig=${Config} all
	fi

	popd >& /dev/null

fi


EINCS="${EPIPHANY_HOME}/tools/host/include"
ELIBS="${EPIPHANY_HOME}/tools/host/lib"


if [ -z "${CROSS_COMPILE+xxx}" ]; then
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_COMPILE=
		;;
	   *)
		# Use cross compiler
		CROSS_COMPILE="arm-linux-gnueabihf-"
		;;
esac
fi

CC=${CROSS_COMPILE}gcc


if [[ "${BUILD_HOST}" == "yes" ]]; then


	pushd ./host/${Config} >& /dev/null
	if [[ "${MK_CLEAN}" == "yes" ]]; then
		echo "*** Cleaning host program"
		make clean
	fi
	if [[ "${MK_ALL}" == "yes" ]]; then
		make CC=${CC} --warn-undefined-variables BuildConfig=${Config} all
	fi
	popd >& /dev/null
fi

e-objcopy --srec-forceS3 --output-target srec device/Release/e_matmul.elf device/Release/e_matmul.srec

exit
