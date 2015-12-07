#!/usr/bin/env bash

ARCH=local

source ../common/compile.sh

$CROSS_COMPILE\gcc \
-static \
-Wall -O2 -DDEXT_UNIT=1 \
main.c $DEXT_DIR/dext.c \
-I$TCC_INC_DIR -I$DEXT_DIR \
-L$TCC_LIB_DIR -ltcc -ldl -lm

$CROSS_COMPILE\strip a.out
