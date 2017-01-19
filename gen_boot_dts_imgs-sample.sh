# building dt.img or not, if unsure, say 1
DTB=1
# using Virtual framebuffer or not. If unsure, say 0
WITH_VFB=0

# AOSP root directory:
AOSP_ROOTDIR=/home/docularxu/AOSP
# kernel source code is in this repository:
#    https://github.com/96boards-hikey/linux/
# Kernel source code directory:
KERNEL_DIR=/home/docularxu/Huawei/hikey960/kernel
# AOSP build output directory:
PRODUCT_OUT=${AOSP_ROOTDIR}/out/target/product/hikey960/
# Tool directory: for mkbootimg and dtbTool
GEN_IMAGES_DIR=/home/docularxu/Huawei/hikey960/gen_images

CURRENT_DIR=${PWD}
if [ ! -e ${PRODUCT_OUT} ]
then
	mkdir -p ${PRODUCT_OUT}
fi

export ARCH=arm64
export CROSS_COMPILE=${AOSP_ROOTDIR}/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

cd ${KERNEL_DIR}
make distclean
ARCH=arm64 scripts/kconfig/merge_config.sh -m arch/arm64/configs/defconfig android/configs/android-base.cfg android/configs/android-recommended.cfg
# make defconfig
# make android_defconfig
make -j7 Image.gz

cp arch/arm64/boot/Image.gz ${GEN_IMAGES_DIR}
if [ $DTB -eq 1 ]; then
	make hisilicon/hi3660-hikey960.dtb
	cp arch/arm64/boot/dts/hisilicon/hi3660-hikey960.dtb ${GEN_IMAGES_DIR}
fi

cd ${GEN_IMAGES_DIR}
if [ $WITH_VFB -eq 1 ]; then
	./mkbootimg  --kernel Image.gz --ramdisk ${PRODUCT_OUT}/ramdisk.img --cmdline "loglevel=15 androidboot.hardware=hikey960 androidboot.selinux=permissive firmware_class.path=/system/etc/firmware video=vfb" --base 0x0 --tags-addr 0x07A00000 --kernel_offset 0x00080000 --ramdisk_offset 0x07c00000 --os_version 7.0 --os_patch_level 2016-08-05  --output boot.img
else
	./mkbootimg  --kernel Image.gz --ramdisk ${PRODUCT_OUT}/ramdisk.img --cmdline "loglevel=15 androidboot.hardware=hikey960 androidboot.selinux=permissive firmware_class.path=/system/etc/firmware" --base 0x0 --tags-addr 0x07A00000 --kernel_offset 0x00080000 --ramdisk_offset 0x07c00000 --os_version 7.0 --os_patch_level 2016-08-05  --output boot.img
fi
if [ $DTB -eq 1 ]; then
	./dtbTool -o dt.img -s 2048 -p ${KERNEL_DIR}/scripts/dtc/ ./
fi
cp -v dt.img ${PRODUCT_OUT}
cp -v boot.img ${PRODUCT_OUT}
ls -l dt.img boot.img
echo "dt.img boot.img is success!"

cd ${CURRENT_DIR}
