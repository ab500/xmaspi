#!/bin/bash

# load kernel module
cd ~pi/xmaspi/lkm
insmod xmas.ko
mknod /dev/xmas c 252 0

# run the init script
cd ~pi/xmaspi
./driver.py



./driver.py 99 200 0 13 0
sleep 1
./driver.py 99 0 0 13 0
sleep .5
./driver.py 99 200 0 13 0
sleep 1
./driver.py 99 0 0 13 0
sleep .5
./driver.py 99 200 0 13 0

