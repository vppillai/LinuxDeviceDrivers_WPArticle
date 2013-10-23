# This sample code implements a simple one byte read-write device. Device nodes are to be created manually 
#  with the dynamic device nombers read from /proc/modules .
#sample commands:
# compilation:
#  make -C /usr/src/linux-headers-3.8.0-31-generic/ M=$PWD modules
# clean:
#  make -C /usr/src/linux-headers-3.8.0-31-generic/ M=$PWD clean
# Creating node:
#  mknod device c 250 0 
#
# Author: Vysakh P Pillai [vysakhpillai[at]gmail.com]
obj-m := device.o
