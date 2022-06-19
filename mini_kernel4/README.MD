# mini kernel

在mini kernel3的基础上，展示键盘中断处理。

## 注意1: 要使用本测试，模拟器需要更新到以下commit之后的版本。

    commit ac069a8ffb8f41fcbc7ed0aa5d420a71fff36581 (HEAD -> loongarch, origin/loongarch)
    Author: Zhang Fuxin <fxzhang@ict.ac.cn>
    Date:   Sun Jun 19 17:10:49 2022 +0800

        add pc keyboard support.
        
        The pc PS2 keyboard controller is added at 0x1fe00060, with
        keyboard interrupt connected to ls7a irq 3/4.

## 注意2: 为了方便观察输出，启动命令行可以设为：

    ./qemu-system-loongarch64 -m 4G -smp 1 -bios ./loongarch_bios_0310.bin -kernel ./mini_kernel4/kernel -vga virtio -serial stdio

其中-vga选项使能图形选项，键盘输入只有在有图形界面的情况下起作用。-serial stdio在命令行界面实现一个模拟串口，用于观察内核的输出。


## 预期界面

在图形界面试图输入一些键，每输入一个键，应该有两次键盘中断，内核打印出其扫描码。

命令行输出：

    There is 2 args for kernel:
    cmd arg 0: a0
    cmd arg 1: root=/dev/ram console=ttyS0,115200 rdinit=/init
    efi system table at 0x00000000fffd0018
    efi extend list at 0x00000000fffda430
    keyboard reponse 55
    test result 0
    reset result fa

    estat 4, ecfg bfc
    era=0x00000000001005d8 eentry=0x0000000000101000
    irq=1c
    aa key done
    mouse interrupt

    estat 800, ecfg bfc
    era=0x000000000010036c eentry=0x0000000000101000
    timer interrupt

    estat 4, ecfg bfc
    era=0x000000000010036c eentry=0x0000000000101000
    irq=8
    29 key done

    estat 4, ecfg bfc
    era=0x000000000010036c eentry=0x0000000000101000
    irq=8
    f0 29 key done

