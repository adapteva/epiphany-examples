#!/bin/bash

LOGFILE=eth-test.log
IFCONFIG=/sbin/ifconfig

if [ -f $LOGFILE ] ; then
rm $LOGFILE
fi

#########################################################
##GET IP ADDRESS, INFER GATEWAY ADDRESS
#########################################################
ETHPORT=`$IFCONFIG | grep -E -o -m 1 "eth[0-9.]+"`
IPADDR=`$IFCONFIG $ETHPORT | grep -E -o -m 1 "inet addr:[0-9.]+"`
# echo $IPADDR
if [ $? != 0 ] ; then
    echo "FAILED TO FIND MY IP ADDRESS"
    exit 2
fi
IPADDR=${IPADDR:10}
GWADDR=${IPADDR%.*}.1
#GWADDR=192.168.100.1

#########################################################
##TESTING ETHERNET###
#########################################################
touch $LOGFILE
ping -c 2 -s 1000 -W 1 $GWADDR > $LOGFILE
# echo $GWADDR
if grep "icmp_seq=" $LOGFILE >& /dev/null
	then
	:
#sudo ethtool eth0 | grep "Link detected: yes" > $LOGFILE
#if grep "Link detected: yes" $LOGFILE >& /dev/null 
#   then
#    :
else
    echo "BOARD FAILED (ETHERNET-TEST)"
    exit 1
fi

ifconfig | grep HWaddr >> $LOGFILE
ifconfig | grep HWaddr
echo "PASS (ETHERNET-TEST)"
exit 0
