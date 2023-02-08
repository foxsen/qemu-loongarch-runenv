#!/bin/bash

cd tianocore
top_dir=`pwd`

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
