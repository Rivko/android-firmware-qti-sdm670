/**************************************************************************
 * FILE: devprg_storage_null.c
 *
 * NULL Device implementation. Returns Success for every operation.
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_null.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2015-11-17   wek   Create.

===========================================================================*/

#include "devprg_storage_null.h"
#include "devprg_log.h"

/* Dummy handle. */
struct dp_storage_null
{
  char state;
};
static struct dp_storage_null dp_null_data;

void devprg_storage_null_init(void)
{
  dp_null_data.state = 'n';
}

dp_res_t devprg_storage_null_deinit(void)
{
  dp_null_data.state = '0';
  return DEVPRG_SUCCESS;
}

void *devprg_storage_null_open(uint32 slot, uint32 partition, dp_res_t *error)
{
  (void) slot;
  (void) partition;
  (void) error;
  dp_null_data.state = 'N';
  return &dp_null_data;
}

dp_res_t devprg_storage_null_close(void *device)
{
  struct dp_storage_null *dev = (struct dp_storage_null *)device;
  dev->state = 'n';
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_io_options(void *device,
                                        struct devprg_io_options *io_options)
{
  (void) device;
  (void) io_options;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_read(void *device, void *buffer,
                                  uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) buffer;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_write(void *device, void *buffer,
                                   uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) buffer;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_erase(void *device,
                                   uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_format(void *device)
{
  (void) device;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_set_bootable(void *device, int enable)
{
  (void) device;
  (void) enable;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_get_info(void *device,
                                      struct devprg_storage_info *dev_info)
{
  (void) device;
  (void) dev_info;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_configure(uint32 slot,
                                       struct devprg_storage_cfg_data *config)
{
  (void) slot;
  (void) config;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_fw_update(uint32 slot, void *buffer,
                                       uint32 size_in_bytes)
{
  (void) slot;
  (void) buffer;
  (void) size_in_bytes;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  (void) size;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_info_raw(void *device, void *buffer,
                                      uint32 *size)
{
  (void) device;
  (void) buffer;
  (void) size;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_null_info_raw_print(void *device, void *buffer,
                                            uint32 *size)
{
  (void) device;
  (void) buffer;
  (void) size;
  return DEVPRG_SUCCESS;
}
