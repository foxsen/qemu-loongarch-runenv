#!/bin/bash

## download and install cross toolchain

wget -c https://github.com/loongson/build-tools/releases/download/2022.05.29/loongarch64-clfs-5.0-cross-tools-gcc-glibc.tar.xz
tar -C /opt -xpvf ./loongarch64-clfs-5.0-cross-tools-gcc-glibc.tar.xz
rm -f /opt/cross-tools/lib/bfd-plugins/libdep.so
