/**************************************************************************
 * FILE: devprg_storage_spinor.c
 *
 * SPI-NOR Device implementation.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_spinor.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2017-02-02   wek   Add io options to change how data is read/write.
2017-01-06   wek   Support the two types of SPI-NOR Flash APIs.
2016-03-09   wek   Reduce the amount of logging on erase.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2016-01-07   wek   Get FW version from get info.
2015-11-19   wek   Create.

===========================================================================*/

#include "devprg_storage.h"
#include "devprg_storage_spinor.h"
#include "devprg_log.h"

/* If SPI NOR V1 APIs Support is enabled. */
#ifdef FEATURE_DEVPRG_SPINOR_V1


#include "flash.h"
#include <string.h>
#include "stringl/stringl.h"

struct dp_storage_spinor
{
  flash_handle_t handle;
  uint32 pages_per_block;
  char in_use;
};

/* Simulated sector size for SPI-NOR. The sector size in SPI-NOR is
 * really 256bytes, the erase block size is 4K. To simplify the logic
 * we make the sector size as 4k, that way we don't have to re-compute
 * things on the PC based on a different sector size depending on the
 * device. */
#define DEVPRG_SPINOR_SIM_SECTOR_SIZE   4096

static struct dp_storage_spinor dp_spinor_data;

static dp_res_t translate_flash_error_simple(FLASH_STATUS error)
{
  dp_res_t e;

  switch(error)
  {
  case FLASH_DEVICE_DONE:
    e = DEVPRG_SUCCESS;
    break;
  case FLASH_DEVICE_NOT_SUPPORTED:
    e = DEVPRG_ERROR_NOT_SUPPORTED;
    break;
  case FLASH_DEVICE_INVALID_PARAMETER:
    e = DEVPRG_ERROR_INVAL_PARAM;
    break;
  case FLASH_DEVICE_FAIL:
  default:
    e = DEVPRG_ERROR_STORAGE;
    break;
  }

  if(e != DEVPRG_SUCCESS)
  {
    DP_LOGE("SPINOR Error %d (%d)", error, e);
  }

  return e;
}

static dp_res_t translate_flash_error(FLASH_STATUS error)
{
  dp_res_t e;

  e = translate_flash_error_simple(error);

  if(e != DEVPRG_SUCCESS)
  {
    DP_LOGE("SPINOR Error %d (%d)", error, e);
  }

  return e;
}

void devprg_storage_spinor_init(void)
{
  dp_spinor_data.handle = NULL;
  dp_spinor_data.in_use = 0;
}

dp_res_t devprg_storage_spinor_deinit(void)
{
  return DEVPRG_SUCCESS;
}

void *devprg_storage_spinor_open(uint32 slot, uint32 partition, dp_res_t *error)
{
  struct dp_storage_spinor *dev = &dp_spinor_data;
  flash_handle_t handle;
  FLASH_STATUS result;
  struct flash_info info;
  uint32 erase_block_size;

  if(slot != 0)
  {
    DP_LOGE("Flash API does not support more than one device %d", slot);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  if(partition != 0)
  {
    DP_LOGE("Flash API does not support multiple physical partitions %d",
            partition);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  /* Device has already been open, return it. */
  if(dev->in_use == 1)
  {
    DP_LOGD("SPINOR was previously open, returning existing handle %d", 0);
    return dev;
  }

  result = flash_device_open(0, &handle);

  if(result != FLASH_DEVICE_DONE)
  {
    *error = translate_flash_error(result);
    DP_LOGE("Failed to open the SPI NOR Device %d (%d)", result, *error);
    return NULL;
  }

  result = flash_get_info(handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    flash_device_close(handle);
    *error = translate_flash_error(result);
    DP_LOGE("Failed to get info the SPI NOR Device %d (%d)", result, *error);
    return NULL;
  }

  erase_block_size = info.page_size_bytes * info.pages_per_block;

  if(erase_block_size != DEVPRG_SPINOR_SIM_SECTOR_SIZE)
  {
    flash_device_close(handle);
    DP_LOGE("Can't support storage part with erase block size of %d != %d",
            erase_block_size, DEVPRG_SPINOR_SIM_SECTOR_SIZE);
    DP_LOGI("page_size=%d, pages_per_block=%d, block_count=%d, maker_id=%d, device_id=%d",
            info.page_size_bytes, info.pages_per_block, info.block_count,
            info.maker_id, info.device_id);
    return NULL;
  }

  dev->pages_per_block = info.pages_per_block;
  dev->handle = handle;
  dev->in_use = 1;

  return dev;
}

dp_res_t devprg_storage_spinor_close(void *device)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;

  result = flash_device_close(dev->handle);

  if(result == FLASH_DEVICE_DONE)
  {
    dev->in_use = 0;
    dev->handle = NULL;
  }

  return translate_flash_error(result);
}

dp_res_t devprg_storage_spinor_io_options(void *device,
                                          struct devprg_io_options *io_options)
{
  int result = DEVPRG_SUCCESS;
  (void) device; /* SPINOR only supports default */
  if (io_options->skip_bb != BAD_BLOCK_SKIP)
  {
    DP_LOGE("SPINOR Does not support bad block options %d", io_options->skip_bb);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->get_spare != DATA_ONLY)
  {
    DP_LOGE("SPINOR Does not support get spare options %d", io_options->get_spare);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->ecc_state != ECC_ENABLED)
  {
    DP_LOGE("SPINOR Does not support ECC options %d", io_options->ecc_state);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }

  return result;
}

dp_res_t devprg_storage_spinor_read(void *device, void *buffer,
                                    uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  uint32 offset, count;

  offset = start_sector * dev->pages_per_block;
  count = num_sectors * dev->pages_per_block;

  result = flash_read_pages(dev->handle, buffer, offset, count);

  return translate_flash_error_simple(result);

}

/* Check if the data inside the buffer is erased, AKA filled with 0xFF.
 *
 * return 1 When filled with 0xFF, 0 otherwise. */
static int check_erase(void *buffer_in, uint32 size)
{
  uint32 i;
  uint8 *buf = (uint8 *)buffer_in;

  for(i = 0; i < size; i++)
  {
    if(buf[i] != 0xFF)
      return 0;
  }
  return 1;
}

/* Buffer used to read data and check all bytes are 0xFF. */
uint8 erase_check_buffer[DEVPRG_SPINOR_SIM_SECTOR_SIZE];
/* Check if a chunk is erased, if it isn't, erases it. */
static FLASH_STATUS spinor_check_erase(struct dp_storage_spinor *dev,
                                       uint32 start, uint32 count)
{
  FLASH_STATUS result = FLASH_DEVICE_DONE;
  uint32 page, read_chunk;
  uint32 end_page;

  if (count == 0)
    return FLASH_DEVICE_DONE;

  page = start * dev->pages_per_block;
  end_page = page + count * dev->pages_per_block;

  read_chunk = dev->pages_per_block;

  while(page < end_page)
  {
    result = flash_read_pages(dev->handle, erase_check_buffer, page, read_chunk);
    if(result != FLASH_DEVICE_DONE)
    {
      DP_LOGD("SPINOR Can't read page %d chunk %d", page, read_chunk);
      break;
    }

    /* Check if the buffer is filled with 0xFF, if it is not, needs erase. */
    if(check_erase(erase_check_buffer, read_chunk) == 0)
    {
      /* Erase the whole thing, it may be faster if chunk is large.
       * and flash driver supports erasing large chunks. */
      result = flash_erase_blocks(dev->handle, start, count);
      break;
    }
    page += read_chunk;
  }

  return result;
}

#define DEVPRG_ERASE_BLOCK_COUNT   128
/* Checks if the sectors are erased, if they are not, erase them. */
static dp_res_t devprg_storage_spinor_erase_log(void *device, uint64 start_sector,
                                                uint64 num_sectors, int log)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result = FLASH_DEVICE_DONE;
  dp_res_t error;
  uint32 blk, blk_count, erase_size, total_count;

  blk = start_sector;
  total_count = blk_count = num_sectors;
  erase_size = DEVPRG_ERASE_BLOCK_COUNT;
  /* If the start block is not aligned, then the first erase will
   * align the start sector. */
  if(blk % DEVPRG_ERASE_BLOCK_COUNT != 0)
    erase_size = DEVPRG_ERASE_BLOCK_COUNT - (blk % DEVPRG_ERASE_BLOCK_COUNT);

  if(log)
    DP_LOGD("SPINOR Erasing sector %d Num blocks %d", blk, blk_count);

  while (blk_count >= erase_size && result == FLASH_DEVICE_DONE)
  {
    result = spinor_check_erase(dev, blk, erase_size);
    blk += erase_size;
    blk_count -= erase_size;
    if(log)
      DP_LOGI("SPINOR Erase progress %d%% (%d of %d)",
              100 - (blk_count * 100) / total_count,
              total_count - blk_count, total_count);
    erase_size = DEVPRG_ERASE_BLOCK_COUNT;
  }

  if(blk_count < erase_size && result == FLASH_DEVICE_DONE)
  {
    result = spinor_check_erase(dev, blk, blk_count);
    blk_count -= blk_count;
    if(log)
      DP_LOGI("SPINOR Erase progress %d%% (%d of %d).",
              100 - (blk_count * 100) / total_count,
              total_count - blk_count, total_count);
  }


  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}


dp_res_t devprg_storage_spinor_write(void *device, void *buffer,
                                     uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  uint32 offset, count;
  dp_res_t error;

  error = devprg_storage_spinor_erase_log(device, start_sector,
                                          num_sectors, 0);
  if (error != DEVPRG_SUCCESS)
    return error;

  offset = start_sector * dev->pages_per_block;
  count = num_sectors * dev->pages_per_block;

  result = flash_write_pages(dev->handle, buffer, offset, count);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_erase(void *device, uint64 start_sector,
                                     uint64 num_sectors)
{
  return devprg_storage_spinor_erase_log(device, start_sector, num_sectors, 1);
}

dp_res_t devprg_storage_spinor_format(void *device)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  struct flash_info info;
  dp_res_t error;

  result = flash_get_info(dev->handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    DP_LOGE("Get Info Failed in format %s", " ");
    return error;
  }

  DP_LOGD("SPINOR Erasing whole disk of %d blocks", info.block_count);
  error = devprg_storage_spinor_erase(device, 0, info.block_count);

  return error;
}


dp_res_t devprg_storage_spinor_set_bootable(void *device, int enable)
{
  (void) device;
  (void) enable;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_get_info(void *device,
                                        struct devprg_storage_info *dev_info)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  struct flash_info info;
  dp_res_t error;

  result = flash_get_info(dev->handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  dev_info->page_size       = info.page_size_bytes;
  dev_info->block_size      = info.pages_per_block * info.page_size_bytes;
  dev_info->total_blocks    = info.block_count;
  dev_info->manufacturer_id = info.maker_id;
  dev_info->serial_num      = info.device_id;
  dev_info->num_physical    = 0;
  dev_info->fw_version[0]   = 0;
  strlcpy(dev_info->memory_type, "NOR", sizeof(dev_info->memory_type));
  dev_info->product_name[0] = 0;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_configure(uint32 slot,
                                         struct devprg_storage_cfg_data *config)
{
  (void) slot;
  (void) config;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_fw_update(uint32 slot, void *buffer,
                                         uint32 size_in_bytes)
{
  (void) slot;
  (void) buffer;
  (void) size_in_bytes;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  *size = 0;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_info_raw(void *device, void *buffer,
                                        uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_info_raw_print(void *device, void *buffer,
                                              uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_SUCCESS;
}

#elif FEATURE_DEVPRG_SPINOR_V2


#include "flash.h"
#include "flash_image_id.h"
#include "flash_init_config.h"
#include <stringl/stringl.h>

struct dp_storage_spinor
{
  flash_handle_t handle;
  char in_use;
};

/* Simulated sector size for SPI-NOR. The sector size in SPI-NOR is
 * really 256bytes, the erase block size is 4K. To simplify the logic
 * we make the sector size as 4k, that way we don't have to re-compute
 * things on the PC based on a different sector size depending on the
 * device. */
#define DEVPRG_SPINOR_SIM_SECTOR_SIZE   4096

static struct dp_storage_spinor dp_spinor_data;

static dp_res_t translate_flash_error_simple(FLASH_STATUS error)
{
  dp_res_t e;

  switch(error)
  {
  case FLASH_DEVICE_DONE:
    e = DEVPRG_SUCCESS;
    break;
  case FLASH_DEVICE_NOT_SUPPORTED:
    e = DEVPRG_ERROR_NOT_SUPPORTED;
    break;
  case FLASH_DEVICE_INVALID_PARAMETER:
    e = DEVPRG_ERROR_INVAL_PARAM;
    break;
  case FLASH_DEVICE_FAIL:
  case FLASH_DEVICE_IMAGE_NOT_FOUND:
  default:
    e = DEVPRG_ERROR_STORAGE;
    break;
  }

  if(e != DEVPRG_SUCCESS)
  {
    DP_LOGE("SPINOR Error %d (%d)", error, e);
  }

  return e;
}

static dp_res_t translate_flash_error(FLASH_STATUS error)
{
  dp_res_t e;

  e = translate_flash_error_simple(error);

  if(e != DEVPRG_SUCCESS)
  {
    DP_LOGE("SPINOR Error %d (%d)", error, e);
  }

  return e;
}

void devprg_storage_spinor_init(void)
{
  dp_spinor_data.handle = NULL;
  dp_spinor_data.in_use = 0;
  flash_driver_config_init();
}

dp_res_t devprg_storage_spinor_deinit(void)
{
  FLASH_STATUS result;
  result = flash_deinit();
  return translate_flash_error(result);
}

void *devprg_storage_spinor_open(uint32 slot, uint32 partition, dp_res_t *error)
{
  struct dp_storage_spinor *dev = &dp_spinor_data;
  flash_handle_t handle;
  FLASH_STATUS result;
  struct flash_info info;
  uint32 erase_block_size;

  if(slot != 0)
  {
    DP_LOGE("Flash API does not support more than one device %d", slot);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  if(partition != 0)
  {
    DP_LOGE("Flash API does not support multiple physical partitions %d",
            partition);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  /* Device has already been open, return it. */
  if(dev->in_use == 1)
  {
    DP_LOGD("SPINOR was previously open, returning existing handle %d", 0);
    return dev;
  }

  result = flash_open(FLASH_ALL, &handle);

  if(result != FLASH_DEVICE_DONE)
  {
    *error = translate_flash_error(result);
    DP_LOGE("Failed to open the SPI NOR Device %d (%d)", result, *error);
    return NULL;
  }

  result = flash_get_info(handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    flash_close(handle);
    *error = translate_flash_error(result);
    DP_LOGE("Failed to get info the SPI NOR Device %d (%d)", result, *error);
    return NULL;
  }

  erase_block_size = info.page_size_bytes * info.pages_per_block;

  if(erase_block_size != DEVPRG_SPINOR_SIM_SECTOR_SIZE)
  {
    flash_close(handle);
    DP_LOGE("Can't support storage part with erase block size of %d != %d",
            erase_block_size, DEVPRG_SPINOR_SIM_SECTOR_SIZE);
    DP_LOGI("page_size=%d, pages_per_block=%d, block_count=%d, maker_id=%d, device_id=%d",
            info.page_size_bytes, info.pages_per_block, info.block_count,
            info.maker_id, info.device_id);
    return NULL;
  }

  dev->handle = handle;
  dev->in_use = 1;

  return dev;
}

dp_res_t devprg_storage_spinor_close(void *device)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;

  result = flash_close(dev->handle);

  if(result == FLASH_DEVICE_DONE)
  {
    dev->in_use = 0;
    dev->handle = NULL;
  }

  return translate_flash_error(result);
}

dp_res_t devprg_storage_spinor_io_options(void *device,
                                          struct devprg_io_options *io_options)
{
  int result = DEVPRG_SUCCESS;
  (void) device; /* SPINOR only supports default */
  if (io_options->skip_bb != BAD_BLOCK_SKIP)
  {
    DP_LOGE("SPINOR Does not support bad block options %d", io_options->skip_bb);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->get_spare != DATA_ONLY)
  {
    DP_LOGE("SPINOR Does not support get spare options %d", io_options->get_spare);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->ecc_state != ECC_ENABLED)
  {
    DP_LOGE("SPINOR Does not support ECC options %d", io_options->ecc_state);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }

  return result;
}


dp_res_t devprg_storage_spinor_read(void *device, void *buffer,
                                    uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  uint32 offset, count;

  offset = start_sector * DEVPRG_SPINOR_SIM_SECTOR_SIZE;
  count = num_sectors * DEVPRG_SPINOR_SIM_SECTOR_SIZE;

  result = flash_read(dev->handle, offset, count, buffer);

  return translate_flash_error_simple(result);

}

/* Check if the data inside the buffer is erased, AKA filled with 0xFF.
 *
 * return 1 When filled with 0xFF, 0 otherwise. */
static int check_erase(void *buffer_in, uint32 size)
{
  uint32 i;
  uint8 *buf = (uint8 *)buffer_in;

  for(i = 0; i < size; i++)
  {
    if(buf[i] != 0xFF)
      return 0;
  }
  return 1;
}

/* Buffer used to read data and check all bytes are 0xFF. */
uint8 erase_check_buffer[DEVPRG_SPINOR_SIM_SECTOR_SIZE];
/* Check if a chunk is erased, if it isn't, erases it. */
static FLASH_STATUS spinor_check_erase(struct dp_storage_spinor *dev,
                                       uint32 start, uint32 count)
{
  FLASH_STATUS result = FLASH_DEVICE_DONE;
  uint32 offset, read_chunk;
  uint32 end_offset;

  if (count == 0)
    return FLASH_DEVICE_DONE;

  offset = start * DEVPRG_SPINOR_SIM_SECTOR_SIZE;
  end_offset = offset + count * DEVPRG_SPINOR_SIM_SECTOR_SIZE;

  read_chunk = sizeof(erase_check_buffer);

  while(offset < end_offset)
  {
    result = flash_read(dev->handle, offset, read_chunk, erase_check_buffer);
    if(result != FLASH_DEVICE_DONE)
    {
      DP_LOGD("SPINOR Can't read offset %d chunk %d", offset, read_chunk);
      break;
    }

    /* Check if the buffer is filled with 0xFF, if it is not, needs erase. */
    if(check_erase(erase_check_buffer, read_chunk) == 0)
    {
      /* Erase the whole thing, it may be faster if chunk is large.
       * and flash driver supports erasing large chunks. */
      result = flash_erase(dev->handle, start, count);
      if(result != FLASH_DEVICE_DONE)
        DP_LOGD("SPINOR Can't Erase offset %d chunk %d", offset, read_chunk);

      break;
    }
    offset += read_chunk;
  }

  return result;
}

#define DEVPRG_ERASE_BLOCK_COUNT   16
/* Checks if the sectors are erased, if they are not, erase them. */
static dp_res_t devprg_storage_spinor_erase_log(void *device, uint64 start_sector,
                                                uint64 num_sectors, int log)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result = FLASH_DEVICE_DONE;
  dp_res_t error;
  uint32 blk, blk_count, erase_size, total_count;

  blk = start_sector;
  total_count = blk_count = num_sectors;
  erase_size = DEVPRG_ERASE_BLOCK_COUNT;
  /* If the start block is not aligned, then the first erase will
   * align the start sector. */
  if(blk % DEVPRG_ERASE_BLOCK_COUNT != 0)
    erase_size = DEVPRG_ERASE_BLOCK_COUNT - (blk % DEVPRG_ERASE_BLOCK_COUNT);

  if(log)
    DP_LOGD("SPINOR Erasing sector %d Num blocks %d", blk, blk_count);

  while (blk_count >= erase_size && result == FLASH_DEVICE_DONE)
  {
    result = spinor_check_erase(dev, blk, erase_size);
    blk += erase_size;
    blk_count -= erase_size;
    if(log)
      DP_LOGI("SPINOR Erase progress %d%% (%d of %d)",
              100 - (blk_count * 100) / total_count,
              total_count - blk_count, total_count);
    erase_size = DEVPRG_ERASE_BLOCK_COUNT;
  }

  if(blk_count < erase_size && result == FLASH_DEVICE_DONE)
  {
    result = spinor_check_erase(dev, blk, blk_count);
    blk_count -= blk_count;
    if(log)
      DP_LOGI("SPINOR Erase progress %d%% (%d of %d).",
              100 - (blk_count * 100) / total_count,
              total_count - blk_count, total_count);
  }


  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}


dp_res_t devprg_storage_spinor_write(void *device, void *buffer,
                                     uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  uint32 offset, count;
  dp_res_t error;

  error = devprg_storage_spinor_erase_log(device, start_sector,
                                          num_sectors, 0);
  if (error != DEVPRG_SUCCESS)
    return error;

  offset = start_sector * DEVPRG_SPINOR_SIM_SECTOR_SIZE;
  count = num_sectors * DEVPRG_SPINOR_SIM_SECTOR_SIZE;

  result = flash_write(dev->handle, offset, count, buffer);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_erase(void *device, uint64 start_sector,
                                     uint64 num_sectors)
{
  return devprg_storage_spinor_erase_log(device, start_sector, num_sectors, 1);
}

dp_res_t devprg_storage_spinor_format(void *device)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  struct flash_info info;
  int error;

  result = flash_get_info(dev->handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    DP_LOGE("Get Info Failed in format %s", " ");
    return error;
  }

  DP_LOGD("SPINOR Erasing whole disk of %d blocks", info.block_count);
  error = devprg_storage_spinor_erase(device, 0, info.block_count);

  return error;
}


dp_res_t devprg_storage_spinor_set_bootable(void *device, int enable)
{
  (void) device;
  (void) enable;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_get_info(void *device,
                                        struct devprg_storage_info *dev_info)
{
  struct dp_storage_spinor *dev = (struct dp_storage_spinor *)device;
  FLASH_STATUS result;
  struct flash_info info;
  dp_res_t error;

  result = flash_get_info(dev->handle, &info);

  if(result != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(result);
    return error;
  }

  dev_info->page_size       = info.page_size_bytes;
  dev_info->block_size      = info.pages_per_block * info.page_size_bytes;
  dev_info->total_blocks    = info.block_count;
  dev_info->manufacturer_id = info.maker_id;
  dev_info->serial_num      = info.device_id;
  dev_info->num_physical    = 0;
  dev_info->fw_version[0]   = 0;
  strlcpy(dev_info->memory_type, "NOR", sizeof(dev_info->memory_type));
  dev_info->product_name[0] = 0;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_configure(uint32 slot,
                                         struct devprg_storage_cfg_data *config)
{
  (void) slot;
  (void) config;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_fw_update(uint32 slot, void *buffer,
                                         uint32 size_in_bytes)
{
  (void) slot;
  (void) buffer;
  (void) size_in_bytes;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  *size = 0;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_spinor_info_raw(void *device, void *buffer,
                                        uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_spinor_info_raw_print(void *device, void *buffer,
                                              uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_SUCCESS;
}
#else /* If SPI NOR Support is disabled (FEATURE_DEVPRG_SPINOR not defined)*/

#include "devprg_storage_stubs.h"
void devprg_storage_spinor_init(void)
{
  devprg_storage_stub_init();
}

dp_res_t devprg_storage_spinor_deinit(void)
{
  return devprg_storage_stub_deinit();
}

void *devprg_storage_spinor_open(uint32 slot, uint32 partition,
                                 dp_res_t *error)
{
  return devprg_storage_stub_open(slot, partition, error);
}

dp_res_t devprg_storage_spinor_io_options(void *device,
                                          struct devprg_io_options *io_options)
{
  return devprg_storage_stub_io_options(device, io_options);
}

dp_res_t devprg_storage_spinor_close(void *device)
{
  return devprg_storage_stub_close(device);
}

dp_res_t devprg_storage_spinor_read(void *device, void *buffer,
                                    uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_read(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_spinor_write(void *device, void *buffer,
                                     uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_write(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_spinor_erase(void *device,
                                     uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_erase(device, start_sector, num_sectors);
}

dp_res_t devprg_storage_spinor_format(void *device)
{
  return devprg_storage_stub_format(device);
}

dp_res_t devprg_storage_spinor_set_bootable(void *device, int enable)
{
  return devprg_storage_stub_set_bootable(device, enable);
}

dp_res_t devprg_storage_spinor_get_info(void *device,
                                        struct devprg_storage_info *dev_info)
{
  return devprg_storage_stub_get_info(device, dev_info);
}

dp_res_t devprg_storage_spinor_configure(uint32 slot,
                                         struct devprg_storage_cfg_data *config)
{
  return devprg_storage_stub_configure(slot, config);
}

dp_res_t devprg_storage_spinor_fw_update(uint32 slot, void *buffer,
                                         uint32 size_in_bytes)
{
  return devprg_storage_stub_fw_update(slot, buffer, size_in_bytes);
}

dp_res_t devprg_storage_spinor_info_raw_size(void *device, uint32 *size)
{
  return devprg_storage_stub_info_raw_size(device, size);
}

dp_res_t devprg_storage_spinor_info_raw(void *device, void *buffer,
                                        uint32 *size)
{
  return devprg_storage_stub_info_raw(device, buffer, size);
}
dp_res_t devprg_storage_spinor_info_raw_print(void *device, void *buffer,
                                              uint32 *size)
{
  return devprg_storage_stub_info_raw_print(device, buffer, size);
}


#endif /*  FEATURE_DEVPRG_SPINOR */
