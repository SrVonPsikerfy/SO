#!/bin/bash

# 1.
if [ -d "./mount-point" ]; then 
  rm -r "./mount-point"
fi
mkdir "./mount-point"

./fs-fuse -t 2097152 -a virtual-disk -f "-d -s mount-point"

if [ -d "./temp" ]; then 
  rm -r "./temp"
fi
mkdir temp

cp "./src/fuseLib.c" "./temp"
cp "./src/myFS.h" "./temp"

cp "./src/fuseLib.c" "./mount-point"
cp "./src/myFS.h" "./mount-point"

# 2.
./my-fsck "virtual-disk" 1>/dev/null

DIFF1=$(diff -q "./src/fuseLib.c" "./mount-point/fuseLib.c")
if [ ! DIFF1 ]; then
  echo "something went wrong while copying fuseLib.c"
else 
  echo "fuseLib.c succesfully copied"
fi

DIFF2=$(diff -q "./src/myFS.h" "./mount-point/myFS.h")
if [ ! DIFF2 ]; then
  echo "Something went wrong while copying myFS.h"
  exit -1
else 
  echo "myFS.h succesfully copied"
fi

truncate -s -4096 "./temp/fuseLib.c"
truncate -s -4096 "./mount-point/fuseLib.c"

# 3.
./my-fsck "virtual-disk" 1>/dev/null

diff -q "./temp/fuseLib.c" "./mount-point/fuseLib.c" 1>/dev/null # dev null erases output
if [ $? != "0" ]; then
  echo "fuseLib.c truncate went wrong"
  exit -1
else 
  echo "fuseLib.c succesfully truncated"
fi

# 4.
cp "./Makefile" "./mount-point"

# 5.
./my-fsck "virtual-disk" 1>/dev/null

diff -q "./Makefile" "./mount-point/Makefile" 1>/dev/null # dev null erases output
if [ $? != "0" ]; then
  echo "Makefile was not succesfully copied"
  exit -1
else 
  echo "Makefile succesfully copied"
fi

# 6.
truncate -s 4096 "./temp/myFS.h"
truncate -s 4096 "./mount-point/myFS.h"

# 7.
./my-fsck "virtual-disk" 1>/dev/null

diff -q "./temp/myFS.h" "./mount-point/myFS.h" 1>/dev/null
if [ $? != "0" ]; then
  echo "myFS.h was not succesfully truncated"
  exit -1
else
  echo "myFS.h was succesfully truncated"
fi

# deletes the FS
# fusermount -u mount-point

exit 0