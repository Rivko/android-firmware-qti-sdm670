/**************************************************************************
 * FILE: devprg_transfer.c
 *
 * Transfer abstraction layer for TX and RX. Includes VIP.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_transfer.c#3 $
  $DateTime: 2018/01/31 00:50:24 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-09-11   svl   Fix initialization order of VIP structure.
2017-01-04   wek   Create. (22-m)

===========================================================================*/

#include "devprg_log.h"
#include "devprg_target.h"
#include "devprg_security.h"
#include "devprg_transfer.h"
#include "devprg_usb.h"
#include "devprg_xml.h"
#include <stdio.h>
#include <string.h>

#define DEVPRG_HASH_SIZE  32

enum vip_state
{
  VIP_INIT, /* Waiting for the initial VIP Signed table. */
  VIP_GET_NEXT_TABLE, /* The next received chunk will be a new Hash table */
  VIP_DATA, /* The next data chunk will be data to validate.  */
  VIP_DISABLED, /* Don't use VIP, go directly through USB. */
  VIP_MAX, /* for bounds check. */
};

struct vip_data
{
  enum vip_state state;
  int curr_index;
  int max_index;
  int max_hashes;
  uint8 always_hash;
  size_t table_size;
  size_t buffer_size;
  uint8 *table;
  uint8 *payload;
  uint8 *curr_hash;
  uint64 packet_count;
  uint64 table_count;
};

static struct vip_data vip_data;


void devprg_init_vip_state(void)
{
  struct vip_data *vip = &vip_data;
  vip->table = (uint8 *)devprg_get_vip_buffer(&vip->buffer_size);
  vip->table_size = 8192;
  vip->curr_hash = vip->table;
  vip->max_index = 0;
  vip->max_hashes = 54;
  vip->curr_index = 0;
  vip->packet_count = 0;
  vip->table_count = 0;
  vip->state = VIP_INIT;
}

void devprg_log_hash(uint8 *data, size_t size)
{
  char buffer[33*3] = {0};
  size_t i, offset, remain;
  int result;

  offset = 0;
  remain = sizeof(buffer);
  for(i = 0; i < size; i++)
  {
    result = snprintf(&buffer[offset], remain, "%02X ", data[i]);
    if (result > 0)
    {
      offset += result;
      remain -= result;
    }
    else
    {
      break;
    }

    if (remain < 4)
    {
      DP_LOGI("%s", buffer);
      offset = 0;
      remain = sizeof(buffer);
    }
  }

  if (offset > 0)
    DP_LOGI("%s", buffer);

}

int devprg_vip_send_xml_response(int ack)
{
  char *xml_buf;
  size_t xml_size;
  int rawmode;
  int res;
  dp_res_t rstatus;

  /* Hack to save space to reuse the log buffer. */
  xml_buf = devprg_get_log_buffer(&xml_size);
  rawmode = 0;
  res = devprg_xml_build_response(xml_buf, xml_size, ack, rawmode);
  if (res <= 0)
    return res;

  rstatus = devprg_usb_tx_blocking(xml_buf, res);
  if (rstatus != DEVPRG_SUCCESS)
    return -1;

  return 0;
}

dp_res_t devprg_rx_queue(void *data, size_t size)
{
  struct vip_data *vip = &vip_data;
  dp_res_t rstatus = DEVPRG_ERROR_AUTHENTICATION;
  int64 rsize;
  int ack;

  if (vip->state == VIP_DISABLED)
  {
    vip->payload = data;
    return devprg_usb_rx_queue(data, size);
  }

  if(vip->state == VIP_INIT)
  {
    int result;
    uint8 *table;
    uint32 table_size;
    /* We need to receive the hash table and authenticate it. */
    DP_LOGI("VIP is enabled, receiving the signed table of size %d",
            vip->table_size);
    rsize = devprg_usb_rx_blocking(vip->table, vip->buffer_size);
    if (rsize < 0)
    {
      DP_LOGE("Failed to receive signed hash table %d %d", rsize, vip->buffer_size);
      rstatus = DEVPRG_ERROR_RECEIVE;
      goto error;
    }

    result = devprg_authenticate_blob(vip->table, rsize, &table, &table_size);
    if (result == 0)
    {
      DP_LOGE("Authentication of signed hash failed %d", result);
      rstatus = DEVPRG_ERROR_AUTHENTICATION;
      goto error;
    }

    vip->curr_hash = table;
    vip->max_index = table_size / DEVPRG_HASH_SIZE;
    vip->max_hashes = 54;
    vip->curr_index = 0;
    DP_LOGD("Number of VIP hashes %d on initial table", vip->max_index);
    if (vip->max_index > vip->max_hashes)
    {
      DP_LOGE("VIP table contains more than %d hashes, %d/%d=%d",
              vip->max_hashes, table_size, DEVPRG_HASH_SIZE, vip->max_index);
      rstatus = DEVPRG_ERROR_RECEIVE;
      goto error;
    }
    vip->state = VIP_DATA;
    vip->packet_count = 1;
    vip->table_count = 1;

    ack = 1;
    devprg_vip_send_xml_response(ack);
  }

  if (vip->state == VIP_GET_NEXT_TABLE)
  {
    uint8 expected_hash[DEVPRG_HASH_SIZE];
    uint8 calculated_hash[DEVPRG_HASH_SIZE];

    memcpy (expected_hash, vip->curr_hash, sizeof(expected_hash));

    rsize = devprg_usb_rx_blocking(vip->table, vip->table_size);
    if (rsize < 0)
    {
      DP_LOGE("Failed to read the next hash table %d, packet %d",
              vip->table_count, vip->packet_count);
      devprg_init_vip_state();
      rstatus = DEVPRG_ERROR_RECEIVE;
      goto error;
    }

    memset(calculated_hash, 0, sizeof(calculated_hash));
    devprg_sha256_direct(vip->table, rsize, calculated_hash);
    if (memcmp (expected_hash, calculated_hash, sizeof(calculated_hash)) != 0)
    {
      DP_LOGE("Hash of new table doesn't match the expected hash %d", rsize);
      DP_LOGI("Expected hash (%d)", sizeof(expected_hash));
      devprg_log_hash(expected_hash, sizeof(expected_hash));
      DP_LOGI("Calculated hash (%d)", sizeof(calculated_hash));
      devprg_log_hash(calculated_hash, sizeof(calculated_hash));
      DP_LOGI("Packets received %d", vip->packet_count);
      DP_LOGI("Tables received %d", vip->table_count);
      DP_LOGI("Make sure that the same command line arguments were used when --createdigest was run%c", '.');
      rstatus = DEVPRG_ERROR_AUTHENTICATION;
      goto error;
    }
    DP_LOGD("VIP hash table accepted %d packet %d", vip->table_count,
            vip->packet_count);
    vip->max_index = rsize / DEVPRG_HASH_SIZE;
    vip->max_hashes = vip->table_size / DEVPRG_HASH_SIZE;
    vip->curr_index = 0;
    vip->curr_hash = vip->table;
    vip->state = VIP_DATA;

    ack = 1;
    devprg_vip_send_xml_response(ack);
  }

  if (vip->state == VIP_DATA)
  {
    vip->payload = data;
    rstatus = devprg_usb_rx_queue(data, size);
    return rstatus;
  }

error:
  ack = 0;
  devprg_vip_send_xml_response(ack);
  devprg_init_vip_state();
  return rstatus;
}

dp_res_t devprg_rx_queue_wait(size_t *rx_bytes)
{
  struct vip_data *vip = &vip_data;
  dp_res_t rstatus = DEVPRG_ERROR_AUTHENTICATION;
  uint8 calculated_hash[DEVPRG_HASH_SIZE];

  if (vip->state == VIP_DISABLED)
  {
    rstatus = devprg_usb_rx_queue_wait(rx_bytes);

    if (vip->always_hash != 0)
    {
      memset(calculated_hash, 0, sizeof(calculated_hash));
      devprg_sha256_direct(vip->payload, *rx_bytes, calculated_hash);
    }

    return rstatus;
  }

  if (vip->state != VIP_DATA)
  {
    DP_LOGE("Invalid date when waiting for data %d", vip->state);
    goto error;
  }
  else
  {
    uint8 *expected_hash;
    expected_hash = vip->curr_hash;

    rstatus = devprg_usb_rx_queue_wait(rx_bytes);
    if (rstatus != DEVPRG_SUCCESS)
    {
      DP_LOGE("Failed to receive data packet %d", vip->packet_count);
      goto error;
    }

    memset(calculated_hash, 0, sizeof(calculated_hash));
    devprg_sha256_direct(vip->payload, *rx_bytes, calculated_hash);
    if (memcmp (expected_hash, calculated_hash, sizeof(calculated_hash)) != 0)
    {
      DP_LOGE("Hash of data doesn't match the expected hash %d", rx_bytes);
      DP_LOGI("Expected hash (%d)", DEVPRG_HASH_SIZE);
      devprg_log_hash(expected_hash, DEVPRG_HASH_SIZE);
      DP_LOGI("Calculated hash (%d)", sizeof(calculated_hash));
      devprg_log_hash(calculated_hash, sizeof(calculated_hash));
      DP_LOGI("Packets received %d", vip->packet_count);
      DP_LOGI("Tables received %d", vip->table_count);
      DP_LOGI("Make sure that the same command line arguments were used when --createdigest was run%c", '.');
      rstatus = DEVPRG_ERROR_AUTHENTICATION;
      goto error;
    }
    DP_LOGD("VIP data verified table %d packet %d", vip->table_count,
            vip->packet_count);
    vip->curr_index++;
    vip->curr_hash += DEVPRG_HASH_SIZE;
    /* If the table is full and this is the last hash, the current hash
     * would be of the next table. */
    if (vip->max_index >= vip->max_hashes &&
        vip->curr_index + 1 >= vip->max_index)
    {
      vip->state = VIP_GET_NEXT_TABLE;
    }
    /* If the table is not completely filled, and this is the last
     * hash, then we are done with this transfer. */
    else if (vip->max_index < vip->max_hashes &&
             vip->curr_index >= vip->max_index)
    {
      DP_LOGI("Last packet received, re-initializing VIP %d %d",
              vip->max_index, vip->max_hashes);
      devprg_init_vip_state();
    }
  }

  return rstatus;

error:
  devprg_init_vip_state();
  return rstatus;
}

int64 devprg_rx_blocking(void *data, size_t size)
{
  dp_res_t res;
  size_t ret_size;
  int64 read_size;
  struct vip_data *vip = &vip_data;

  if (vip->state == VIP_DISABLED)
    return devprg_usb_rx_blocking(data, size);

  res = devprg_rx_queue(data, size);
  if (res == DEVPRG_SUCCESS)
    res = devprg_rx_queue_wait(&ret_size);

  if (res == DEVPRG_SUCCESS)
    read_size = ret_size;
  else
    read_size = -1;

  return read_size;
}

dp_res_t devprg_tx_blocking(void *data, size_t size)
{
  return devprg_usb_tx_blocking(data, size);
}

dp_res_t devprg_transfer_set_zlp_mode(unsigned int val)
{
  return devprg_usb_set_zlp_mode(val);
}

int devprg_transfer_init(void)
{
  int secboot, result;
  struct vip_data *vip = &vip_data;

  devprg_init_vip_state();
  secboot = devprg_is_secure_boot_enabled();
  if (secboot == 0)
    vip->state = VIP_DISABLED;

  result = devprg_usb_init();

  return result;
}



int devprg_transfer_set_always_validate(int value)
{
  struct vip_data *vip = &vip_data;
  vip->always_hash = value;
  return 0;
}


size_t devprg_transfer_get_max_table_size(void)
{
  struct vip_data *vip = &vip_data;
  return vip->table_size;
}
