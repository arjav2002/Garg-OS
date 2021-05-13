make -B
qemu-system-i386 -drive file=bin/os_image.iso,format=raw,index=0,media=disk -device ich9-usb-uhci1 -drive id=my_usb_disk,file=usbtestfile.iso,if=none -device usb-storage,drive=my_usb_disk -d guest_errors -monitor stdio -no-reboot -no-shutdown 
@echo off
rem for emulating XHCI use -drive if=none,id=stick,file=usbtestfile.iso -device nec-usb-xhci,id=xhci -device usb-storage,bus=xhci.0,drive=stick
rem or you can also use -device qemu-xhci
rem for emulating UHCI use:
rem -device ich9-usb-uhci1 \
rem -drive id=my_usb_disk,file=usbtestfile.iso,if=none \
rem -device usb-storage,drive=my_usb_disk \
rem for emulating EHCI use -device usb-ehci
rem for emulating OHCI use -device pci-ohci
rem for more info on USB emulation in QEMU visit https://github.com/qemu/qemu/blob/master/docs/usb-storage.txt https://github.com/qemu/qemu/blob/master/docs/usb2.txt https://en.wikibooks.org/wiki/QEMU/Devices/USB/Root
rem for booting from HDD use -drive file=bin/os_image.iso,format=raw,index=0,media=disk 
