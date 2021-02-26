/***********************************************************************
 * Coldplug partition parser module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug partition parser module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_parse.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_parse.h"
#include "coldplug_device.h"
#include "coldplug_gpt.h"
#include "coldplug_mbr.h"
#include "coldplug_fat.h"
#include "coldplug_assert.h"

#include <string.h>


int coldplug_parse_lun(struct coldplug_lun *clun)
{
	struct coldplug_pcache_data *pcache_data;
	struct coldplug_partition *cparti;
	struct coldplug_partition_in_ram *parti;
	int result = COLDPLUG_RESULT_UNKNOWN;

	coldplug_lun_validate(clun);

	cparti = coldplug_partition_alloc(clun);
	if (cparti == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto End;
	}

	cparti->start_lba = 0;
	cparti->total_lba = clun->total_lba;
	parti = &cparti->parti_info;
	parti->partition_type = COLDPLUG_PARTITION_TYPE_RAW;

	pcache_data = coldplug_pcache_init_for_lun(clun);
	if (pcache_data == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto End;
	}

	result = coldplug_gpt_parse(clun, pcache_data);
	if (result == 0)
		goto End;

	result = coldplug_mbr_parse(clun, pcache_data);
	if (result == 0)
		goto End;

	result = coldplug_fat_parse(clun, pcache_data);

End:
	return result;
}
