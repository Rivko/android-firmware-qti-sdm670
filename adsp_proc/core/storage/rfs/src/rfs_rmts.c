/***********************************************************************
 * rfs_rmts.c
 *
 * RFS wrapper for Remote Storage Server.
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Remote File System wrapper layer on top of Remote Storage server. Maps the
 * RFS APIs and data structres to RMTS APIs data structures and vice versa.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_rmts.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2014-12-03   dks   Move rfs init to a task
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-01-24   dks   Create

===========================================================================*/

#include "rfs_config_i.h"

#ifdef FEATURE_RFS_USE_RMTS

#include "rfs_rmts.h"
#include "rfs_ops.h"
#include "rfs_errno.h"
#include "fs_os.h"
#include "comdef.h"
#include "rfs_assert.h"
#include "rfs_api_priv.h"
#include <string.h>
#include "remotefs_qmi_server.h"
#include "remotefs_comm_api.h"

struct rfs_rmts_info_type
{
  fs_os_mutex_t cs;
  int rmts_connect_done;
};

static struct rfs_rmts_info_type rfs_rmts_info;

static remotefs_status_type
rfs_rmts_connect (void)
{
  remotefs_status_type result = RMFS_NO_ERROR;

  fs_os_mutex_lock (&rfs_rmts_info.cs);

  if (!rfs_rmts_info.rmts_connect_done)
  {
    result = rmtfs_refresh_file_mapping ();
    if (result == RMFS_NO_ERROR)
    {
      rfs_rmts_info.rmts_connect_done = 1;
    }
  }

  fs_os_mutex_unlock (&rfs_rmts_info.cs);

  return result;
}

static int
map_rmts_to_rfs_errno (remotefs_status_type rmts_errno)
{
  switch (rmts_errno)
  {
    case RMFS_NO_ERROR:
      return RFS_ENOERROR;

    case RMFS_ERROR_PARAM:
      return RFS_EINVAL;

    case RMFS_ERROR_BUSY:
      return RFS_EBUSY;

    case RMFS_ERROR_PIPE:
    case RMFS_ERROR_UNINIT:
    case RMFS_ERROR_DEVICE:
      return RFS_ESYSTEM;

    default:
      RFS_ASSERT (0);
      return RFS_ESYSTEM;
  }
}

static int32
rfs_rmts_put (const char *path, void *data, uint32 length, int oflag)
{
  int32 result;
  remotefs_status_type rmts_result;
  (void) oflag;

  rmts_result = rfs_rmts_connect ();
  if (rmts_result == RMFS_NO_ERROR)
  {
    rmts_result = rmtfs_write_file (path, data, length);
  }

  if (rmts_result != RMFS_NO_ERROR)
  {
    result = map_rmts_to_rfs_errno (rmts_result);
  }
  else
  {
    result = length;
  }

  return result;
}

static int32
rfs_rmts_get (const char *path, void *data, uint32 length)
{
  int32 result;
  remotefs_status_type rmts_result;

  rmts_result = rfs_rmts_connect ();
  if (rmts_result == RMFS_NO_ERROR)
  {
    rmts_result = rmtfs_read_file (path, data, length);
  }

  if (rmts_result != RMFS_NO_ERROR)
  {
    result = map_rmts_to_rfs_errno (rmts_result);
  }
  else
  {
    result = length;
  }

  return result;
}

static int32
rfs_rmts_invalid_read (const char *filename, uint32 offset,
                       void *buf, uint32 nbyte)
{
  (void) filename;
  (void) offset;
  (void) buf;
  (void) nbyte;
  return RFS_EPERM;
}

static int32
rfs_rmts_invalid_write (const char *filename, uint32 offset, int oflag,
                        const void *buf, uint32 nbyte)
{
  (void) filename;
  (void) offset;
  (void) oflag;
  (void) buf;
  (void) nbyte;
  return RFS_EPERM;
}

static int
rfs_rmts_invalid_unlink (const char *path)
{
  (void) path;
  return RFS_EPERM;
}

static int
rfs_rmts_invalid_stat (const char *path, struct rfs_stat_buf *buf)
{
  (void) path;
  (void) buf;
  return RFS_EPERM;
}

int32
rfs_rmts_put_debug (const char *path, void *data, uint32 length, int32 offset,
		   int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
		   struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length; (void) oflag;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

int32
rfs_rmts_get_debug (const char *path, void *data, uint32 length, int32 offset,
		                RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
             		    struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

int
rfs_rmts_set_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return RFS_ENOTSUPPORTED;
}

int
rfs_rmts_get_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return -RFS_ENOTSUPPORTED;
}

int
rfs_rmts_init (struct rfs_ops_type *base_ops)
{
  RFS_ASSERT (base_ops != NULL);
  if (base_ops == NULL)
  {
    return -1;
  }

  memset (base_ops, 0x0, sizeof (struct rfs_ops_type));

  base_ops->read = rfs_rmts_invalid_read;
  base_ops->write = rfs_rmts_invalid_write;
  base_ops->unlink = rfs_rmts_invalid_unlink;
  base_ops->stat = rfs_rmts_invalid_stat;
  base_ops->put = rfs_rmts_put;
  base_ops->get = rfs_rmts_get;
  base_ops->put_debug = rfs_rmts_put_debug;
  base_ops->get_debug = rfs_rmts_get_debug;
  base_ops->set_config = rfs_rmts_set_config;
  base_ops->get_config = rfs_rmts_get_config;

  memset (&rfs_rmts_info, 0x0, sizeof (rfs_rmts_info));
  fs_os_mutex_init (&rfs_rmts_info.cs);

  return 0;
}

#endif /* FEATURE_RFS_USE_EFS */


