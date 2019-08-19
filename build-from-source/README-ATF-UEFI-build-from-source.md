This document describes how to build ATF, UEFI for HiKey960 from source.

Note: at the time of this writing, ATF and UEFI for HiKey960 are not officially released yet. Please use with caution.

Bootloader
====================

1. Code Locations
-----------------

   * ARM Trusted Firmware:
   [link](https://github.com/ARM-software/arm-trusted-firmware/tree/integration)

   * edk2:
   [link](https://github.com/96boards-hikey/edk2/tree/testing/hikey960_v2.5)

   * OpenPlatformPkg:
   [link](https://github.com/96boards-hikey/OpenPlatformPkg/tree/testing/hikey960_v1.3.4)

   * l-loader:
   [link](https://github.com/96boards-hikey/l-loader/tree/testing/hikey960_v1.2)

   * atf-fastboot: (only for HiKey)
   [link](https://github.com/96boards-hikey/atf-fastboot/tree/master)

2. Build Procedure
------------------

   * Fetch all the above repositories into local host.
     Make all the repositories in the same ${BUILD_PATH}.
     ```
     git clone https://github.com/ARM-software/arm-trusted-firmware -b integration
     git clone https://github.com/96boards-hikey/edk2 -b testing/hikey960_v2.5
     git clone https://github.com/96boards-hikey/OpenPlatformPkg -b testing/hikey960_v1.3.4
     git clone https://github.com/96boards-hikey/l-loader -b testing/hikey960_v1.2
     git clone https://github.com/96boards-hikey/atf-fastboot
     ```

   * Create the symbol link to OpenPlatformPkg in edk2.
     <br>`$cd ${BUILD_PATH}/edk2`</br>
     <br>`$ln -sf ../OpenPlatformPkg`</br>

   * Prepare AARCH64 toolchain from Linaro. [link](http://releases.linaro.org/components/toolchain/binaries/)

   * If your hikey960 hardware is v1, update _uefi-tools/platform.config_ first. _(optional)_
     <br>__Uncomment the below sentence. Otherwise, UEFI can't output messages on serial
     console on hikey960 v1.__</br>
     <br>`BUILDFLAGS=-DSERIAL_BASE=0xFDF05000`</br>
     <br>If your hikey960 hardware is v2 or newer, nothing to do.</br>

   * Build it as debug/release mode with l-loader.sh script file. Setup your choice
     on debug or release mode in the script. Set your toolchain path in the script.
     <br>`export LOADER_DIR=${BUILD_PATH}/l-loader`</br>
     <br>`cd ${LOADER_DIR}`</br>
     <br>`# Build UEFI & ARM Trust Firmware`</br>
     <br>`bash ./l-loader.sh hikey960`</br>
     All images including partition table are generated in l-loader directory.


3. Setup Console
----------------

   * Install ser2net. Use telnet as the console since UEFI will output window that fails to display in minicom.
     <br>`$sudo apt-get install ser2net`</br>

   * Configure ser2net.
     <br>`$sudo vi /etc/ser2net.conf`</br>
     <br>Append one line for serial-over-USB in below.</br>
     <br>_#ser2net.conf_</br>
     <br>`2004:telnet:0:/dev/ttyUSB0:115200 8DATABITS NONE 1STOPBIT banner`</br>

   * Open the console.
     <br>`$telnet localhost 2004`</br>
     <br>And you could open the console remotely, too.</br>


4. Boot UEFI in recovery mode
-----------------------------

   * Fetch that are used in recovery mode. They're generated in l-loader directory according to the steps above.

   * Prepare config file.
     <br>_$vi config_</br>
     <br>_# The content of config file_</br>
     <br>`./hisi-sec_usb_xloader.img 0x00020000`</br>
     <br>`./hisi-sec_uce_boot.img 0x6A908000`</br>
     <br>`./recovery.bin 0x1AC00000`</br>

   * Remove the modemmanager package. This package may causes hikey_idt tool failure.
     <br>`$sudo apt-get purge modemmanager`</br>

   * Run the command to download l-loader.bin into HiKey960.
     <br>`$sudo ./hikey_idt -c config -p /dev/ttyUSB1`</br>

   * UEFI running in recovery mode.
     <br>When prompt '.' is displayed on console, press hotkey 'f' in keyboard. Then Android fastboot app is running.</br>
     <br>The timeout of prompt '.' is 10 seconds.</br>

   * Update images.
     <br>`$sudo fastboot flash ptable prm_ptable.img`</br>
     <br>`$sudo fastboot flash xloader hisi-sec_xloader.img`</br>
     <br>`$sudo fastboot flash fastboot l-loader.bin`</br>
     <br>`$sudo fastboot flash fip fip.bin`</br>
     <br>`$sudo fastboot flash boot boot.img`</br>
     <br>`$sudo fastboot flash cache cache.img`</br>
     <br>`$sudo fastboot flash system system.img`</br>
     <br>`$sudo fastboot flash userdata userdata.img`</br>

   * Notice: UEFI could also boot kernel in recovery mode, but BL31 isn't loaded in
     recovery mode.


5. Boot UEFI in normal mode
-----------------------------

   * Hotkey in UEFI
     <br>_When prompt '.' is displayed on console, user could input hotkey from keyboard._</br>
     <br>If hotkey 'f' is pressed, run AndroidFastbootApp.</br>
     <br>If hotkey Enter is pressed, load abootimg (android kernel) from UFS by AndroidBootApp.</br>
     <br>If hotkey Esc is pressed, load boot manager menu. The menu can't display well
     on minicom. So recommend to use telnet instead.</br>

   * Exit from AndroidFastbootApp.
     <br>If hotkey Space or Enter is pressed, exit AndroidFastbootApp and load abootimg (android
     kernel) from UFS by AndroidBootApp.</br>

   * Operation in boot manager menu. _(optional)_
     <br>A window is displayed in console.</br>
     <br>  - Select "Android Fastboot" entry to run fastboot protocol.</br>
     <br>  - Select "Android Boot" entry to boot android kernel from UFS device. If it's flushed already.</br>


6. Build Android kernel image
-----------------------------

   * Both raw kernel and gzipped kernel are supported now. But DTB must be appended on kernel image to generate abootimg.
     <br>The script file cc in l-loader git repository describes how to build an abootimg on HiKey960 for UEFI.</br>


7. Miscellaneous
-----------------------------

   * Generate new random serial number.
     <br>$fastboot oem serialno</br>

   * HiKey Support.
     <br>The build process of HiKey platform isn't listed in this document. Please refer the reference build script file __build_uefi.sh__ in l-loader git repository. This reference build file could build ATF/UEFI for both HiKey and HiKey960 platform.</br>
     <br>atf-fastboot git repository is only for HiKey platform. It derives from old ATF on HiKey. It only contains the feature of downloading images and flushing to eMMC.</br>

   * Partition table.
     <br>The image format of partition table is simplified. The old hisi-ptable.img is discarded. Now both HiKey and HiKey960 use primary partition table image (__prm_ptable.img__).</br>
     <br>The instructions of generating partition table is different between HiKey and HiKey960. Please refer to __build_uefi.sh__.</br>


8. Known Issues
-----------------------------

   * Fail to run sgdisk when generate partition table.
     <br>The patch is also provided in l-loader git repository.</br>
     <br>`sudo apt-get source gdisk`</br>
     <br>`cd gdisk-1.0.1`</br>
     <br>`patch -p1 < ${l-loader}/0001-sgdisk-force-blocksize-as-4096.patch`</br>
     <br>`make`</br>
     <br>`cp sgdisk ${l-loader}/sgdisk`</br>
