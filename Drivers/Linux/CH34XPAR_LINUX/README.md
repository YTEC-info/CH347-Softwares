# ch341/ch347 linux SDK

## Description

This directory contains 3 parts, usb device driver, application library and examples. This driver and applications support usb bus interface chip ch341 and ch347, ch341 implements full-speed usb to uart/spi/i2c/gpio/parport/etc., ch347 implements high-speed usb to uart/jtag/spi/i2c/gpio/etc. This driver is not applicable to uart.

## Driver Operating Overview

1. Open "Terminal"
2. Switch to "driver" directory
3. Compile the driver using "make", you will see the module "ch34x_pis.ko" if successful
4. Type "sudo make load" or "sudo insmod ch34x_pis.ko" to load the driver dynamically
5. Type "sudo make unload" or "sudo rmmod ch34x_pis.ko" to unload the driver
6. Type "sudo make install" to make the driver work permanently
7. Type "sudo make uninstall" to remove the driver

Before the driver works, you should make sure that the ch341/ch347 device has been plugged in and is working properly, you can use shell command "lsusb" or "dmesg" to confirm that, VID of ch341/ch347 is [1A86].

If ch341/ch347 device works well, the driver will created devices named "ch34x_pis*" in /dev directory.

## Application Operating Overview

1. Copy the dynamic library in "lib" directory to system default library path: "sudo cp libch347.so /usr/lib"
2. Switch to "demo" directory
3. Type "gcc ch347_demo.c -o app -lch347" to compile the demo

## Note

**ch341 supports 2 modes:**

mode0: [uart]

mode1: [spi + i2c + gpio]

**ch347 supports 4 modes:**

mode0: [uart * 2] in vcp/cdc driver mode, the devices are named /dev/tty*

mode1: [spi + i2c + uart * 1] in vcp driver mode, the devices are named /dev/ch34x_pis* and /dev/tty*

mode2: [spi + i2c + uart * 1] in hid driver mode, the devices are named /dev/hidraw*

mode3: [jtag + uart * 1] in vcp driver mode, the devices are named /dev/ch34x_pis* and /dev/tty*

This driver only can be used with **ch341 mode1** or **ch347 mode1/mode3**.

This application library and examples can be used with all modes.

Any question, you can send feedback to mail: tech@wch.cn
