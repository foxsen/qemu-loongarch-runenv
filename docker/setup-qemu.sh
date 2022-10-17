#!/bin/bash

## download and build qemu

git clone https://github.com/qemu/qemu
cd qemu
git checkout v7.1.0
mkdir build
cd build && ../configure --target-list=loongarch64-softmmu
make -j8
make install



