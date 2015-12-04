#!/usr/bin/env bash

if `false`; then
CROSS_COMPILE=
TCC_DIR=/usr/local
TCC_LIB_DIR=$TCC_DIR/lib64
fi

if `false`; then
CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/arm-buildroot-linux-uclibcgnueabi/bin/arm-buildroot-linux-uclibcgnueabi-
TCC_DIR=/segfs/linux/dance_sdk/deps/arm-buildroot-linux-uclibcgnueabi
TCC_LIB_DIR=$TCC_DIR/lib
fi

if `false`; then
CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-
TCC_DIR=/segfs/linux/dance_sdk/deps/i686-nptl-linux-gnu
TCC_LIB_DIR=$TCC_DIR/lib
fi

if `true`; then
TCC_DIR=/segfs/linux/dance_sdk/deps/x86_64-rnice
TCC_LIB_DIR=$TCC_DIR/lib64
fi

TCC_INC_DIR=$TCC_DIR/include

$CROSS_COMPILE\gcc \
-static \
main.c ../common/dext.c \
-I$TCC_INC_DIR -Wall -O2 \
-L$TCC_LIB_DIR -ltcc -ldl -lm

$CROSS_COMPILE\strip a.out
