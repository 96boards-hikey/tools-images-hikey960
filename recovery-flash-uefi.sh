#/bin/bash
DEVICE=$1
IMG_FOLDER=${PWD}

# UEFI dir. Downloaded from
#   https://builds.96boards.org/snapshots/reference-platform/components/uefi-staging/
#   Eg: 26/hikey960/release/
# Then, modify UEFI_BUILD_PATH accordingly.
UEFI_BUILD_PATH=/home/docularxu/Huawei/hikey960/gen_images/snapshot.aosp/uefi.14/release

if [ "${DEVICE}" == "" ]; then
	DEVICE=/dev/ttyUSB1
fi

cd ${UEFI_BUILD_PATH}
sudo ${UEFI_BUILD_PATH}/hikey_idt -c ${UEFI_BUILD_PATH}/config -p ${DEVICE}

# partition table
fastboot flash ptable ${UEFI_BUILD_PATH}/prm_ptable.img

# bootloader
fastboot flash xloader ${UEFI_BUILD_PATH}/sec_xloader.img
fastboot flash fastboot ${UEFI_BUILD_PATH}/l-loader.bin
fastboot flash fip ${UEFI_BUILD_PATH}/fip.bin

# extra images
fastboot flash nvme   ${IMG_FOLDER}/nvme.img
fastboot flash fw_lpm3   ${IMG_FOLDER}/lpm3.img
fastboot flash trustfirmware   ${IMG_FOLDER}/bl31.bin
