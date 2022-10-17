#!/bin/bash

## download and build UEFI for loongarch qemu virtual platform

git clone -b LoongArch https://github.com/loongson/edk2-LoongarchVirt.git
cd edk2-LoongarchVirt
git submodule update --init
export PATH=/opt/cross-tools/bin:$PATH
export WORKSPACE=`pwd`
export PACKAGES_PATH=$WORKSPACE
export GCC5_LOONGARCH64_PREFIX=loongarch64-unknown-linux-gnu-
. ./edksetup.sh

make -C BaseTools

## build debug version
build --buildtarget=DEBUG --tagname=GCC5 --arch=LOONGARCH64  --platform=OvmfPkg/LoongArchQemu/Loongson.dsc

## build release version
build --buildtarget=RELEASE --tagname=GCC5 --arch=LOONGARCH64  --platform=OvmfPkg/LoongArchQemu/Loongson.dsc
