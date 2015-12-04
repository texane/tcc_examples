#!/usr/bin/env bash

make clean distclean

./configure \
--cpu=i386 \
--prefix=/segfs/linux/dance_sdk/deps/i686-nptl-linux-gnu \
--cc=/segfs/linux/dance_sdk/toolchain/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc

make && make install
