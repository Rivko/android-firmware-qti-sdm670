/***********************************************************************
 * Coldplug Device.
 *
 * Coldplug
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Coldplug Device functions.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/inc/coldplug_device.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-05-18   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_DEVICE_H__
#define __COLDPLUG_DEVICE_H__

#include "coldplug_config_i.h"
#include "coldplug_comdef.h"
#include "coldplug_api.h"
#include "coldplug_bdev_ops.h"

struct coldplug_device;

enum coldplug_partition_type {
	COLDPLUG_PARTITION_TYPE_INVALID = 0,
	COLDPLUG_PARTITION_TYPE_RAW = 1,
	COLDPLUG_PARTITION_TYPE_GPT = 2,
	COLDPLUG_PARTITION_TYPE_MBR = 3,
	COLDPLUG_PARTITION_TYPE_FAT = 4,
};

struct coldplug_raw_partition_in_ram {
};

/* GPT partition fields in RAM */
struct coldplug_gpt_partition_in_ram {
	struct coldplug_guid type_guid;
#ifdef COLDPLUG_GPT_ALL_PARTITION_FIELDS_IN_RAM
	struct coldplug_guid unique_guid;
	uint64 start_lba;
	uint64 end_lba;
	uint64 attributes;
	uint8 partition_name[72];
#endif
};

struct coldplug_mbr_partition_in_ram {
	uint32 mbr_id;
};

struct coldplug_partition_in_ram {
	enum coldplug_partition_type partition_type;
	union {
		struct coldplug_mbr_partition_in_ram mbr;
		struct coldplug_gpt_partition_in_ram gpt;
	} u;
};

struct coldplug_partition {
	int is_in_use;
	uint32 magic;
	struct coldplug_lun *clun;

	struct coldplug_partition_in_ram parti_info;

	uint64 start_lba;
	uint64 total_lba;
	int is_bootable;
	char name[COLDPLUG_MAX_PARTITION_NAME_LEN];
};

struct coldplug_lun {
	int is_in_use;
	uint32 magic;
	struct coldplug_slot *cslot;
	uint32 lun_no;
	int is_boot_lun;

	void *bdev_hdl;

	uint32 lba_size;
	uint64 total_lba;
};

struct coldplug_slot {
	int is_in_use;
	uint32 magic;
	struct coldplug_device *cdev;
	uint32 slot_no;
	int is_embedded_device;

	int is_boot_lun_initialized;
	uint32 resolved_boot_lun_no;
	struct coldplug_lun *boot_lun;

	int boot_luns_count;
	int user_luns_count;
};

struct coldplug_device {
	int is_in_use;
	uint32 magic;
	enum coldplug_device_type device_type;
	struct coldplug_bdev_ops *bdev_ops;

	uint32 max_slots;
	uint32 max_luns;
	uint32 boot_lun_no;
};

struct coldplug_handle {
	int is_in_use;
	uint32 magic;
	struct coldplug_partition *cparti;
};


extern struct coldplug_partition coldplug_partitions[COLDPLUG_MAX_PARTITIONS];


void coldplug_device_init(void);

/*----------------------------------------------------------------------------
 * Device functions
 *--------------------------------------------------------------------------*/
struct coldplug_device* coldplug_device_alloc(
		enum coldplug_device_type dev_type);
void coldplug_device_free(struct coldplug_device *cdev);
struct coldplug_device* coldplug_device_find(
		enum coldplug_device_type dev_type);
void coldplug_device_validate(struct coldplug_device *cdev);

/*----------------------------------------------------------------------------
 * Slot functions
 *--------------------------------------------------------------------------*/
struct coldplug_slot* coldplug_slot_alloc(struct coldplug_device *cdev,
		uint32 slot_no, int is_embedded_device);
void coldplug_slot_free(struct coldplug_slot *cslot);
struct coldplug_slot* coldplug_slot_find(struct coldplug_device *cdev,
		uint32 slot_no, uint32 is_embedded_device);
void coldplug_slot_validate(struct coldplug_slot *cslot);

/*----------------------------------------------------------------------------
 * LUN functions
 *--------------------------------------------------------------------------*/
struct coldplug_lun* coldplug_lun_alloc(struct coldplug_slot *cslot,
		uint32 lun_no);
void coldplug_lun_free(struct coldplug_lun *clun);
struct coldplug_lun* coldplug_lun_find(struct coldplug_slot *cslot,
		uint32 lun_no);
void coldplug_lun_validate(struct coldplug_lun *clun);

/*----------------------------------------------------------------------------
 * Partition functions
 *--------------------------------------------------------------------------*/
struct coldplug_partition* coldplug_partition_alloc(
		struct coldplug_lun *clun);
void coldplug_partition_free(struct coldplug_partition *cparti);
void coldplug_partition_validate(struct coldplug_partition *cparti);
int coldplug_partition_is_fat(struct coldplug_partition *cparti);

/*----------------------------------------------------------------------------
 * Handle functions
 *--------------------------------------------------------------------------*/
struct coldplug_handle* coldplug_handle_alloc(
	struct coldplug_partition *cparti);
void coldplug_handle_free(struct coldplug_handle *chdl);
void coldplug_handle_validate(struct coldplug_handle *chdl);
int coldplug_is_handle_valid(struct coldplug_handle *chdl);
void coldplug_handle_invalidate(struct coldplug_handle *chdl);

#endif /* __COLDPLUG_DEVICE_H__ */
