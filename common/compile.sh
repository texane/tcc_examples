#!/usr/bin/env bash


[ -z $ARCH ] && ARCH='local'

if [ $ARCH = 'local' ]; then
CROSS_COMPILE=
TCC_DIR=/usr/local
TCC_LIB_DIR=$TCC_DIR/lib64
fi

if [ $ARCH = 'seco_imx6' ]; then
CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/arm-buildroot-linux-uclibcgnueabi/bin/arm-buildroot-linux-uclibcgnueabi-
TCC_DIR=/segfs/linux/dance_sdk/deps/arm-buildroot-linux-uclibcgnueabi
TCC_LIB_DIR=$TCC_DIR/lib
fi

if [ $ARCH = 'kontron_type10' ]; then
CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-
TCC_DIR=/segfs/linux/dance_sdk/deps/i686-nptl-linux-gnu
TCC_LIB_DIR=$TCC_DIR/lib
fi

if [ $ARCH = 'rnice' ]; then
TCC_DIR=/segfs/linux/dance_sdk/deps/x86_64-rnice
TCC_LIB_DIR=$TCC_DIR/lib64
fi

TCC_INC_DIR=$TCC_DIR/include
