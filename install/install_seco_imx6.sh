#!/usr/bin/env bash

PATCH_DIR=`dirname $0`

< $PATCH_DIR/arm_vfp.patch patch -p1 -N

make clean distclean

./configure \
--cpu=armv7l \
--prefix=/segfs/linux/dance_sdk/deps/arm-buildroot-linux-uclibcgnueabi \
--cc=/segfs/linux/dance_sdk/toolchain/arm-buildroot-linux-uclibcgnueabi/bin/arm-buildroot-linux-uclibcgnueabi-gcc

make install
