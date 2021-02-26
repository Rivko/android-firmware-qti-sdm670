/**************************************************************************
 * FILE: devprg_buffers_ddr.c
 *
 * This file contains the buffers used for USB transfers and Storage IO
 * that are allocated in DDR. Having a separate file makes it easy to
 * pace the bss of this file in one region.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprgb_buffers_ddr.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-12-21   wek   Create
===========================================================================*/

#include "devprg_firehose.h"

#define ALGNB DEVPRG_BUFFER_ALIGNMENT

static char devprg_ddr_buffer[DEVPRG_MAX_TRANSFER_BUFFERS][FIREHOSE_DDR_CHANNEL_BUFFER_SIZE] __attribute__((__aligned__(ALGNB)));


char *devprg_get_ddr_buffer(size_t index, size_t *size)
{
  *size = sizeof(devprg_ddr_buffer[index]);
  return devprg_ddr_buffer[index];
}
