/***********************************************************************
 * Coldplug FAT module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug FAT module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_fat.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_FAT_H__
#define __COLDPLUG_FAT_H__

#include "coldplug_config_i.h"
#include "coldplug_api.h"
#include "coldplug_device.h"
#include "coldplug_pcache.h"

int coldplug_fat_parse(struct coldplug_lun *clun,
	struct coldplug_pcache_data *data_cache);

#endif /* __COLDPLUG_FAT_H__ */
