# qemu-loongarch-runenv

This is an environment for running loongarch bios and OS on X86 machines, all
components are open source:

    * Qemu. The simulator that is used to simulated a loongarch machine on PC.
    * Toolchain. For cross compiling loongarch binaries.
    * Gdb. For cross debugging loongarch binaries that are running inside qemu.
    * UEFI bios. UEFI bios for loongarch.
    * Kernel. Linux kernel for loongarch.

The source code of these components can be found at github.com/loongson.However,
it is not so easy to find  proper combination of all components that can work.
I forked them, make necessary patches and tag the source versions for binaries
here to help you to reproduce and further customizations.

This environment is mainly intended for bios/kernel developers.

## usage

Clone this repository, then run ./run_loongarch.sh. This script has a help:

```bash
Run a loongarch virtual machine.

Syntax: run_loongarch.sh [-b|c|d|D|g|h|i|k|m|q]
options:
b <bios>    use this file as BIOS
c <NCPU>    simulated cpu number
d use -s to listen on 1234 debug port
D use -s -S to listen on 1234 debug port and stop waiting for attach
g     Use graphic UI.
h     Print this Help.
i <initrd> use this file as initrd
k <kernel> use this file as kernel
m <mem> specify simulated memory size
q <qemu> use this file as qemu
```

All options are optional and default values will be used if run without options.

The script is just a wrapper for running qemu with loongarch BIOS, kernel and rootfs.
If you want to use more options of qemu, you can run qemu binary directly. But since
we don't fully install the qemu, it might request for more roms or other materials.
Build and install a qemu by yourself to prevent this kinds of problems.

Examples:

* To run with a qemu GUI: ./run_loongarch.sh -g
* To run with a debug stub: ./run_loongarch.sh -d or ./run_loongarch.sh -D
* To test your own kernel: ./run_loongarch.sh -k <yourkernel>

The execution of qemu has three major stages:

* First, it loads and run a Loongarch BIOS. The BIOS is a standard UEFI BIOS so you can find many tutorials for using it.
* Second, with -kernel option, the BIOS will try to load and execute the kernel. If no -kernel option given, it will run a UEFI shell.
* Third, with -initrd option and given kernel cmdline, the linux kernel will try to run a minimal system.

Default booted interface without any options:

![booted interface](./pics/booted.png)

Booted interface with ./run_loongarch.sh -g

![graphic booted interface](./pics/graphic-booted.png)

## emulated machine

This qemu emulator emulates a target machine ‘virt’, which is different from a real loongarch machine. The real loongarch machine today can have many different brands and configuration, but most of them are using Loongson3A5000/Loongson3B5000/Loongson3C5000 CPUs, and Loongson7A1000/7A2000 bridge chips. While the 'virt' machine, as its name indicates, use qemu's virtio framework to simulate all kinds of peripherals, such as the disk, the network adapter and so on.

The machine and cpu supported:

    foxsen@foxsen-ThinkPad-T450:~/software/qemu-loongarch-runenv$ ./qemu-system-loongarch64 -M ?
    Supported machines are:
    none                 empty machine
    virt                 Loongson-3A5000 LS7A1000 machine (default)
    foxsen@foxsen-ThinkPad-T450:~/software/qemu-loongarch-runenv$ ./qemu-system-loongarch64 -cpu ?
    Loongson-3A5000-loongarch-cpu

For the CPU part, most Loongson3A5000 features are supported, so that the open source loongson kernel can be directly used. You can refer to Loongson Architecture Manuals and Loongson3A5000 manual for more details, check https://github.com/loongson/LoongArch-Documentation for them.

For the machine part, you can add any virtio device to the machine via qemu's command line options. For example, we are using these options to add vga/keyboard/mouse to the machine:

    -vga virtio -device virtio-keyboard-pci -device virtio-mouse-pci

By default, a virtio network adapter and a virtio VGA device is added to the emulated machine.

Read qemu's manual for adding more devices if needed. If you want to write a driver for these devices, refer to the linux kernel. You can use -device ? option to show all the devices that can be added.

Beside the virtio devices, the Loongson7A1000 bridge's pcie controller, UART serial port, Real Time Clock and power management ports are emulated. For example, you can find a 16550 serial port at physical address 0x1fe001e0, a RTC port at 0x100d0010, PCI config space at 0x20000000, IO at 0x18004000UL, and so on. Read the code in qemu's source directory hw/loongarch and the 7A1000 manual to find more information. Refer to the linux kernel source for example usage of the devices.

The following LS3A5000 functions are NOT implemented(might be incomplete):

* address routing registers, table 3-6
* IOCSR is not mapped to system physical memory address space, that is, you cannot access them via load/store via phys addr 0x1fe00000 + offset. You must use iocsr special read/write instruction iocsrrd/wr to access IOCSR registers. Normal load/stores will trigger exceptions.
* part of IOCSR is not emulated, writes will be discarded, reads will return random value
    - all writable chip configuration register in chapter4, such as function set register(iocsr 0x180, table 4-5)
    - legacy interrupts, section 11.1. so iocsrrd/wr of 0x1400/0x1420 etc will fail(no exception, but data is random)
* GPIO, chapter 10
* Temperature Sensor, Chapter 12
* DDR4 sdram controller, Chapter 13
* HyperTransPort Controller, Chapter 14
* UART1, SPI(except its memory space 0x1c000000, where the bios is put), I2C0/1

Only a few of LS7A1000's functions are implemented:

* interrupt controller, include io-apic and PCIE msi
    - at 0x10000000-0x10000fff
    - but the interrupt source connection is different, not everything work as manual
    - see the section 'interrupt hierarchy' for more information
* PCIE controller(only a standard model is implemented)
    - pcie config space map at physical address range 0x2000-0000 to 0x27ff-ffff
    - pcie memory mapped at 0x4000-0000 to 0x7fff-ffff
    - pcie io mapped at 0x1804-0000 to 0x1804-ffff
* RTC
    - at 0x100d0100
* ACPI
    - at 0x100d0000

Note: 

The uart device is implemented as a mixture of 3A5000 and LS7A1000, its physical address is from 3A5000 uart0, which is 0x1fe001e0; but its interrupts go through 7A1000 interrupt controller. It should be fixed. On real hardware, 3A5000 has two uart devices and LS7A1000 has another 4 uart devices(configurable).

### address spaces

The final address spaces look like this:

```bash
(qemu) info mtree
address-space: cpu-memory-0
address-space: memory // cpu physical memory address space, access via load/store instructions via corresponding virtual addresses
  0000000000000000-ffffffffffffffff (prio 0, i/o): system
    0000000000000000-000000000fffffff (prio 0, ram): alias loongarch.lowram @loongarch.ram 0000000000000000-000000000fffffff //low 256MB memory
    0000000010000000-00000000100000ff (prio 0, i/o): loongarch_pch_pic.reg32_part1 // LS7A1000's interrupt controller regs
    0000000010000100-000000001000039f (prio 0, i/o): loongarch_pch_pic.reg8
    00000000100003a0-0000000010000fff (prio 0, i/o): loongarch_pch_pic.reg32_part2
    000000001001041c-000000001001041f (prio -1000, i/o): pci-dma-cfg
    0000000010013ffc-0000000010013fff (prio -1000, i/o): mmio fallback 1
    00000000100d0000-00000000100d00ff (prio 0, i/o): ls7a_pm  // LS7A1000 power management functions
      00000000100d000c-00000000100d0013 (prio 0, i/o): acpi-evt
      00000000100d0014-00000000100d0017 (prio 0, i/o): acpi-cnt
      00000000100d0018-00000000100d001b (prio 0, i/o): acpi-tmr
      00000000100d0028-00000000100d002f (prio 0, i/o): acpi-gpe0
      00000000100d0030-00000000100d0033 (prio 0, i/o): acpi-reset
    00000000100d0100-00000000100d01ff (prio 0, i/o): ls7a_rtc
    0000000018000000-0000000018003fff (prio 0, i/o): alias isa-io @io 0000000000000000-0000000000003fff // legacy io mapped to 0x1800-0000
    0000000018004000-000000001800ffff (prio 0, i/o): alias pcie-io @gpex_ioport_window 0000000000004000-000000000000ffff // pcie io mapped here
    000000001c000000-000000001c3fffff (prio 0, rom): loongarch.bios // bios start at 0x1c000000
    000000001c400000-000000001c4fffff (prio 0, rom): fdt
    000000001e020000-000000001e020007 (prio 0, i/o): fwcfg.data
    000000001e020008-000000001e020009 (prio 0, i/o): fwcfg.ctl
    000000001fe001e0-000000001fe001e7 (prio 0, i/o): serial // uart port
    0000000020000000-0000000027ffffff (prio 0, i/o): alias pcie-ecam @pcie-mmcfg-mmio 0000000000000000-0000000007ffffff //pcie config mapped
    000000002ff00000-000000002ff00007 (prio 0, i/o): loongarch_pch_msi //LS7A MSI interrupt
    0000000040000000-000000007fffffff (prio 0, i/o): alias pcie-mmio @gpex_mmio_window 0000000040000000-000000007fffffff //pcie memory mapped
    0000000090000000-000000017fffffff (prio 0, ram): alias loongarch.highmem @loongarch.ram 0000000010000000-00000000ffffffff // high part ram

...

address-space: IOCSR  // a special address space accessed via iocsrrd/iocsrwr instructions.
  0000000000000000-ffffffffffffffff (prio 0, i/o): iocsr
    0000000000000000-0000000000000427 (prio 0, i/o): iocsr_misc  // only FEATURE_REG(0x8)/VENDOR_REG(0x10)/CPUNAME_REG(0x20)/MISC_FUNC_REG(0x420) read simulated
    0000000000001000-00000000000010ff (prio 0, i/o): loongarch_ipi
    0000000000001400-0000000000001cff (prio 0, i/o): loongarch_extioi // not every part of this simulated, only the following subregions, that is, no legacy irqs, only extended irqs
      0000000000001400-00000000000014bf (prio 0, i/o): loongarch_extioi.nodetype
      00000000000014c0-000000000000167f (prio 0, i/o): loongarch_extioi.ipmap_enable
      0000000000001680-0000000000001bff (prio 0, i/o): loongarch_extioi.bounce_coreisr
      0000000000001c00-0000000000001cff (prio 0, i/o): loongarch_extioi.coremap

memory-region: gpex_ioport_window //pcie IO space
  0000000000000000-000000000000ffff (prio 0, i/o): gpex_ioport_window
    0000000000000000-000000000000ffff (prio 0, i/o): gpex_ioport
      0000000000004000-000000000000401f (prio 1, i/o): virtio-pci

memory-region: pcie-mmcfg-mmio //pcie cfg space
  0000000000000000-000000001fffffff (prio 0, i/o): pcie-mmcfg-mmio

memory-region: gpex_mmio_window  //pcie memory space
  0000000000000000-ffffffffffffffff (prio 0, i/o): gpex_mmio_window
    0000000000000000-ffffffffffffffff (prio 0, i/o): gpex_mmio
      0000000040040000-0000000040043fff (prio 1, i/o): virtio-pci
        0000000040040000-0000000040040fff (prio 0, i/o): virtio-pci-common-virtio-net
        0000000040041000-0000000040041fff (prio 0, i/o): virtio-pci-isr-virtio-net
        0000000040042000-0000000040042fff (prio 0, i/o): virtio-pci-device-virtio-net
        0000000040043000-0000000040043fff (prio 0, i/o): virtio-pci-notify-virtio-net
      0000000040044000-0000000040044fff (prio 1, i/o): virtio-net-pci-msix
        0000000040044000-000000004004403f (prio 0, i/o): msix-table
        0000000040044800-0000000040044807 (prio 0, i/o): msix-pba
...
```

Some notes:

1. If a load/store visit some physical address but it does not exist in cpu-memory-0 address space, an exception will be triggered.
2. For addresses that do exist, the access might not work at all. For i/o address ranges, it depends on whether qemu has handled all the addresses in that range. For example, [0-0x428) is claimed by iocsr_misc,

    0000000000000000-0000000000000427 (prio 0, i/o): iocsr_misc

The accesses falled in this range will be handled by loongarch_qemu_ops:

    memory_region_init_io(&env->iocsr_mem, OBJECT(la_cpu), &loongarch_qemu_ops,
                              NULL, "iocsr_misc", IOCSR_MEM_SIZE);

but loongarch_qemu_ops is implemented like this:

    static void loongarch_qemu_write(void *opaque, hwaddr addr,
                                     uint64_t val, unsigned size)
    {
    }

    static uint64_t loongarch_qemu_read(void *opaque, hwaddr addr, unsigned size)
    {
        uint64_t feature = 0UL;

        switch (addr) {
        case FEATURE_REG:
            feature |= 1UL << IOCSRF_MSI | 1UL << IOCSRF_EXTIOI |
                       1UL << IOCSRF_CSRIPI;
            return feature ;
        case VENDOR_REG:
            return *(uint64_t *)"Loongson";
        case CPUNAME_REG:
            return *(uint64_t *)"3A5000";
        case MISC_FUNC_REG:
            return 1UL << IOCSRM_EXTIOI_EN;
        }
        return 0;
    }

    static const MemoryRegionOps loongarch_qemu_ops = {
        .read = loongarch_qemu_read,
        .write = loongarch_qemu_write,
        .endianness = DEVICE_LITTLE_ENDIAN,
        .valid = {
            .min_access_size = 4,
            .max_access_size = 8,
        },
        .impl = {
            .min_access_size = 4,
            .max_access_size = 8,
        },
    };

Nothing will be do for writes, and only 4 addresses will give out meaningful responses. Besides, only 4 or 8 bytes accesses are allowed.

* to make sure the exact emulated status, the best way is to check the qemu source code: hw/loongarch/loongson3.c and the source code for referenced qemu device classes(like hw/intc/loongarch_extioi.c for TYPE_LOONGARCH_EXTIOI).

### interrupt hierarchy

External interrupts roughly goes into the cpu via such path: sources -> level3 -> level 2 -> level 1

    - level 1: CPU core interrupt controller(HWI0-HWI7)
    - level 2: extended Interrupt controller(extioi_pic in qemu, 256 irqs)
    - level 3: LS7A1000 interrupt controller(pch_pic in qemu, 64 irqs; pch_msi, 192 irqs, refer to 7A1000 user manual)
    - interrupt sources:
        - 4 PCIE interrupt output connect to pch_pic 16,17,18,19
        - LS7A UART/RTC/SCI connect to pch_pic 2/3/4

Notes on the cpu core interrupt controller:

1. Refer to LoongArch architecture manual, chapter 6
2. some interrupts generated by cpu itself are connected to this intc directly, such as stable timer, performance counter, IPI(inter-processor-interrupt) etc.

Notes on the extioi interrupt controller:

1. extioi stand for extended io interrupt. Refer to 3A5000 user manual 11.2.
2. Each extioi irq is mapped to one of the cpu core intc inputs(HWI0-7)
    - every 32bit as a group is mapped to cpu core intc by iocsr reg 0x14c0-0x14c7(table 11-12). By default, all route regs are zero, all 256 irqs are mapped to HWI0(cpu int 2). 
    - Each extioi irq is mapped to some cpu core, by iocsr reg 0x1c00-0x1cfe. For example, if iocsr 0x1c00 is written to 0x48, the cpu use will value in EXT_IOI_node_type4 as target node, then use core 3(8=1000, so bit 3 set) in that node. Each node of Loongson3 has 4 cores, and maximum 16 nodes are supported in a system. By default, all irqs routed to node 0 core 0. 
    - Bounce mode not supported. So for each node type, only one node bit out of total 16 bits can be set to 1.
3. Description of qemu's implementation by registers:
    - table 11-6. iocsr 0x420 is not fully emulated, read only, writes discarded. EXT_INT_en is default on.
    - table 11-7. work partly. only take effect at changing time. Other times are effectively always enabled.
    - table 11-8. Read/Writable, but no real effect.
    - table 11-9, not supported.
    - table 11-10, work as expected.
    - table 11-12, work as expected.
    - table 11-13/14/15, mostly work as expected, no bounce support

Notes on LS7A1000 interrupt controller

1. Refer to LS7A1000 user manual, chapter 5.
2. all pch_pic irqs are mapped to extioi_pic irqs by ht message interrupt vectors(offset 0x200-0x238). By default, all irqs are mapped to extioi input 0.
3. Description of qemu's implementation by registers:
    - offset 0/0x20/0x60/0x80/0x380/0x3a0/0x3e0, work
    - offset 0x40/0xc0/0xe0/0x100-0x13f, read/writable, no effect
    - offset 0x200-0x23f, work as a general routing map, default all zero. each byte map a pch_pic to extioi
    - offset 0x300/0x320, not supported

To support Uart(io port at 0x1fe001e0) interrupt, you should:
   1. set LS7A1000 interrupt controller:
        - unmask input pin2 (bit 2 of reg at 0x20)
        - set edge trigger or level trigger mode(0x60)
        - set ht message interrupt vector (byte at offset 0x202 equal the target extioi irq number)
   2. setup extioi
        - enable mapped extioi irq
   3. setup cpu core irq
   4. global irq enable

When a uart irq triggers, you should:

1. ack cpu intc
    - level triggered, by clear the input source, that is, use next step to make input from extioi low
2. ack extioi intc
    - by write 1 to its isr reg(iocsr 0x1800-)
3. ack LS7A1000 intc
    - by write 1<<irq bit to intclr reg(edge triggered mode)
4. handle the device state(receive pending chars or send out fifo chars)
       
There are some internal interrupts that can go other ways. For example, inter-core IPI,
timer, performance counter interrupt goes directly into cpu core.

Read qemu source code(hw/intc/loongarch_*, hw/loongarch/loongson3.c) for more exact information. 

## files

Here is a short explanation for major files in this repository:

* qemu-system-loongarch64. The emulator binary, meant to be run on Linux/X86_64. Built on ubuntu 20.04.
* vgabios-stdvga.bin, efi-virtio.rom, vgabios-virtio.bin. Some rom files used by qemu.
* loongarch_bios_0310.bin, loongarch_bios_0310_debug.bin. The loongarch UEFI bios binary.
* vmlinux. The loongarch linux kernel binary.
* busybox-rootfs.img. A minimal root file system for linux/loongarch built out of busybox 1.35.0.
* loongarch-cross-gdb_v12.0.50.20220221-git.tar.bz2. Cross gdb to debug loongarch binaries on X86. sudo tar xjvf -C /opt/

## source and build methods

For bios, kernel and busybox, the resulting code is loongarch, so a cross compiling toolchain is needed. Used toolchain is [here](https://github.com/loongson/build-tools/releases/download/2021.12.21/loongarch64-clfs-2021-12-18-cross-tools-gcc-full.tar.xz). Unpack this package to /opt, then add /opt/cross-tools/bin to your PATH.

    foxsen@foxsen-ThinkPad-T450:~/xinyan/algo$ /opt/cross-tools-1218/bin/loongarch64-unknown-linux-gnu-gcc -v
    Using built-in specs.
    COLLECT_GCC=/opt/cross-tools-1218/bin/loongarch64-unknown-linux-gnu-gcc
    COLLECT_LTO_WRAPPER=/opt/cross-tools-1218/bin/../libexec/gcc/loongarch64-unknown-linux-gnu/12.0.0/lto-wrapper
    Target: loongarch64-unknown-linux-gnu
    Configured with: ../configure --prefix=/opt/cross-tools --build=x86_64-cross-linux-gnu --host=x86_64-cross-linux-gnu --target=loongarch64-unknown-linux-gnu --with-sysroot=/opt/cross-tools/target --disable-multilib --enable-nls --enable-__cxa_atexit --enable-threads=posix --with-system-zlib --enable-libstdcxx-time --enable-checking=release --enable-languages=c,c++,fortran,objc,obj-c++,lto
    Thread model: posix
    Supported LTO compression algorithms: zlib zstd
    gcc version 12.0.0 20211202 (experimental) (GCC)

This toolchain has two known flaws on ubuntu platforms:

1. Due to statically linking, the bfd plugin libdep.so will lead to failures. rm -f /opt/cross-tools/lib/bfd-plugins/libdep.so can fix it.
2. Also due to statically linking, the locale handling is not so right that it fail the kernel script check. run the following command before make:

    export LC_ALL=C;
    export LANG=C;
    export LANGUAGE=C

Otherwise you will see error like this:

      HOSTCC  scripts/kconfig/symbol.o
      HOSTCC  scripts/kconfig/util.o
      HOSTLD  scripts/kconfig/conf
    loongarch64-unknown-linux-gnu-gcc: unknown assembler invoked
    scripts/Kconfig.include:50: Sorry, this assembler is not supported.
    make[2]: *** [scripts/kconfig/Makefile:77：syncconfig] 错误 1
    make[1]: *** [Makefile:619：syncconfig] 错误 2
    make: *** [Makefile:721：include/config/auto.conf.cmd] 错误 2
    make: *** [include/config/auto.conf.cmd] 正在删除文件“include/generated/autoconf.h”

Will update this when a better version is released.

Note: Please use given toolchain. Don't use gcc from gcc.org. Support for loongarch is not yet upstreamed! And we need a cross compiling toolchain, that is, running on x86 and producing code for loongarch.
Note: Please DON'T use gcc8 from loongnix: the ABI is changing! we are using the lastest open source BIOS and kernel code, which requires open source toolchain(gcc v12.x).

### qemu

The binary here is built from https://github.com/foxsen/qemu.git tag loongarch-v2022-03-10-1 on ubuntu 20.04.

git clone https://github.com/foxsen/qemu.git
<install necessary dependence packages, refer to qemu build doc>
git checkout loongarch-v2022-03-10-1
cd qemu
mkdir build
cd ./build/
../configure --target-list=loongarch64-softmmu --disable-werror (调试版本加--enable-debug)
make -j4

Check branch loongarch for possible updates.

### UEFI bios

The binary here is built from https://github.com/foxsen/{edk2.git,edk2-platforms.git} tag loongarch-v2022-03-10 on centos stream 8.


On ubuntu 20.04 the following error is met so we change to a centos platform:

    foxsen@foxsen-ThinkPad-T450:~/software/uefi$ build --buildtarget=DEBUG --tagname=GCC5 --arch=LOONGARCH64  --platform=Platform/Loongson/LoongArchQemuPkg/Loongson.dsc
    Build environment: Linux-5.4.0-100-generic-x86_64-with-glibc2.29
    Build start time: 00:21:56, Mar.10 2022

    WORKSPACE        = /home/foxsen/software/uefi
    PACKAGES_PATH    = /home/foxsen/software/uefi/edk2:/home/foxsen/software/uefi/edk2-platforms:/home/foxsen/software/uefi/edk2-non-osi
    EDK_TOOLS_PATH   = /home/foxsen/software/uefi/edk2/BaseTools
    CONF_PATH        = /home/foxsen/software/uefi/edk2/Conf
    PYTHON_COMMAND   = /usr/bin/python3.8


    Architecture(s)  = LOONGARCH64
    Build target     = DEBUG
    Toolchain        = GCC5

    Active Platform          = /home/foxsen/software/uefi/edk2-platforms/Platform/Loongson/LoongArchQemuPkg/Loongson.dsc

    Processing meta-data ......

    build.py...
     : error F000: CC_PATH doesn't exist in GCC5 ToolChain and LOONGARCH64 Arch.
        [/home/foxsen/software/uefi/edk2/MdeModulePkg/Universal/PCD/Pei/Pcd.inf]
    .

    - Failed -
    Build end time: 00:22:03, Mar.10 2022
    Build total time: 00:00:07

Build method:

```bash
git clone https://github.com/foxsen/edk2.git
git checkout loongarch-v2022-03-10
git clone https://github.com/foxsen/edk2-platforms.git
git checkout loongarch-v2022-03-10
```

Refer to edk2-platforms Platform/Loongson/LoongArchQemuPkg/Readme.md for detail build instructions.

For debug build, resulting bios binary is Build/LoongArchQemu/DEBUG_GCC5/FV/QEMU_EFI.fd.
For release build, resulting bios binary is Build/LoongArchQemu/RELEASE_GCC5/FV/QEMU_EFI.fd.

Check loongarch-qemu branch of edk2 and edk2-platforms for updates.

### linux kernel

The binary here is built from https://github.com/foxsen/linux.git tag loongarch-v2022-03-10-1 on ubuntu 20.04.

git clone https://github.com/foxsen/linux.git
cd linux
set cross compiler path and environment. Refer to the script below:
```bash
CC_PREFIX=/opt/cross-tools

export PATH=$CC_PREFIX/bin:$PATH
export LD_LIBRARY_PATH=$CC_PREFIX/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$CC_PREFIX/loongarch64-unknown-linux-gnu/lib/:$LD_LIBRARY_PATH

export ARCH=loongarch
export CROSS_COMPILE=loongarch64-unknown-linux-gnu-
```
You can create a file with the above content, for example, env.sh. then type:

```bash
source env.sh
```

before the following operations. This is to setup necessary environments in current shell.
Don't run it like this because it won't affect current shell:

```bash
./env.sh
```

```bash
cp arch/loongarch/configs/loongson3_defconfig_qemu .config
make menuconfig
make -j4 (if your machine have more cpu cores, 4 can be changed to the core number you have)
```

Check loongarch-qemu branch for updates.

### gdb

The binary here is built from https://github.com/foxsen/binutils-gdb tag loongarch-v2022-03-10 on ubuntu 20.04.

git clone https://github.com/foxsen/binutils-gdb
git checkout loongarch-v2022-03-10
mkdir build
cd build
../configure --target=loongarch64-unknown-linux-gnu --prefix=/opt/gdb
make
sudo make install

A simple example session is demoed in the following pictures:

![run with debug on](./pics/run-with-debug.png)
![remote debug](./pics/remote-debug.png)

## some notes

### about the dependence of qemu/gdb binary

Different hosts have different installed packages and versions, it is normal that the provided qemu and gdb binary might fail to run. Please building your own version according the instructions in previous section.

### Cross debugging the linux kernel

The vmlinux binary in the repository has been stripped off its symbols to reduce the size.

To make a meaningful debug, please build a kernel with debug information as previous section's instruction.

Install cross gdb: cd /opt; sudo tar xjvf <path to loongarch-cross-gdb_v12.0.50.20220221-git.tar.bz2>

Then:

1. run qemu with debug stub on, that is, run_loongarch.sh -d (or -D if you want to qemu to wait for debugger before run). Qemu will listen on port 1234 for debugger attachment.
2. in another terminal, use cross gdb:
    * /opt/gdb/bin/loongarch-unknown-linux-gnu-gdb ./vmlinux
    * input "target remote :1234" to attach to the qemu session
    * use gdb to debug kernel actions. For example, you can 'break *0x9000000002000000' to break at the kernel entrypoint. Then type 'continue' to continue run.

### 关于内核启动

龙芯之前定义了一个启动规范，定义了BIOS和内核的交互接口，但是在推动相关补丁进入上游社区时，内核的维护者们提出了不同意见。社区倾向于采用EFI标准提供的启动功能，即编译内核时生成一个vmlinux.efi这样的EFI模块，它可以不用任何grub之类的装载器实现启动。因为还没有最终定论，导致龙芯开源版本的内核和BIOS互相没有直接支持。因此我们不得不从github.com/loongson fork了相应的软件，进行了一点修改。这里对目前的启动约定做一个简单的说明：

* UEFI bios装载内核时，会把从内核elf文件获取的入口点地址（可以用readelf -h或者-l vmlinux看到）抹去高32位使用。比如vmlinux链接的地址是0x9000000001034804，实际bios跳转的地址将是0x1034804，代码装载的位置也是物理内存0x1034804。BIOS这么做是因为它逻辑上相当于用物理地址去访问内存，高的虚拟地址空间没有映射不能直接用。
* 内核启动入口代码需要做两件事：（参见arch/loongarch/kernel/head.S）

    1. 设置一个直接地址映射窗口（参见loongarch体系结构手册，5.2.1节），把内核用到的64地址抹去高位映射到物理内存。目前linux内核是设置0x8000xxxx-xxxxxxxx和0x9000xxxx-xxxxxxxx地址抹去最高的8和9为其物理地址，前者用于uncache访问(即不通过高速缓存去load/store)，后者用于cache访问。
    2. 做个代码自跳转，使得后续代码执行的PC和链接用的虚拟地址匹配。BIOS刚跳转到内核时，用的地址是抹去了高32位的地址（相当于物理地址），步骤1使得链接时的高地址可以访问到同样的物理内存，这里则换回到原始的虚拟地址。

head.S相应代码如下：

    SYM_CODE_START(kernel_entry)            # kernel entry point
        /* Config direct window and set PG */
        li.d        t0, CSR_DMW0_INIT   # UC, PLV0, 0x8000 xxxx xxxx xxxx
        csrwr       t0, LOONGARCH_CSR_DMWIN0
        li.d        t0, CSR_DMW1_INIT   # CA, PLV0, 0x9000 xxxx xxxx xxxx
        csrwr       t0, LOONGARCH_CSR_DMWIN1

        /* We might not get launched at the address the kernel is linked to,
         * so we jump there. We must setup direct access window before this.
         */
        la.abs      t0, 0f  //把标号0的地方的代码地址装入寄存器t0
        jirl        zero, t0, 0 //跳转到t0，此时t0是链接是用的高位没有抹去0的虚拟地址，而前面设置的窗口保证了它能访问到bios将其装入的那段物理内存
    0:
        la      t0, __bss_start     # clear .bss
        st.d        zero, t0, 0
        la      t1, __bss_stop - LONGSIZE

新移植内核的时候，可以参考linux kernel的实现。 远程调试的时候，建议不要把断点设置到最早的几条只用低位访问的代码（标号0:之后就行了）

### TODO

* Improve the bios speed. Presently, the bios code is not customized for qemu. It will take around 10 seconds to boot to the point of kernel loading. We plan to reduce this time later.
* some full examples of kernel debugging
* a docker file to reproduce all the components
* More tests and fixes

## References

* See https://github.com/loongson and https://github.com/loongarch64 for loongarch documents and important software repositories.
* See https://github.com/sunhaiyong1978/CLFS-for-LoongArch/ to learn some knowledge about linux distribution making. The beginning part of the CLFS document is how to make the cross toolchain.
* See https://qemu.org for general qemu related documents.

