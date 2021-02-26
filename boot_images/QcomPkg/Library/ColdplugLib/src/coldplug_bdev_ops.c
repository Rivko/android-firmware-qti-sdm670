/***********************************************************************
 * Coldplug Block Device Operations.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug Block Device Operations.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_bdev_ops.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_bdev_ops.h"
#include "coldplug_assert.h"

#include "coldplug_bdev_devnull.h"
#include "coldplug_bdev_sd.h"
#include "coldplug_bdev_ufs.h"
#include "coldplug_bdev_spi_nor.h"

void coldplug_bdev_ops_init(void)
{
	coldplug_bdev_sd_init();
	coldplug_bdev_ufs_init();
	coldplug_bdev_spi_nor_init();
}

struct coldplug_bdev_ops* coldplug_bdev_ops_get(
		enum coldplug_device_type dev_type)
{
	struct coldplug_bdev_ops *bdev_ops = NULL;

	switch (dev_type) {
	case COLDPLUG_DEVICE_DEVNULL:
		bdev_ops = coldplug_bdev_devnull_get_ops();
		break;

	case COLDPLUG_DEVICE_SD:
		bdev_ops = coldplug_bdev_sd_get_ops();
		break;

	case COLDPLUG_DEVICE_UFS:
		bdev_ops = coldplug_bdev_ufs_get_ops();
		break;

	case COLDPLUG_DEVICE_SPI_NOR:
		bdev_ops = coldplug_bdev_spi_nor_get_ops();
		break;

	default:
		COLDPLUG_ASSERT(0);
		break;
	}

	if (bdev_ops == NULL)
		bdev_ops = coldplug_bdev_devnull_get_ops();

	return bdev_ops;
}
