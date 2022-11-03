#!/bin/bash

############################################################
# Help                                                     #
############################################################
Help()
{
   # Display Help
   echo "Run a loongarch virtual machine."
   echo
   echo "Syntax: run_loongarch.sh [-b|c|d|D|g|h|i|k|m|q]"
   echo "options:"
   echo "b <bios>    use this file as BIOS"
   echo "c <NCPU>    simulated cpu number"
   echo "d use -s to listen on 1234 debug port"
   echo "D use -s -S to listen on 1234 debug port and stop waiting for attach"
   echo "g     Use graphic UI."
   echo "h     Print this Help."
   echo "i <initrd> use this file as initrd"
   echo "k <kernel> use this file as kernel"
   echo "m <mem> specify simulated memory size"
   echo "q <qemu> use this file as qemu"
   echo
}

# >= 512M
MEM="4G"
# 1-4
CPUS="1"
#BIOS="./loongarch_bios_0310.bin"
BIOS="./loongarch_bios_0310_debug.bin"
KERNEL="./vmlinux"
INITRD="busybox-rootfs.img"
USE_GRAPHIC="no"
DEBUG=''
HOST_ARCH=$(uname -m)
QEMU="./qemu/$HOST_ARCH/qemu-system-loongarch64"

# Get the options
while getopts ":b:c:dDghi:k:m:q:" option; do
   case $option in
      b)
        BIOS=$OPTARG;;
      c)
        CPUS=$OPTARG;;
      d)
        DEBUG='-s';;
      D)
        DEBUG='-s -S';;
      g) 
         USE_GRAPHIC="yes";;
      h) # display Help
         Help
         exit;;
      i)
         INITRD=$OPTARG;;
      k)
         KERNEL=$OPTARG;;
      q)
         QEMU=$OPTARG;;
      \?) # invalid option
        echo "invalid option"
        exit;;
   esac
done

if [ $USE_GRAPHIC = "no" ] ; then
    # run without graphic
    CMDLINE="root=/dev/ram console=ttyS0,115200 rdinit=/init"
    GRAPHIC="-vga none -nographic"
else
    # run with graphic
    CMDLINE="root=/dev/ram console=tty0 rdinit=/init"
#    GRAPHIC="-vga virtio -device virtio-keyboard-pci -device virtio-mouse-pci"
    GRAPHIC="-vga virtio -device qemu-xhci -device usb-kbd -device usb-mouse"
fi

set -x
$QEMU -m $MEM -smp $CPUS -bios $BIOS -kernel $KERNEL -initrd $INITRD -append "$CMDLINE" $GRAPHIC $DEBUG
