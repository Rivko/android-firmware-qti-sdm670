/***********************************************************************
 * rfs_nodev.c
 *
 * RFS Nodev Operations.
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Stub operations which return error for all RFS APIs when there are no
 * Filessytem implementing RFS APIs
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_nodev.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-12-03   dks   Move rfs init to a task
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-07-29   dks   Fix error-codes.
2013-11-12   dks   Create

===========================================================================*/

#include "rfs_nodev.h"
#include "rfs_ops.h"
#include "rfs_errno.h"
#include "rfs_assert.h"

static int32
rfs_nodev_read (const char * path, uint32 offset, void *buf, uint32 nbyte)
{
  (void) path;
  (void) offset;
  (void) buf;
  (void) nbyte;
  return RFS_ECONFIG;
}

static int32
rfs_nodev_write (const char * path, uint32 offset, int oflag, const void *buf,
                 uint32 nbyte)
{
  (void) path;
  (void) offset;
  (void) oflag;
  (void) nbyte;
  return RFS_ECONFIG;
}

static int
rfs_nodev_unlink (const char *path)
{
  (void) path;
  return RFS_ECONFIG;
}

static int
rfs_nodev_stat (const char *path, struct rfs_stat_buf *buf)
{
  (void) path;
  (void) buf;
  return RFS_ECONFIG;
}

static int32
rfs_nodev_put (const char *path, void *data, uint32 length, int oflag)
{
  (void) path;
  (void) data;
  (void) length;
  (void) oflag;
  return RFS_ECONFIG;
}

static int32
rfs_nodev_get (const char *path, void *data, uint32 length)
{
  (void) path;
  (void) data;
  (void) length;
  return RFS_ECONFIG;
}

int32
rfs_nodev_put_debug (const char *path, void *data, uint32 length, int32 offset,
       int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
       struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length; (void) oflag;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return -RFS_ECONFIG;
}

int32
rfs_nodev_get_debug (const char *path, void *data, uint32 length, int32 offset,
                     RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                     struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return -RFS_ECONFIG;
}

int
rfs_nodev_set_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return -RFS_ECONFIG;
}

int
rfs_nodev_get_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return -RFS_ECONFIG;
}

int
rfs_nodev_init (struct rfs_ops_type *base_ops)
{
  ASSERT (base_ops != NULL);
  if (base_ops == NULL)
  {
    return -1;
  }

  memset (base_ops, 0x0, sizeof (struct rfs_ops_type));

  base_ops->read = rfs_nodev_read;
  base_ops->write = rfs_nodev_write;
  base_ops->unlink = rfs_nodev_unlink;
  base_ops->stat = rfs_nodev_stat;
  base_ops->put = rfs_nodev_put;
  base_ops->get = rfs_nodev_get;
  base_ops->put_debug = rfs_nodev_put_debug;
  base_ops->get_debug = rfs_nodev_get_debug;
  base_ops->set_config = rfs_nodev_set_config;
  base_ops->get_config = rfs_nodev_get_config;

  return 0;
}
