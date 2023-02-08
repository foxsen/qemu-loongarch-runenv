qemu-system-loongarch64 -machine virt -m 1024 -cpu la464-loongarch-cpu \
      -smp 1 \
      -bios ./tianocore/Build/LoongArchQemu/RELEASE_GCC5/FV/QEMU_EFI.fd \
      -kernel ./linux/arch/loongarch/boot/vmlinuz.efi \
      --drive id=disk0,file=./poky-minimal.ext4,if=none,format=raw -device virtio-blk-pci,drive=disk0
      -append "root=/dev/vda rdinit=/sbin/init console=ttyS0,115200" \
      --nographic
