/*
 * Partition Table Tool
 *
 * Copyright (c) 2017 Hisilicon Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
#ifndef _HISI_PARTITION_TABLE_H_
#define _HISI_PARTITION_TABLE_H_

enum {
	EMMC_USER_PART = 0x0,
	EMMC_BOOT_MAJOR_PART = 0x1,
	EMMC_BOOT_BACKUP_PART = 0x2,
};

enum {
	UFS_PART_0 = 0x0,
	UFS_PART_1 = 0x1,
	UFS_PART_2 = 0x2,
	UFS_PART_3 = 0x3,
};

#define PART_NAMELEN                    (36)

#define MAX_UFS_LUN			(8)
#define UFS_OFFSET_128KB		(128*1024)
#define UFS_PTABLE_SIZE_17KB		(17*1024)

#define UNIT_USING_COUNT       		(4)


/*partition macro definition*/
#define PART_PTABLE                    "ptable"
#define PART_XLOADER_RESERVED1         "xloader_reserved1"
#define PART_XLOADER_RESERVED2         "xloader_reserved2"
#define PART_FW_LPM3                   "fw_lpm3"
#define PART_FRP                       "frp"
#define PART_FASTBOOT                  "fastboot"
#define PART_NVME                      "nvme"
#define PART_TRUSTFIRMWARE             "trustfirmware"
#define PART_BOOT                      "boot"
#define PART_DTS                       "dts"
#define PART_CACHE                     "cache"
#define PART_SYSTEM                    "system"
#define PART_USERDATA                  "userdata"
#define PART_VENDOR                    "vendor"
#define PART_PTABLE_LU3                "ptable_lu3"
#define PART_XLOADER_A                 "xloader_a"
#define PART_XLOADER_B                 "xloader_b"
#define PART_FIP                       "fip"
#endif
