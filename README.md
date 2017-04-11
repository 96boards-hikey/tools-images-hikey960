# Base Firmware files and installation

This sections shows how to install all base firmware components for the HiKey960. Once finished with these instructions, please continue to the [HiKey960 documentation landing page](https://github.com/sdrobertw/HiKey960/blob/master/README.md) to flash an operating system

***

- **Step 1**: Material and Preperations
- **Step 2**: Dependencies
- **Step 3**: Enter recovery/forced-download mode on HiKey960
- **Step 4**: Flash Base Firmware
- **Step 5**: Explore other modes, proceed to OS installation

## Step 1: Materials and Preperations

**Materials**

- [HiKey960](http://www.96boards.org/product/hikey960/)
- USB Type-A (Host machine) to USB Type-C (96Boards) cable
- [96Boards compliant power supply](http://www.96boards.org/product/power/)

**Preperations**

There are two versions of HiKey960: v1 and v2. One important difference you should know is: v1 is using Jumper (J2001), while v2 is using DIP switch (SW2201). Both are located in the bottom side of board.

There are two versions of HiKey960  |
----------------------------------- |
v1 uses Jumpers (J2001)             |
v2 uses DIP Switches (SW2201)       |

Both are located at the bottom side of the board  |
------------------------------------------------- |
Jumper Pin 1-2 = DIP switch 1                     |
Jumper Pin 3-4 = DIP switch 2                     |
Jumper Pin 5-6 = DIP switch 3                     |

- To boot into fastboot mode everytime set switch 1 & 3 to ON state
  and switch 2 to OFF state.
- To boot into fastboot mode at every alternate rebooot set switch 1
  to ON and switch 2 & 3 to OFF state.
- To boot into recovery mode set switch 1 & 2 to ON state and switch 3
  to OFF state

## Step 2: Dependencies

**Host Linux Machine**

- Android SDK “Platform-Tools” for Linux can be downloaded <a href="https://developer.android.com/studio/releases/platform-tools.html" target="_blank">here</a>

## Step 3: Enter recovery/forced-download mode on HiKey960:

* Remove power from the board
* Change Jumper/DIP switch settings, to enter recovery/forced-download mode:

Name          | Link / Switch       | State
------------- | ------------------- | ----------
Auto Power up | Link 1-2 / Switch 1 | closed / ON
Recovery      | Link 3-4 / Switch 2 | closed / ON
Fastboot      | Link 5-6 / Switch 3 | open / OFF

* Apply power to the board using [96Boards compliant power supply](http://www.96boards.org/product/power/)
* Insert USB Type-C cable (OTG port) to the board, and connect the other end to your Linux PC
* Check whether there is a device node "/dev/ttyUSBx". If there is, it means your PC has detected the target board; If there is not, try to repeat previous steps.

## Step 4: Flash base firmware

Once again using the terminal on your host machine, execute the following command. Be sure to replace /dev/ttyUSBx with the USB value detected by your machine.

```sh
./recovery-flash.sh /dev/ttyUSBx
```

After it completes, the base firmware will be flashed to the device, this does not mean OS.

The board will then be in fastboot mode.

## Step 5: Explore other modes, proceed to OS installation

- Remove power from the board
- Change Jumper settings, to use normal boot or fastboot mode

**Normal boot mode**

Name          | Link / Switch       | State
------------- | ------------------- | ----------
Auto Power up | Link 1-2 / Switch 1 | closed / ON
Recovery      | Link 3-4 / Switch 2 | oepn / OFF
Fastboot      | Link 5-6 / Switch 3 | open / OFF

**Fastboot mode**

Name          | Link / Switch       | State
------------- | ------------------- | ----------
Auto Power up | Link 1-2 / Switch 1 | closed / ON
Recovery      | Link 3-4 / Switch 2 | open / OFF
Fastboot      | Link 5-6 / Switch 3 | closed / ON

- Apply power to the board using 96Boards compliant power supply
- Proceed to OS installation through the [HiKey960 documentation landing page](https://github.com/sdrobertw/HiKey960/blob/master/README.md)

## Troubleshooting

* If you run into trouble, see the [README-technical.md](README-technical.md) file in this directory.
