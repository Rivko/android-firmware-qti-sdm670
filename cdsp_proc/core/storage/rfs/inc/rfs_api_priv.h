/***********************************************************************
 * rfs_api_priv.h
 *
 * Private APIs in the RFS module.
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * Private APIs in the RFS module. These APIs are not exposed to outside
 * clients.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/inc/rfs_api_priv.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-10-20   rp    Create

===========================================================================*/

#ifndef __RFS_API_PRIV_H__
#define __RFS_API_PRIV_H__

#include "comdef.h"
#include "rfs_config_i.h"

PACKED struct rfs_config_info_type
{
  uint32 block_size;
  uint32 window_size;
  uint32 timeout_in_ms;
  uint32 max_pkt_retry_count;
  uint32 max_wrong_pkt_count;
} PACKED_POST;

int rfs_set_config (struct rfs_config_info_type *config_info);
int rfs_get_config (struct rfs_config_info_type *config_info);

PACKED struct rfs_debug_info_type
{
  uint32 block_size;
  uint64 total_blocks;
  uint64 total_api_time;
  uint64 sock_open_time;
  uint64 req_time;
  uint64 data_xfer_time;
  uint64 total_tx_time;
  uint64 total_rx_time;
  uint64 min_tx_time;
  uint64 max_tx_time;
  uint64 min_rx_time;
  uint64 max_rx_time;
} PACKED_POST;


typedef int32 (*RFS_PUT_CALLBACK_FUNC_TYPE) (void *param,
               uint16 block_number, uint8 **data_buf);

int32 rfs_put_debug (const char *path, void *data, uint32 length, int32 offset,
                     int oflag, int mode, RFS_PUT_CALLBACK_FUNC_TYPE cb_func,
                     void *cb_param, struct rfs_debug_info_type *debug_info);


typedef int32 (*RFS_GET_CALLBACK_FUNC_TYPE) (void *param,
               uint16 block_number, uint8 *data_buf, uint32 data_buf_size);

int32 rfs_get_debug (const char *path, void *data, uint32 length, int32 offset,
                     RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                     struct rfs_debug_info_type *debug_info);

#endif /* not __RFS_API_PRIV_H__ */
