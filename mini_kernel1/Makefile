OBJS := hello_kernel.o
TOOLPREFIX = loongarch64-unknown-linux-gnu-

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld

CFLAGS = -Wall -O2 -g3 -march=loongarch64 -mabi=lp64s -ffreestanding -fno-common -nostdlib -I. -fno-stack-protector -fno-pie -no-pie 
LDFLAGS = -z max-page-size=4096

kernel: $(OBJS) ld.script
	$(LD) $(LDFLAGS) -T ld.script -o kernel $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: kernel 

clean: 
	rm -f *.o ./kernel
