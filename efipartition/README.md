### HiKey960 Ptable Tool

This is an independent tool with source code. It can be integrated into Android  
Source code or use it independented on linux pc.


"Make" then use "efipartition ptable.img", you can get a ptable image.

```
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
```

In order to be more genera, the ptable can be used for both emmc and ufs. It contain a  
128Kb emmc partition at the beginning. 

**Note:**

Xloader-partition is a static partition table used by xloader, which cannot be modified.   
It's defined in xloader partition.h

Partition-table-ufs in partition.h is the main partition used by hikey. It contain the   
static xloader-partition. These are also cannot be modified.

In one word, the partition base size below cannot be changed.

Please also make sure that "device/linaro/hikey" the mount partition number is the same as partition_table_ufs.
```
  {PART_XLOADER_A,        0,         2*1024,          UFS_PART_0},
  {PART_XLOADER_B,        0,         2*1024,          UFS_PART_1},
  {PART_PTABLE,           0,         1024,            UFS_PART_2},/* ptable          512K */
  {PART_FRP,              1024,      7*1024,          UFS_PART_2},/* frp             512K   p1*/
  {PART_PTABLE_LU3,       0,         1024,            UFS_PART_3},/* ptable_lu3    1M    sdd  */
  {PART_XLOADER_RESERVED1,1024,       512,            UFS_PART_3},/* vrl             512K   sdd1*/
  {PART_XLOADER_RESERVED2,1536,      512,             UFS_PART_3},/* vrl backup      512K   sdd2*/
  {PART_FASTBOOT,         2*1024,      12*1024,       UFS_PART_3},/* fastboot      12M   sdd3 */
  {PART_NVME,             14*1024,   6*1024,          UFS_PART_3},/* nvme          6M    sdd4 */
  {PART_FW_LPM3,          20*1024,  1024,            UFS_PART_3},/* mcuimage      1M    sdd5 */
  {PART_BOOT,             21*1024,  64*1024,         UFS_PART_3},/* boot          64M   sdd6 */
  {PART_DTS,              85*1024,  16*1024,         UFS_PART_3},/* dtimage       16M   sdd7 */
  {PART_TRUSTFIRMWARE,    101*1024,  2*1024,          UFS_PART_3},/* trustfirmware 2M    sdd8 */
```
