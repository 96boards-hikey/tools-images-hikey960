This document describes how to build ATF, UEFI, Linux kernel and Android for HiKey960 from source.

Category
========
* [Bootloader](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README-build-from-source.md#bootloader)
* [Linux Kernel](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README-build-from-source.md#linux-kernel)
* [Android](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/README-build-from-source.md#android)

Bootloader
====================

1. Code Locations
-----------------

   * ARM Trusted Firmware:
   https://github.com/96boards-hikey/arm-trusted-firmware/tree/testing/hikey960_v1

   * edk2:
   https://github.com/96boards-hikey/edk2/tree/testing/hikey960_v2.3

   * OpenPlatformPkg:
   https://github.com/96boards-hikey/OpenPlatformPkg/tree/testing/hikey960_v1.3.1

   * l-loader:
   https://github.com/96boards-hikey/l-loader/tree/testing/hikey960_v1

   * uefi-tools:
   https://github.com/96boards-hikey/uefi-tools/tree/hikey960


2. Build Procedure
------------------

   * Fetch all the above 5 repositories into local host.
     Make all the repositories in the same ${BUILD_PATH}.

   * Create the symbol link to OpenPlatformPkg in edk2.
     $cd ${BUILD_PATH}/edk2
     $ln -sf ../OpenPlatformPkg

   * Prepare AARCH64 toolchain.

   * Build it as debug mode. Create script file for build.

     BUILD_OPTION=DEBUG
     export AARCH64_TOOLCHAIN=GCC48
     export UEFI_TOOLS_DIR=${BUILD_PATH}/uefi-tools
     export EDK2_DIR=${BUILD_PATH}/edk2
     EDK2_OUTPUT_DIR=${EDK2_DIR}/Build/HiKey960/${BUILD_OPTION}_${AARCH64_TOOLCHAIN}

     cd ${EDK2_DIR}
     # Build UEFI & ARM Trust Firmware
     ${UEFI_TOOLS_DIR}/uefi-build.sh -b ${BUILD_OPTION} -a ../arm-trusted-firmware hikey960

     # Generate l-loader.bin
     cd ${BUILD_PATH}/l-loader
     ln -sf ${EDK2_OUTPUT_DIR}/FV/bl1.bin
     ln -sf ${EDK2_OUTPUT_DIR}/FV/fip.bin
     ln -sf ${EDK2_OUTPUT_DIR}/FV/BL33_AP_UEFI.fd
     python gen_loader.py -o l-loader.bin --img_bl1=bl1.bin --img_ns_bl1u=BL33_AP_UEFI.fd

   * Generate partition table
     Make sure that you're using the sgdisk in the l-loader directory.
     $PTABLE=aosp-8g bash -x generate_ptable.sh


3. Setup Console
----------------

   * Install ser2net. Use telnet as the console since UEFI will output window
     that fails to display in minicom.
     $sudo apt-get install ser2net

   * Configure ser2net.
     $sudo vi /etc/ser2net.conf
     Append one line for serial-over-USB in below.
     # ser2net.conf
     2004:telnet:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT banner

   * Open the console.
     $telnet localhost 2004
     And you could open the console remotely, too.


4. Boot UEFI in recovery mode
-----------------------------

   * Fetch binaries that are used in recovery mode. The code location is in below.
       https://github.com/96boards-hikey/tools-images-hikey960

   * Get l-loader.bin.
     $cd tools-images-hikey960
     $ln -sf ${BUILD_PATH}/l-loader/l-loader.bin

   * Prepare config file.
     $vi config
     # The content of config file
     ./sec_user_xloader.img 0x00020000
     ./sec_uce_boot.img 0x6A908000
     ./l-loader.bin 0x1AC00000

   * Run the command to download l-loader.bin into HiKey960.
     $sudo ./hikey_idt -c config -p /dev/ttyUSB1

   * Operation in console. (optional)
     Now autoboot feature isn't supported yet. UEFI will boot to shell directly.
     So we need to make a choice in UEFI shell.
     Shell>exit

     A window is displayed in console.
       - Select "Android Fastboot" entry to run fastboot protocol.
       - Select "Android Boot" entry to boot android kernel from UFS device. If it's flushed already.



Linux Kernel
============

Code Locations
--------------
Git repository: https://github.com/96boards-hikey/linux/
Branch: [android-hikey-linaro-4.4](https://github.com/96boards-hikey/linux/commits/android-hikey-linaro-4.4)

Note: 
a) We also maintain two other kernel branches for HiKey960: [v4.9](https://github.com/96boards-hikey/linux/commits/hikey960-v4.9) and [upstream rebase](https://github.com/96boards-hikey/linux/commits/hikey960-upstream-rebase). Their build methods are similar to v4.4, but they are not listed here. Send email to [Guodong Xu](mailto:guodong.xu@linaro.org) in case you want to know more.

Build Procedure
---------------
Step 0: preparations.
* Toolchain: AOSP gcc 4.9 for `aarch64-linux-android-` is recommended. Fetch from [here](https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/+/master).
* To generate boot.img, you need a `ramdisk.img`. There are two ways to get a ramdisk.img. 
  * Either from your AOSP build output folder: ${AOSP_ROOTDIR}/out/target/product/hikey960
  * Or, fetch it from a released build. (TODO: public release doesn't exist yet)
* To generate boot.img and dt.img, you need download these tools:
  * [`mkbootimg`](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/mkbootimg)
  * [`mkdtimg`](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/mkdtimg)

Step 1: build Image.gz and hi3660-hikey960.dtb
```sh
export ARCH=arm64
export CROSS_COMPILE=${AOSP_ROOTDIR}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

REBUILD_FROM_SOURCE=1
if [ $REBUILD_FROM_SOURCE -eq 1 ]; then
        # kernel make
        make distclean
        make hikey960_defconfig
        make -j7 Image.gz
        # build DTB
        make hisilicon/hi3660-hikey960.dtb
fi
```

Find build results here under your kernel source code tree:
```sh
arch/arm64/boot/Image.gz
arch/arm64/boot/dts/hisilicon/hi3660-hikey960.dtb
```

Step 2: generate boot.img and dt.img
Note: Suppose you save `mkbootimg` and `mkdtimg` into this dir: ${GEN_IMAGES_DIR}, and your kernel source code is in ${KERNEL_DIR}

```sh
AOSP_ROOTDIR=/home/your-aosp-rootdir/
KERNEL_DIR=/home/your-kernel-code/
PRODUCT_OUT=${AOSP_ROOTDIR}/out/target/product/hikey960/
GEN_IMAGES_DIR=/home/where-mkbootimg-mkdtimg/

# ramdisk.img choices
RAMDISK_IMG=${PRODUCT_OUT}/ramdisk.img

# copy built result into ${GEN_IMAGES_DIR}
cd ${KERNEL_DIR}
cp -v arch/arm64/boot/Image.gz ${GEN_IMAGES_DIR}
cp -v arch/arm64/boot/dts/hisilicon/hi3660-hikey960.dtb \
		${GEN_IMAGES_DIR}

# make boot.img and dt.img
cd ${GEN_IMAGES_DIR}
./mkbootimg  --kernel Image.gz --ramdisk ${RAMDISK_IMG} --cmdline "loglevel=15 androidboot.hardware=hikey960 androidboot.selinux=permissive firmware_class.path=/system/etc/firmware init=/init" --base 0x0 --tags-addr 0x07A00000 --kernel_offset 0x00080000 --ramdisk_offset 0x07c00000 --os_version 7.0 --os_patch_level 2016-08-05  --output ${PRODUCT_OUT}/boot.img
./mkdtimg -d hi3660-hikey960.dtb -s 2048 -c -o ${PRODUCT_OUT}/dt.img

# list them and double check
ls -l ${PRODUCT_OUT}/dt.img ${PRODUCT_OUT}/boot.img
echo "dt.img boot.img succeeds!"
```


Android
=======

Please read [this file](https://github.com/96boards-hikey/android_manifest/blob/aosp-master/README.md) to learn how to build Android for HiKey960.


