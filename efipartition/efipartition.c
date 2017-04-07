/*
 * Partition Table Tool
 *
 * Copyright (c) 2017 Hisilicon Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>/*lint !e451*/
#include "zlib.h"
#include "partition.h"

//#define local static
#define ZEXPORT	/* empty */
unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long	u64;

#define EFI_PARTITION_GEN_VERSION "1.0"
#define NUM_KB	    1024
#define EFI_VERSION 0x00010000
#define EFI_ENTRIES 128		/*MAX 128*/
#define EFI_NAMELEN 36

static const u8 partition_type[16] = {
	0xa2, 0xa0, 0xd0, 0xeb, 0xe5, 0xb9, 0x33, 0x44,
	0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7,
};

static const u8 random_uuid[16] = {
	0xff, 0x1f, 0xf2, 0xf9, 0xd4, 0xa8, 0x0e, 0x5f,
	0x97, 0x46, 0x59, 0x48, 0x69, 0xae, 0xc3, 0x4e,
};

struct efi_entry {
	u8 type_uuid[16];
	u8 uniq_uuid[16];
	u64 first_lba;
	u64 last_lba;
	u64 attr;
	u16 name[EFI_NAMELEN];
};

struct efi_header {
	u8 magic[8];

	u32 version;
	u32 header_sz;

	u32 crc32;
	u32 reserved;
	u64 header_lba;
	u64 backup_lba;
	u64 first_lba;
	u64 last_lba;

	u8 volume_uuid[16];

	u64 entries_lba;

	u32 entries_count;
	u32 entries_size;
	u32 entries_crc32;
} __attribute__((packed));

/* !!! warning !!! when ptable struct changed,please change  vendor\hisi\build\scripts\gen_img_vrl_tbl.py  accordingly */
struct ptable {
	u8 mbr[512];
	union {
		struct efi_header header;
		u8 block[512];
	};
	struct efi_entry entry[EFI_ENTRIES];
};


static struct ptable the_ptable;

/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
const uLong crc_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
uLong ZEXPORT crc32(crc, buf, len)
    uLong crc;
    const Bytef *buf;
    uInt len;
{
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = crc ^ 0xffffffffL;
    while (len >= 8) {
		DO8(buf);
		len -= 8;
    }
    if (len) do {
		DO1(buf);
    } while (--len);

    return crc ^ 0xffffffffL;
}

static void init_mbr(u8 *mbr)
{
	mbr[0x1be] = 0x00; // nonbootable
	mbr[0x1bf] = 0xFF; // bogus CHS
	mbr[0x1c0] = 0xFF;
	mbr[0x1c1] = 0xFF;

	mbr[0x1c2] = 0xEE; // GPT partition
	mbr[0x1c3] = 0xFF; // bogus CHS
	mbr[0x1c4] = 0xFF;
	mbr[0x1c5] = 0xFF;

	mbr[0x1c6] = 0x01; // start
	mbr[0x1c7] = 0x00;
	mbr[0x1c8] = 0x00;
	mbr[0x1c9] = 0x00;

//	memcpy(mbr + 0x1ca, &blocks, sizeof(u32));

	mbr[0x1fe] = 0x55;
	mbr[0x1ff] = 0xaa;
}

static void start_ptbl(struct ptable *ptbl)
{
	struct efi_header *hdr = &ptbl->header;

	memset(ptbl, 0, sizeof(*ptbl));

	init_mbr(ptbl->mbr);

	memcpy(hdr->magic, "EFI PART", 8);
	hdr->version = EFI_VERSION;
	hdr->header_sz = sizeof(struct efi_header);
	hdr->header_lba = 1;
//	hdr->backup_lba = blocks - 1;
	hdr->first_lba = 34;
//	hdr->last_lba = blocks - 1;
	memcpy(hdr->volume_uuid, random_uuid, 16);
	hdr->entries_lba = 2;
	hdr->entries_count = EFI_ENTRIES;
	hdr->entries_size = sizeof(struct efi_entry);
}

static void end_ptbl(struct ptable *ptbl)
{
	struct efi_header *hdr = &ptbl->header;
	u32 n;

	n = crc32(0, 0, 0);
	n = crc32(n, (void*) ptbl->entry, sizeof(ptbl->entry));
	hdr->entries_crc32 = n;

	(void)crc32(0, 0, 0);
	n = crc32(0, (void*) &ptbl->header, sizeof(ptbl->header));
	hdr->crc32 = n;
}

static int add_ptn(struct ptable *ptbl, u64 first, u64 last, const char *name)
{
	struct efi_entry *entry = ptbl->entry;
	unsigned n;

	if (first < 34) {
		printf("partition '%s' overlaps partition table\n", name);
		return -1;
	}

	for (n = 0; n < EFI_ENTRIES; n++, entry++) {
		if (entry->last_lba)
			continue;
		memcpy(entry->type_uuid, partition_type, 16);
		memcpy(entry->uniq_uuid, random_uuid, 16);
		entry->uniq_uuid[0] = (u8)n;
		entry->first_lba = first;
		entry->last_lba = last;

		for (n = 0; (n < EFI_NAMELEN) && *name; n++)
			entry->name[n] = (u16)(u8)(*name++);
		return 0;
	}

	printf("out of partition table entries\n");
	return -1;
}


static int need_format_ptable(unsigned int lu_num)
{
	unsigned int i;
	unsigned int current_lu_num = 0;

	for (i = 0; i < (sizeof(partition_table_ufs)/sizeof(partition_table_ufs[0])) - 1; i++) {
		if (partition_table_ufs[i].flags != lu_num) {
			continue;
		} else {
			current_lu_num++;
		}
	}

	/*If current lu only contains 0 or 1 partition, don't need format ptable*/
	if (current_lu_num <= 1)
		return -1;
	else
		return 0;
}


//global FILE handle.
FILE 	*file1 = NULL;
#define STORAGE_EMMC	1
#define STORAGE_UFS	2

static int do_format(const struct partition * current_partition_pable, unsigned int storage_type, unsigned int lun_num)
{
	struct ptable *ptbl = &the_ptable;
	u64 next, sz;
	int n;

	start_ptbl(ptbl);

	for (n = 0, next = 0; ; n++) {
		if (STORAGE_EMMC == storage_type) {
			sz = (current_partition_pable + n)->length * (1024 / 512);
		} else if (STORAGE_UFS == storage_type) {
			sz = (current_partition_pable + n)->length / (4096 / 1024);
		} else {
			printf("invalid storage type\n");
			return -1;
		}

		if (sz == 0) {
			break;
		}

		if (lun_num != (current_partition_pable + n)->flags) {
			continue;
		} else if ((0 == (current_partition_pable + n)->start) && (lun_num == (current_partition_pable + n)->flags)) {
			next += sz;
			continue;
		}

		if (add_ptn(ptbl, next, next + sz - 1, (current_partition_pable + n)->name))
			return -1;

		next += sz;
	}

	end_ptbl(ptbl);

	fwrite((void*)ptbl, sizeof(struct ptable), 1, file1);

	return 0;
}

static int print_and_verify_ptable_address(struct partition * current_partition_pable)
{
	unsigned int loop;

	for (loop = 0; (current_partition_pable + loop)->length; loop++) {
		if (0 == (current_partition_pable + loop)->start) {
			printf("partition[%u], name:%s, size is %llu KB\n",
				loop, (current_partition_pable + loop)->name, (current_partition_pable + loop)->length);
		} else if ((current_partition_pable + loop)->start ==
			(current_partition_pable + loop - 1)->start + (current_partition_pable + loop - 1)->length) {
			printf("partition[%u], name:%s, size is %llu KB\n",
				loop, (current_partition_pable + loop)->name, (current_partition_pable + loop)->length);
		} else {
			printf("partition %u start address calculate error\n", loop);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned int loop;
	int ret;

	if (2 != argc) {
		printf("[efipartition]:input argc error:\n");
		return -1;
	}

	printf("[efipartition]: version = %s\n", EFI_PARTITION_GEN_VERSION);

	printf("[efipartition]: ufs ptable info is :\n");
	if (print_and_verify_ptable_address((struct partition *)partition_table_ufs))
		return -1;

/*format emmc patble*/
	file1 = fopen(argv[1], "w+");
	if (file1 == NULL) {
		printf("[efipartition]: fopen file %s failed!\n", argv[1]);
		return -1;
	}


/*------------------------|
 |          emmc          | 128K bytes
 *------------------------|
 |        ufs lu0         | 17K bytes
 *----------------------- |
 |        ufs lu1         | 17K bytes
 *----------------------- |
 |        .......         |
 *----------------------- |
 |        ufs lu6         | 17K bytes
 *----------------------- |
 |        ufs lu7         | 17K bytes
 *----------------------- |
*/

/*format ufs patble*/
	for (loop = 0; loop < MAX_UFS_LUN; loop++) {
		ret = fseek(file1, (long)(UFS_OFFSET_128KB + loop * UFS_PTABLE_SIZE_17KB), 0);
		if (ret) {
			printf("[efipartition]: fseek failed!\n");
			return -1;
		}
		ret = need_format_ptable(loop);
		if (0 == ret) {
			do_format((struct partition *)partition_table_ufs, STORAGE_UFS, loop);
		}
	}
	fclose(file1);

	file1 = NULL;

	printf("efi partition bin file \"%s\" gen succeed!\n", argv[1]);
	return 0;
}

