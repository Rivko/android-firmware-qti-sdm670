#ifndef BOOT_FLASH_DEV_DAL_H
#define BOOT_FLASH_DEV_DAL_H

/*===========================================================================

                Boot Flash DAL Header File

GENERAL DESCRIPTION
  This module implements a device-independent view of the low-level
  flash system devices.  This interface hides the implementation details
  and device peculiarities of a diverse range of devices supported by
  this module.

Copyright 2008-2009, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------     ---     ----------------------------------------------------------
2015-10-05   ck      Corrected copyright
2009-01-19   ebb     Inherited from flash drivers.
2008-11-10   mh      created

===========================================================================*/

#include "flash.h"

/*------------------------------------------------------------
 *  Flash Device Definitions
 * ---------------------------------------------------------*/

/* Possible result codes returned by flash drive routines */
#define FS_NO_DEVICE                              ((fs_device_t) 0)
#define FS_DEVICE_OK                              0
#define FS_DEVICE_DONE                            0
#define FS_DEVICE_FAIL                            (-1)
#define FS_DEVICE_BAD_BLOCK                       (-2)
#define FS_DEVICE_BUSY                            (-3)
#define FS_DEVICE_PARTITION_NOT_FOUND             (-4)
#define FS_DEVICE_NO_PARTITION_TABLE              (-5)
#define FS_DEVICE_INVALID_DEVICE                  (-6)
#define FS_DEVICE_INVALID_PARTITION_NAME          (-7)
#define FS_DEVICE_INVALID_FLASH                   (-8)
#define FS_DEVICE_PARTITION_TABLE_FULL            (-9)
#define FS_DEVICE_PARTITION_INVALID_NAME          (-10)
#define FS_DEVICE_PARTITION_TABLE_EXISTS          (-11)
#define FS_DEVICE_DUPLICATE_PARTITION             (-12)
#define FS_DEVICE_INVALID_PARTITION_TABLE         (-13)
#define FS_DEVICE_PARTITION_TABLE_VERSION_SKEW    (-14)
#define FS_DEVICE_NOT_SUPPORTED                   (-15)
#define FS_DEVICE_CP_READ_FAIL                    (-16)
#define FS_DEVICE_CP_WRITE_FAIL                   (-17)
#define FS_DEVICE_FAIL_PAGE_ERASED                (-18)

/* Types used by flash driver routines */
typedef uint32 page_id;
typedef uint32 block_id;

/* Device structure */
struct fs_device_data;

/* Device descriptor which is a pointer to the device structure */
typedef struct fs_device_data *fs_device_t;

/* Public interface to the flash device */
struct fs_device_data {

 struct flash_info      info;
 flash_handle_t         handle_flash;
 uint8 * page_buf;

 char * (*device_name) (fs_device_t self);
 int (*device_maker_id) (fs_device_t self);
 int (*device_id) (fs_device_t self);
 uint32 (*block_count) (fs_device_t self);
 uint32 (*block_size) (fs_device_t self);
 uint32 (*page_size) (fs_device_t self);
 uint32 (*total_page_size) (fs_device_t self);
 uint32 (*device_type) (fs_device_t self);
 uint32 (*device_width) (fs_device_t self);
 int (*bad_block_check) (fs_device_t self, block_id block);
 int (*mark_block_bad) (fs_device_t self, block_id block);
 int (*write_page) (fs_device_t self, page_id page, void *data);
 int (*erase_block) (fs_device_t self, block_id block);
 int (*read_page) (fs_device_t self, page_id page, void *data);
 int (*is_erased) (fs_device_t self, page_id page);
 int (*partial_write) (fs_device_t self, page_id page, void *data,
                        int offset, int length);
 int (*begin_erase_block) (fs_device_t self, block_id block);
 int (*suspend_erase) (fs_device_t self);
 int (*resume_erase) (fs_device_t self);
 int (*erase_status) (fs_device_t self);
 int (*is_block_erased) (fs_device_t self, block_id block);
 int (*read_spare_bytes) (fs_device_t self, page_id page, uint32 spare_offset,
                           uint32 len, void *data);
 int (*read_mpages) (fs_device_t self, page_id start_page, page_id num_pages,
                      void *data, void (*wdog_kick_fn_ptr)(void));
 int (*copy_page) (fs_device_t self, page_id src_page, page_id dest_page);
  int (*lock_ops) (fs_device_t self, page_id start_page, page_id end_page,
                   uint32 opcode);
  int (*get_spare_corrected_bytes) (fs_device_t self);
  int (*write_page_and_udata) (fs_device_t self, page_id page, void *page_data,
                               void *spare_udata, uint32 spare_udata_blen);
  int (*read_spare_udata) (fs_device_t self, page_id page, void *spare_udata,
                           uint32 spare_udata_blen);
  int (*read_page_and_udata) (fs_device_t self, page_id page, void *data,
                              void *spare_udata, uint32 spare_udata_blen);
  int (*read_otp_page_and_spare) (fs_device_t self, page_id page,  void *data,
                                  void *spare_udata, uint32 spare_udata_len,
                                  void *dev_otp_params);
  int (*open_partition) (fs_device_t self, char *partition_name);
  int (*get_partition_phy_limits) (fs_device_t self, uint32 *start,
                                   uint32 *limit, void *rsrvd);
  int (*get_partition_attribs) (fs_device_t self, uint32 *attribs);
  int (*set_ecc_state) (fs_device_t self, int state);

}; /* End of public interface to the flash device */

/* Description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
typedef enum {

  /* NOR devices where arbitrary bits can be set to zero.  SLC NOR devices
   * generally fall in this category. */
  FS_DEVICE_WRITES_SIMPLE,

  /* NAND devices requiring entire pages to be written, and can only be
   * written to once.  These devices do not allow 'partial_write'. */
  FS_DEVICE_WRITES_PAGED,

  /* NOR devices where adjacent pairs of bits are written together.  A pair
   * can be written from '11' to a value, and from a value to '00'.  If
   * recovery from failed writes is needed, the only two values that should
   * be used are '11' and '00'.  After a failed write, it may not be
   * possible to write the pair to any value other than '00'. */
  FS_DEVICE_WRITES_PAIRED_BITS,

  /* Indicates a NOR device with an unsupported write style.  No
   * assumptions can be made about how the data is written.  Pages must be
   * written in their entirety, and pages that have bad powerdowns cannot
   * be used. */
  FS_DEVICE_WRITES_UNSUPPORTED = 0x1000,
} fs_device_write_style_t;

/*===========================================================================
  Probe for the flash and open the requested partition if a partition table
  exists.
===========================================================================*/
fs_device_t flash_open(char *partition_name, int whichflash);

/*===========================================================================
DESCRIPTION
  Close the flash device. This API will free the flash allocated
  device structure.
===========================================================================*/
int flash_close(fs_device_t flash_device);

#endif /* __BOOT_FLASH_DEV_DAL_H__ */
