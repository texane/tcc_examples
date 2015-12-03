#!/usr/bin/env bash

CROSS_COMPILE=
TCC_DIR=../../tinycc

if `false`; then
TCC_LIB_DIR=$TCC_DIR/install_host/lib64
TCC_INC_DIR=$TCC_DIR/install_host/include
else
CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/arm-buildroot-linux-uclibcgnueabi/bin/arm-buildroot-linux-uclibcgnueabi-
TCC_LIB_DIR=$TCC_DIR/install_arm/lib
TCC_INC_DIR=$TCC_DIR/install_arm/include
fi

$CROSS_COMPILE\gcc \
-static \
main.c \
-I$TCC_INC_DIR -Wall -O2 \
-L$TCC_LIB_DIR -ltcc -ldl -lm

$CROSS_COMPILE\strip a.out
