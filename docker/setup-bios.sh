#!/bin/bash

## download and build UEFI for loongarch qemu virtual platform

mkdir tianocore
cd tianocore
top_dir=`pwd`

git clone https://github.com/tianocore/edk2.git
cd $top_dir/edk2
git submodule update --init

cd $top_dir
git clone https://github.com/tianocore/edk2-platforms.git
cd $top_dir/edk2-platforms
git submodule update --init

cd $top_dir
export PATH=/opt/cross-tools/bin:$PATH
export WORKSPACE=$top_dir
export PACKAGES_PATH=$WORKSPACE/edk2:$WORKSPACE/edk2-platforms
export GCC5_LOONGARCH64_PREFIX=loongarch64-unknown-linux-gnu-

. $top_dir/edk2/edksetup.sh

make -C ./edk2/BaseTools

## build debug version
build --buildtarget=DEBUG --tagname=GCC5 --arch=LOONGARCH64  --platform=edk2-platforms/Platform/Loongson/LoongArchQemuPkg/Loongson.dsc

## build release version
build --buildtarget=RELEASE --tagname=GCC5 --arch=LOONGARCH64  --platform=edk2-platforms/Platform/Loongson/LoongArchQemuPkg/Loongson.dsc
