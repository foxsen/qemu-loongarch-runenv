#!/bin/bash

## download and build linux kernel 

#git clone -b loongarch-next --depth 1 https://github.com/loongson/linux
#cd linux
VER=6.1.4

wget -c -t 3 https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${VER}.tar.xz
tar xvpf ./linux-${VER}.tar.xz
ln -sf ./linux-${VER} ./linux
cd ./linux-${VER}

CC_PREFIX=/opt/cross-tools
export PATH=$CC_PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$CC_PREFIX/lib:$CC_PREFIX/loongarch64-unknown-linux-gnu/lib/:$LD_LIBRARY_PATH
export ARCH=loongarch
export CROSS_COMPILE=loongarch64-unknown-linux-gnu-

cp arch/loongarch/configs/loongson3_defconfig .config
make olddefconfig
make -j12
