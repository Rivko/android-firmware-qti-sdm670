/***********************************************************************
 * Coldplug Device module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug Device functions.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_device.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_device.h"
#include "coldplug_api.h"
#include "coldplug_bdev_ops.h"
#include "coldplug_assert.h"
#include "coldplug_gpt.h"
#include "coldplug_mbr.h"
#include "coldplug_util.h"

#include <string.h>
#include <stdio.h>

#define COLDPLUG_DEVICE_MAGIC (0x76656463)
#define COLDPLUG_SLOT_MAGIC (0x746F6C73)
#define COLDPLUG_LUN_MAGIC (0x6E756C63)
#define COLDPLUG_PARTI_MAGIC (0x74726170)
#define COLDPLUG_HANDLE_MAGIC (0x6C646863)

struct coldplug_device coldplug_devices[COLDPLUG_MAX_DEVICES];
struct coldplug_slot coldplug_slots[COLDPLUG_MAX_SLOTS];
struct coldplug_lun coldplug_luns[COLDPLUG_MAX_LUNS];
struct coldplug_partition coldplug_partitions[COLDPLUG_MAX_PARTITIONS];
struct coldplug_handle coldplug_handles[COLDPLUG_MAX_HANDLES];

void coldplug_device_init(void)
{
	memset(&coldplug_devices, 0, sizeof(coldplug_devices));
	memset(&coldplug_slots, 0, sizeof(coldplug_slots));
	memset(&coldplug_luns, 0, sizeof(coldplug_luns));
	memset(&coldplug_partitions, 0, sizeof(coldplug_partitions));
	memset(&coldplug_handles, 0, sizeof(coldplug_handles));
}

/*----------------------------------------------------------------------------
 * Coldplug device functions
 *--------------------------------------------------------------------------*/
struct coldplug_device* coldplug_device_alloc(
		enum coldplug_device_type dev_type)
{
	struct coldplug_device *cdev = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_DEVICES; ++i) {
		if (!coldplug_devices[i].is_in_use) {
			cdev = &coldplug_devices[i];
			memset(cdev, 0, sizeof(struct coldplug_device));
			cdev->is_in_use = 1;
			cdev->magic = COLDPLUG_DEVICE_MAGIC;
			cdev->device_type = dev_type;
			cdev->bdev_ops = coldplug_bdev_ops_get(dev_type);
			return cdev;
		}
	}

	return NULL;
}

void coldplug_device_free(struct coldplug_device *cdev)
{
	struct coldplug_slot *cslot;
	int i;

	for (i = 0; i < COLDPLUG_MAX_SLOTS; ++i) {
		cslot = &coldplug_slots[i];
		if (!cslot->is_in_use)
			continue;

		if (cslot->cdev == cdev) {
			coldplug_slot_free(cslot);
		}
	}

	memset(cdev, 0, sizeof(*cdev));
}

struct coldplug_device* coldplug_device_find(
		enum coldplug_device_type dev_type)
{
	struct coldplug_device *cdev = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_DEVICES; ++i) {
		cdev = &coldplug_devices[i];

		if (!cdev->is_in_use)
			continue;

		if (cdev->device_type == dev_type) {
			coldplug_device_validate(cdev);
			return cdev;
		}
	}

	return NULL;
}

void coldplug_device_validate(struct coldplug_device *cdev)
{
	COLDPLUG_ASSERT(cdev != NULL);
	COLDPLUG_ASSERT(cdev->is_in_use == 1);
	COLDPLUG_ASSERT(cdev->magic == COLDPLUG_DEVICE_MAGIC);
	COLDPLUG_ASSERT(cdev->bdev_ops != NULL);
}

/*----------------------------------------------------------------------------
 * Coldplug slot functions
 *--------------------------------------------------------------------------*/
struct coldplug_slot* coldplug_slot_alloc(struct coldplug_device *cdev,
		uint32 slot_no,	int is_embedded_device)
{
	struct coldplug_slot *cslot = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_SLOTS; ++i) {
		if (!coldplug_slots[i].is_in_use) {
			cslot = &coldplug_slots[i];
			memset(cslot, 0, sizeof(struct coldplug_slot));
			cslot->is_in_use = 1;
			cslot->magic = COLDPLUG_SLOT_MAGIC;
			cslot->slot_no = slot_no;
			cslot->cdev = cdev;
			cslot->is_embedded_device = is_embedded_device;
			return cslot;
		}
	}

	return NULL;
}

void coldplug_slot_free(struct coldplug_slot *cslot)
{
	struct coldplug_lun *clun;
	int i;

	for (i = 0; i < COLDPLUG_MAX_LUNS; ++i) {
		clun = &coldplug_luns[i];
		if (!clun->is_in_use)
			continue;

		if (clun->cslot == cslot) {
			coldplug_lun_free(clun);
		}
	}

	memset(cslot, 0, sizeof(*cslot));
}

struct coldplug_slot* coldplug_slot_find(struct coldplug_device *cdev,
		uint32 slot_no, uint32 is_embedded_device)
{
	struct coldplug_slot *cslot = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_SLOTS; ++i) {
		cslot = &coldplug_slots[i];

		if (!cslot->is_in_use)
			continue;

		if ((cslot->cdev == cdev) &&
		    (cslot->slot_no == slot_no) &&
		    (cslot->is_embedded_device == is_embedded_device)) {
			coldplug_slot_validate(cslot);
			return cslot;
		}
	}

	return NULL;
}

void coldplug_slot_validate(struct coldplug_slot *cslot)
{
	COLDPLUG_ASSERT(cslot != NULL);
	COLDPLUG_ASSERT(cslot->is_in_use == 1);
	COLDPLUG_ASSERT(cslot->magic == COLDPLUG_SLOT_MAGIC);
	COLDPLUG_ASSERT(cslot->cdev != NULL);
}

/*----------------------------------------------------------------------------
 * Coldplug lun functions
 *--------------------------------------------------------------------------*/
struct coldplug_lun* coldplug_lun_alloc(struct coldplug_slot *cslot,
		uint32 lun_no)
{
	struct coldplug_lun *clun = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_LUNS; ++i) {
		if (!coldplug_luns[i].is_in_use) {
			clun = &coldplug_luns[i];
			memset(clun, 0, sizeof(struct coldplug_lun));
			clun->is_in_use = 1;
			clun->magic = COLDPLUG_LUN_MAGIC;
			clun->cslot = cslot;
			clun->lun_no = lun_no;
			return clun;
		}
	}

	return NULL;
}

void coldplug_lun_free(struct coldplug_lun *clun)
{
	struct coldplug_partition *cparti;
	int i;

	coldplug_lun_validate(clun);

	for (i = 0; i < COLDPLUG_MAX_PARTITIONS; ++i) {
		cparti = &coldplug_partitions[i];
		if (!cparti->is_in_use)
			continue;

		if (cparti->clun == clun) {
			coldplug_partition_free(cparti);
		}
	}

	memset(clun, 0, sizeof(*clun));
}

struct coldplug_lun* coldplug_lun_find(struct coldplug_slot *cslot,
		uint32 lun_no)
{
	struct coldplug_lun *clun = NULL;
	uint32 i;

	for (i= 0; i < COLDPLUG_MAX_LUNS; ++i) {
		clun = &coldplug_luns[i];

		if (!clun->is_in_use)
			continue;

		if ((clun->cslot == cslot) &&
		    (clun->lun_no == lun_no)) {
			coldplug_lun_validate(clun);
			return clun;
		}
	}

	return NULL;
}

void coldplug_lun_validate(struct coldplug_lun *clun)
{
	COLDPLUG_ASSERT(clun != NULL);
	COLDPLUG_ASSERT(clun->is_in_use == 1);
	COLDPLUG_ASSERT(clun->magic == COLDPLUG_LUN_MAGIC);
	COLDPLUG_ASSERT(clun->cslot != NULL);
	coldplug_slot_validate(clun->cslot);
}

/*----------------------------------------------------------------------------
 * Coldplug partition functions
 *--------------------------------------------------------------------------*/
struct coldplug_partition* coldplug_partition_alloc(
			struct coldplug_lun *clun)
{
	struct coldplug_partition *cparti = NULL;
	uint32 i;

	coldplug_lun_validate(clun);

	for (i= 0; i < COLDPLUG_MAX_PARTITIONS; ++i) {
		if (!coldplug_partitions[i].is_in_use) {
			cparti = &coldplug_partitions[i];
			memset(cparti, 0, sizeof(struct coldplug_partition));
			cparti->is_in_use = 1;
			cparti->magic = COLDPLUG_PARTI_MAGIC;
			cparti->clun = clun;
			snprintf(cparti->name, sizeof(cparti->name), "p-%d", i);
			return cparti;
		}
	}

	return NULL;
}

void coldplug_partition_free(struct coldplug_partition *cparti)
{
	struct coldplug_handle *chdl;
	int i;

	coldplug_partition_validate(cparti);

	for (i = 0; i < COLDPLUG_MAX_HANDLES; ++i) {
		chdl = &coldplug_handles[i];
		if (!chdl->is_in_use)
			continue;

		if (chdl->cparti == cparti) {
			coldplug_handle_invalidate(chdl);
		}
	}

	memset(cparti, 0, sizeof(*cparti));
}

void coldplug_partition_validate(struct coldplug_partition *cparti)
{
	COLDPLUG_ASSERT(cparti != NULL);
	COLDPLUG_ASSERT(cparti->is_in_use == 1);
	COLDPLUG_ASSERT(cparti->magic == COLDPLUG_PARTI_MAGIC);
	coldplug_lun_validate(cparti->clun);
}

int coldplug_partition_get_info(struct coldplug_handle *chdl,
	struct coldplug_partition *cparti)
{
	coldplug_handle_validate(chdl);
	COLDPLUG_ASSERT(cparti != NULL);

	coldplug_memscpy(cparti, sizeof (*cparti),
		chdl->cparti, sizeof(*cparti));

	return 0;
}

int coldplug_partition_is_fat(struct coldplug_partition *cparti)
{
	int is_fat = 0;

	coldplug_partition_validate(cparti);

	switch (cparti->parti_info.partition_type) {
	case COLDPLUG_PARTITION_TYPE_RAW:
		is_fat = 0;
		break;

	case COLDPLUG_PARTITION_TYPE_GPT:
		is_fat = coldplug_gpt_partition_is_fat(cparti);
		break;

	case COLDPLUG_PARTITION_TYPE_MBR:
		is_fat = coldplug_mbr_partition_is_fat(cparti);
		break;

	case COLDPLUG_PARTITION_TYPE_FAT:
		is_fat = 1;
		break;

	default:
		COLDPLUG_ASSERT(0);
		break;
	}

	return is_fat;
}



/*----------------------------------------------------------------------------
 * Handle functions
 *--------------------------------------------------------------------------*/
struct coldplug_handle* coldplug_handle_alloc(
	struct coldplug_partition *cparti)
{
	struct coldplug_handle *chdl = NULL;
	uint32 i;

	coldplug_partition_validate(cparti);

	for (i= 0; i < COLDPLUG_MAX_HANDLES; ++i) {
		if (!coldplug_handles[i].is_in_use) {
			chdl = &coldplug_handles[i];
			memset(chdl, 0, sizeof(struct coldplug_handle));
			chdl->is_in_use = 1;
			chdl->magic = COLDPLUG_HANDLE_MAGIC;
			chdl->cparti = cparti;
			break;
		}
	}

	return chdl;
}

void coldplug_handle_free(struct coldplug_handle *chdl)
{
	coldplug_handle_validate(chdl);
	memset(chdl, 0, sizeof(struct coldplug_handle));
}

void coldplug_handle_validate(struct coldplug_handle *chdl)
{
	COLDPLUG_ASSERT(chdl != NULL);
	COLDPLUG_ASSERT(chdl->is_in_use == 1);
	COLDPLUG_ASSERT(chdl->magic == COLDPLUG_HANDLE_MAGIC);
}

int coldplug_is_handle_valid(struct coldplug_handle *chdl)
{
	if (chdl == NULL)
		return -1;

	if (!chdl->is_in_use)
		return -1;

	if (chdl->magic != COLDPLUG_HANDLE_MAGIC)
		return -1;

	return 0;
}

void coldplug_handle_invalidate(struct coldplug_handle *chdl)
{
	coldplug_handle_validate(chdl);
	chdl->cparti = NULL;
}
