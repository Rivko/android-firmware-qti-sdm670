/***********************************************************************
 * Coldplug Null Block Device.
 *
 * Coldplug
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Coldplug Null Block device.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_bdev_devnull.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  201-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_bdev_ops.h"
#include "coldplug_device.h"
#include "coldplug_api.h"

static int coldplug_bdev_devnull_ops_get_slot_and_lun_config(
		uint32 *max_slots, uint32 *max_luns, uint32 *boot_lun_no)
{
	(void) max_slots; (void) max_luns; (void) boot_lun_no;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_open(void **bdev_hdl,
		uint32 slot_no, uint32 lun_no)
{
	(void) bdev_hdl; (void) slot_no; (void) lun_no;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_close(void *bdev_hdl)
{
	(void) bdev_hdl;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_read(void *bdev_hdl,
		uint64 lba, uint64 count, uint8 *buff)
{
	(void) bdev_hdl; (void) lba; (void) count; (void) buff;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_write(void *bdev_hdl,
		uint64 lba, uint64 count, uint8 *buff)
{
	(void) bdev_hdl; (void) lba; (void) count; (void) buff;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_get_size(void *bdev_hdl,
		uint32 *lba_size, uint64 *total_lba)
{
	(void) bdev_hdl; (void) lba_size; (void) total_lba;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_get_info(void *bdev_hdl,
		struct coldplug_device_info *dev_info)
{
	(void) bdev_hdl; (void) dev_info;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_set_pon_wp(void *bdev_hdl,
		uint64 start_lba, uint64 total_lba)
{
	(void) bdev_hdl; (void) start_lba; (void) total_lba;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_erase(void *bdev_hdl,
		uint64 start_lba, uint64 total_lba)
{
	(void) bdev_hdl; (void) start_lba; (void) total_lba;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_format(void *bdev_hdl)
{
	(void) bdev_hdl;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_set_bootable_partition(void *bdev_hdl)
{
	(void) bdev_hdl;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_background_init_start(uint32 slot_no,
		uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_devnull_ops_background_init_finish(uint32 slot_no,
		uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_ENODEV;
}


static struct coldplug_bdev_ops coldplug_bdev_devnull_ops =
{
	coldplug_bdev_devnull_ops_get_slot_and_lun_config,
	coldplug_bdev_devnull_ops_open,
	coldplug_bdev_devnull_ops_close,
	coldplug_bdev_devnull_ops_read,
	coldplug_bdev_devnull_ops_write,
	coldplug_bdev_devnull_ops_get_size,
	coldplug_bdev_devnull_ops_get_info,
	coldplug_bdev_devnull_ops_set_pon_wp,
	coldplug_bdev_devnull_ops_erase,
	coldplug_bdev_devnull_ops_format,
	coldplug_bdev_devnull_ops_set_bootable_partition,
	coldplug_bdev_devnull_ops_background_init_start,
	coldplug_bdev_devnull_ops_background_init_finish,
};

struct coldplug_bdev_ops* coldplug_bdev_devnull_get_ops(void)
{
	return &coldplug_bdev_devnull_ops;
}
