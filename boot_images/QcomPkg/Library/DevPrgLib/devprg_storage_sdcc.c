/**************************************************************************
 * FILE: devprg_storage_sdcc.c
 *
 * SDCC device interface.
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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_sdcc.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-04-20   wek   Include the start and end sectors in set io options.
2017-02-02   wek   Add io options to change how data is read/write.
2016-02-10   wek   Add the option to format whole dev or erase a portion.
2016-01-07   wek   Get FW version from get info.
2015-11-19   wek   Create.

===========================================================================*/

#ifdef FEATURE_DEVPRG_SDCC

#include "devprg_storage.h"
#include "devprg_storage_sdcc.h"
#include "devprg_log.h"
#include "sdcc_api.h"
#include <stdio.h> /* for snprintf in fw_version. */
#include <string.h>
#include "stringl/stringl.h"

#define SDCC_USER_PARTITION 0

struct dp_storage_sdcc
{
  struct sdcc_device *handle;
  char in_use;
};

#ifndef DEVPRG_MAX_SDCC_PHY_PARTITIONS
#define DEVPRG_MAX_SDCC_PHY_PARTITIONS   8
#endif

/* Maximum number of SDCC slots. */
#define DEVPRG_MAX_SDCC_SLOTS   4

static struct dp_storage_sdcc dp_sdcc_data[DEVPRG_MAX_SDCC_PHY_PARTITIONS];

static dp_res_t translate_sdcc_error_simple(SDCC_STATUS error)
{
  dp_res_t e;

  switch(error)
  {
  case SDCC_NO_ERROR:
    e = DEVPRG_SUCCESS;
    break;
  case SDCC_ERR_OP_NOT_SUPPORTED:
    e = DEVPRG_ERROR_NOT_SUPPORTED;
    break;
  case SDCC_ERR_INVALID_PARAM:
    e = DEVPRG_ERROR_INVAL_PARAM;
    break;
  case SDCC_ERR_UNKNOWN:
  case SDCC_ERR_CMD_TIMEOUT:
  case SDCC_ERR_DATA_TIMEOUT:
  case SDCC_ERR_CMD_CRC_FAIL:
  case SDCC_ERR_DATA_CRC_FAIL:
  case SDCC_ERR_CMD_SENT:
  case SDCC_ERR_PROG_DONE:
  case SDCC_ERR_CARD_READY:
  case SDCC_ERR_INVALID_TX_STATE:
  case SDCC_ERR_SET_BLKSZ:
  case SDCC_ERR_SDIO_R5_RESP:
  case SDCC_ERR_DMA:
  case SDCC_ERR_READ_FIFO:
  case SDCC_ERR_WRITE_FIFO:
  case SDCC_ERR_ERASE:
  case SDCC_ERR_SDIO:
  case SDCC_ERR_SDIO_READ:
  case SDCC_ERR_SDIO_WRITE:
  case SDCC_ERR_SWITCH:
  case SDCC_ERR_CARD_UNDETECTED:
  case SDCC_ERR_FEATURE_UNSUPPORTED:
  case SDCC_ERR_SECURE_COMMAND_IN_PROGRESS:
  case SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED:
  case SDCC_ERR_ABORT_READ_SEC_CMD:
  case SDCC_ERR_CARD_INIT:
  case SDCC_ERR_CARD_REMOVED:
  case SDCC_ERR_PWR_ON_WRITE_PROT:
  case SDCC_ERR_WP_VIOLATION:
  case SDCC_ERR_SPS_MODE:
  case SDCC_ERR_RPMB_RESPONSE:
  case SDCC_ERR_RPMB_RESULT:
  case SDCC_ERR_RPMB_NONCE:
  case SDCC_ERR_FW_GENERAL_ERROR:
  case SDCC_ERR_FW_INSTALL:
  case SDCC_ERR_FW_DOWNLOAD:
  case SDCC_ERR_CARD_IN_USE:
  default:
    e = DEVPRG_ERROR_STORAGE;
    break;
  }

  return e;
}

static dp_res_t translate_sdcc_error(SDCC_STATUS error)
{
  dp_res_t e;

  e = translate_sdcc_error_simple(error);

  if(e != DEVPRG_SUCCESS)
  {
    DP_LOGE("SDCC Error %d (%d)", error, e);
  }

  return e;
}

void devprg_storage_sdcc_init(void)
{
  int i;

  for(i = 0; i < DEVPRG_MAX_SDCC_PHY_PARTITIONS; i++)
  {
    dp_sdcc_data[i].handle = NULL;
    dp_sdcc_data[i].in_use = 0;
  }
}

dp_res_t devprg_storage_sdcc_deinit(void)
{
  return DEVPRG_SUCCESS;
}

void *devprg_storage_sdcc_open(uint32 slot, uint32 partition, dp_res_t *error)
{
  struct dp_storage_sdcc *dev;
  struct sdcc_device *handle;
  int i;

  if(slot > DEVPRG_MAX_SDCC_SLOTS)
  {
    DP_LOGE("Code supports maximum of %d slots, tried to open slot %d",
            DEVPRG_MAX_SDCC_SLOTS, slot);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  /* find an empty handle. */
  dev = NULL;

  for(i = 0; i < DEVPRG_MAX_SDCC_PHY_PARTITIONS; i++)
  {
    if(dp_sdcc_data[i].in_use == 0)
    {
      dev = &dp_sdcc_data[i];
      break;
    }
  }

  if(dev == NULL)
  {
    *error = DEVPRG_ERROR_NOMEM;
    DP_LOGE("Ran out of physical partitions to allocate %d",
            DEVPRG_MAX_SDCC_PHY_PARTITIONS);
    DP_LOGI("Device type sdcc slot %d, partition %d, error %d",
            slot, partition, *error);
    return NULL;
  }

  if(partition == DEVPRG_WHOLE_DEVICE_PARTITION)
    partition = SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE;

  handle = sdcc_handle_open(slot, partition);

  if(handle == NULL)
  {
    DP_LOGE("Failed to open the SDCC Device slot %d partition %d",
            slot, partition);
    return NULL;
  }

  dev->handle = handle;
  dev->in_use = 1;

  return dev;
}

dp_res_t devprg_storage_sdcc_close(void *device)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;

  result = sdcc_handle_close(dev->handle);

  if(result == SDCC_NO_ERROR)
  {
    dev->in_use = 0;
    dev->handle = NULL;
  }

  return translate_sdcc_error(result);
}

dp_res_t devprg_storage_sdcc_io_options(void *device,
                                        struct devprg_io_options *io_options)
{
  int result = DEVPRG_SUCCESS;
  (void) device;
  if (io_options->skip_bb != BAD_BLOCK_SKIP)
  {
    DP_LOGE("SDCC Does not support bad block options %d", io_options->skip_bb);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->get_spare != DATA_ONLY)
  {
    DP_LOGE("SDCC Does not support get spare options %d", io_options->get_spare);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }
  if (io_options->ecc_state != ECC_ENABLED)
  {
    DP_LOGE("SDCC Does not support ECC options %d", io_options->ecc_state);
    result = DEVPRG_ERROR_INVAL_PARAM;
  }

  return result;
}

dp_res_t devprg_storage_sdcc_read(void *device, void *buffer,
                                  uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result = 0;

  result = sdcc_handle_read(dev->handle, start_sector, buffer, num_sectors);

  return translate_sdcc_error_simple(result);

}

dp_res_t devprg_storage_sdcc_write(void *device, void *buffer,
                                   uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  dp_res_t error;

  result = sdcc_handle_write(dev->handle, start_sector, buffer, num_sectors);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_sdcc_erase(void *device,
                                   uint64 start_sector, uint64 num_sectors)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  dp_res_t error;

  result = sdcc_handle_erase(dev->handle, start_sector, num_sectors);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_sdcc_format(void *device)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  sdcc_mem_info_type info;
  dp_res_t error;

  result = sdcc_handle_mem_get_device_info(dev->handle, &info);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Get Info Failed in format %s", " ");
    return error;
  }

  DP_LOGD("sdcc Erasing whole disk of %d blocks", info.card_size_in_sectors);
  result = sdcc_handle_erase(dev->handle, 0, info.card_size_in_sectors);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_sdcc_set_bootable(void *device, int enable)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  dp_res_t error;
  (void) enable;

  result = sdcc_handle_set_active_bootable_partition(dev->handle);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_sdcc_get_info(void *device,
                                      struct devprg_storage_info *dev_info)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  dp_res_t error;
  sdcc_mem_info_type info;
  uint64 fw_ver = 0;
  int res;

  result = sdcc_handle_mem_get_device_info(dev->handle, &info);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  dev_info->page_size       = info.block_len;
  dev_info->block_size      = info.block_len;
  dev_info->total_blocks    = info.card_size_in_sectors;
  dev_info->manufacturer_id = info.mfr_id;
  dev_info->serial_num      = info.prod_serial_num;
  dev_info->num_physical    = info.num_phy_partition_created;

  result = sdcc_handle_get_fw_version(dev->handle, &fw_ver);
  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Get FW Version failed %ld", fw_ver);
    return error;
  }

  res = snprintf(dev_info->fw_version, sizeof(dev_info->fw_version),
                 "%llu", ((unsigned long long int)1) * fw_ver);
  if(res < 0 || (size_t)res > sizeof(dev_info->fw_version))
  {
    DP_LOGE("FW Version truncated %d,%d", res, sizeof(dev_info->fw_version));
    return DEVPRG_ERROR_NOMEM;
  }
  strlcpy(dev_info->memory_type, "eMMC", sizeof(dev_info->memory_type));
  strlcpy(dev_info->product_name, (char *)info.prod_name, sizeof(dev_info->product_name));

  return DEVPRG_SUCCESS;
}

static void
devprg_sdcc_copy_config (struct sdcc_emmc_gpp_enh_desc *emmc_config,
                         struct devprg_sdcc_emmc_gpp_enh_desc *configure)
{
  emmc_config->GPP_size[0] = configure->GPP_size[0];
  emmc_config->GPP_size[1] = configure->GPP_size[1];
  emmc_config->GPP_size[2] = configure->GPP_size[2];
  emmc_config->GPP_size[3] = configure->GPP_size[3];
  emmc_config->ENH_size = configure->ENH_size;
  emmc_config->ENH_start_addr = configure->ENH_start_addr;
  emmc_config->GPP_enh_flag = configure->GPP_enh_flag;
}

dp_res_t devprg_storage_sdcc_configure(uint32 slot,
                                       struct devprg_storage_cfg_data *configure)
{
  SDCC_STATUS result;
  dp_res_t error = DEVPRG_SUCCESS;
  struct sdcc_device *dev;
  struct sdcc_emmc_gpp_enh_desc emmc_config;

  devprg_sdcc_copy_config(&emmc_config, &configure->u.emmc_config);

  DP_LOGD("In devprg_storage_sdcc_configure(%d,x)", slot);

  DP_LOGD("DRIVE4_SIZE_IN_KB %d", emmc_config.GPP_size[0]);
  DP_LOGD("DRIVE5_SIZE_IN_KB %d", emmc_config.GPP_size[1]);
  DP_LOGD("DRIVE6_SIZE_IN_KB %d", emmc_config.GPP_size[2]);
  DP_LOGD("DRIVE7_SIZE_IN_KB %d", emmc_config.GPP_size[3]);
  DP_LOGD("ENH_SIZE %d", emmc_config.ENH_size);
  DP_LOGD("ENH_START_ADDR %d", emmc_config.ENH_start_addr);
  DP_LOGD("GPP_ENH_FLAG %d", emmc_config.GPP_enh_flag);

  // These values are in KB right now. Therefore Y * 1024/512 becomes Y * 2 in terms of sectors
  emmc_config.GPP_size[0] *= 2;
  emmc_config.GPP_size[1] *= 2;
  emmc_config.GPP_size[2] *= 2;
  emmc_config.GPP_size[3] *= 2;

  dev = sdcc_handle_open(slot, SDCC_USER_PARTITION);

  if(dev == NULL)
  {
    DP_LOGE("Open Failed for slot %d", slot);
    return DEVPRG_ERROR_STORAGE;
  }

  result = sdcc_config_emmc_gpp_enh(dev, &emmc_config);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Configure Failed slot %d", slot);
  }
  else
  {
    DP_LOGI("Successfully configured SDCC device %d", 0);
  }

  result = sdcc_handle_close(dev);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Close Failed slot %d", slot);
  }

  return error;
}

dp_res_t devprg_storage_sdcc_fw_update(uint32 slot, void *buffer,
                                       uint32 size_in_bytes)
{
  struct sdcc_device *dev;
  SDCC_STATUS result;
  int status;
  int failed = 0;
  uint64 fw_ver_before = 0;
  uint64 fw_ver_after = 0;
  int num_sectors;
  sdcc_mem_info_type sdcc_info;
  dp_res_t error = DEVPRG_SUCCESS;


  dev = sdcc_handle_open(slot, SDCC_USER_PARTITION);

  if(dev == NULL)
  {
    DP_LOGE("Open Failed FW Update slot %d", slot);
    return DEVPRG_ERROR_STORAGE;
  }

  result = sdcc_handle_get_fw_version(dev, &fw_ver_before);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Get FW Version failed slot", slot);
    goto close_dev;
  }

  result = sdcc_handle_mem_get_device_info(dev, &sdcc_info);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Get Info Failed slot %d", slot);
    goto close_dev;
  }

  /* The SDCC API size argument is in sectors, Validate the size of the FW. */
  if(size_in_bytes % sdcc_info.block_len != 0)
  {
    DP_LOGE("FW Image size %d not a multiple of sector size %d",
            size_in_bytes, sdcc_info.block_len);
    error = DEVPRG_ERROR_STORAGE;
    goto close_dev;
  }

  num_sectors = size_in_bytes / sdcc_info.block_len;
  result = sdcc_handle_fw_update(dev, buffer, num_sectors);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("FW Update failed slot %d", slot);
    failed = 1; /* Failed update. close the handle and de-initialize the sdcc
                   return after to return the system in a stable state. */
  }

  /* Upgrade returned success, close and de-init to apply. */
  result = sdcc_handle_close(dev);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Close Failed FW Update slot", slot);
    return error;
  }

  result = sdcc_deinit_device(slot);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Deinit Failed after FW Update slot", slot);
    return error;
  }

  /* Firnware update failed, return after card re-initialization to leave the
   * system in a stable state. */
  if(failed != 0)
  {
    return error;
  }

  /* Now lets check the new firmware version. */
  dev = sdcc_handle_open(slot, SDCC_USER_PARTITION);

  if(dev == NULL)
  {
    DP_LOGE("Open Failed after FW Update slot %d", slot);
    return DEVPRG_ERROR_STORAGE;
  }

  result = sdcc_handle_get_ffu_status(dev, &status);

  if(result == SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("FFU Status Failed slot %d", slot);
    goto close_dev;
  }

  if(status != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("FW Update failed slot %d", slot);
    goto close_dev;
  }

  result = sdcc_handle_get_fw_version(dev, &fw_ver_after);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Get FW Version Failed after slot %d", slot);
    goto close_dev;
  }

  DP_LOGI("FW Update successful. Old version 0x%X, new version 0x%X",
          fw_ver_before, fw_ver_after);

close_dev:
  result = sdcc_handle_close(dev);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    DP_LOGE("Close Failed on end FW Update slot %d", slot);
  }

  return error;
}

dp_res_t devprg_storage_sdcc_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  *size = 532; /* 532 is the same value as EMMC_RAW_DATA_SIZE in the sdcc
                  driver */
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_sdcc_info_raw(void *device, void *buffer,
                                      uint32 *size)
{
  struct dp_storage_sdcc *dev = (struct dp_storage_sdcc *)device;
  SDCC_STATUS result;
  dp_res_t error;
  uint32 max_size;

  devprg_storage_sdcc_info_raw_size(device, &max_size);

  if(*size < max_size)
  {
    DP_LOGE("SDCC Get info raw size too small %d < %d", *size, max_size);
    return DEVPRG_ERROR_NOMEM;
  }

  result = sdcc_get_emmc_info_raw_data(dev->handle, buffer, size);

  if(result != SDCC_NO_ERROR)
  {
    error = translate_sdcc_error(result);
    return error;
  }

  *size = max_size;

  return DEVPRG_SUCCESS;

}

#define MAX_PRINT_SIZE 356 /* A good number to avoid cropping the output*/
dp_res_t devprg_storage_sdcc_info_raw_print(void *device, void *buffer,
                                            uint32 *size)
{
  dp_res_t error;
  uint32 raw_size = *size;
  uint32 last_index;

  error = devprg_storage_sdcc_info_raw(device, buffer, &raw_size);
  if (error == DEVPRG_SUCCESS)
  {
    char *raw_data = (char *)buffer;
    char *out_str;
    uint32 max_out_size, out_size;
    uint32 i;

    /* We are re-using the provided input buffer to print the string.
     * this is a hack, we can clean it up once we get better logging. */
    max_out_size = (*size) - raw_size;
    if (max_out_size < MAX_PRINT_SIZE)
    {
      error = DEVPRG_ERROR_NOMEM;
      goto cleanup;
    }
    DP_LOGI("eMMC Extended CSD Revision: 0x%x", raw_data[192]);
    DP_LOGI("eMMC Firmware Version: 0x%x", raw_data[254]);

    out_str = &raw_data[raw_size];
    memset(out_str, 0, max_out_size);
    out_size = 0;
    last_index = 0;
    for(i = 0; i < raw_size; i++)
    {
      int result;
      int max_size = max_out_size - out_size;
      result = snprintf(&out_str[out_size], max_size, "%02X", raw_data[i]&0xFF);
      if (result < 0)
      {
        error = DEVPRG_ERROR_NOMEM;
        goto cleanup;
      }
      out_size += result;
      if (out_size >= MAX_PRINT_SIZE - 1)
      {
        DP_LOGI("eMMC_RAW_DATA[%d:%d]%s", last_index, i, out_str);
        last_index = i + 1;
        memset(out_str, 0, out_size);
        out_size = 0;
      }
    }
    if (out_size > 0)
      DP_LOGI("eMMC_RAW_DATA[%d:%d]%s", last_index, i, out_str);
  }

cleanup:
  return error;
}

#else /* If SDCC Support is disabled (FEATURE_DEVPRG_SDCC not defined)*/

#include "devprg_storage_stubs.h"
void devprg_storage_sdcc_init(void)
{
  devprg_storage_stub_init();
}

dp_res_t devprg_storage_sdcc_deinit(void)
{
  return devprg_storage_stub_deinit();
}

void *devprg_storage_sdcc_open(uint32 slot, uint32 partition,
                               dp_res_t *error)
{
  return devprg_storage_stub_open(slot, partition, error);
}

dp_res_t devprg_storage_sdcc_close(void *device)
{
  return devprg_storage_stub_close(device);
}

dp_res_t devprg_storage_sdcc_io_options(void *device,
                                        struct devprg_io_options *io_options)
{
  return devprg_storage_stub_io_options(device, io_options);
}

dp_res_t devprg_storage_sdcc_read(void *device, void *buffer,
                                  uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_read(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_sdcc_write(void *device, void *buffer,
                                   uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_write(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_sdcc_erase(void *device,
                                   uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_erase(device, start_sector, num_sectors);
}

dp_res_t devprg_storage_sdcc_format(void *device)
{
  return devprg_storage_stub_format(device);
}

dp_res_t devprg_storage_sdcc_set_bootable(void *device, int enable)
{
  return devprg_storage_stub_set_bootable(device, enable);
}

dp_res_t devprg_storage_sdcc_get_info(void *device,
                                      struct devprg_storage_info *dev_info)
{
  return devprg_storage_stub_get_info(device, dev_info);
}

dp_res_t devprg_storage_sdcc_configure(uint32 slot,
                                       struct devprg_storage_cfg_data *config)
{
  return devprg_storage_stub_configure(slot, config);
}

dp_res_t devprg_storage_sdcc_fw_update(uint32 slot, void *buffer,
                                       uint32 size_in_bytes)
{
  return devprg_storage_stub_fw_update(slot, buffer, size_in_bytes);
}

dp_res_t devprg_storage_sdcc_info_raw_size(void *device, uint32 *size)
{
  return devprg_storage_stub_info_raw_size(device, size);
}

dp_res_t devprg_storage_sdcc_info_raw(void *device, void *buffer,
                                      uint32 *size)
{
  return devprg_storage_stub_info_raw(device, buffer, size);
}

dp_res_t devprg_storage_sdcc_info_raw_print(void *device, void *buffer,
                                            uint32 *size)
{
  return devprg_storage_stub_info_raw_print(device, buffer, size);
}

#endif /* FEATURE_DEVPRG_SDCC */
