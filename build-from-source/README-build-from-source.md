This document describes how to Linux kernel and Android for HiKey960 from source.

Note: source code of hisi-bl31.bin, hisi-fastboot.img, hikey_idt, hisi-lpm3.img, hisi-nvme.img, hisi-ptable.img, hisi-sec_fastboot.img, hisi-sec_uce_boot.img, hisi-sec_usb_xloader.img and hisi-sec_xloader.img are not available.

Category
========
* [Linux Kernel](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/build-from-source/README-build-from-source.md#linux-kernel)
* [Android](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/build-from-source/README-build-from-source.md#android)

Linux Kernel
============

Code Locations
--------------
Git repository: https://github.com/96boards-hikey/linux/
Branch: [working-android-hikey-linaro-4.4](https://github.com/96boards-hikey/linux/commits/working-android-hikey-linaro-4.4)

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
  * [`mkbootimg`](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/build-from-source/mkbootimg)
  * [`mkdtimg`](https://github.com/96boards-hikey/tools-images-hikey960/blob/master/build-from-source/mkdtimg)

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

Please read [this file](https://source.android.com/source/devices#960hikey) to learn how to build Android for HiKey960. HiKey960 is one of development boards [supported by Google AOSP since 26,Apr,2017](https://www.linaro.org/news/linaro-announces-latest-96boards-product-aosp-development/).


