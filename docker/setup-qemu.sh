#!/bin/bash

## download and build qemu

#git clone https://github.com/qemu/qemu
#cd qemu
#git checkout v7.2.0-rc1

VER=7.2.0

wget -c -t 3 https://download.qemu.org/qemu-${VER}.tar.xz
tar xpvf ./qemu-${VER}.tar.xz
ln -sf ./qemu-${VER} ./qemu
cd qemu-${VER}

mkdir build
cd build && ../configure --target-list=loongarch64-softmmu
make -j8
make install
