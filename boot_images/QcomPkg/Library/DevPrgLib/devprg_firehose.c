/**************************************************************************
 * FILE: devprg_firehose.c
 *
 * Firehose protocol handler.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_firehose.c#3 $
  $DateTime: 2018/01/31 00:50:24 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2018-01-23   wek   Return error for peek/poke if secure boot is enabled
2017-09-18   wek   Correctly clear the always validate flag
2017-09-15   wek   Implement having peek/poke commands disabled by default
2017-07-21   wek   Add support for fixing gpt tables on target.
2017-05-03   wek   Verify that SHA256 is working by comparing with known
2017-05-02   wek   Implement the benchmark write to handle NAND devices
2017-04-26   wek   Validate the sector size of the device against the xml
2017-04-20   wek   Include the start and end sectors in set io options.
2017-04-14   wek   Add XML initialization of data structures.
2017-04-13   wek   Nand support in memory name.
2017-04-11   wek   Solve logging while doing a read from disk.
2017-03-20   wek   Add support for changing the way NAND is read.
2017-03-09   wek   Repair some compile and runtime errors found on target.
2017-03-08   wek   Add getting the sector number from string disk size. (1-m)
2017-03-07   wek   Enhance the output of the benchmarks.
2017-03-04   wek   Make patch start sector a string defining end of disk (2y)
2017-03-02   wek   Make storage info not log info in json format if requested
2017-02-02   wek   Add io options to change io output.
2017-01-09   wek   Calculate the hash when always validate is enabled.
2016-12-22   wek   Lay down the code for handling all transfers through USB.
2016-12-21   wek   Add the transfer buffers to a separate file.
2016-12-20   wek   Implement the protocol configure handler.
2016-12-14   wek   Re-write the eMMC configure to support UFS.
2016-12-14   wek   EMMC configure routine added.
2016-12-05   wek   Write the code to handle benchmark tag.
2016-12-02   wek   Implement a simple attribute getter with log on error.
2016-12-02   wek   Lay down the code to handle poke tag.
2016-12-01   wek   Lay down the code to handle peek tag.
2016-11-30   wek   Implement the handler for get storage info tag.
2016-11-29   wek   Add handler for flash firmware update.
2016-11-28   wek   Make the handler for power tag.
2016-11-28   wek   Keen implementation to handle erase tag.
2016-11-23   wek   Implement the handler for set bootable partition.
2016-11-23   wek   Make the handler for patch tag.
2016-11-22   wek   Buffers should be allocated and freed.
2016-11-21   wek   Establish an handler for nop tag.
2016-11-18   wek   Rough implementation of handle read tag.
2016-11-14   wek   Lay down the code to handle the program tag.
2016-11-07   wek   Yet another implementation of Firehose.
2016-11-04   wek   Create. (20-m)

===========================================================================*/

#include <string.h>
#include <stringl/stringl.h>
#include <stdint.h>
#include <limits.h>
#include "boot_msm.h"

#include "devprg_firehose.h"
#include "devprg_gpt.h"
#include "devprg_log.h"
#include "devprg_target.h"
#include "devprg_utils.h"
#include "devprg_security.h"
#include "devprg_storage.h"
#include "devprg_transfer.h"
#include "devprg_xml.h"

#include "pm_boot.h"

typedef int (*command_handler)(struct dp_xml_handle *);

struct devprg_firehose_cmd_list
{
  const char *tag;      /* String in the XML for the command, e.g. program */
  command_handler handler; /* Function ponter to the handler for the tag. */
};

struct dp_basic_dev_data
{
  enum devprg_storage_type storage_type;
  uint32 slot;
  uint32 lun;
};

struct dp_basic_io_data
{
  uint32 sector_size;
  uint64 num_sectors;
  uint64 start_sector;
};

/* Protocol configuration parameters. */
struct dp_protocol_config
{
  enum devprg_storage_type storage_type;
  uint64 max_transfer_size;
  uint64 transfer_size;

  /* verbose */
  /* always validate */
  /* digest table size */
};

static struct dp_protocol_config protoc_cfg;

/* Converts an decimal ascii string to a uint64 value.
 *
 * param  ascii   NULL terminated string containing the number.
 * param  value[out]  Converted value.
 *
 * returns  0 on success, -1 on error
 **/
static int dec_atoi(const char *ascii, uint64 *value)
{
  unsigned int multiplier = 10;
  int i = 0;
  char v;
  uint64 result = 0;
  uint64 val;

  /* Check for empty string. */
  if(ascii[0] == '\0')
    return -1;

  while(ascii[i] != '\0')
  {
    result = result * multiplier;
    v = ascii[i];
    if (v >= '0' && v <= '9')
      val = (v - 48);  /* 48 is the ascii value for 0. */
    else
      return -1;

    /* Check for overflow. */
    if(result > UINT64_MAX - val)
      return -1;

    result += val;
    i++;
  }
  *value = result;
  return 0;
}

/* Converts an decimal ascii string to a uint64 value.
 *
 * param  ascii   NULL terminated string containing the number.
 * param  value[out]  Converted value.
 *
 * returns  0 on success, -1 on error
 **/
static int hex_atoi(const char *ascii, uint64 *value)
{
  unsigned int multiplier = 16;
  int i = 0;
  char v;
  uint64 result = 0;
  uint64 val;

  /* Check for empty string. */
  if(ascii[0] == '\0')
    return -1;

  while(ascii[i] != '\0')
  {
    result = result * multiplier;
    v = ascii[i];
    if (v >= '0' && v <= '9')
      val = (v - 48);  /* 48 is the ascii value for 0. */
    else if (v >= 'A' && v <= 'F')
      val = (v - 'A' + 10); /* 64 ascii value for A, +10 because 0xA=10 */
    else if (v >= 'a' && v <= 'f')
      val = (v - 'a' + 10); /* 98 ascii value for a, +10 because 0xa=10 */
    else
      return -1;

    /* Check for overflow. */
    if(result > UINT64_MAX - val)
      return -1;

    result += val;
    i++;
  }
  *value = result;
  return 0;
}

/* Converts an ascii string (hex or decimal) to a uint64 value.
 *
 * param  ascii   NULL terminated string containing the number.
 * param  value[out]  Converted value.
 *
 * returns  0 on success, -1 on error
 **/
static int convert_atoi(const char *ascii, uint64 *value)
{
  /* check if it starts with HEX or not. */
  if (ascii == NULL)
    return -1;

  if(ascii[0] == '0' && (ascii[1] == 'x' || ascii[1] == 'X'))
    return hex_atoi(&ascii[2], value);
  else
    return dec_atoi(&ascii[0], value);
}


enum devprg_storage_type get_storage_type (const char *storage_type)
{
  if(strncasecmp("UFS", storage_type, 3) == 0)
    return DEVPRG_STORAGE_UFS;
  if(strncasecmp("eMMC", storage_type, 4) == 0)
    return DEVPRG_STORAGE_SDCC;
  if(strncasecmp("spinor", storage_type, 6) == 0)
    return DEVPRG_STORAGE_SPINOR;
  if(strncasecmp("nand", storage_type, 4) == 0)
    return DEVPRG_STORAGE_NAND;

  return DEVPRG_STORAGE_NULL;
}

const char *get_storage_type_string (enum devprg_storage_type type)
{
  if(type == DEVPRG_STORAGE_UFS)
    return "UFS";
  if(type == DEVPRG_STORAGE_SDCC)
    return "eMMC";
  if(type == DEVPRG_STORAGE_SPINOR)
    return "spinor";
  if(type == DEVPRG_STORAGE_NAND)
    return "nand";

  return "none";
}

static int get_uint64_attr(struct dp_xml_handle *xml_handle, const char *attr,
                           uint64 *value)
{
  int res;
  const char *string;
  string = devprg_xml_get_value(xml_handle, attr);
  res = convert_atoi(string, value);
  if(res != 0)
  {
    DP_LOGD("Can't decode attribute %s with value %s", attr, string);
    return -1;
  }
  return 0;
}

static int get_uint32_attr(struct dp_xml_handle *xml_handle, const char *attr,
                           uint32 *value)
{
  int res;
  uint64 val64;
  res = get_uint64_attr(xml_handle, attr, &val64);
  if(res != 0)
    return -1;

  if (val64 > UINT32_MAX)
  {
    DP_LOGE("Integer value for %s is too high %d %d", attr, val64, UINT32_MAX);
    return -1;
  }
  *value = (uint32) val64;

  return 0;
}

static int get_uint8_attr(struct dp_xml_handle *xml_handle, const char *attr,
                          uint8 *value)
{
  int res;
  uint64 val64;
  res = get_uint64_attr(xml_handle, attr, &val64);
  if(res != 0)
    return -1;

  if (val64 > UINT8_MAX)
  {
    DP_LOGE("Integer value for %s is too high %d %d", attr, val64, UINT8_MAX);
    return -1;
  }
  *value = (uint8) val64;

  return 0;
}

static int get_dev_data(struct dp_xml_handle *handle, struct dp_basic_dev_data *io)
{
  const char *string;
  int res;

  string = devprg_xml_get_value(handle, "storage_type");
  if (string[0] == '\0')
  {
    /* Default to UFS, should default to global value. */
    io->storage_type = protoc_cfg.storage_type;
  }
  else
  {
    io->storage_type = get_storage_type(string);
  }

  res = get_uint32_attr(handle, "slot", &io->slot);
  if (res != 0)
    io->slot = 0; /* If slot is not specified, default to zero. */

  res = get_uint32_attr(handle, "physical_partition_number", &io->lun);
  if (res != 0)
    io->lun = 0; /* If LUN is not specified, default to zero. */

  return 0;
}

/* Converts the string that corresponds to a sector to an actual sector
 * value. */
static int get_sector_value(const char *value,
                            struct devprg_storage *store_handle,
                            uint64 *out)
{
  uint64 num;
  int res;
  if (strncmp("NUM_DISK_SECTORS", value, 16) == 0 && value[16] == '-')
  {
    /* The string indicates we need to do some math to get the actual
     * sector value. */
    struct devprg_storage_info dev_info;
    dp_res_t error;
    uint64 pages_per_block, total_sectors;
    size_t i, len;
    char sub[21];

    error = devprg_storage_get_info(store_handle, &dev_info);

    len = strlcpy(sub, &value[17], sizeof(sub));
    if (len > sizeof(sub))
    {
      DP_LOGE("Invalid size of int, too large %d > %d",
              strlen(&value[17]), sizeof(sub));
      return -1;
    }

    /* The input data may end with a dot. Remove the dot and put NULL. */
    /* 20 is the max number of digits a uint64 can expand to in a string */
    for (i = 0; i < sizeof(sub); i++)
    {
      if (sub[i] == '.')
        sub[i] = 0;

      /* Reached the end of the string. */
      if (sub[i] == 0)
        break;

    }

    res = convert_atoi(sub, &num);
    if (res != 0)
    {
      DP_LOGE("Invalid value following NUM_DISK_SECTORS %s", &value[16]);
      return -1;
    }

    if(dev_info.page_size == 0 || error != DEVPRG_SUCCESS)
    {
      DP_LOGE("Invalid value for page_size for get info %d", 0);
      return -1;
    }

    pages_per_block = dev_info.block_size / dev_info.page_size;
    if(pages_per_block == 0)
    {
      DP_LOGE("Invalid pages per block, block size %d, page_size",
              dev_info.block_size, dev_info.page_size);
      return -1;
    }

    total_sectors = dev_info.total_blocks * pages_per_block;
    if(total_sectors < num)
    {
      DP_LOGE("Asked NUM_DISK_SECTOR-%d that is outside the total sectors %d",
              num, total_sectors);
      return -1;
    }
    num = total_sectors - num;
  }
  else /* The string does not start with NUM_DISK_SECTORS */
  {
    res = convert_atoi(value, &num);
    if (res != 0)
      return -1;
  }

  *out = num;
  return 0;
}

/* Get the IO data from the XML.
*  Get the start sector, number of sectors */
static int get_io_data(struct dp_xml_handle *handle,
                       struct devprg_storage *store_handle,
                       struct dp_basic_io_data *io)
{
  const char *string;
  int res;

  res = get_uint32_attr(handle, "SECTOR_SIZE_IN_BYTES", &io->sector_size);
  if (res != 0)
  {
    DP_LOGI("invalid SECTOR_SIZE_IN_BYTES result %d", res);
    return -1;
  }

  string = devprg_xml_get_value(handle, "num_partition_sectors");
  res = get_sector_value(string, store_handle, &io->num_sectors);
  if (res != 0)
  {
    DP_LOGI("invalid num_partition_sectors result %d", res);
    return -1;
  }

  string = devprg_xml_get_value(handle, "start_sector");
  res = get_sector_value(string, store_handle, &io->start_sector);
  if (res != 0)
  {
    DP_LOGI("invalid start_sector result %d", res);
    return -1;
  }

  return 0;
}

/* Open a storage device, generic open from the command header. */
static struct devprg_storage *open_dev(enum devprg_storage_type storage_type,
                                       uint32 slot, uint32 lun,
                                       dp_res_t *error)
{
  struct devprg_storage *handle = NULL;

  if (error == NULL)
  {
    DP_LOGE("Caller MUST provide an valid error pointer to return error %d", 0);
    return NULL;
  }

  *error = DEVPRG_SUCCESS;
  handle = devprg_storage_open(storage_type, slot, lun, error);

  /* Sanity check, to make sure we don't shoot ourselves in the foot. */
  if (handle == NULL && *error == DEVPRG_SUCCESS)
  {
    DP_LOGE("OPEN handle NULL and no error, weird %d", error);
    *error = DEVPRG_ERROR_STORAGE;
  }

  if (handle == NULL)
  {
    DP_LOGE("Failed to open device, type:%s, slot:%d, lun:%d error:%d",
            get_storage_type_string(storage_type), slot, lun, *error);
  }

  return handle;
}

/* Close an open device, created for consistency of open/close. */
static int close_dev(struct devprg_storage *handle)
{
  dp_res_t error;
  if (handle != NULL)
  {
    error = devprg_storage_close(handle);
    if (error != DEVPRG_SUCCESS)
    {
      return -1;
    }
  }
  return 0;
}

/* Check if the sector size in the XML matches the one
 * in the device.
 * This size may be different than the one reported by
 * get storage info, for example in SPI-NOR the sector
 * size configured is 4K but the real sector size is
 * 256 bytes.*/
static int check_sector_size (struct devprg_storage *store_handle,
                              uint32 xml_page_size,
                              enum devprg_storage_type mem_type)
{
  uint32 dev_page_size;
  struct devprg_storage_info dev_info;
  dp_res_t res;

  switch(mem_type)
  {
  case DEVPRG_STORAGE_SDCC:
    dev_page_size = 512;
    break;
  case DEVPRG_STORAGE_NULL:
    dev_page_size = xml_page_size;
    break;
  case DEVPRG_STORAGE_NAND:
    res = devprg_storage_get_info(store_handle, &dev_info);
    if (res != DEVPRG_SUCCESS)
    {
      DP_LOGE("Failed to get storage info from NAND %d", res);
      return -1;
    }
    dev_page_size = dev_info.page_size;
    break;
  case DEVPRG_STORAGE_SPINOR:
  case DEVPRG_STORAGE_UFS:
  default:
    dev_page_size = 4096;
    break;
  }
  if (xml_page_size == dev_page_size)
    return 0;

  DP_LOGE("sector size defined in XML (%u) different from device sector size (%u)",
          (unsigned int)xml_page_size, (unsigned int)dev_page_size);
  return -1;
}

static int send_reponse_buffer(void *buffer, size_t buf_size)
{
  int result = 0;
  dp_res_t error;
  error = devprg_tx_blocking(buffer, buf_size);
  if (error != DEVPRG_SUCCESS)
  {
    DP_LOGE("Failed to send buffer! %d", error);
    result = -1;
  }

  return result;
}

static int send_reponse_xml(void *buffer, size_t buf_size,
                            int ack, int rawmode)
{
  int result = -1;
  result = devprg_xml_build_response(buffer, buf_size, ack, rawmode);
  if (result > 0)
    result = send_reponse_buffer(buffer, result);

  return result;
}

static int set_io_options(struct dp_xml_handle *xml_handle,
                          struct devprg_storage *store_handle,
                          uint64 start_sector)
{
  struct devprg_io_options io_options;
  uint8 ecc_disabled, spare, bad_block;
  uint64 max_sector, min_sector;
  dp_res_t error = DEVPRG_SUCCESS;
  int result;

  if(store_handle == NULL || xml_handle == NULL)
  {
    return -1;
  }

  result = get_uint64_attr(xml_handle, "start_sector", &min_sector);
  if (result != 0)
    min_sector = start_sector;
  io_options.min_sector = min_sector;

  result = get_uint64_attr(xml_handle, "last_sector", &max_sector);
  if (result != 0)
    max_sector = UINT64_MAX;
  io_options.max_sector = max_sector;

  /* Only skip bad blocks when the flag is set to 0 (false). */
  result = get_uint8_attr(xml_handle, "skip_bad_block", &bad_block);
  if(result == 0 && bad_block != 0)
    io_options.skip_bb = BAD_BLOCK_FILL;
  else
    io_options.skip_bb = BAD_BLOCK_SKIP;

  /* Check if we need to get spare data. */
  result = get_uint8_attr(xml_handle, "get_spare", &spare);
  if(result == 0 && spare != 0)
    io_options.get_spare = DATA_AND_SPARE;
  else
    io_options.get_spare = DATA_ONLY;

  /* Disable ECC only when we have valid value, and value is positive. */
  result = get_uint8_attr(xml_handle, "ecc_disabled", &ecc_disabled);
  if (result == 0 && ecc_disabled != 0)
    io_options.ecc_state = ECC_DISABLED;
  else
    io_options.ecc_state = ECC_ENABLED;

  error = devprg_storage_io_options(store_handle, &io_options);
  if(error != DEVPRG_SUCCESS)
  {
    DP_LOGE("Failed to set the IO options %d, %d %d", io_options.skip_bb,
            io_options.get_spare, io_options.ecc_state);
    return -1;
  }
  return 0;
}


static int handle_write(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_io_data io_data;
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  dp_res_t e2 = DEVPRG_SUCCESS;
  void *data[2];
  size_t buf_size, buf_size2, read_size;
  size_t curr, next, temp;
  uint64 sectors_remaining, sector_count, sector_size, start_sector;

  data[0] = devprg_get_transfer_buffer(&buf_size);
  if (data[0] == NULL)
    return -1;
  data[1] = devprg_get_transfer_buffer(&buf_size2);
  if (data[1] == NULL)
    return -1;

  if (buf_size != buf_size2)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  result = get_io_data(xml_handle, store_handle, &io_data);
  if (result != 0)
    goto cleanup;

  result = set_io_options(xml_handle, store_handle, io_data.start_sector);
  if (result != 0)
    goto cleanup;

  result = check_sector_size (store_handle, io_data.sector_size, dev_data.storage_type);
  if (result != 0)
    goto cleanup;

  /* Now that we have all the data from XML and it looks good, send an ACK.*/
  ack = 1; rawmode = 1;
  result = send_reponse_xml(data[0], buf_size, ack, rawmode);
  if (result != 0)
    goto cleanup;

  start_sector = io_data.start_sector;
  sectors_remaining = io_data.num_sectors;
  sector_size = io_data.sector_size;
  read_size = sectors_remaining * sector_size;
  if (read_size > buf_size)
    read_size = (buf_size / sector_size) * sector_size; /* Round down */

  error = devprg_rx_queue(data[0], read_size);
  curr = 0;
  next = 1;

  while (sectors_remaining != 0)
  {
    size_t rx_bytes;
    e2 = devprg_rx_queue_wait(&rx_bytes);
    if(e2 == DEVPRG_SUCCESS && (rx_bytes % sector_size) != 0)
    {
      error = e2;
      DP_LOGE("Read non multiple sector size value from USB %d sectsize%d",
              rx_bytes, sector_size);
    }

    /* if there is still data pending, queue the next packet. */
    sector_count = rx_bytes / sector_size;
    read_size = sectors_remaining - sector_count;
    if(read_size > 0)
    {
      /* Convert sectors to bytes*/
      read_size *= sector_size;
      if (read_size > buf_size)
        read_size = (buf_size / sector_size) * sector_size; /* Round down */

      devprg_rx_queue(data[next], read_size);
    }

    if (error == DEVPRG_SUCCESS)
      error = devprg_storage_write(store_handle, data[curr], start_sector,
                                   sector_count);

    start_sector += sector_count;
    sectors_remaining -= sector_count;

    /* Swap the buffers. */
    temp = curr;
    curr = next;
    next = temp;
  }

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data[0], buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data[0]);
  devprg_free_transfer_buffer(data[1]);

  return result;
}

static int handle_read(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_io_data io_data;
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  void *data;
  size_t buf_size, read_size;
  uint64 sectors_remaining, sector_count, sector_size, start_sector;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  result = get_io_data(xml_handle, store_handle, &io_data);
  if (result != 0)
    goto cleanup;

  result = set_io_options(xml_handle, store_handle, io_data.start_sector);
  if (result != 0)
    goto cleanup;

  result = check_sector_size (store_handle, io_data.sector_size, dev_data.storage_type);
  if (result != 0)
    goto cleanup;

  /* Now that we have all the data from XML and it looks good,
     disable USB log and send an ACK.*/
  devprg_log_usb_set_state(LOG_USB_DISABLE);
  ack = 1; rawmode = 1;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  if (result != 0)
    goto cleanup;

  start_sector = io_data.start_sector;
  sectors_remaining = io_data.num_sectors;
  sector_size = io_data.sector_size;
  read_size = sectors_remaining * sector_size;
  if (read_size > buf_size)
    read_size = (buf_size / sector_size) * sector_size; /* Round down */

  while (sectors_remaining != 0)
  {
    sector_count = read_size / sector_size;
    if (error == DEVPRG_SUCCESS)
      error = devprg_storage_read(store_handle, data, start_sector, sector_count);
    else
      memset(data, 0, read_size);

    devprg_tx_blocking(data, read_size);

    /* if there is still data pending, queue the next packet. */
    start_sector += sector_count;
    sectors_remaining -= sector_count;
    read_size = sectors_remaining * sector_size;
    if (read_size > buf_size)
      read_size = (buf_size / sector_size) * sector_size; /* Round down */
  }

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("R Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  /* Enable USB Logs back. */
  devprg_log_usb_set_state(LOG_USB_ENABLE);

  return result;
}


static void log_basic_dev_characteristics(void);
static int handle_nop(struct dp_xml_handle *xml_handle)
{
  int result;
  int ack, rawmode;
  void *data;
  size_t buf_size;
  (void) xml_handle;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  log_basic_dev_characteristics();

  ack = 1;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int crc32_over_sectors(struct devprg_storage *store_handle,
                              uint64 start_sector, uint64 num_bytes,
                              void *buffer, size_t buf_size,
                              uint32 sec_size, uint32 *crc_out)
{
  uint64 num_sectors;
  dp_res_t error;

  if (buf_size < num_bytes && buf_size < sec_size)
    return -1;

  /* Round up to the sector size. */
  num_sectors = ((num_bytes - 1) / sec_size) + 1;

  error = devprg_storage_read(store_handle, buffer, start_sector, num_sectors);
  if (error != DEVPRG_SUCCESS)
    return -1;

  *crc_out = devprg_calc_crc32(buffer, num_bytes);
  return 0;
}

static int handle_patch(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  char *data;
  size_t buf_size;
  const char *string;
  const char *patch_value_str;
  uint64 patch_value, start_sector;
  uint32 sector_size;
  uint32 byte_offset;
  uint32 patch_size_in_bytes = 0;

  DP_LOGD("patch command is deprecated%c", '.');

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  string = devprg_xml_get_value(xml_handle, "filename");
  if (strncasecmp("DISK", string, 4) != 0)
  {
    /* We only need to patch the ones that say filename="DISK" */
    result = 0;
    goto cleanup;
  }

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  result = get_uint32_attr(xml_handle, "SECTOR_SIZE_IN_BYTES", &sector_size);
  if (result != 0)
    goto cleanup;

  result = check_sector_size (store_handle, sector_size, dev_data.storage_type);
  if (result != 0)
    goto cleanup;

  string = devprg_xml_get_value(xml_handle, "start_sector");
  result = get_sector_value(string, store_handle, &start_sector);
  if (result != 0)
    goto cleanup;

  result = get_uint32_attr(xml_handle, "byte_offset", &byte_offset);
  if (result != 0)
    goto cleanup;

  result = get_uint32_attr(xml_handle, "size_in_bytes", &patch_size_in_bytes);
  if (result != 0)
    goto cleanup;

  if (patch_size_in_bytes > 8)
  {
    DP_LOGE("patch size too large %d", patch_size_in_bytes);
    result = -1;
    goto cleanup;
  }
  patch_value_str = devprg_xml_get_value(xml_handle, "value");
  if(strncmp(patch_value_str, "CRC32(", 6) != 0)
  {
    result = get_sector_value(patch_value_str, store_handle, &patch_value);
    if (result != 0)
      goto cleanup;
  }
  else
  {
    char temp_str[64];
    uint64 crc_start_sector;
    char *comma_offset;
    char *p_offset; /* parenthesis offset. */
    uint64 num_bytes_crc;
    uint32 crc32;

//    DP_LOGW("PATCHING USING CRC IS DEPRECATED and scheduled to be removed%c",'.');

    result = strlcpy(temp_str, &patch_value_str[6], 64);
    if (result >= 64)
    {
      DP_LOGE("too large CRC string %s, %d", patch_value_str, result);
      goto cleanup;
    }

    comma_offset = strchr(temp_str, ',');
    if (comma_offset == NULL)
    {
      DP_LOGE("Not a proper CRC function format%c", '.');
      goto cleanup;
    }
    *comma_offset = '\0';
    result = get_sector_value(temp_str, store_handle, &crc_start_sector);
    if (result != 0)
    {
      DP_LOGE("Invalid format on CRC(BAD,X) start sector %s", patch_value_str);
      goto cleanup;
    }

    result = strlcpy(temp_str, comma_offset+1, 64);
    if (result >= 64)
    {
      DP_LOGE("too large CRC string %s, %d", patch_value_str, result);
      goto cleanup;
    }

    p_offset = strchr(temp_str, ')');
    if (p_offset == NULL)
    {
      DP_LOGE("Not a proper CRC function format %s!=CRC(s,l)", patch_value_str);
      goto cleanup;
    }
    *p_offset = '\0';
    result = convert_atoi(temp_str, &num_bytes_crc);
    if (result != 0)
    {
      DP_LOGE("Invalid CRC function function format %s", patch_value_str);
      goto cleanup;
    }

    result = crc32_over_sectors(store_handle,crc_start_sector,num_bytes_crc,
                                data, buf_size, sector_size, &crc32);
    if (result != 0)
    {
      DP_LOGE("Failed to compute CRC on given sectors %d %d",
              crc_start_sector, num_bytes_crc);
      goto cleanup;
    }
    patch_value = crc32;
  }

  error = devprg_storage_read(store_handle, data, start_sector, 1);
  if (error != DEVPRG_SUCCESS)
    goto cleanup;

  memscpy(&data[byte_offset], buf_size - byte_offset, &patch_value,
          patch_size_in_bytes);

  error = devprg_storage_write(store_handle, data, start_sector, 1);

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("P Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int handle_set_bootable_drive(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  char *data;
  size_t buf_size;
  uint32 boot_lun;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  result = get_uint32_attr(xml_handle, "value", &boot_lun);
  if (result == 0)
  {
    DP_LOGI("Using scheme of value= %d", boot_lun);
    dev_data.lun = boot_lun;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  error = devprg_storage_set_bootable(store_handle, 1);

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("Set Bootable Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int handle_erase(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_io_data io_data;
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  char *data;
  size_t buf_size;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  result = get_io_data(xml_handle, store_handle, &io_data);
  if (result == 0)
  {
    uint64 num_sectors, start_sector;
    start_sector = io_data.start_sector;
    num_sectors = io_data.num_sectors;

    result = set_io_options(xml_handle, store_handle, start_sector);
    if (result != 0)
      goto cleanup;

    DP_LOGI("Erasing start sector %d, num sectors %d",
            start_sector, num_sectors);
    error = devprg_storage_erase(store_handle, start_sector, num_sectors);
  }
  else
  {
    result = 0;
    DP_LOGI("Erasing the whole disk/LUN %d", dev_data.lun);
    error = devprg_storage_format(store_handle);
  }
  if(error != DEVPRG_SUCCESS)
    DP_LOGE("Failed to erase the disk/lun %d", dev_data.lun);

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("Erase Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int handle_power(struct dp_xml_handle *xml_handle)
{
  int result = -1;
  int ack, rawmode;
  void *data;
  size_t buf_size;
  const char *string;
  enum power_state {POWER_RESET, POWER_OFF, POWER_RESET_EDL};
  enum power_state power_state = POWER_RESET;
  uint64 delay_sec;
  uint64 delay_usec;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  string = devprg_xml_get_value(xml_handle, "value");
  if (string == NULL)
    goto error;

  if (strncasecmp(string, "reset_to_edl", 13) == 0)
    power_state = POWER_RESET_EDL;
  else if (strncasecmp(string, "reset", 5) == 0)
    power_state = POWER_RESET;
  else if (strncasecmp(string, "off", 3) == 0)
    power_state = POWER_OFF;
  else
    DP_LOGE("power/reset option not specified, default is do reset %d", POWER_RESET);

  result = get_uint64_attr(xml_handle, "DelayInSeconds", &delay_sec);
  if(result == 0)
    delay_usec = delay_sec * 1000000;
  else
    delay_usec = 100;

  ack = 1;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  DP_LOGI("Will issue reset/power off %ld useconds, if this hangs check if watchdog is enabled", delay_usec);

  switch(power_state)
  {
  default:
  case POWER_RESET:
    DP_LOGI("bsp_target_reset() %d", power_state);
    devprg_usleep(delay_usec);
    devprg_target_reset();
    break;
  case POWER_RESET_EDL:
    DP_LOGI("bsp_target_reset() %d", power_state);
    devprg_usleep(delay_usec);
    devprg_target_reset_edl();
    break;
  case POWER_OFF:
    DP_LOGI("bsp_target_poweroff() %d", power_state);
    devprg_usleep(delay_usec);
    devprg_target_poweroff();
    break;
  }

  DP_LOGI("If you are seeing this message the target didn't reset. Watchdog should be enabled %d", power_state);

  return -1;

error:
  ack = 0;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int handle_firmware_update(struct dp_xml_handle *xml_handle)
{
  int result = 0, ack, rawmode;
  dp_res_t error = DEVPRG_SUCCESS;
  void *data;
  size_t buf_size;
  uint64 num_bytes = 0;
  uint32 sector_size = 0;
  struct dp_basic_dev_data dev_data;
  int64 fw_size;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  result = get_uint32_attr(xml_handle, "SECTOR_SIZE_IN_BYTES", &sector_size);
  if (result != 0)
    goto cleanup;
  if(sector_size != 1)
  {
    DP_LOGE("Sector size in bytes for firmware update must be set to %d", 1);
    goto cleanup;
  }

  result = get_uint64_attr(xml_handle, "num_partition_sectors", &num_bytes);
  if (result != 0)
    goto cleanup;
  if (num_bytes == 0)
  {
    DP_LOGE("invalid num_partition_sectors %d", num_bytes);
    result = -1;
    goto cleanup;
  }

  if (num_bytes > buf_size)
  {
    result = -1;
    DP_LOGE("Buffer (%d) not large enough (%d) to hold the firmware image",
            buf_size, num_bytes);
    goto cleanup;
  }

  /* Now that we have all the data from XML and it looks good, send an ACK.*/
  ack = 1; rawmode = 1;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  if (result != 0)
    goto cleanup;

  fw_size = devprg_rx_blocking(data, num_bytes);
  if(fw_size > 0)
  {
    DP_LOGI ("Will do firmware update on storage part of size %d", num_bytes);
    error = devprg_storage_fw_update(dev_data.storage_type, dev_data.slot,
                                     data, fw_size);

  }
  else
  {
    DP_LOGE ("Error while receiving bytes %d", fw_size);
  }

cleanup:
  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int handle_storage_info(struct dp_xml_handle *xml_handle)
{
  int result = 0, ack, rawmode, json_res;
  dp_res_t error = DEVPRG_SUCCESS;
  void *data;
  uint32 buf_size;
  size_t data_size;
  struct dp_basic_dev_data dev_data;
  struct devprg_storage *store_handle = NULL;
  struct devprg_storage_info dev_info;
  uint32 raw_size;

  uint64 print_json = 1;

  data = devprg_get_transfer_buffer(&data_size);
  buf_size = data_size;
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                          &error);
  if(store_handle == NULL)
  {
    result = -1;
    goto cleanup;
  }

  error = devprg_storage_get_info(store_handle, &dev_info);
  if (error != DEVPRG_SUCCESS)
  {
    DP_LOGE("Failed to get storage info %d", error);
    goto cleanup;
  }

  json_res = get_uint64_attr(xml_handle, "print_json", &print_json);
  if (json_res != 0)
    print_json = 1; /* By default set as print the JSON for backwards compatibility*/

  if (print_json)
  {
    DP_LOGI("Device Total Logical Blocks: 0x%llx", dev_info.total_blocks);
    DP_LOGI("Device Block Size in Bytes: 0x%x", dev_info.block_size);
    DP_LOGI("Device Total Physical Partitions: 0x%x", dev_info.num_physical);
    DP_LOGI("Device Manufacturer ID: 0x%llx", dev_info.manufacturer_id);
    DP_LOGI("Device Serial Number: 0x%llx", dev_info.serial_num);

    DP_LOGI("{&quot;storage_info&quot;: {&quot;total_blocks&quot;:%lld, "
           "&quot;block_size&quot;:%d, &quot;page_size&quot;:%d, "
           "&quot;num_physical&quot;:%d, &quot;manufacturer_id&quot;:%llu, "
           "&quot;serial_num&quot;:%llu, &quot;fw_version&quot;:&quot;%s&quot;,"
            "&quot;mem_type&quot;:&quot;%s&quot;,&quot;prod_name&quot;:&quot;%s&quot;}}",
            dev_info.total_blocks, dev_info.block_size, dev_info.page_size,
            dev_info.num_physical,
            (unsigned long long) dev_info.manufacturer_id,
            (unsigned long long) dev_info.serial_num,
            dev_info.fw_version, dev_info.memory_type,
            dev_info.product_name);

  }

  error = devprg_storage_info_raw_size(store_handle, &raw_size);
  if (error != DEVPRG_SUCCESS)
  {
    DP_LOGE("Failed to get the raw info size %d", error);
    goto cleanup;
  }

  if (raw_size > buf_size)
  {
    DP_LOGE("Raw info size% %d larger than the largest buffer %d",
            raw_size, buf_size);
    result = -1;
    goto cleanup;
  }

  if (raw_size > 0)
    error = devprg_storage_info_raw_print(store_handle, data, &buf_size);

cleanup:
  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("Storage info Failed to close device%c", '.');
    result = -1;
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int peek_poke_get_basic_attr(struct dp_xml_handle *xml_handle,
                                    char **addr, uint64 *size)
{
  int result = -1;
  char address64[8]={0};

  result = get_uint64_attr(xml_handle, "size_in_bytes", size);
  if (result != 0)
    goto error;

  if (*size == 0)
    DP_LOGE("size in bytes is %d, nothing to peek/poke", 0);

  result = get_uint64_attr(xml_handle, "address64", (uint64 *)&address64);
  if (result != 0)
    goto error;

  if (addr == NULL)
    goto error;

  *(uint64*)addr = *(uint64*)address64;

  if (*addr == NULL)
  {
    DP_LOGE("Invalid address of NULL %d", 0);
    goto error;
  }

  return 0;
error:
  return -1;
}

int handle_peek(struct dp_xml_handle *xml_handle)
{
  int result = -1;
  int ack, rawmode;
  void *data;
  char *output;
  size_t buf_size;
  uint64 size_in_bytes, i;
  char *addr;
  int max_log_size = DEVPRG_MAX_LOG_SIZE;
  int len;
  int offset;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  /* If secure boot is enabled, disable peek. */
  if (devprg_is_secure_boot_enabled() != 0)
  {
    DP_LOGW("Peek is disabled on secure boot devices%c", '.');
    goto error;
  }

  output = (char *) data;

  result = peek_poke_get_basic_attr(xml_handle, &addr, &size_in_bytes);
  if (result != 0)
    goto error;

  offset = 0;
  memset(output, 0, buf_size);
  for (i = 0; i < size_in_bytes; i++)
  {
    len = snprintf(&output[offset], buf_size - offset, "0x%02X ", addr[i] & 0xFF);
    if (len < 0)
    {
      DP_LOGE("Error while copy data to output buffer %d", len);
      goto error;
    }

    offset += len;
    if (offset > max_log_size)
    {
      DP_LOGI("%s", output);
      memset(output, 0, offset);
      offset = 0;
    }
  }
  if (offset > 0)
  {
    DP_LOGI("%s", output);
    memset(output, 0, offset);
  }

  ack = 1;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return 0;

error:
  ack = 0;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

int handle_poke(struct dp_xml_handle *xml_handle)
{
  int result = -1;
  int ack, rawmode;
  void *data;
  size_t buf_size;
  uint64 size_in_bytes, i, value;
  char *addr;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  /* If secure boot is enabled, disable peek. */
  if (devprg_is_secure_boot_enabled() != 0)
  {
    DP_LOGW("Peek is disabled on secure boot devices%c", '.');
    goto error;
  }

  result = peek_poke_get_basic_attr(xml_handle, &addr, &size_in_bytes);
  if (result != 0)
    goto error;

  if (size_in_bytes > sizeof(uint64))
  {
    DP_LOGE("The poke size %d is larger than allowed %d",
            size_in_bytes, sizeof(uint64));
    goto error;
  }

  result = get_uint64_attr(xml_handle, "value64", &value);
  if (result != 0)
    goto error;


  for (i = 0; i < size_in_bytes; i++)
  {
    addr[i] = value & 0xFF;
    value = value >> 8;
  }

  ack = 1;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return 0;

error:
  ack = 0;
  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

#define BENCHMARK_SEED 0x074a60277488409d

/* If we generate a 4K pattern using the seed and fast_rand above, we should get
 * the sha256 hash in the variable pattern_4k_sha256. */
const unsigned char sha256_4k_pattern[] = {
  0x34, 0x65, 0x42, 0x6c, 0xa6, 0x95, 0x27, 0x5d,
  0xaa, 0x38, 0x42, 0xc9, 0xd8, 0x39, 0x9a, 0x87,
  0xb1, 0x32, 0xab, 0x52, 0x93, 0xbe, 0x4c, 0xbc,
  0x3d, 0xea, 0x45, 0xf6, 0x38, 0x36, 0x14, 0x9e};

uint64 fast_rand64(uint64 *seed)
{
  uint64 val = *seed;
  val = (3935559000370003845LL * val + 3037000493LL);
  *seed = val;
  return val & 0x7FFFFFFFFFFFFFFFLL;
}

void fill_pattern(char *pattern, size_t pattern_size)
{
  size_t i;
  size_t size = pattern_size/sizeof(uint64);
  uint64 val;
  uint64 *buffer = (uint64 *)pattern;
  uint64 seed = BENCHMARK_SEED;

  for (i = 0; i < size; i++)
  {
    val = fast_rand64(&seed);
    buffer[i] = val;
  }
}

#define BENCHMARK_MAX_INTERVAL_US   (8 * 1000 * 1000) /* 8 seconds */
/* Run the write benchmark test.
 *
 * This test is not perfect, it makes assumptions and may corrupt the
 * data if it is terminated early. It is written this way because
 * it was inherited from previous baselines. */
static int benchmark_writes(struct devprg_storage *store_handle,
                            struct devprg_storage_info *dev_info,
                            void *buffer, size_t buf_size, uint32 trials)
{
  uint32 sector_size;
  uint64 start_sector, sector_num;
  uint64 start_time, end_time, total_time = 0;
  uint32 sectors_in_buffer, total_io, size;
  int result;
  uint32 count;

  sector_size = dev_info->page_size;
  start_sector = 0;
  sectors_in_buffer = buf_size / sector_size;
  if (sectors_in_buffer == 0)
  {
    DP_LOGE("Buffer size is smaller than a sector %d < %d",
            buf_size, sector_size);
    return -1;
  }

  /* Round down in case the buffer is not a multiple of sector size. */
  buf_size = sectors_in_buffer * sector_size;

  DP_LOGW("Benchmark Writes Corrupts the disk, device will need to be re-loaded%c", '.');

  fill_pattern(buffer, buf_size);

  /* Loop through each chunk size */
  for (size = 1; size <= sectors_in_buffer; size = size*2)
  {
    sector_num = start_sector;
    total_io = 0;
    count = 0;
    start_time = devprg_time_usec();
    do
    {
      result = devprg_storage_write(store_handle, buffer, sector_num, size);
      if (result != 0)
      {
        DP_LOGE("Storage Write failed %d at sector %d size in sectors %d",
                result, sector_num, size);
        break;
      }
      total_io += size;
      sector_num += size;
      count++;
      end_time = devprg_time_usec();
      total_time = (end_time - start_time);
    } while (total_time < BENCHMARK_MAX_INTERVAL_US && count < trials);
    total_io *= sector_size;
    if (total_time != 0)
    {
      uint64 throughput;
      throughput = (((uint64)total_io) * 1000 * 1000)/((uint64)total_time);
      DP_LOGI("Write throughput 6 %uBps, Chunk size %u, wrote %u in %uus",
              throughput, size * sector_size, total_io, total_time);
    }
    else
    {
      DP_LOGE("Invalid total time of zero for write size %d", size * sector_size);
    }
  }
  return 0;
}

/* Run the read benchmark test. */
static int benchmark_reads(struct devprg_storage *store_handle,
                           struct devprg_storage_info *dev_info,
                           void *buffer, size_t buf_size, uint32 trials)
{
  uint32 sector_size;
  uint64 start_sector, sector_num;
  uint64 start_time, end_time, total_time = 0;
  uint32 sectors_in_buffer, total_io, size;
  int result;
  uint32 count;

  sector_size = dev_info->page_size;
  start_sector = 0;
  sectors_in_buffer = buf_size / sector_size;
  if (sectors_in_buffer == 0)
  {
    DP_LOGE("Buffer size is smaller than a sector %d < %d",
            buf_size, sector_size);
    return -1;
  }

  /* Round down in case the buffer is not a multiple of sector size. */
  buf_size = sectors_in_buffer * sector_size;

  /* Loop through each chunk size */
  for (size = 1; size <= sectors_in_buffer; size = size*2)
  {
    sector_num = start_sector;
    total_io = 0;
    count = 0;
    start_time = devprg_time_usec();
    do
    {
      result = devprg_storage_read(store_handle, buffer, sector_num, size);
      if (result != 0)
      {
        DP_LOGE("Storage read failed %d at sector %d size in sectors %d",
                result, sector_num, size);
        break;
      }
      total_io += size;
      sector_num += size;
      count++;
      end_time = devprg_time_usec();
      total_time = (end_time - start_time);
    } while (total_time < BENCHMARK_MAX_INTERVAL_US && count < trials);
    total_io *= sector_size;
    if (total_time != 0)
    {
      uint64 throughput;
      throughput = (((uint64)total_io) * 1000 * 1000)/((uint64)total_time);
      DP_LOGI("Read throughput %uBps, Chunk size %u, wrote %u in %uus",
              throughput, size * sector_size, total_io, total_time);
    }
    else
    {
      DP_LOGE("Invalid total time of zero for read size %d", size * sector_size);
    }
  }

  return 0;
}

/* Run the read benchmark test. */
static int benchmark_sha256(void *buffer, size_t buf_size, uint32 trials)
{
  uint64 start_time, end_time, total_time = 0;
  uint32 total_io;
  uint32 count, size;
  uint8 temp_hash_value[32] = {0};
  uint64 *value;
  uint64 seed = BENCHMARK_SEED;
  int res;

  fill_pattern(buffer, buf_size);
  value = (uint64 *)buffer;

  devprg_sha256_direct(buffer, 4096, temp_hash_value);
  res = memcmp(temp_hash_value, sha256_4k_pattern, sizeof(temp_hash_value));
  if(res != 0)
  {
    uint8 *c;
    DP_LOGE("SHA256 returned a wrong HASH value for the verification pattern",
            res);
    c = (uint8 *)sha256_4k_pattern;
    DP_LOGE("expected %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                     "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
            c[8], c[9], c[10],c[11],c[12],c[13],c[14],c[15],
            c[16],c[17],c[18],c[19],c[20],c[21],c[22],c[23],
            c[24],c[25],c[26],c[27],c[28],c[29],c[30],c[31]);

    c = (uint8 *)temp_hash_value;
    DP_LOGE("calculat %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                     "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
            c[8], c[9], c[10],c[11],c[12],c[13],c[14],c[15],
            c[16],c[17],c[18],c[19],c[20],c[21],c[22],c[23],
            c[24],c[25],c[26],c[27],c[28],c[29],c[30],c[31]);
  }
  else
  {
    DP_LOGI("SHA256 output matches SHA256 of verification pattern%c", '.');
  }

  /* Loop through each chunk size */
  for (size = 4096; size <= buf_size; size = size*2)
  {
    total_io = 0;
    count = 0;
    start_time = devprg_time_usec();
    do
    {
      *value = fast_rand64(&seed); /* Change the buffer to generate new data. */
      devprg_sha256_direct(buffer, size, temp_hash_value);
      total_io += size;
      count++;
      end_time = devprg_time_usec();
      total_time = (end_time - start_time);
    } while (total_time < BENCHMARK_MAX_INTERVAL_US && count < trials);
    if (total_time != 0)
    {
      uint64 throughput;
      throughput = (((uint64)total_io) * 1000 * 1000)/((uint64)total_time);
      DP_LOGI("SHA256 throughput %uBps, Chunk size %u, wrote %u in %uus",
              throughput, size, total_io, total_time);
    }
    else
    {
      DP_LOGE("Invalid total time of zero for hash size %d", size);
    }
  }

  return 0;
}

static int handle_benchmark(struct dp_xml_handle *xml_handle)
{
  int result = 0, ack, rawmode;
  dp_res_t error = DEVPRG_SUCCESS;
  void *data;
  size_t buf_size;
  struct dp_basic_dev_data dev_data;
  struct devprg_storage *store_handle = NULL;
  struct devprg_storage_info dev_info;
  uint32 trials;
  uint32 read, write, hash;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_uint32_attr(xml_handle, "trials", &trials);
  if (result != 0)
  {
    trials = 10000;
    DP_LOGI("Max trials not provided, using %d", trials);
    result = 0;
  }

  read = write = hash = 0;
  get_uint32_attr(xml_handle, "TestDigestPerformance", &hash);
  get_uint32_attr(xml_handle, "TestWritePerformance", &write);
  get_uint32_attr(xml_handle, "TestReadPerformance", &read);


  if(hash == 1)
  {
    benchmark_sha256(data, buf_size, trials);
  }

  if(write == 1 || read == 1)
  {
    result = get_dev_data (xml_handle, &dev_data);
    if (result == 0)
      store_handle = open_dev(dev_data.storage_type, dev_data.slot, dev_data.lun,
                              &error);
    if(store_handle == NULL)
      result = -1;

    if(result == 0)
    {
      set_io_options(xml_handle, store_handle, 0);

      error = devprg_storage_get_info(store_handle, &dev_info);
      if (error != DEVPRG_SUCCESS)
      {
        DP_LOGE("Failed to get storage info %d", error);
        result = -1;
      }
    }
    if (result == 0)
    {
      if (write == 1)
        benchmark_writes(store_handle, &dev_info, data, buf_size, trials);
      if (read == 1)
        benchmark_reads(store_handle, &dev_info, data, buf_size, trials);
    }

    if (close_dev(store_handle) != 0)
    {
      DP_LOGE("benchmark Failed to close device%c", '.');
      result = -1;
    }
  }

  if(result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

/* Storage Configuration Data. This structure is used to hold
 * data between configure commands. */
char has_storage_cfg_been_init = 0;
struct devprg_storage_cfg_data storage_cfg;

static int emmc_parse_configure(struct dp_xml_handle *xml,
                                struct devprg_storage_cfg_data *c)
{
  struct devprg_sdcc_emmc_gpp_enh_desc *emmc = &c->u.emmc_config;
  const char *tag;

  tag = devprg_xml_get_tag(xml);
  if (strncmp(tag, "emmc", 4) != 0)
  {
    DP_LOGE("Tried to configure %s but memoryname is emmc", tag);
    return -1;
  }

  get_uint32_attr(xml, "DRIVE4_SIZE_IN_KB", &emmc->GPP_size[0]);
  get_uint32_attr(xml, "DRIVE5_SIZE_IN_KB", &emmc->GPP_size[1]);
  get_uint32_attr(xml, "DRIVE6_SIZE_IN_KB", &emmc->GPP_size[2]);
  get_uint32_attr(xml, "DRIVE7_SIZE_IN_KB", &emmc->GPP_size[3]);
  get_uint32_attr(xml, "ENH_SIZE", &emmc->ENH_size);
  get_uint32_attr(xml, "ENH_START_ADDR", &emmc->ENH_start_addr);
  get_uint8_attr(xml, "GPP_ENH_FLAG", &emmc->GPP_enh_flag);

  return 0;
}

static int ufs_parse_configure(struct dp_xml_handle *xml,
                                struct devprg_storage_cfg_data *c)
{
  uint32 result;
  uint8 lun;
  uint64 value;
  const char *string;
  struct devprg_ufs_config_descr *ufs = &c->u.ufs_config;
  const char *tag;

  tag = devprg_xml_get_tag(xml);
  if (strncmp(tag, "ufs", 3) != 0)
  {
    DP_LOGE("Tried to configure %s but memoryname is ufs", tag);
    return -1;
  }

  result = get_uint8_attr(xml, "LUNum", &lun);
  if(result == 0 && lun < sizeof(ufs->unit)/sizeof(ufs->unit[0])/*=8*/)
  {

    /* This data belongs to this LUN. */
    get_uint8_attr(xml, "bLUEnable", &ufs->unit[lun].bLUEnable);
    get_uint8_attr(xml, "bBootLunID", &ufs->unit[lun].bBootLunID);
    get_uint8_attr(xml, "bLUWriteProtect", &ufs->unit[lun].bLUWriteProtect);
    get_uint8_attr(xml, "bMemoryType", &ufs->unit[lun].bMemoryType);
    get_uint64_attr(xml, "size_in_kb", &ufs->unit[lun].qSizeInKb);
    get_uint8_attr(xml, "bDataReliability", &ufs->unit[lun].bDataReliability);
    get_uint8_attr(xml, "bLogicalBlockSize", &ufs->unit[lun].bLogicalBlockSize);
    get_uint8_attr(xml, "bProvisioningType", &ufs->unit[lun].bProvisioningType);
    get_uint64_attr(xml, "wContextCapabilities", &value);
    ufs->unit[lun].wContextCapabilities = value & 0xFFFF;
    return 0;
  }

  /* else */
  get_uint8_attr(xml, "bNumberLU", &ufs->bNumberLU);
  get_uint8_attr(xml, "bBootEnable", &ufs->bBootEnable);
  get_uint8_attr(xml, "bDescrAccessEn", &ufs->bDescrAccessEn);
  get_uint8_attr(xml, "bInitPowerMode", &ufs->bInitPowerMode);
  get_uint8_attr(xml, "bHighPriorityLUN", &ufs->bHighPriorityLUN);
  get_uint8_attr(xml, "bSecureRemovalType", &ufs->bSecureRemovalType);
  get_uint8_attr(xml, "bInitActiveICCLevel", &ufs->bInitActiveICCLevel);
  get_uint64_attr(xml, "wPeriodicRTCUpdate", &value);
  ufs->wPeriodicRTCUpdate = value & 0xFFFF;
  get_uint8_attr(xml, "bConfigDescrLock", &ufs->bConfigDescrLock);
  get_uint32_attr(xml, "qVendorConfigCode", &ufs->qVendorConfigCode);

  value = 0;
  string = devprg_xml_get_value(xml, "LUNtoGrow");
  if (string != NULL && strncmp(string, "-1", 3) == 0)
  {
    ufs->lun_to_grow = -1;
  }
  else
  {
    result = convert_atoi(string, &value);
    if(result == 0)
      ufs->lun_to_grow = value & 0xFF;
  }

  return 0;
}

static int handle_storage_configure(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  char *data;
  size_t buf_size;
  uint32 commit = 0;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = get_dev_data (xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  if(has_storage_cfg_been_init == 0)
  {
    has_storage_cfg_been_init = 1;
    memset(&storage_cfg, 0, sizeof(storage_cfg));
  }

  result = -1;
  if(dev_data.storage_type == DEVPRG_STORAGE_UFS)
    result = ufs_parse_configure(xml_handle, &storage_cfg);
  else if(dev_data.storage_type == DEVPRG_STORAGE_SDCC)
    result = emmc_parse_configure(xml_handle, &storage_cfg);
  else
    DP_LOGE("No configuration/provisioning allowed for this device type %d",
            dev_data.storage_type);

  if (result != 0)
    goto cleanup;

  get_uint32_attr(xml_handle, "commit", &commit);
  if (commit == 1)
  {
    result = devprg_storage_configure(dev_data.storage_type, dev_data.slot,
                                      &storage_cfg);
    /* Change the flag so it is clear before the next run. */
    has_storage_cfg_been_init = 0;
    if(result != DEVPRG_SUCCESS)
    {
      DP_LOGE("Failed to configure device, type:%s, slot:%d",
              get_storage_type_string(dev_data.storage_type), dev_data.slot);
      result = -1;
      goto cleanup;
    }

  }

cleanup:
  if(result == 0)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;
  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}


static int fix_lun_gpt(enum devprg_storage_type storage_type, uint32 slot,
                       uint32 lun, uint32 grow_last_partition)
{
  dp_res_t error;
  struct devprg_storage *store_handle = NULL;

  store_handle = open_dev(storage_type, slot, lun, &error);
  if (store_handle == NULL)
  {
    DP_LOGE("G Couldn't open LUN/PhyPart %d, error %d", lun, error);
    return -1;
  }

  error = devprg_fix_gpt(store_handle, grow_last_partition);

  if (close_dev(store_handle) != 0)
  {
    DP_LOGE("G Failed to close device%c", '.');
    error = DEVPRG_ERROR_STORAGE;
  }

  if (error != DEVPRG_SUCCESS)
    return -1;

  return 0;
}

static int handle_fix_gpt(struct dp_xml_handle *xml_handle)
{
  struct dp_basic_dev_data dev_data;
  int result = 0, ack, rawmode;
  struct devprg_storage *store_handle = NULL;
  dp_res_t error = DEVPRG_SUCCESS;
  char *data;
  size_t buf_size;
  const char *string;
  uint8 grow_last_partition = 1;
  uint32 lun_to_modify = 0;
  int lun_all = 0, total_luns = 0;
  struct devprg_storage_info dev_info;

  result = get_dev_data(xml_handle, &dev_data);
  if (result != 0)
  {
    result = -1;
    goto cleanup;
  }

  result = get_uint8_attr(xml_handle, "grow_last_partition", &grow_last_partition);
  /* On error of if the attribute does not exit, default to grow. */
  if (result != 0 || grow_last_partition != 0)
    grow_last_partition = 1;

  string = devprg_xml_get_value(xml_handle, "lun");
  if (strncasecmp("all", string, 3) == 0)
  {
    lun_all = 1;
  }
  else
  {
    lun_all = 0;
    result = get_uint32_attr(xml_handle, "lun", &lun_to_modify);
    if (result != 0)
      goto cleanup;
  }

  if (lun_all == 1)
  {
    int lun;
    int failed = 0;

    store_handle = open_dev(dev_data.storage_type, dev_data.slot, DEVPRG_WHOLE_DEVICE_PARTITION, &error);
    if (store_handle == NULL || error != DEVPRG_SUCCESS)
    {
      result = -1;
      DP_LOGE("G Couldn't open store_handle%c", '.');
      goto cleanup;
    }

    error = devprg_storage_get_info(store_handle, &dev_info);
    if (error != DEVPRG_SUCCESS)
      goto cleanup;

    total_luns = dev_info.num_physical;

    if (close_dev(store_handle) != 0)
    {
      DP_LOGE("G Failed to close device LUN %d", DEVPRG_WHOLE_DEVICE_PARTITION);
      result = -1;
      goto cleanup;
    }

    for (lun = 0; lun < total_luns; lun++)
    {
      result = fix_lun_gpt(dev_data.storage_type, dev_data.slot, lun,
                           grow_last_partition);
      if (result != 0)
      {
        DP_LOGE("G Failed to fix the GPT for LUN/PhyPart %d", lun);
        failed = 1;
      }
    }
    if (failed != 0)
      result = -1;
  }
  else
  {
    result = fix_lun_gpt(dev_data.storage_type, dev_data.slot, lun_to_modify,
                         grow_last_partition);
    if (result != 0)
      DP_LOGE("G Failed to fix the GPT for LUN/PhyPart %d", lun_to_modify);
  }

cleanup:
  if (result == 0 && error == DEVPRG_SUCCESS)
    ack = 1;
  else
    ack = 0;

  rawmode = 0;

  data = devprg_get_transfer_buffer(&buf_size);
  if (data == NULL)
    return -1;

  result = send_reponse_xml(data, buf_size, ack, rawmode);
  devprg_free_transfer_buffer(data);

  return result;
}

static int devprg_get_max_supported_transfer_size(void)
{
#if (FIREHOSE_LITE_CHANNEL_BUFFER_SIZE <= FIREHOSE_DDR_CHANNEL_BUFFER_SIZE)
  return FIREHOSE_DDR_CHANNEL_BUFFER_SIZE;
#else
  return FIREHOSE_LITE_CHANNEL_BUFFER_SIZE;
#endif
}

static int handle_protocol_configure(struct dp_xml_handle *xml_handle)
{
  int result = -1;
  int ack = 1;
  char *buffer;
  size_t buf_size;
  uint64 value;
  const char *string;
  struct devprg_xml_write *xmlw;
  size_t xml_size;
  size_t max_vip_size;

  buffer = devprg_get_transfer_buffer(&buf_size);
  if (buffer == NULL)
    return -1;

  string = devprg_xml_get_value(xml_handle, "MemoryName");
  if (string != NULL)
  {
    protoc_cfg.storage_type = get_storage_type(string);
    DP_LOGI("Storage type set to value %s",
            get_storage_type_string(protoc_cfg.storage_type));
  }

  string = devprg_xml_get_value(xml_handle, "Verbose");
  if (string != NULL && string[0] == '1')
    devprg_set_log_level(LOG_DEBUG);

  result = get_uint64_attr(xml_handle, "MaxPayloadSizeToTargetInBytes", &value);
  if (result == 0)
  {
    protoc_cfg.max_transfer_size = devprg_get_max_supported_transfer_size();
    if (value > protoc_cfg.max_transfer_size)
    {
      DP_LOGW("NAK: MaxPayloadSizeToTargetInBytes sent by host %d larger than supported %d",
              value, protoc_cfg.max_transfer_size);
      ack = 0;
      protoc_cfg.transfer_size = protoc_cfg.max_transfer_size;
    }
    else
    {
      protoc_cfg.transfer_size = value;
    }
  }

  string = devprg_xml_get_value(xml_handle, "AlwaysValidate");
  if (string != NULL && string[0] == '1')
    devprg_transfer_set_always_validate(1);
  else
    devprg_transfer_set_always_validate(0);

  max_vip_size = devprg_transfer_get_max_table_size();
  result = get_uint64_attr(xml_handle, "MaxDigestTableSizeInBytes", &value);
  if (result == 0 && value > max_vip_size)
  {
    DP_LOGE("NAK: Host wants to send a Hash table %d larger than supported %d",
            value, max_vip_size);
    ack = 0;
  }

  string = devprg_xml_get_value(xml_handle, "ZlpAwareHost");
  if (string != NULL && string[0] == '0')
  {
    DP_LOGD("Calling usb_al_bulk_set_zlp_mode(FALSE), zlp is set to %d", 0);
    devprg_transfer_set_zlp_mode(0);
  }
  else
  {
    DP_LOGD("Calling usb_al_bulk_set_zlp_mode(TRUE), zlp is not set to %d", 0);
    devprg_transfer_set_zlp_mode(1);
  }

  string = devprg_xml_get_value(xml_handle, "SkipWrite");
  if (string != NULL && string[0] == '1')
  {
    DP_LOGI("SkipWrites is set, Setting the device as NULL device%c", '.');
    protoc_cfg.storage_type = DEVPRG_STORAGE_NULL;
  }


  xmlw = devprg_xml_resp_open(buffer, buf_size);
  if (xmlw == NULL)
  {
    result = -1;
    goto cleanup;
  }

  devprg_xml_resp_tag(xmlw, "response");
  devprg_xml_resp_ack(xmlw, ack);
  devprg_xml_resp_attr_s(xmlw, "MemoryName",
                         get_storage_type_string (protoc_cfg.storage_type));
  devprg_xml_resp_attr_i(xmlw, "MinVersionSupported", 1);
  devprg_xml_resp_attr_i(xmlw, "Version", 1);
  devprg_xml_resp_attr_i(xmlw, "MaxPayloadSizeToTargetInBytes", protoc_cfg.transfer_size);
  devprg_xml_resp_attr_i(xmlw, "MaxPayloadSizeToTargetInBytesSupported", protoc_cfg.max_transfer_size);
  devprg_get_xml_buffer(&xml_size);
  devprg_xml_resp_attr_i(xmlw, "MaxXMLSizeInBytes", xml_size);
  devprg_xml_resp_attr_s(xmlw, "DateTime", __DATE__ " - " __TIME__);
  devprg_xml_resp_close(xmlw);

  result = send_reponse_buffer(buffer, strlen(buffer));
cleanup:
  devprg_free_transfer_buffer(buffer);

  return result;
}

struct devprg_firehose_cmd_list cmd_handlers[] =
{
  {"program", handle_write},
  {"read", handle_read},
  {"nop", handle_nop},
  {"patch", handle_patch},
  {"configure", handle_protocol_configure},
  {"setbootablestoragedrive", handle_set_bootable_drive},
  {"erase", handle_erase},
  {"power", handle_power},
  {"firmwarewrite", handle_firmware_update},
  {"getstorageinfo", handle_storage_info},
  {"benchmark", handle_benchmark},
#ifdef FEATURE_DEVPRG_PEEK_POKE
  {"peek", handle_peek},
  {"poke", handle_poke},
#endif
  {"emmc", handle_storage_configure},
  {"ufs", handle_storage_configure},
  {"fixgpt", handle_fix_gpt},
};

static void log_basic_dev_characteristics(void)
{
  uint32 serial_num;
  int res;
  size_t i;
  size_t num_handlers = sizeof(cmd_handlers) /
    sizeof(struct devprg_firehose_cmd_list);

  DP_LOGI("Binary build date: %s @ %s\n", __DATE__, __TIME__);

  res = devprg_get_main_serial_number(&serial_num);
  if (res == 0)
    DP_LOGI("Chip serial num: %u (0x%x)", serial_num, serial_num);

  DP_LOGI("Supported Functions (%d):", num_handlers);
  for (i = 0; i < num_handlers; i++)
    DP_LOGI("%s", cmd_handlers[i].tag);

  DP_LOGI("End of supported functions %d", i);
}


int handle_xml(char *xml_buffer, size_t size)
{
  struct dp_xml_handle *xml;
  const char *tag;
  int result = -1;
  size_t num_handlers = sizeof(cmd_handlers) /
    sizeof(struct devprg_firehose_cmd_list);
  size_t i;

  xml = devprg_xml_command_open(xml_buffer, size);
  if (xml == NULL)
    return -1;

  tag = devprg_xml_get_tag(xml);
  /* Scan through each of the handlers to see who handles this tag. */
  for (i = 0; i < num_handlers; i++)
  {
    if (strncasecmp(tag, cmd_handlers[i].tag, size) == 0)
    {
      DP_LOGI("Calling handler for %s", tag);
      result = cmd_handlers[i].handler(xml);
      break;
    }
  }

  devprg_xml_command_close(xml);
  return result;
}



void devprg_hw_init(void)
{
  int result;

  result = devprg_target_init();

  devprg_transfer_init();

  if (result != DEVPRG_SUCCESS)
    DP_LOGE("DEVPRG hw init failed %d\n", result);

}

void main_loop(void)
{
  int result;
  int64 read_size;
  char *xml_buffer;
  size_t xml_buf_size;

  devprg_buffers_init();
  devprg_hw_init();
  devprg_xml_init();
  devprg_storage_init();

  DP_LOGI("Binary build date: %s @ %s", __DATE__, __TIME__);
  log_basic_dev_characteristics();
  xml_buffer = devprg_get_xml_buffer(&xml_buf_size);

  for(;;)
  {
    read_size = devprg_rx_blocking(xml_buffer, xml_buf_size);
    if (read_size > 0)
    {
      result = handle_xml(xml_buffer, read_size);
      if (result != 0)
        DP_LOGE("Failed to run the last command %d", result);
    }
    else
    {
      DP_LOGE("Failed to read XML command %d", read_size);
    }
  }
}

#include "boot_sbl_if.h"
void deviceprogrammer_entry(boot_pbl_shared_data_type *pbl_shared)
{
  (void) pbl_shared;
  main_loop();
  while(1);
}
