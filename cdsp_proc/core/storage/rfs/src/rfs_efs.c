/***********************************************************************
 * rfs_efs.c
 *
 * RFS wrapper for EFS.
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Remote File System wrapper layer on top of EFS. Maps the RFS APIs and
 * data structres to EFS APIs data structures and vice versa.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_efs.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-12-03   dks   Move rfs init to a task
2014-11-18   dks   Add auto_dir support for rfs on efs.
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-01-27   dks   Add support for RFS over RMTS APIs.
2013-11-14   dks   Create

===========================================================================*/

#include "rfs_config_i.h"

#ifdef FEATURE_RFS_USE_EFS

#include "rfs_efs.h"
#include "rfs_ops.h"
#include "rfs_fcntl.h"
#include "rfs_types.h"
#include "rfs_errno.h"
#include "rfs_assert.h"
#include "fs_public.h"
#include "fs_fcntl.h"
#include "fs_sys_types.h"
#include "fs_lib.h"
#include "fs_errno.h"
#include "fs_priv_funcs.h"
#include "fs_os_malloc.h"
#include "comdef.h"
#include "stringl/stringl.h"
#include <stdlib.h>

static int
map_efs_to_rfs_errno (int error_code)
{
  switch (error_code)
  {
    case ENOENT:
      return RFS_ENOENT;
    case EACCES:
      return RFS_EACCESS;
    case ENOMEM:
      return RFS_ENOMEM;
    case EPERM:
      return RFS_EPERM;
    case EBADF:
      return RFS_EBADF;
    case EEXIST:
      return RFS_EEXISTS;
    case ENODEV:
      return RFS_ENODEV;
    case EINVAL:
      return RFS_EINVAL;
    case EMFILE:
      return RFS_EMFILE;
    case ENOSPC:
      return RFS_ENOSPC;
    case ENAMETOOLONG:
      return RFS_ENAMETOOLONG;
    default:
      return RFS_ESYSTEM;
  }
}

static int
map_rfs_to_efs_oflag (int oflag)
{
  int efs_oflag = 0;

  if ((oflag & RFS_O_RDONLY) == RFS_O_RDONLY)
    efs_oflag |= O_RDONLY;

  if ((oflag & RFS_O_WRONLY) == RFS_O_WRONLY)
    efs_oflag |= O_WRONLY;

  if ((oflag & RFS_O_RDWR) == RFS_O_RDWR)
    efs_oflag |= O_RDWR;

  if ((oflag & RFS_O_CREAT) == RFS_O_CREAT)
    efs_oflag |= O_CREAT;

  if ((oflag & RFS_O_TRUNC) == RFS_O_TRUNC)
    efs_oflag |= O_TRUNC;

  if ((oflag & RFS_O_APPEND) == RFS_O_APPEND)
    efs_oflag |= O_APPEND;

  return efs_oflag;
}

static int32
rfs_efs_read (const char *path, uint32 offset, void *buf, uint32 nbyte)
{
  int32 result = RFS_ESYSTEM;
  uint32 bytes_to_read = nbyte;
  uint32 bytes_read = 0;
  int fd;
  uint8 *data_buf =  (uint8*) buf;

  if (nbyte == 0)
  {
    return 0;
  }

  fd = efs_open (path, O_RDONLY, 0666);
  if (fd < 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    return result;
  }

  result = efs_lseek (fd, offset, SEEK_SET);
  if (result < 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    goto End;
  }
  else if (result != offset)
  {
    result = RFS_ESYSTEM;
    goto End;
  }

  while (bytes_to_read > 0)
  {
    result = efs_read (fd, data_buf + bytes_read, bytes_to_read);
    if (result < 0)
    {
      if (bytes_read == 0)
      {
        result = map_efs_to_rfs_errno (efs_errno);
      }
      goto End;
    }

    if(result == 0)
    {
      goto End;
    }

    bytes_read += result;
    bytes_to_read -= result;
  }

End:
  if (fd >= 0)
  {
    (void) efs_close (fd);
  }

  if (bytes_read > 0)
  {
    result = bytes_read;
  }

  return result;
}

static int32
rfs_efs_write (const char * path, uint32 offset, int oflag, const void *buf,
               uint32 nbyte)
{
  int32 result = RFS_ESYSTEM;
  uint32 bytes_to_write;
  uint32 bytes_written;
  int fd;
  int efs_oflag;
  uint32 path_len, i;
  char *parent_path = NULL;
  uint8* data_buf = (uint8*) buf;

  fd = -1;
  bytes_to_write = nbyte;
  bytes_written = 0;

  /* Extract parent path to mkdir */
  path_len = strlen(path);

  parent_path = fs_os_malloc (path_len + 1);
  if (parent_path == NULL)
  {
    return RFS_ENOMEM;
  }

  strlcpy (parent_path, path, path_len + 1);
  for (i = path_len - 1; i > 0; i--)
  {
    if (parent_path[i] == '/')
    {
      parent_path[i] = 0;
      break;
    }
  }

  result = efs_auto_mkdir (parent_path);
  if (result != 0)
  {
    result = map_rfs_to_efs_oflag (efs_errno);
    goto End;
  }

  /* Parent path now exists, create the file */

  efs_oflag = map_rfs_to_efs_oflag (oflag);
  fd = efs_open (path, efs_oflag, 0666);
  if (fd < 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    goto End;
  }

  result = efs_lseek (fd, offset, SEEK_SET);
  if (result < 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    goto End;
  }
  else if (result != offset)
  {
    result = RFS_ESYSTEM;
    goto End;
  }

  result = 0;
  while (bytes_to_write > 0)
  {
    result = efs_write (fd, data_buf + bytes_written, bytes_to_write);
    if (result < 0)
    {
      goto End;
    }

    bytes_written += result;
    bytes_to_write -= result;
  }

End:
  if (parent_path != NULL)
  {
    fs_os_free (parent_path);
  }

  if ((result < 0) && (bytes_written == 0)) /*No data written;fill error code*/
  {
    result = map_efs_to_rfs_errno (efs_errno);
  }
  else
  {
    result = bytes_written;
  }

  if (fd >= 0)
  {
    (void) efs_close (fd);
  }

  return result;
}


static int
rfs_efs_unlink (const char *path)
{
  int result;

  result = efs_unlink (path);
  if (result != 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
  }

  return result;
}

static int
rfs_efs_stat (const char *path, struct rfs_stat_buf *buf)
{
  int result;
  struct fs_stat fs_buff;

  result = efs_stat (path, &fs_buff);
  if (result != 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    return result;
  }

  buf->st_mode = fs_buff.st_mode;
  buf->st_size = fs_buff.st_size;

  return result;
}

static int32
rfs_efs_put (const char *path, void *data, uint32 length, int oflag)
{
  int32 result;
  int efs_oflag = 0;

  efs_oflag = map_rfs_to_efs_oflag (oflag);
  efs_oflag = efs_oflag | O_AUTODIR;

  result = efs_put (path, data, length, efs_oflag, 0666);
  if (result != 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    return result;
  }

  return (int32) length;
}

static int32
rfs_efs_get (const char *path, void *data, uint32 length)
{
  int result;

  result = efs_get (path, data, length);
  if (result < 0)
  {
    result = map_efs_to_rfs_errno (efs_errno);
    return result;
  }

  return (int32) length;
}


static int32
rfs_efs_put_debug (const char *path, void *data, uint32 length, int32 offset,
		   int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
		   struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length; (void) oflag;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

static int32
rfs_efs_get_debug (const char *path, void *data, uint32 length, int32 offset,
            		   RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
            		   struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length;
  (void) cb_func; (void) cb_param; (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

static int
rfs_efs_set_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return RFS_ENOTSUPPORTED;
}

static int
rfs_efs_get_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return -RFS_ENOTSUPPORTED;
}

int
rfs_efs_init (struct rfs_ops_type *base_ops)
{
  RFS_ASSERT (base_ops != NULL);
  if (base_ops == NULL)
  {
    return -1;
  }

  memset (base_ops, 0x0, sizeof (struct rfs_ops_type));

  base_ops->read = rfs_efs_read;
  base_ops->write = rfs_efs_write;
  base_ops->unlink = rfs_efs_unlink;
  base_ops->stat = rfs_efs_stat;
  base_ops->put = rfs_efs_put;
  base_ops->get = rfs_efs_get;
  base_ops->put_debug = rfs_efs_put_debug;
  base_ops->get_debug = rfs_efs_get_debug;
  base_ops->set_config = rfs_efs_set_config;
  base_ops->get_config = rfs_efs_get_config;

  return 0;
}

#endif /* FEATURE_RFS_USE_EFS */

