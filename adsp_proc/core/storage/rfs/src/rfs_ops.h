/***********************************************************************
 * rfs_ops.h
 *
 * RFS OPS
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Remote File System Ops to hook up different filesystem implemenations
 * to RFS APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_ops.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-12-03   dks   Move rfs init to a task
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-01-27   dks   Add support for RFS over RMTS APIs.
2013-11-14   dks   Create

===========================================================================*/

#ifndef __RFS_OPS_H__
#define __RFS_OPS_H__

#include "comdef.h"
#include "rfs_types.h"
#include "rfs_api_priv.h"

/* Ops table to map the RFS APIs to appropriate underlying Filesystem APIs */
struct rfs_ops_type
{
  int32 (*read) (const char *path, uint32 offset, void *buf, uint32 nbyte);

  int32 (*write) (const char *path, uint32 offset, int oflag, const void *buf,
                  uint32 nbyte);

  int (*unlink) (const char *path);

  int (*stat) (const char *path, struct rfs_stat_buf *buf);

  int32 (*put) (const char *path, void *data, uint32 length, int oflag);
  int32 (*get) (const char *path, void *data, uint32 length);

  /* Not exposed internal functions for debug and profiling. */
  int32 (*put_debug) (const char *path, void *data, uint32 length,
                      int32 offset, int oflag,
                      RFS_PUT_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                      struct rfs_debug_info_type *debug_info);

  int32 (*get_debug) (const char *path, void *data, uint32 length,
                      int32 offset,
                      RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                      struct rfs_debug_info_type *debug_info);

  int (*set_config) (struct rfs_config_info_type *config_info);

  int (*get_config) (struct rfs_config_info_type *config_info);
};

#endif /* not __RFS_OPS_H__ */
