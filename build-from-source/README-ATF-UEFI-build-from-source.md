This document describes how to build ATF, UEFI for HiKey960 from source.

Note: at the time of this writing, ATF and UEFI for HiKey960 are not officially released yet. Please use with caution.

Bootloader
====================

1. Code Locations
-----------------

   * ARM Trusted Firmware:
   https://github.com/96boards-hikey/arm-trusted-firmware/tree/testing/hikey960_v1.1

   * edk2:
   https://github.com/96boards-hikey/edk2/tree/testing/hikey960_v2.5

   * OpenPlatformPkg:
   https://github.com/96boards-hikey/OpenPlatformPkg/tree/testing/hikey960_v1.3.4

   * l-loader:
   https://github.com/96boards-hikey/l-loader/tree/testing/hikey960_v1.2

   * uefi-tools:
   https://github.com/96boards-hikey/uefi-tools/tree/hikey960_v1


2. Build Procedure
------------------

   * Fetch all the above 5 repositories into local host.
     Make all the repositories in the same ${BUILD_PATH}.

   * Create the symbol link to OpenPlatformPkg in edk2.
     $cd ${BUILD_PATH}/edk2
     $ln -sf ../OpenPlatformPkg

   * Prepare AARCH64 toolchain.

   * If your hikey960 hardware is v1, update uefi-tools/platform.config first. (optional)
     Uncomment the below sentence. Otherwise, UEFI can't output messages on serial
     console on hikey960 v1.
     BUILDFLAGS=-DSERIAL_BASE=0xFDF05000
     If your hikey960 hardware is v2 or newer, nothing to do.

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
     $PTABLE=aosp-32g bash -x generate_ptable.sh


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

   * UEFI running in recovery mode. When prompt '.' is displayed on console, press
     hotkey 'f' in keyboard. Then Android fastboot app is running.
     The timeout of prompt '.' is 10 seconds.

   * Update images.
     $sudo fastboot flash ptable prm_ptable.img
     $sudo fastboot flash xloader sec_xloader.img
     $sudo fastboot flash fastboot l-loader.bin
     $sudo fastboot flash fip fip.bin
     $sudo fastboot flash boot boot.img
     $sudo fastboot flash cache cache.img
     $sudo fastboot flash system system.img
     $sudo fastboot flash userdata userdata.img

5. Boot UEFI in normal mode
-----------------------------

   * Hotkey in UEFI
     When prompt '.' is displayed on console, user could input hotkey from keyboard.
     If hotkey 'f' is pressed, run AndroidFastbootApp.
     If hotkey Enter is pressed, load abootimg (android kernel) from UFS by AndroidBootApp.
     If hotkey Esc is pressed, load boot manager menu. The menu can't display well
     on minicom. So recommend to use telnet instead.

   * Exit from AndroidFastbootApp.
     If hotkey Space or Enter is pressed, exit AndroidFastbootApp and load abootimg (android
     kernel) from UFS by AndroidBootApp.

   * Operation in boot manager menu. (optional)
     A window is displayed in console.
       - Select "Android Fastboot" entry to run fastboot protocol.
       - Select "Android Boot" entry to boot android kernel from UFS device. If it's flushed already.

6. Build Android kernel image
-----------------------------

   * Now gzipped kernel isn't supported yet. Only support Image + DTB format in abootimg.
     The script file cc in l-loader git repository describes how to build an abootimg on HiKey960 for UEFI.

7. Known Issues
-----------------------------

   * Kernel panic
     Since a few patches are not merged int bootloader yet, it'll result in kernel panic.

   * BL31 breaks by UEFI
     It's the memory layout issue. A few patches are not merged yet.

   * CPUFREQ & CPUIDLE isn't supported.
     A few patches are not merged yet.

   * SMP isn't supported.
     A few patches are not merged yet.

   * Can't get early console when boot kernel.
     A few patches are not merged yet.
     Workaround:
       Edit {EDK2}/EmbeddedPkg/Library/AbootimgLib.c.
       Find L" initrd=0x%x,0x%x" in AbootimgInstallFdt ().
       Replace it by L" initrd=0x%x,0x%x earlycon=pl011,0xfdf05000,115200 console=ttyAMA5" for hikey960 v1.
       Replace it by L" initrd=0x%x,0x%x earlycon=pl011,0xfff32000,115200 console=ttyAMA6" for hikey960 v2.
