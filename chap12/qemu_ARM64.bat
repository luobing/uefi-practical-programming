qemu-system-aarch64 -M virt -cpu cortex-a57 -bios QEMU_EFI.fd -device virtio-gpu-pci  -hda fat:rw:hda-contents/ -net none
