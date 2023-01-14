#!/bin/bash

## download and install cross toolchain

wget -c https://github.com/loongson/build-tools/releases/download/2022.09.06/loongarch64-clfs-6.3-cross-tools-gcc-glibc.tar.xz
tar -C /opt -xpvf ./loongarch64-clfs-6.3-cross-tools-gcc-glibc.tar.xz
rm -f /opt/cross-tools/lib/bfd-plugins/libdep.so
