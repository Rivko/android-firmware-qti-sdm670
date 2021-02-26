/**************************************************************************
 * FILE: devprg_buffers_lite.c
 *
 * This file contains the buffers used for USB transfers and Storage IO
 * that are not allocated in DDR. Having a separate file makes it easy to
 * pace the bss of this file in one region.
 *
 * Copyright (c) 2016-2017 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprgb_buffers_lite.c#5 $
  $DateTime: 2020/02/20 22:42:44 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2020-02-20   sb    remove VIP table SIZE here and add in inf file.
2017-09-26   sb    VIP with 4K Hi exponent Hash support
2016-12-21   wek   Create
===========================================================================*/

#include <string.h>
#include "devprg_firehose.h"
#include "devprg_log.h"

#define DEVPRG_MAX_XML_SIZE   4096
#define DEVPRG_LOG_SIZE       4096

#define ALGNB DEVPRG_BUFFER_ALIGNMENT

/* Declaration of the function declared in the ddr file. */
char *devprg_get_ddr_buffer(size_t index, size_t *size);

static char devprg_lite_buffer[DEVPRG_MAX_TRANSFER_BUFFERS][FIREHOSE_LITE_CHANNEL_BUFFER_SIZE] __attribute__((__aligned__(ALGNB)));
static char devprg_buffer_in_use[DEVPRG_MAX_TRANSFER_BUFFERS] = {0};
static char devprg_use_ddr = 0;
static char devprg_log_buffer[DEVPRG_LOG_SIZE] __attribute__((__aligned__(ALGNB)));
static char devprg_xml_buffer[DEVPRG_MAX_XML_SIZE] __attribute__((__aligned__(ALGNB)));
static char devprg_vip_hash_table[DEVPRG_VIP_TABLE_SIZE] __attribute__((__aligned__(ALGNB)));




char *devprg_get_xml_buffer(size_t *size)
{
  *size = sizeof(devprg_xml_buffer);
  return devprg_xml_buffer;
}

char *devprg_get_log_buffer(size_t *size)
{
  *size = sizeof(devprg_log_buffer);
  return devprg_log_buffer;
}

char *devprg_get_vip_buffer(size_t *size)
{
  *size = sizeof(devprg_vip_hash_table);
  return devprg_vip_hash_table;
}

static char *devprg_get_lite_buffer(size_t index, size_t *size)
{
  *size = sizeof(devprg_lite_buffer[index]);
  return devprg_lite_buffer[index];
}

char *devprg_get_transfer_buffer(size_t *size)
{
  int i;
  for (i = 0; i < DEVPRG_MAX_TRANSFER_BUFFERS; i++)
  {
    if (devprg_buffer_in_use[i] == 0)
    {
      devprg_buffer_in_use[i] = 1;
      if (devprg_use_ddr == 1)
        return devprg_get_ddr_buffer(i, size);
      else
        return devprg_get_lite_buffer(i, size);
    }
  }
  DP_LOGE("Failed to get a buffer for transfer", i);
  return NULL;
}

void devprg_free_transfer_buffer(char *buffer_to_free)
{
  int i;
  char* buf;
  size_t size;
  for (i = 0; i < DEVPRG_MAX_TRANSFER_BUFFERS; i++)
  {
    if (FIREHOSE_LITE_CHANNEL_BUFFER_SIZE >= FIREHOSE_DDR_CHANNEL_BUFFER_SIZE)
      buf = devprg_get_lite_buffer(i, &size);
    else
      buf = devprg_get_ddr_buffer(i, &size);

    if (buf == buffer_to_free)
    {
      if (devprg_buffer_in_use[i] == 0)
        DP_LOGE("Trying to free an already freed buffer %d", i);
      devprg_buffer_in_use[i] = 0;
      return;
    }
  }
}


#ifdef FEATURE_DEVICEPROGRAMMER_INIT_DDR_TEST
int memset_verify (byte *buffer, byte val, int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    if (buffer[i] != val)
      return -1;
  }
  return 0;
}

int ddr_set_and_test (byte val)
{
  byte *channelA;
  byte *channelB;
  uint32 channel_size;
  int result;
  size_t size;
  uint32 max_verify_size = 64*1024;

  channelA = devprg_get_ddr_buffer(0, &size);
  channelB = devprg_get_ddr_buffer(1, &size);

  /* Takes to long to fill all the buffers, fill at most 64K */
  if (size > max_verify_size)
    channel_size = max_verify_size;
  else
    channel_size = size;

  memset (channelA, val, channel_size);
  memset (channelB, val, channel_size);

  result = memset_verify (channelA, val, channel_size);
  if (result != 0)
    return -1;
  result = memset_verify (channelB, val, channel_size);
  if (result != 0)
    return -1;

  return 0;
}

int ddr_fill_and_check_test (void)
{
  byte *channelA;
  byte *channelB;
  uint32 channel_size;
  int i;
  byte val;
  uint32 max_verify_size = 64*1024;

  channelA = devprg_get_ddr_buffer(0, &size);
  channelB = devprg_get_ddr_buffer(1, &size);
  /* Takes to long to fill all the buffers, fill at most 64K */
  if (size > max_verify_size)
    channel_size = max_verify_size;
  else
    channel_size = size;

  val = 0;
  /* Fill the buffers with some pattern. */
  for (i = 0; i < channel_size; i++)
  {
    channelA[i] = val;
    channelB[i] = val;
    val++;
     /* Make the counter wrap around at an odd value. This causes that same
        value won't fall in the same "column" in memory. If we use a multiple
        of 2, then the same value will fall in the same colum every time. */
    if (val >= 253)
      val = 0;
  }
  val = 0;
   /* Verify the pattern that we just filled. */
  for (i = 0; i < channel_size; i++)
  {
    /* The buffers should be filled with an specific pattern. Verify that pattern. */
    if (channelA[i] != val || channelB[i] != val)
      return -1;

    val++;
    if (val >= 253)
      val = 0;
  }

  return 0;
}


/* This feature will run some tests on DDR. If the test fail it will fall back
 * and use the Lite buffers and return a NAK for any command sent. */
static int basic_test_ddr(void)
{
  int result;

  /* Set DDR to some values and read back the pattern to make sure it is there. */
  result = ddr_set_and_test (0x55);
  if (result != 0)
    return -1;
  result = ddr_set_and_test (0xAA);
  if (result != 0)
    return -1;

  /* Fill the buffers with a non repetitive pattern and verify it */
  result = ddr_fill_and_check_test ();
  if (result != 0)
    return -1;

  /* Last do the test filling the memory to zero, esentially initializing it. */
  result = ddr_set_and_test (0x00);
  if (result != 0)
    return -1;

  return 0;
}
#else
static int basic_test_ddr(void)
{
  return 0;
}
#endif

void devprg_buffers_init(void)
{
  int result;

  memset(devprg_buffer_in_use, 0, sizeof(devprg_buffer_in_use));

  if (FIREHOSE_LITE_CHANNEL_BUFFER_SIZE >= FIREHOSE_DDR_CHANNEL_BUFFER_SIZE)
  {
    devprg_use_ddr = 0;
  }
  else
  {
    result = basic_test_ddr();
    if (result == 0)
    {
      devprg_use_ddr = 1;
    }
    else
    {
      devprg_use_ddr = 0;
      DP_LOGE("DDR validation failed, using Lite buffers", result);
    }
  }
}
