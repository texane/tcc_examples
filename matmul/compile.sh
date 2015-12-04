#!/usr/bin/env bash

ARCH=local

source ../common/compile.sh

$CROSS_COMPILE\gcc \
-static \
main.c ../common/dext.c \
-I$TCC_INC_DIR -Wall -O2 \
-L$TCC_LIB_DIR -ltcc -ldl -lm

$CROSS_COMPILE\strip a.out
