/**************************************************************************
 * FILE: devprg_storage_null.h
 *
 * NULL Device. Returns Success for every operation.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_null.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2015-11-17   wek   Create.

===========================================================================*/

#ifndef __DEVPRG_STORAGE_NULL_H__
#define __DEVPRG_STORAGE_NULL_H__

#include "devprg_storage.h"

/* For documentation see devprg_storage.h */
void devprg_storage_null_init(void);
dp_res_t devprg_storage_null_deinit(void);
void *devprg_storage_null_open(uint32 slot, uint32 partition, dp_res_t *error);
dp_res_t devprg_storage_null_close(void *device);
dp_res_t devprg_storage_null_io_options(void *device,
                                        struct devprg_io_options *io_options);
dp_res_t devprg_storage_null_read(void *device, void *buffer,
                             uint64 start_sector, uint64 num_sectors);
dp_res_t devprg_storage_null_write(void *device, void *buffer,
                                   uint64 start_sector, uint64 num_sectors);
dp_res_t devprg_storage_null_erase(void *device,
                              uint64 start_sector, uint64 num_sectors);
dp_res_t devprg_storage_null_format(void *device);
dp_res_t devprg_storage_null_set_bootable(void *device, int enable);
dp_res_t devprg_storage_null_get_info(void *device,
                                 struct devprg_storage_info *dev_info);
dp_res_t devprg_storage_null_configure(uint32 slot,
                                  struct devprg_storage_cfg_data *config);
dp_res_t devprg_storage_null_fw_update(uint32 slot, void *buffer,
                                  uint32 size_in_bytes);
dp_res_t devprg_storage_null_info_raw_size(void *device, uint32 *size);
dp_res_t devprg_storage_null_info_raw(void *device, void *buffer,
                                 uint32 *size);
dp_res_t devprg_storage_null_info_raw_print(void *device, void *buffer,
                                       uint32 *size);

#endif /* __DEVPRG_STORAGE_NULL_H__ */
