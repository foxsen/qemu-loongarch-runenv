#!/bin/bash

## download and build linux kernel 

git clone -b loongarch-next --depth 1 https://github.com/loongson/linux
cd linux

CC_PREFIX=/opt/cross-tools
export PATH=$CC_PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$CC_PREFIX/lib:$CC_PREFIX/loongarch64-unknown-linux-gnu/lib/:$LD_LIBRARY_PATH
export ARCH=loongarch
export CROSS_COMPILE=loongarch64-unknown-linux-gnu-

cp arch/loongarch/configs/loongson3_defconfig .config
make olddefconfig
make -j12




