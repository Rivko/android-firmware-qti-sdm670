/***********************************************************************
 * Coldplug UFS Block Device.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug UFS Block Device module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_bdev_ufs.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_BDEV_UFS__H__
#define __COLDPLUG_BDEV_UFS__H__

#include "coldplug_config_i.h"
#include "coldplug_bdev_ops.h"

#ifdef COLDPLUG_UFS_ENABLE

void coldplug_bdev_ufs_init(void);
struct coldplug_bdev_ops* coldplug_bdev_ufs_get_ops(void);

#else

static inline void coldplug_bdev_ufs_init(void)
{
}

static inline struct coldplug_bdev_ops* coldplug_bdev_ufs_get_ops(void)
{
	return NULL;
}

#endif /* COLDPLUG_UFS_ENABLE */

#endif /* __COLDPLUG_BDEV_UFS__H__ */
