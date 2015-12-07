#!/usr/bin/env bash


[ -z $ARCH ] && ARCH='local'

if [ $ARCH = 'local' ]; then

CROSS_COMPILE=
TCC_DIR=/usr/local
TCC_LIB_DIR=$TCC_DIR/lib64

elif [ $ARCH = 'seco_imx6' ]; then

CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/arm-buildroot-linux-uclibcgnueabi/bin/arm-buildroot-linux-uclibcgnueabi-
TCC_DIR=/segfs/linux/dance_sdk/deps/arm-buildroot-linux-uclibcgnueabi
TCC_LIB_DIR=$TCC_DIR/lib

elif [ $ARCH = 'kontron_type10' ]; then

CROSS_COMPILE=/segfs/linux/dance_sdk/toolchain/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-
TCC_DIR=/segfs/linux/dance_sdk/deps/i686-nptl-linux-gnu
TCC_LIB_DIR=$TCC_DIR/lib

elif [ $ARCH = 'rnice' ]; then

TCC_DIR=/segfs/linux/dance_sdk/deps/x86_64-rnice
TCC_LIB_DIR=$TCC_DIR/lib64

else

echo ERROR: invalid architecture $ARCH
exit -1

fi

TCC_INC_DIR=$TCC_DIR/include

DEXT_DIR=../common
# DEXT_DIR=$HOME/segfs/repo/gaia/DanceTree/components/libdance/src