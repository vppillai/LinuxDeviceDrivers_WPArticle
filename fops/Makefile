# This sample code illustrated differentiating different minor numbers from the driver code by altering teh file_operations structure to use.
# This is done by modifying the f_op member of filep  from the backend open routine
#
# compilation:
#  make -C /usr/src/linux-headers-3.8.0-31-generic/ M=$PWD modules
# clean:
#  make -C /usr/src/linux-headers-3.8.0-31-generic/ M=$PWD clean
# node creation:
#  mknod fops0 c 250 0 
#  mknod fops1 c 250 1
#  mknod fops2 c 250 2 #- to see invalid case
#
# Author: Vysakh P Pillai [vysakhpillai[at]gmail.com]
obj-m := fops.o
