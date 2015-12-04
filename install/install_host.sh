#!/usr/bin/env bash

make clean distclean
./configure
make && sudo make install
