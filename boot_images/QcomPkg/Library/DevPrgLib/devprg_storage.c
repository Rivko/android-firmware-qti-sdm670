/**************************************************************************
 * FILE: devprg_storage.c
 *
 * Common APIs for accessing any storage device.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2017-02-06   wek   Call close only for NAND.
2017-02-06   wek   Improve search of open handles.
2017-02-02   wek   Add io options to change how data is read/write.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2015-11-13   wek   Create.

===========================================================================*/

#include "devprg_storage.h"
#include "devprg_storage_nand.h"
#include "devprg_storage_null.h"
#include "devprg_storage_sdcc.h"
#include "devprg_storage_spinor.h"
#include "devprg_storage_ufs.h"
#include "devprg_log.h"

struct devprg_storage
{
  enum devprg_storage_type type;
  uint32 slot;
  uint32 partition;
  void *handle; /* Handle from the underlying layer. */
  char in_use; /* Is this entry in the structure in use? */
};

/* Maximum number of physical partitions or LUNs that can be handled. */
#ifndef DEVPRG_MAX_PHY_PARTITIONS
#define DEVPRG_MAX_PHY_PARTITIONS 16
#endif

static struct devprg_storage devprg_storage_data[DEVPRG_MAX_PHY_PARTITIONS];

typedef void (*dpstorage_init_ops)(void);
typedef dp_res_t (*dpstorage_deinit_ops)(void);

typedef void* (*dpstorage_open_ops)(uint32 slot, uint32 partition,
                                    dp_res_t *error);
typedef dp_res_t (*dpstorage_close_ops)(void *device);
typedef dp_res_t (*dpstorage_io_options_ops)(void *device,
                                          struct devprg_io_options *io_options);
typedef dp_res_t (*dpstorage_read_ops)(void *device, void *buffer,
                                    uint64 start_sector, uint64 num_sectors);
typedef dp_res_t (*dpstorage_write_ops)(void *device, void *buffer,
                                        uint64 start_sector, uint64 num_sectors);

typedef dp_res_t (*dpstorage_erase_ops)(void *device,
                                    uint64 start_sector, uint64 num_sectors);
typedef dp_res_t (*dpstorage_format_ops)(void *device);
typedef dp_res_t (*dpstorage_set_bootable_ops)(void *device, int enable);
typedef dp_res_t (*dpstorage_get_info_ops)(void *device,
                                       struct devprg_storage_info *dev_info);
typedef dp_res_t (*dpstorage_configure_ops)(uint32 slot,
                                     struct devprg_storage_cfg_data *config);
typedef dp_res_t (*dpstorage_fw_update_ops)(uint32 slot,
                                         void *buffer, uint32 size_in_bytes);
typedef dp_res_t (*dpstorage_info_raw_size_ops)(void *device, uint32 *size);
typedef dp_res_t (*dpstorage_info_raw_ops)(void *device,
                                           void *buffer, uint32 *size);
typedef dp_res_t (*dpstorage_info_raw_print_ops)(void *device,
                                                 void *buffer, uint32 *size);

/* Define the operations supported by the storage device.
 *
 * The operations are intentionally split into different variables to
 * allow the linker to remove unused functions and variables when they
 * are not referenced.
 * Another possible (and more typical) implementation is to have one
 * large ops structure, where all the operations are defined. Each
 * device takes care of defining all the operations. The drawback
 * of this implementation is that it creates references to the functions
 * since they are defined in the whole structure.
 *
 * Having each operation as a separate variable allows the linker to
 * remove all the code (and the whole code tree) and variables by removing
 * the call on the top. */

static dpstorage_init_ops dpstorage_init[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_init,
  devprg_storage_sdcc_init,
  devprg_storage_spinor_init,
  devprg_storage_ufs_init,
  devprg_storage_nand_init,
};

static dpstorage_deinit_ops dpstorage_deinit[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_deinit,
  devprg_storage_sdcc_deinit,
  devprg_storage_spinor_deinit,
  devprg_storage_ufs_deinit,
  devprg_storage_nand_deinit,
};


static dpstorage_open_ops dpstorage_open[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_open,
  devprg_storage_sdcc_open,
  devprg_storage_spinor_open,
  devprg_storage_ufs_open,
  devprg_storage_nand_open,
};

static dpstorage_close_ops dpstorage_close[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_close,
  devprg_storage_sdcc_close,
  devprg_storage_spinor_close,
  devprg_storage_ufs_close,
  devprg_storage_nand_close,
};

static dpstorage_io_options_ops dpstorage_io_options[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_io_options,
  devprg_storage_sdcc_io_options,
  devprg_storage_spinor_io_options,
  devprg_storage_ufs_io_options,
  devprg_storage_nand_io_options,
};

static dpstorage_read_ops dpstorage_read[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_read,
  devprg_storage_sdcc_read,
  devprg_storage_spinor_read,
  devprg_storage_ufs_read,
  devprg_storage_nand_read,
};

static dpstorage_write_ops dpstorage_write[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_write,
  devprg_storage_sdcc_write,
  devprg_storage_spinor_write,
  devprg_storage_ufs_write,
  devprg_storage_nand_write,
};

static dpstorage_erase_ops dpstorage_erase[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_erase,
  devprg_storage_sdcc_erase,
  devprg_storage_spinor_erase,
  devprg_storage_ufs_erase,
  devprg_storage_nand_erase,
};

static dpstorage_format_ops dpstorage_format[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_format,
  devprg_storage_sdcc_format,
  devprg_storage_spinor_format,
  devprg_storage_ufs_format,
  devprg_storage_nand_format,
};

static dpstorage_set_bootable_ops dpstorage_set_bootable[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_set_bootable,
  devprg_storage_sdcc_set_bootable,
  devprg_storage_spinor_set_bootable,
  devprg_storage_ufs_set_bootable,
  devprg_storage_nand_set_bootable,
};

static dpstorage_get_info_ops dpstorage_get_info[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_get_info,
  devprg_storage_sdcc_get_info,
  devprg_storage_spinor_get_info,
  devprg_storage_ufs_get_info,
  devprg_storage_nand_get_info,
};

static dpstorage_configure_ops dpstorage_configure[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_configure,
  devprg_storage_sdcc_configure,
  devprg_storage_spinor_configure,
  devprg_storage_ufs_configure,
  devprg_storage_nand_configure,
};

static dpstorage_fw_update_ops dpstorage_fw_update[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_fw_update,
  devprg_storage_sdcc_fw_update,
  devprg_storage_spinor_fw_update,
  devprg_storage_ufs_fw_update,
  devprg_storage_nand_fw_update,
};

static dpstorage_info_raw_size_ops dpstorage_info_raw_size[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_info_raw_size,
  devprg_storage_sdcc_info_raw_size,
  devprg_storage_spinor_info_raw_size,
  devprg_storage_ufs_info_raw_size,
  devprg_storage_nand_info_raw_size,
};

static dpstorage_info_raw_ops dpstorage_info_raw[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_info_raw,
  devprg_storage_sdcc_info_raw,
  devprg_storage_spinor_info_raw,
  devprg_storage_ufs_info_raw,
  devprg_storage_nand_info_raw,
};

static dpstorage_info_raw_print_ops dpstorage_info_raw_print[DEVPRG_STORAGE_MAX_TYPE] =
{
  devprg_storage_null_info_raw_print,
  devprg_storage_sdcc_info_raw_print,
  devprg_storage_spinor_info_raw_print,
  devprg_storage_ufs_info_raw_print,
  devprg_storage_nand_info_raw_print,
};


void devprg_storage_init(void)
{
  struct devprg_storage *dev;
  int i;

  for(i = 0; i < DEVPRG_MAX_PHY_PARTITIONS; i++)
  {
    dev = &devprg_storage_data[i];

    dev->type = DEVPRG_STORAGE_NULL;
    dev->slot = 0;
    dev->partition = 0;
    dev->handle = NULL;
    dev->in_use = 0;
  }

  for(i = 0; i < DEVPRG_STORAGE_MAX_TYPE; i++)
  {
    dpstorage_init[i]();
  }
}

dp_res_t devprg_storage_deinit(void)
{
  int i;
  struct devprg_storage *dev;

  /* If there are any open handles, close them. */
  for(i = 0; i < DEVPRG_MAX_PHY_PARTITIONS; i++)
  {
    dev = &devprg_storage_data[i];

    if(dev->in_use != 0)
    {
      dp_res_t result;
      result = dpstorage_close[dev->type](dev->handle);

      if(result != DEVPRG_SUCCESS)
        DP_LOGE("Failed to close storage %d slot %d partition %d",
                dev->type, dev->slot, dev->partition);
      else
        dev->in_use = 0;
    }
  }

  for(i = 0; i < DEVPRG_STORAGE_MAX_TYPE; i++)
  {
    dpstorage_deinit[i]();
  }

  return DEVPRG_SUCCESS;
}

struct devprg_storage *devprg_storage_open(enum devprg_storage_type type,
    uint32 slot, uint32 partition, dp_res_t *error)
{
  struct devprg_storage *dev, *avail_dev;
  uint32 start, end;
  int found = 0;

  if(type >= DEVPRG_STORAGE_MAX_TYPE)
  {
    *error = DEVPRG_ERROR_INVAL_PARAM;
    DP_LOGE("Invalid storage type in open %d, error %d", type, *error);
    DP_LOGI("Device slot %d, partition %d", slot, partition);
    return NULL;
  }


  /* Find an unused entry in the array. */
  /* Start at an specified index, we add "type*8" to make different
   * storage types fall under different index. */
  start = (partition + (type * 8)) % DEVPRG_MAX_PHY_PARTITIONS;
  end = start;

  /* Loop through all the handles and find an available space in the array
   * and if the device has been opened before. */
  avail_dev = NULL;
  do
  {
    dev = &devprg_storage_data[start];
    start++;

    /* The device has been open before, use it. */
    if(dev->type == type && dev->in_use == 1 &&
       dev->slot == slot && dev->partition == partition)
    {
      found = 1;
      break;
    }

    if (avail_dev == NULL && dev->in_use == 0)
      avail_dev = dev;

    if(start >= DEVPRG_MAX_PHY_PARTITIONS)
      start = 0;
  }
  while(start != end);

  /* Check if we looped around and didn't find any handles available. */
  if(found == 0 && avail_dev == NULL)
  {
    *error = DEVPRG_ERROR_NOMEM;
    DP_LOGE("Ran out of physical partitions to allocate %d",
            DEVPRG_MAX_PHY_PARTITIONS);
    DP_LOGI("Device type %d, slot %d, partition %d, error %d",
            type, slot, partition, *error);

    return NULL;
  }

  if(found == 0)  /* New handle, open the device.*/
  {
    dev = avail_dev;
    dev->handle = dpstorage_open[type](slot, partition, error);

    if(dev->handle == NULL)
    {
      /* The error value should be set by the open call above. */
      DP_LOGE("Failed to open the device:%d slot:%d partition:%d error:%d", type,
              slot, partition, *error);
      dev = NULL;
    }
    else /* Open successful.  */
    {
      dev->type = type;
      dev->in_use = 1;
      dev->slot = slot;
      dev->partition = partition;
    }
  }
  else /* handle previously opened. */
  {
    /* If the handle is in use, it was previously opened and there is nothing
       to do. */
  }

  return dev;
}

dp_res_t devprg_storage_close(struct devprg_storage *dev)
{
  dp_res_t result = DEVPRG_SUCCESS;
  enum devprg_storage_type type = dev->type;

  /* NAND needs to know when we are done with IO to reset the skip bad
   * block counters. Call close only for NAND as an optimization for
   * the other devices (to call open only once). */
  if (type == DEVPRG_STORAGE_NAND)
  {
    void *handle = dev->handle;

    result = dpstorage_close[type](handle);
    if (result != DEVPRG_SUCCESS)
      DP_LOGE("Close of NAND device failed %d", result);

    /* Mark as free even on error to trigger another open. */
    dev->in_use = 0;
  }

  return result;
}

dp_res_t devprg_storage_io_options(struct devprg_storage *dev,
                                   struct devprg_io_options *io_options)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  return dpstorage_io_options[type](handle, io_options);
}

dp_res_t devprg_storage_read(struct devprg_storage *dev, void *buffer,
                             uint64 start_sector, uint64 num_sectors)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_read[type](handle, buffer, start_sector, num_sectors);
  if(result != DEVPRG_SUCCESS)
    DP_LOGI("Read Failed sector %d, size %d result %d",
            start_sector, num_sectors, result); /* This log won't be sent through USB */

  return result;
}


dp_res_t devprg_storage_write(struct devprg_storage *dev, void *buffer,
                              uint64 start_sector, uint64 num_sectors)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_write[type](handle, buffer, start_sector, num_sectors);
  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Write Failed sector %d, size %d result %d",
            start_sector, num_sectors, result);

  return result;
}

dp_res_t devprg_storage_erase(struct devprg_storage *dev,
                              uint64 start_sector, uint64 num_sectors)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_erase[type](handle, start_sector, num_sectors);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Erase Failed sector %d, size %d", start_sector, num_sectors);

  return result;
}

dp_res_t devprg_storage_format(struct devprg_storage *dev)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_format[type](handle);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Format Failed%s", " ");

  return result;
}

dp_res_t devprg_storage_set_bootable(struct devprg_storage *dev, int enable)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_set_bootable[type](handle, enable);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Set Bootable Failed %d", enable);

  return result;
}

dp_res_t devprg_storage_get_info(struct devprg_storage *dev,
                                 struct devprg_storage_info *dev_info)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_get_info[type](handle, dev_info);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Get Info Failed %d", result);

  return result;
}

dp_res_t devprg_storage_configure(enum devprg_storage_type type, uint32 slot,
                                  struct devprg_storage_cfg_data *config)
{
  dp_res_t result;
  if(type >= DEVPRG_STORAGE_MAX_TYPE)
  {
    DP_LOGE("Invalid storage type in Configure %d, slot %d", type, slot);
    return DEVPRG_ERROR_INVAL_PARAM;
  }
  result = dpstorage_configure[type](slot, config);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Configure Failed slot %d", slot);

  return result;
}

dp_res_t devprg_storage_fw_update(enum devprg_storage_type type, uint32 slot,
                                  void *buffer, uint32 size_in_bytes)
{
  dp_res_t result;
  if(type >= DEVPRG_STORAGE_MAX_TYPE)
  {
    DP_LOGE("Invalid storage type in FW Update %d, slot %d", type, slot);
    return DEVPRG_ERROR_INVAL_PARAM;
  }
  result = dpstorage_fw_update[type](slot, buffer, size_in_bytes);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("FW Update Failed slot %d size bytes %d", slot, size_in_bytes);

  return result;
}

dp_res_t devprg_storage_info_raw_size(struct devprg_storage *dev, uint32 *size)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_info_raw_size[type](handle, size);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Get Info Raw Size Failed %d", *size);

  return result;
}

dp_res_t devprg_storage_info_raw(struct devprg_storage *dev, void *buffer,
                                 uint32 *size)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_info_raw[type](handle, buffer, size);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Get Raw Info Failed %d", *size);

  return result;
}

dp_res_t devprg_storage_info_raw_print(struct devprg_storage *dev,
                                       void *buffer, uint32 *size)
{
  enum devprg_storage_type type = dev->type;
  void *handle = dev->handle;
  dp_res_t result;
  result = dpstorage_info_raw_print[type](handle, buffer, size);

  if(result != DEVPRG_SUCCESS)
    DP_LOGE("Get Raw Info Failed %d", *size);

  return result;
}

