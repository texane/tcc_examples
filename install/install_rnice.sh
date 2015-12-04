#!/usr/bin/env bash

make clean distclean

./configure \
--cpu=x86_64 \
--prefix=/segfs/linux/dance_sdk/deps/x86_64-rnice

make && make install
