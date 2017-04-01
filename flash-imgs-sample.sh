IMG_FOLDER=${PWD}

sudo ./hikey_idt -c config -p /dev/ttyUSB1

# partition table
fastboot flash ptable ${IMG_FOLDER}/ptable.img

# bootloader
fastboot flash xloader ${IMG_FOLDER}/sec_xloader.img
fastboot flash fastboot ${IMG_FOLDER}/fastboot.img

# extra images
fastboot flash nvme   ${IMG_FOLDER}/nvme.img
fastboot flash vector ${IMG_FOLDER}/vector.img
fastboot flash fw_lpm3   ${IMG_FOLDER}/lpm3.img
fastboot flash trustfirmware   ${IMG_FOLDER}/bl31.bin
