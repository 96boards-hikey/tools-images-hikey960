This document describes how to build ATF, UEFI for HiKey960 from source.

Note: at the time of this writing, ATF and UEFI for HiKey960 are not officially released yet. Please use with caution.

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
