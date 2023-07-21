# shadow stack kernel

这是用于测试shadow stack功能的小内核。

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

## 运行测试

为了简单起见，这个测试内核可以直接装载运行，不需要bios配合。

借用-d pcall来输出shadow stack相关的调试信息。

例如，如下命令可以运行测试内核，并把影子栈的访问日志输出到log文件

```bash
qemu-system-loongarch64 -kernel ./kernel -nographic -d pcall 2> log
```
