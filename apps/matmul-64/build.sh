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


CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac

CC=${CROSS_PREFIX}gcc


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

exit
