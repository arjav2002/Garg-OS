make -B
qemu-system-i386 -drive file=bin/os_image.iso,format=raw,index=0,media=disk -drive if=none,id=stick,file=usbtestfile.iso,format=raw -usb -device usb-storage,bus=usb-bus.0,drive=stick -d guest_errors -monitor stdio -no-reboot -no-shutdown 
# for emulating XHCI use -drive if=none,id=stick,file=usbtestfile.iso -device nec-usb-xhci,id=xhci -device usb-storage,bus=xhci.0,drive=stick
# or you can also use -device qemu-xhci
# for emulating UHCI use -device ich9-usb-uhci1
# for emulating EHCI use -device usb-ehci
# for emulating OHCI use -device pci-ohci
# for more info on USB emulation in QEMU visit https://github.com/qemu/qemu/blob/master/docs/usb-storage.txt https://github.com/qemu/qemu/blob/master/docs/usb2.txt https://en.wikibooks.org/wiki/QEMU/Devices/USB/Root
# for booting from HDD use -drive file=bin/os_image.iso,format=raw,index=0,media=disk 
