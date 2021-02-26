/**************************************************************************
 *
 * Stubs for when the device is not implemented
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_stubs.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2015-12-07   wek   Create.

===========================================================================*/

#ifndef __DEVPRG_STORAGE_STUBS_H__
#define __DEVPRG_STORAGE_STUBS_H__

#include "devprg_storage.h"


static inline void devprg_storage_stub_init(void)
{
}

static inline dp_res_t devprg_storage_stub_deinit(void)
{
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline void *devprg_storage_stub_open(uint32 slot, uint32 partition,
                                             dp_res_t *error)
{
  (void) slot;
  (void) partition;
  *error = DEVPRG_ERROR_NOT_SUPPORTED;
  return (void *)0xDEADDEAD;
}

static inline dp_res_t devprg_storage_stub_close(void *device)
{
  (void) device;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static dp_res_t devprg_storage_stub_io_options(void *device,
                                        struct devprg_io_options *io_options)
{
  (void) device;
  (void) io_options;

  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_read(void *device, void *buffer,
                                                uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) buffer;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_write(void *device, void *buffer,
                                    uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) buffer;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_erase(void *device,
                                                 uint64 start_sector, uint64 num_sectors)
{
  (void) device;
  (void) start_sector;
  (void) num_sectors;

  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_format(void *device)
{
  (void) device;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_set_bootable(void *device, int enable)
{
  (void) device;
  (void) enable;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_get_info(void *device,
                                                    struct devprg_storage_info *dev_info)
{
  (void) device;
  (void) dev_info;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}


static inline dp_res_t devprg_storage_stub_configure(uint32 slot,
                                                     struct devprg_storage_cfg_data *config)
{
  (void) slot;
  (void) config;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_fw_update(uint32 slot, void *buffer,
                                                     uint32 size_in_bytes)
{
  (void) slot;
  (void) buffer;
  (void) size_in_bytes;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  (void) size;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_info_raw(void *device, void *buffer,
                                                    uint32 *size)
{
  (void) device;
  (void) buffer;
  (void) size;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

static inline dp_res_t devprg_storage_stub_info_raw_print(void *device, void *buffer,
                                                          uint32 *size)
{
  (void) device;
  (void) buffer;
  (void) size;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

#endif /* __DEVPRG_STORAGE_STUBS_H__ */
