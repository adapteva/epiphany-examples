#!/bin/bash

# The target must be a (simple) file that exists on the USB drive
TARGET=/media/usb/pic2.JPG
LOCAL=mycopy.tmp
MTPT=/media/usb
DEV=/dev/sda1
MOUNT=0
RET=0

rm -f $LOCAL

if [ -e $DEV ] ; then
  echo GOT DEVICE
else
  sudo mknod /dev/sda1 b 8 1
  if [ -e $DEV ] ; then
    echo CREATED DEVICE
  else
    echo UNABLE TO CREATE USB DEVICE
    exit 1
  fi
fi

if [ -e $MTPT ] ; then
    echo GOT MOUNT POINT
else
    sudo mkdir $MTPT
    if [ -e $MTPT ] ; then
	echo CREATED MOUNT POINT
    else
	echo UNABLE TO CREATE MOUNT POINT
	exit 2
    fi
fi

if [ -f $TARGET ] ; then
    echo FOUND TARGET
else
    echo MOUNTING DRIVE
    sudo mount $DEV $MTPT
    MOUNT=1
fi

if [ -f $TARGET ] ; then
    cp $TARGET $LOCAL
    if [ -e $LOCAL ] ; then
	echo SUCCESS
	rm -f $LOCAL
	RET=0
    else
	echo "CAN'T COPY TARGET!"
	RET=3
    fi
else
    echo UNABLE TO FIND TARGET FILE $TARGET
    RET=4
fi

if [ $MOUNT = 1 ] ; then
    echo UNMOUNTING DRIVE
    sudo umount $MTPT
fi

exit $RET
