# mini kernel

这是一个最小化的内核态运行的程序，它只是启动后打印一个'hello kernel'字符串。

它利用bios提供的elf文件装载和运行功能来实现内核启动，没用到任何汇编代码。

## 代码编译

内核的编译和常规程序有所不同，这里我们用到的编译选项如下：

    CFLAGS = -Wall -O0 -g3 -march=loongarch64 -mabi=lp64s -ffreestanding -fno-common -nostdlib -I. -fno-stack-protector -fno-pie -no-pie 

其中:

* -Wall -O0 -g3是常规的警告信息、优化级别和调试级别选项，可以根据需要修改
* -march=loongarch64 -mabi=lp64s是指定相应的目标架构和ABI(lp64s表示64位、不带浮点支持的LA ABI)
* -ffreestanding选项是告诉编译器不要假设有标准库存在，程序也不一定从main()函数开始，它隐含了-fno-builtin，编译器不会自动识别一些内置的函数(如memcpy)
* -fnostdlib，不链接一些系统启动文件和库(如/crt*.o/libc/libgcc等)
* -fno-common，告诉编译器不要把未初始化的全局变量放到一个common block，而是放到BSS中
* -I. 允许编译器在当前目标找头文件
* -fno-stack-protector，不要自动生成栈保护（检测栈是否溢出等的代码）
* -fno-pie -no-pie，不要生成位置无关的可执行代码。内核的位置一般不会动，这个选项可能提升性能。但如果你的内核需要支持位置无关以提高安全性，则不用这些选项。

## 代码链接

Makefile中使用了定制化的链接脚本来链接内核的目标文件：

	$(LD) $(LDFLAGS) -T ld.script -o kernel $(OBJS)

ld.script指定了目标elf文件的入口为kernel_entry，约定了链接的起始地址，以及如何把各个目标文件的section组合起来等等。

## 从BIOS到内核

BIOS与内核的交互接口是计算机系统中最重要的接口之一，它约定了bios如何与内核进行信息交互，传递内核命令行参数以及当前硬件的各种配置（内存范围、硬件设备类型、中断、资源分配等），详细情况可以参见[龙芯BIOS和内核接口规范v3.1](./loongarch_bios_and_kernel_interface_specification_v3.1.pdf)。

这里采用的龙芯开源UEFI实现也遵循这个规范，被调用的内核入口函数，其第一个参数将是内核命令参数个数，第2个参数将是内核命令参数地址，第3个是BootParamsInterface数据结构的地址，用来传递各种配置信息。

内存方面，BIOS实现了虚拟地址和物理地址相等的一个映射。为了简单起见，本内核利用了这个映射，跳过了常规的汇编初始化代码。在实际的内核代码中，内核将会接管物理内存和虚拟内存，不能一直依赖BIOS建立的映射，也要注意说使用的内存不会破坏BIOS用于传递参数的区域。

链接脚本把内核链接到0x100000开始的地址（既是虚拟地址，也是物理地址）。qemu的-kernel选项可以装载和运行指定内核，在BIOS初始化完毕后，它装入指定内核，然后根据内核ELF文件，找到它的入口地址，跳转到该地址把控制权交给内核。

BIOS启动操作系统有很多可能的做法：

* 像这里这样，直接装载和启动内核elf文件。不少小型操作系统会以elf可执行文件形式提供自己的loader，通常的做法是把内核、硬件配置以及初始文件系统以某种形式打包成一个elf文件，该elf文件运行后找到内核、配置信息和文件系统，进行解压等必要的处理后运行内核，内核就能按某种约定找到硬件配置信息和文件系统。
* 利用UEFI规范提供的启动支持，把内核和相关系统文件生成一个EFI文件，按UEFI模块的形式去执行。 目前Linux倾向于从elf启动转到efi启动。
* 利用grub等第三方loader。 这些loader要做的事情包括支持各种启动设备、分区格式、文件格式，支持内核、启动参数、配置信息和文件系统等组件的灵活交互。

## 运行测试

在项目顶层目录下，用./run_loongarch.sh -k ./mini_kernel1/kernel运行，应该在终端上看到'hello kernel'输出。可以使用gdb远程调试，将断点设置到kernel_entry，然后跟踪内核的执行。
