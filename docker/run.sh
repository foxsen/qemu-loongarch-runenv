qemu-system-loongarch64 -machine virt -m 4G -cpu la464-loongarch-cpu \
      -smp 1 \
      -bios /qemu-loongarch-runenv/edk2-LoongarchVirt/Build/LoongArchQemu/RELEASE_GCC5/FV/QEMU_EFI.fd \
      -kernel /qemu-loongarch-runenv/linux/arch/loongarch/boot/vmlinuz.efi \
      -initrd /qemu-loongarch-runenv/qemu-binary/ramdisk \
      -append "root=/dev/ram rdinit=/sbin/init console=ttyS0,115200" \
      --nographic
