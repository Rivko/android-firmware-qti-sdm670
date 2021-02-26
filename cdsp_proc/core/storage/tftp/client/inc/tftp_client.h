/***********************************************************************
 * tftp_client.h
 *
 * Qualcomm TFTP Client implemenation.
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Implemenation of the qualcomm tftp client APIs. These APIs can be hooked
 * to RFS APIs to provide POSIX like Filesystem implemenation to FS clients.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/client/inc/tftp_client.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-10-20   dks   Remove rfs and tftp dependency
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-09-19   dks   Add hooks to extract performance numbers.
2014-06-04   rp    Switch to IPCRouter sockets.
2014-01-20   rp    Add support to do partial-file reads.
2014-01-16   rp    Handle error-pkt received from server.
2013-12-26   rp    Add tftp-client module.
2013-11-15   dks   Add new tftp APIs to support RFS APIs.
2013-11-14   rp    Create

===========================================================================*/

#ifndef __TFTP_CLIENT_H__
#define __TFTP_CLIENT_H__

#include "comdef.h"
#include "rfs_utils.h"

enum tftp_client_result_type
{
  TFTP_CLIENT_RESULT_UNKNOWN_ERROR,
  TFTP_CLIENT_RESULT_SUCCESS,
  TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS,
  TFTP_CLIENT_RESULT_ENOMEM,
  TFTP_CLIENT_RESULT_ENOENT,
  TFTP_CLIENT_RESULT_EPERM,
  TFTP_CLIENT_RESULT_ENOSPC,
  TFTP_CLIENT_RESULT_EEXIST,
  TFTP_CLIENT_RESULT_TIME_OUT,
  TFTP_CLIENT_RESULT_PKT_ERROR,
  TFTP_CLIENT_RESULT_RECD_WRONG_PKT,
  TFTP_CLIENT_RESULT_NOT_SUPPORTED,
  TFTP_CLIENT_RESULT_ERROR_PKT_RECD,
  TFTP_CLIENT_RESULT_NO_SERVER,
  TFTP_CLIENT_RESULT_INVALID_ARG,
  TFTP_CLIENT_RESULT_ECONFIG
};

PACKED struct tftp_client_config_type
{
  uint32 block_size;
  uint32 window_size;
  uint32 timeout_in_ms;
  uint32 max_pkt_retry_count;
  uint32 max_wrong_pkt_count;
} PACKED_POST;

enum tftp_client_result_type tftp_client_init (void);

typedef int32 (*tftp_client_send_data_cb_type) (void *param,
               uint16 block_number, uint8 **data_buf);

typedef int32 (*tftp_client_recv_data_cb_type) (void *param,
               uint16 block_number, uint8 *data_buf, uint32 data_buf_size);

enum tftp_client_result_type tftp_put (const char *filename,
         const void *buf, uint32 in_buf_size, uint32 *out_buf_size,
         tftp_client_send_data_cb_type cb_func, void *cb_param,
         struct rfs_debug_info_type *debug_info);

enum tftp_client_result_type tftp_get (const char *filename,
         void *buf, uint32 in_buf_size, uint32 *out_buf_size,
         tftp_client_recv_data_cb_type cb_func, void *cb_param,
         struct rfs_debug_info_type *debug_info);

enum tftp_client_result_type tftp_read (const char *filename, uint32 offset,
                                       void *buf, uint32 in_buf_size,
                                       uint32 *out_buf_size,
                                       struct rfs_debug_info_type *debug_info);

enum tftp_client_result_type tftp_write (const char *filename, uint32 offset,
                                       int append_mode, const void *buf,
                                       uint32 in_buf_size,
                                       uint32 *out_buf_size,
                                       struct rfs_debug_info_type *debug_info);

enum tftp_client_result_type tftp_stat (const char *filename,
            uint32 *filesize);

enum tftp_client_result_type tftp_unlink (const char *filename);

int tftp_client_config_set (struct tftp_client_config_type *config);

int tftp_client_config_get (struct tftp_client_config_type *config);


#endif /* __TFTP_CLIENT_H__ */
