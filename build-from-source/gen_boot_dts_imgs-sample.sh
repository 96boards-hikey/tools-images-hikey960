# AOSP root directory:
AOSP_ROOTDIR=/your/AOSP/rootdir/
# AOSP build output directory:
PRODUCT_OUT=${AOSP_ROOTDIR}/out/target/product/hikey960/
# RAMDISK_IMG:
RAMDISK_IMG=${PRODUCT_OUT}/ramdisk.img

# kernel source code is in this repository:
#    https://github.com/96boards-hikey/linux/
# Kernel source code directory:
KERNEL_DIR=/your/kernel/
# Tool directory: for mkbootimg and mkdtimg
GEN_IMAGES_DIR=/your/tooldir/

export ARCH=arm64
export CROSS_COMPILE=${AOSP_ROOTDIR}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

cd ${KERNEL_DIR}
make distclean
make hikey960_defconfig
make -j7 Image.gz
make hisilicon/hi3660-hikey960.dtb

cp arch/arm64/boot/Image.gz ${GEN_IMAGES_DIR}
cp arch/arm64/boot/dts/hisilicon/hi3660-hikey960.dtb ${GEN_IMAGES_DIR}

cd ${GEN_IMAGES_DIR}
# Note: here build result boot.img and dt.img are in folder ${PRODUCT_OUT}
#       To change this, change --output and -o.
./mkbootimg  --kernel Image.gz --ramdisk ${RAMDISK_IMG} --cmdline "loglevel=15 androidboot.hardware=hikey960 androidboot.selinux=permissive firmware_class.path=/system/etc/firmware" --base 0x0 --tags-addr 0x07A00000 --kernel_offset 0x00080000 --ramdisk_offset 0x07c00000 --os_version 7.0 --os_patch_level 2016-08-05  --output ${PRODUCT_OUT}/boot.img
./mkdtimg -d hi3660-hikey960.dtb -s 2048 -c -o ${PRODUCT_OUT}/dt.img

echo "dt.img boot.img build finished!"
