#!/bin/bash -e

DEVICE=$1
IMG_FOLDER=${PWD}

if [ "${DEVICE}" == "" ]; then
	DEVICE=/dev/ttyUSB1
fi

sudo ./hikey_idt -c config -p ${DEVICE}

# partition table
fastboot flash ptable ${IMG_FOLDER}/hisi-ptable.img

# bootloader
fastboot flash xloader ${IMG_FOLDER}/hisi-sec_xloader.img
fastboot flash fastboot ${IMG_FOLDER}/hisi-fastboot.img

# extra images
fastboot flash nvme   ${IMG_FOLDER}/hisi-nvme.img
fastboot flash fw_lpm3   ${IMG_FOLDER}/hisi-lpm3.img
fastboot flash trustfirmware   ${IMG_FOLDER}/hisi-bl31.bin
