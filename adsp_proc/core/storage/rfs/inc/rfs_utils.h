/***********************************************************************
 * rfs_utils.h
 *
 * Utility functions to create and verify files using RFS APIs.
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Utility functions to create and verify files using RFS APIs.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/inc/rfs_utils.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-04   rp    Create

===========================================================================*/

#ifndef __RFS_UTILS_H__
#define __RFS_UTILS_H__

#include "comdef.h"
#include "rfs_api.h"
#include "rfs_api_priv.h"

int rfs_util_create_file (const char *filename, uint32 file_size,
                          uint32 write_size, uint32 write_size_incr,
                          uint32 buf_seed,
                          int verify_file_after_each_write);

int rfs_util_create_file_from_buf (const char *filename, uint8 *file_buf,
            uint32 file_size, uint32 write_size, uint32 write_size_incr);

int rfs_util_verify_file_in_chunks (const char *filename, const uint8 *file_buf,
                                    uint32 file_size, uint32 read_size,
                                    uint32 read_size_incr);

int rfs_util_verify_file (const char *filename, const uint8 *file_buf,
                          uint32 file_size);

int rfs_util_put (const char *filename, uint32 file_size, uint32 buf_seed,
                  int oflag, int mode, int verify_file,
                  struct rfs_debug_info_type *dbg_info);

int rfs_util_get (const char *filename, uint32 file_size, uint32 buf_seed,
                  int verify_file, struct rfs_debug_info_type *dbg_info);

int rfs_util_write (const char *filename, uint32 file_size, uint32 write_size,
                    uint32 buf_seed, int oflag, int mode,
                    struct rfs_debug_info_type *debug_info);

int rfs_util_read (const char *filename, uint32 offset, uint32 read_size,
                   struct rfs_debug_info_type *debug_info);

#endif /* not __RFS_UTILS_H__ */
