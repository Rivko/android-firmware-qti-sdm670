/***********************************************************************
 * Coldplug MBR module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug MBR module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_mbr.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_MBR_H__
#define __COLDPLUG_MBR_H__

#include "coldplug_config_i.h"
#include "coldplug_api.h"
#include "coldplug_device.h"
#include "coldplug_pcache.h"

#define COLDPLUG_PARTITION_ID_GPT            0xEE
#define COLDPLUG_MBR_DOS_EXTENDED_PARTITION  0x05
#define COLDPLUG_MBR_WIN_EXTENDED_PARTITION  0x0F

PACKED struct coldplug_mbr_partition_record {
	uint8 bootable;
	uint8 start_CHS[3]; /* Cylinder, Head, Sector */
	uint8 partition_type;
	uint8 end_CHS[3];
	uint32 start_lba;   /* Start Logical Block Address */
	uint32 size_in_blocks;
} PACKED_POST;

/* Typical MBR layout in a disk, used by MBR and GPT */
PACKED struct coldplug_mbr_layout {
	uint8 boot_code[440];
	uint8 disk_signature[4];
	uint8 pad[2];       /* There is no specified use for this field. */
	struct coldplug_mbr_partition_record records[4];
	uint8 signature[2]; /* Should be set to 0xAA and 0x55 */
} PACKED_POST;


int check_mbr_signature(struct coldplug_pcache_data *data_cache);

int gpt_check_protective_mbr(struct coldplug_pcache_data *data_cache,
				uint32 *total_lba);

int coldplug_mbr_parse(struct coldplug_lun *clun,
			struct coldplug_pcache_data *data_cache);

struct coldplug_handle* coldplug_open_mbr_partition_i(
		enum coldplug_device_type dev_type,
		int is_embedded_device, uint32 mbr_id);

int coldplug_mbr_partition_is_fat(struct coldplug_partition *cparti);


#endif /* __COLDPLUG_MBR_H__ */
