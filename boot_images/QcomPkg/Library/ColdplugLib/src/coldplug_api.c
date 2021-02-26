/***********************************************************************
 * coldplug APIs
 *
 * Coldplug
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Coldplug API implementations.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_api.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-08-21   rp    To correct the parti name comparision in open partition
  2017-08-11   rp    Close the dev handle if get_info fails
  2017-07-12   rp    Fix compiler warning
  2017-05-18   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_api.h"
#include "coldplug_priv_api.h"
#include "coldplug_device.h"
#include "coldplug_bdev_ops.h"
#include "coldplug_parse.h"
#include "coldplug_pcache.h"
#include "coldplug_gpt.h"
#include "coldplug_mbr.h"
#include "coldplug_util.h"
#include "coldplug_debug.h"
#include "coldplug_iter.h"
#include "coldplug_assert.h"

#include <string.h>

static int coldplug_init_done = 0;
static struct coldplug_device_info coldplug_device_info_inst;

int coldplug_init(void)
{
	++coldplug_init_done;

	coldplug_debug_init();
	coldplug_device_init();
	coldplug_pcache_init();

	return COLDPLUG_RESULT_SUCCESS;
}

static struct coldplug_device* coldplug_device_alloc_helper(
	enum coldplug_device_type dev_type)
{
	struct coldplug_device *cdev;

	cdev = coldplug_device_find(dev_type);
	if (cdev != NULL)
		return cdev;

	cdev = coldplug_device_alloc(dev_type);
	return cdev;
}

static struct coldplug_slot* coldplug_slot_alloc_helper(
	struct coldplug_device *cdev, uint32 slot_no, int is_embedded_device)
{
	struct coldplug_slot *cslot;

	cslot = coldplug_slot_find(cdev, slot_no, is_embedded_device);
	if (cslot != NULL)
		return cslot;

	cslot = coldplug_slot_alloc(cdev, slot_no, is_embedded_device);
	return cslot;
}

static struct coldplug_lun* coldplug_lun_alloc_helper(
	struct coldplug_slot *cslot, uint32 lun_no, int is_boot_lun)
{
	struct coldplug_lun *clun;

	COLDPLUG_ASSERT(cslot != NULL);

	clun = coldplug_lun_find(cslot, lun_no);
	COLDPLUG_ASSERT(clun == NULL);

	clun = coldplug_lun_alloc(cslot, lun_no);
	if (clun != NULL) {
		clun->is_boot_lun = is_boot_lun;
	}
	return clun;
}

int coldplug_open_device(enum coldplug_device_type dev_type,
	int is_embedded_device)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_lun *clun;
	struct coldplug_device_info *dev_info;
	uint32 slot_no, lun_no, good_luns_count = 0;
	void *bdev_hdl;
	int result = COLDPLUG_RESULT_UNKNOWN;

	cdev = coldplug_device_alloc_helper(dev_type);
	if (cdev == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto Error;
	}

	cdev->max_slots = cdev->max_luns = cdev->boot_lun_no = 0;
	result = cdev->bdev_ops->get_slot_and_lun_config(&cdev->max_slots,
			&cdev->max_luns, &cdev->boot_lun_no);
	if (result != 0) {
		result = COLDPLUG_RESULT_ENODEV;
		goto Error;
	}
	COLDPLUG_ASSERT(cdev->max_slots > 0);
	COLDPLUG_ASSERT(cdev->max_luns > 0);

	dev_info = &coldplug_device_info_inst;

	for (slot_no = 0; slot_no < cdev->max_slots; ++slot_no) {

		cslot = coldplug_slot_alloc_helper(cdev, slot_no,
				is_embedded_device);
		if (cslot == NULL) {
			result = COLDPLUG_RESULT_ENOMEM;
			goto Error;
		}

		for (lun_no = 0; lun_no < cdev->max_luns; ++lun_no) {

			bdev_hdl = NULL;
			result = cdev->bdev_ops->open(&bdev_hdl,
					cslot->slot_no, lun_no);
			if (result != 0)
			{
				break;
			}
			
			memset(dev_info, 0, sizeof(*dev_info));
			result = cdev->bdev_ops->get_info(bdev_hdl, dev_info);
			if (result != 0) {
				cdev->bdev_ops->close(bdev_hdl);
				break;
			}

			clun = coldplug_lun_alloc_helper(cslot, lun_no, 0);
			if (clun == NULL) {
				(void) cdev->bdev_ops->close(bdev_hdl);
				result = COLDPLUG_RESULT_ENOMEM;
				goto Error;
			}

			clun->bdev_hdl = bdev_hdl;

			clun->cslot->is_embedded_device = dev_info->is_embedded_device;
			clun->lba_size = dev_info->bytes_per_lba;
			clun->total_lba = dev_info->total_lba_count;

			result = coldplug_parse_lun(clun);
			if (result == 0) {
				++cslot->user_luns_count;
				++good_luns_count;
			}
		}
	}

	if (good_luns_count > 0)
		result = COLDPLUG_RESULT_SUCCESS;
	else
		result = COLDPLUG_RESULT_ENODEV;

Error:
	goto End;
End:
	return result;
}

int coldplug_open_boot_device(enum coldplug_device_type dev_type,
	int is_embedded_device)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_lun *clun;
	void *bdev_hdl;
	uint32 slot_no, good_luns_count = 0;
	int result = COLDPLUG_RESULT_UNKNOWN;

	cdev = coldplug_device_alloc_helper(dev_type);
	if (cdev == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto Error;
	}

	cdev->max_slots = cdev->max_luns = cdev->boot_lun_no = 0;
	result = cdev->bdev_ops->get_slot_and_lun_config(&cdev->max_slots,
			&cdev->max_luns, &cdev->boot_lun_no);
	if (result != 0) {
		result = COLDPLUG_RESULT_ENODEV;
		goto Error;
	}

	if (cdev->boot_lun_no == -1) {
		result = COLDPLUG_RESULT_NOT_SUPPORTED;
		goto Error;
	}

	for (slot_no = 0; slot_no < cdev->max_slots; ++slot_no) {

		cslot = coldplug_slot_alloc_helper(cdev, slot_no,
				is_embedded_device);
		if (cslot == NULL) {
			result = COLDPLUG_RESULT_ENOMEM;
			goto Error;
		}

		bdev_hdl = NULL;
		result = cdev->bdev_ops->open(&bdev_hdl,
				cslot->slot_no, cdev->boot_lun_no);
		if (result != 0) {
			result = COLDPLUG_RESULT_DEVICE_ERROR;
			goto Error;
		}

		clun = coldplug_lun_alloc_helper(cslot, cdev->boot_lun_no, 1);
		if (clun == NULL) {
			(void) cdev->bdev_ops->close(bdev_hdl);
			result = COLDPLUG_RESULT_ENOMEM;
			goto Error;
		}

		clun->bdev_hdl = bdev_hdl;
		clun->lba_size = COLDPLUG_LBA_SIZE_IN_BYTES;
		clun->total_lba = 1;
		cslot->boot_lun = clun;

		result = coldplug_parse_lun(clun);
		if (result == 0) {
			++cslot->boot_luns_count;
			++good_luns_count;
		}
	}

	if (good_luns_count > 0)
		result = COLDPLUG_RESULT_SUCCESS;
	else
		result = COLDPLUG_RESULT_ENODEV;

Error:
	goto End;
End:
	return result;
}

int coldplug_close_device(enum coldplug_device_type dev_type,
			int is_embedded_device)
{
	struct coldplug_device *cdev;
	int result = COLDPLUG_RESULT_ENODEV;

	cdev = coldplug_device_find(dev_type);
	if (cdev == NULL)
		goto Error;

	coldplug_device_free(cdev);
	result = 0;

Error:
	return result;
}

struct coldplug_handle* coldplug_open_gpt_partition(
	enum coldplug_device_type dev_type, int is_embedded_device,
	const struct coldplug_guid *guid)
{
	if ((dev_type <= COLDPLUG_DEVICE_DEVNULL) ||
		(dev_type >= COLDPLUG_DEVICE_MAX))
		return NULL;

	if ((is_embedded_device != 0) &&
		(is_embedded_device != 1))
		return NULL;

	if (guid == NULL)
		return NULL;

	return coldplug_open_gpt_partition_i(dev_type,
		is_embedded_device, guid);
}

struct coldplug_handle* coldplug_open_mbr_partition(
	enum coldplug_device_type dev_type, int is_embedded_device,
	uint32 mbr_id)
{
	if ((dev_type <= COLDPLUG_DEVICE_DEVNULL) ||
		(dev_type >= COLDPLUG_DEVICE_MAX))
		return NULL;

	if ((is_embedded_device != 0) &&
		(is_embedded_device != 1))
		return NULL;

	return coldplug_open_mbr_partition_i(dev_type,
		is_embedded_device, mbr_id);
}

struct coldplug_handle* coldplug_open_partition(const char *parti_name)
{
	struct coldplug_handle *chdl = NULL;
	struct coldplug_partition *cparti = NULL;
	int result;
	uint32 i, len1, len2;

	for ( i = 0; i < COLDPLUG_MAX_PARTITIONS; ++i) {
		cparti = &coldplug_partitions[i];
		if (!cparti->is_in_use)
			continue;

		len1 = strlen(cparti->name);
		len2 = strlen(parti_name);
		if (len1 == len2) {
			result = strncmp(cparti->name, parti_name, len1);
			if (result == 0) {
				chdl = coldplug_handle_alloc(cparti);
				return chdl;
			}
		}
	}

	return NULL;
}

int coldplug_close_partition(struct coldplug_handle *chdl)
{
	if (coldplug_is_handle_valid(chdl) != 0)
		return COLDPLUG_RESULT_INVALID_HANDLE;

	coldplug_handle_validate(chdl);

	memset(chdl, 0, sizeof(*chdl));

	return 0;
}

int coldplug_read(struct coldplug_handle *chdl, uint64 lba,
	uint64 lba_count, uint8 *buf)
{
	struct coldplug_partition *cparti;
	struct coldplug_lun *clun;
	struct coldplug_device *cdev;
	uint64 start_lba;

	if (coldplug_is_handle_valid(chdl) != 0)
		return COLDPLUG_RESULT_INVALID_HANDLE;

	if (buf == NULL)
		return COLDPLUG_RESULT_INVALID_ARG;

	coldplug_handle_validate(chdl);

	cparti = chdl->cparti;
	clun = cparti->clun;
	cdev = clun->cslot->cdev;

	if (COLDPLUG_INT_OVERFLOW(lba, lba_count) ||
	   ((lba + lba_count) > cparti->total_lba))
		return COLDPLUG_RESULT_ERANGE;

	start_lba = lba + cparti->start_lba;
	return cdev->bdev_ops->read(clun->bdev_hdl, start_lba, lba_count, buf);
}

int coldplug_write(struct coldplug_handle *chdl, uint64 lba,
			uint64 lba_count, uint8 *buf)
{
	struct coldplug_partition *cparti;
	struct coldplug_lun *clun;
	struct coldplug_device *cdev;
	uint64 start_lba;

	if (coldplug_is_handle_valid(chdl) != 0)
		return COLDPLUG_RESULT_INVALID_HANDLE;

	if (buf == NULL)
		return COLDPLUG_RESULT_INVALID_ARG;

	coldplug_handle_validate(chdl);

	cparti = chdl->cparti;
	clun = cparti->clun;
	cdev = clun->cslot->cdev;

	if (COLDPLUG_INT_OVERFLOW(lba, lba_count) ||
	   ((lba + lba_count) > cparti->total_lba))
		return COLDPLUG_RESULT_ERANGE;

	start_lba = lba + cparti->start_lba;
	return cdev->bdev_ops->write(clun->bdev_hdl,
		start_lba, lba_count, buf);
}

int coldplug_get_size(struct coldplug_handle *chdl, uint32 *lba_size,
			uint64 *lba_count)
{
	struct coldplug_partition *cparti;
	struct coldplug_lun *clun;

	if (coldplug_is_handle_valid(chdl) != 0)
		return COLDPLUG_RESULT_INVALID_HANDLE;

	if ((lba_size == NULL) || (lba_count == NULL))
		return COLDPLUG_RESULT_INVALID_ARG;

	coldplug_handle_validate(chdl);

	cparti = chdl->cparti;
	clun = cparti->clun;

	*lba_size  = clun->lba_size;
	*lba_count = cparti->total_lba;
	return COLDPLUG_RESULT_SUCCESS;
}

int coldplug_get_start_lba(struct coldplug_handle *chdl, uint64 *start_lba_arg)
{
	struct coldplug_partition *cparti;
	uint64 start_lba;

	if (coldplug_is_handle_valid(chdl) != 0)
		return -1;

	if (start_lba_arg == NULL)
		return -1;

	coldplug_handle_validate(chdl);

	cparti = chdl->cparti;
	start_lba = cparti->start_lba;

	*start_lba_arg = start_lba;
	return 0;
}

int coldplug_get_device_info(struct coldplug_handle *chdl,
				struct coldplug_device_info *cdev_info)
{
	struct coldplug_partition *cparti;
	struct coldplug_lun *clun;
	struct coldplug_device *cdev;

	if (coldplug_is_handle_valid(chdl) != 0)
		return COLDPLUG_RESULT_INVALID_HANDLE;

	if (cdev_info == NULL)
		return COLDPLUG_RESULT_INVALID_ARG;

	coldplug_handle_validate(chdl);

	cparti = chdl->cparti;
	clun = cparti->clun;
	cdev = clun->cslot->cdev;

	return cdev->bdev_ops->get_info(clun->bdev_hdl, cdev_info);
}

int coldplug_background_init_start(enum coldplug_device_type device_type,
	int is_embedded_device, uint32 init_context)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_bdev_ops *bdev_ops;
	uint32 slot_no;
	int result = COLDPLUG_RESULT_NOT_SUPPORTED;

	cdev = coldplug_device_find(device_type);
	if (cdev == NULL)
		return COLDPLUG_RESULT_ENODEV;

	coldplug_device_validate(cdev);

	bdev_ops = cdev->bdev_ops;

	for (slot_no = 0; slot_no < cdev->max_slots; ++slot_no) {

		cslot = coldplug_slot_find(cdev, slot_no, is_embedded_device);
		if (cslot == NULL)
			continue;

		result = bdev_ops->background_init_start(slot_no, init_context);
		if (result != 0)
			goto End;
	}
End:
	return result;
}

int coldplug_background_init_finish(enum coldplug_device_type device_type,
				int is_embedded_device, uint32 init_context)
{
	struct coldplug_device *cdev;
	struct coldplug_slot *cslot;
	struct coldplug_bdev_ops *bdev_ops;
	uint32 slot_no;
	int result = COLDPLUG_RESULT_NOT_SUPPORTED;

	cdev = coldplug_device_find(device_type);
	if (cdev == NULL)
		return COLDPLUG_RESULT_ENODEV;

	coldplug_device_validate(cdev);

	bdev_ops = cdev->bdev_ops;

	for (slot_no = 0; slot_no < cdev->max_slots; ++slot_no) {

		cslot = coldplug_slot_find(cdev, slot_no, is_embedded_device);
		if (cslot == NULL)
			continue;

		result = bdev_ops->background_init_finish(slot_no, init_context);
		if (result != 0)
			goto End;
	}
End:
	return result;
}
