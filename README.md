Category
========

* [Hardware Versions](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README.md#hardware-versions)
* [Rescue using hisi_idt](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README.md#rescue-using-hisi_idt)
* [Rescue From Endless-Rebooting](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README.md#rescue-from-endless-rebooting) 
* [Flash Images by Bootloader](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README.md#flash-images-by-bootloader)
* [How to create boot.img and dt.img](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README.md#how-to-create-bootimg-and-dtimg)

Hardware Versions
=================

There are two versions of HiKey960: v1 and v2. One important difference you should know is: v1 is using Jumper (J2001), while v2 is using DIP switch (SW2201). Both are located in the bottom side of board.

* Jumper Pin 1-2 = DIP switch 1
* Jumper Pin 3-4 = DIP switch 2
* JUmper Pin 5-6 = DIP switch 3

Rescue Using hisi_idt
=====================

This document descirbes the steps to use hisi-idt.py to download binaries to HiKey960 through serial port. On HiKey960, serial port means a ttyUSB device emulated from its OTG port.

1. Command syntax
-----------------

```sh
sudo ./hikey_idt -c config -p /dev/ttyUSB1
```

2. Download to DDR
------------------

* In the release package, please find these files:
  - sec_usb_xloader.img
  - sec_uce_boot.img
  - sec_fastboot.img
* Change Jumper settings, to enter forced-download mode:
  - find J2001, short Pin 1-2
  - find J2001, short Pin 3-4
* Apply power to the board.
* Insert USB Type-C cable (OTG port) to the board, and connect the other end to your Linux PC, then
* Check whether there is a device node "/dev/ttyUSBx". If there is, it means your PC has detected the target board; If there is not, try to repeat previous steps.
* Prepare the config file. The third image runs on Cortex A53 core, and others are not. You need to put all images into the same directory of hikey_idt. The contents should be in below.
  - ./sec_usb_xloader.img 0x00020000
  - ./sec_uce_boot.img 0x6A908000
  - ./sec_fastboot.img 0x1AC00000
* Run the script. Eg. if the device node you get from step e is "/dev/ttyUSB1", then use "-d /dev/ttyUSB1". A complete command line looks like:
```sh
$ sudo ./hikey_idt -c config -p /dev/ttyUSB1
```
* Once images are downloaded successfully, it will print out below log:
```sh
Config name: config
Port name: /dev/ttyUSB1
0: Image: ./sec_usb_xloader.img Downalod Address: 0x20000
1: Image: ./sec_uce_boot.img Downalod Address: 0x6a908000
2: Image: ./sec_fastboot.img Downalod Address: 0x1ac00000
Serial port open successfully!
Start downloading ./sec_usb_xloader.img@0x20000...
file total size 104256
downlaod address 0x20000
Finish downloading
Start downloading ./sec_uce_boot.img@0x6a908000...
file total size 24000
downlaod address 0x6a908000
Finish downloading
Start downloading ./sec_fastboot.img@0x1ac00000...
file total size 3110912
downlaod address 0x1ac00000
Finish downloading
```

3. Burn Images
--------------

After download these images onto the board DDR, you MUST use 'fastboot' tool to burn images onto non-volatile storage, saying UFS for HiKey960. Otherwise, those images are in DDR ram, and it gets lost once you plug off the power.

Note: Don't forget to remove Jumper J2001 Pin 3-4 after you 'flash'ed these images.
```sh
# partition table
fastboot flash ptable ${IMG_FOLDER}/ptable.img

# bootloader
sudo fastboot flash xloader  $(IMG_FOLDER)/sec_xloader.img
sudo fastboot flash fastboot $(IMG_FOLDER)/fastboot.img

# extra images
fastboot flash nvme   ${IMG_FOLDER}/nvme.img
fastboot flash fw_lpm3       ${IMG_FOLDER}/lpm3.img
fastboot flash trustfirmware ${IMG_FOLDER}/bl31.bin

# Depending on your needs, the following is optional
# kernel
sudo fastboot flash boot $(IMG_FOLDER)/boot.img
sudo fastboot flash dts $(IMG_FOLDER)/dt.img
# AOSP
sudo fastboot flash system $(IMG_FOLDER)/system.img
sudo fastboot flash userdata $(IMG_FOLDER)/userdata.img
sudo fastboot flash cache $(IMG_FOLDER)/cache.img
```

4. Troubleshooting
------------------

* Need supervisor permission for hisi-idt: "sudo ./hikey_idt"
* Need supervisor permission for fastboot: "sudo fastboot"
* See "rescue from endless-rebooting" below.

Rescue From Endless-Rebooting
=============================

Sometimes, usually after completing powered-off, HiKey960 enters continous rebooting mode. You will notice the following prints in serial console:

```sh
oeminfo: ERROR: oeminfo index 93 not find.
usbloader: cannot get oeminfo lock state info
oeminfo: ERROR: oeminfo index 67 not find.
usbloader: cannot get oeminfo root type info
oeminfo: ERROR: oeminfo index 95 not find.
no_module: get oeminfo fail
oeminfo: ERROR: oeminfo index 98 not find.
no_module: get oeminfo fail
rescue: ^^^^^^^^^[rescue_init] ok !
usbloader: bootmode is 0
shutdown: Enter into shutdown mode!
ufs: ufs power mode = 0x00000033
reboot_reason: set_reboot_type is 0x000
```

To rescue from this error, you need to follow instructions in Above, plug in Jumper J2001 Pin 3-4, using hisi_idt.

Flash Images by Bootloader
============================

You can use fastboot protocol to flash images to HiKey960. To enter fastboot flashing mode, you need to use a serial mezzanine board. See http://www.96boards.org/product/uarts/

* Connect serial mezzanine to HiKey960. Connect micro-USB to PC. And PC side, config USB serial as: 115200, 8N1
* Power up HiKey960:
  - Plug in DC power, then
  - Plug in USB Type-C cable, and connect the other end to your PC.
* On PC side, in serial log, you should observe the following boot message:
```sh
xloader chipid is: 0x36600110, start at 449ms.
Build Date: Nov  1 2016, 16:14:10
[clock_init] +
hi3660 [clk_setup]
[clock_init] --
storage type is UFS
C0
hpm:00000178@857
0:703->626
1:801->710
2:906->808
3:1004->927
4:1102->983
HpmAvs:17ms
C1
hpm:0000019c@815
0:703->626
1:801->717
2:906->801
3:1004->899
4:1102->1067
HpmAvs:11ms
pavs 0
pavs:499ms
pavs 1
pavs:674ms
pavs 2
pavs:938ms
```
* When you any of these lines of messages, press the 'reset' button on serial
   mezzanine.
```sh
pavs 0
pavs:499ms
pavs 1
pavs:674ms
pavs 2
pavs:938ms
```
* This should trigger HiKey960 to reboot with a special boot reason, and enter fastboot flash mode. Confirm that succeeds by oberving the following messages:
```sh
usbloader: bootmode is 4
usb: [USBFINFO]USB RESET
usb: [USBFINFO]USB CONNDONE, highspeed
usb: [USBFINFO]USB RESET
usb: [USBFINFO]USB CONNDONE, highspeed
usbloader: usb: online (highspeed)
usb: [USBFINFO]usb enum done
```
* At this point, on your PC, you should be able to run "sudo fastboot flash
   xxx [xxx.img]". Eg.
```sh
$ sudo fastboot devices
0123456789ABCDEF	fastboot
$ sudo fastboot flash boot boot.img
target reported max download size of 471859200 bytes
sending 'boot' (7548 KB)...
OKAY [  0.198s]
writing 'boot'...
OKAY [  0.069s]
finished. total time: 0.267s
$
```

How to create boot.img and dt.img
=================================

This section describes how to create boot.img and dt.img for use with AOSP. First of all, please make sure you downloaded (or git clone) these two tools from this repository:
```
mkbootimg
mkdtimg
```

Please download this script file, and modify it according to your local environment. Then run it.
```
$ ./gen_boot_dts_imgs-sample.sh
```

Note: Since boot.img depends on ramdisk.img which is generated from AOSP build, please make sure your AOSP build being done first.
