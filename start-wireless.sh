#!/bin/bash

r=1
while [[ $r -eq 1 ]];
do 
    iwconfig wlan0 essid xmaslights
    sleep 1
    dhclient wlan0
    sleep 4
    iwconfig 2>&1 | grep xmaslights > /dev/null
    r=`echo $?`
done
