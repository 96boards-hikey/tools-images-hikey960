#!/usr/bin/env bash

# Copyright 2017 The Fuchsia Authors
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

BASE_URL=https://snapshots.linaro.org/96boards/reference-platform/components/uefi-staging
VERSION=20
PRODUCT=hikey960
RELEASE=release
DEVICE=/dev/ttyUSB1

if [ $# -eq 0 ]; then
	echo "Usage: hikey-efi-flash-image -h"
	exit 0
fi

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
-h)
echo "Look the file to get help"
echo "Use -d or -r to download debug or release version"
echo "Use -t to specify which terminal device to use to connect to the board"
echo "Use -v to specify build no."
echo "Example: "$0" -r -t /dev/ttyUSB1 -v 20"
echo "this downloads build #20 release version and flashes the device"
echo "represented by /dev/ttyUSB1"
exit 0
;;
-d|--debug)
RELEASE=debug
;;
-r|--release)
RELEASE=release
;;
-t|--tty_device)
DEVICE=$2
shift
;;
-v|--version)
VERSION=$2
shift
;;
*)
echo "Unknown option. Use -h for help"
exit 0
;;
esac
shift
done

UEFI_DIR=${1:-./uefi}

UEFI_URL=${BASE_URL}/${VERSION}/${PRODUCT}/${RELEASE}/
echo Creating $UEFI_DIR...
mkdir -p "$UEFI_DIR"
cd "$UEFI_DIR"

echo Fetching $UEFI_URL...
wget -A bin,config,efi,hikey_idt,img,txt -m -nd -np "$UEFI_URL"

echo Running hikey_idt...
chmod +x hikey_idt
./hikey_idt -c config -p ${DEVICE}

echo "Sleeping till device resets... zzz"
sleep 15

fastboot flash ptable prm_ptable.img
fastboot flash xloader hisi-sec_xloader.img
fastboot flash fastboot l-loader.bin
fastboot flash fip fip.bin

# Use fastboot to flash OS after this point.
