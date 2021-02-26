/***********************************************************************
 * rfs_rfsa.c
 *
 * RFS wrapper for RFS_ACCESS
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * APIs to hook up RFS_ACCESS APIs under RFS API layer.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/rfs/src/rfs_rfsa.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-12-03   dks   Move rfs init to a task
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-05-29   dks   Fix RFSA path creation logic
2014-04-02   dks   Create

===========================================================================*/

#include "rfs_config_i.h"

#ifdef FEATURE_RFS_USE_RFSA

#include "rfs_rfsa.h"
#include "rfs_fcntl.h"
#include "rfs_ops.h"
#include "rfs_errno.h"
#include "fs_os_malloc.h"
#include "comdef.h"
#include "rfs_assert.h"
#include <string.h>
#include "rfsa_client.h"
#include <stdio.h>
#include <stdlib.h>

#if defined (FEATURE_RFSA_ON_ADSP)
  #define RFSA_ROOT_DIR "ADSP://"
#elif defined (FEATURE_RFSA_ON_SENSORS)
  #define RFSA_ROOT_DIR "SNSR://"
#elif defined (FEATURE_RFSA_ON_MPSS)
  #define RFSA_ROOT_DIR "MPSS://"
#else
  #error "RFS on RFSA PD not configured. Check build config"
#endif

static int
map_rfsa_to_rfs_errno (int rfsa_errno)
{
  switch (rfsa_errno)
  {
    case -1:
      return RFS_ESYSTEM;

    default:
      RFS_ASSERT (0);
      return RFS_ESYSTEM;
  }
}

static int
map_rfs_to_rfsa_oflag (int oflag)
{
  int rfsa_oflag = 0;

  if ((oflag & RFS_O_RDONLY) == RFS_O_RDONLY)
    rfsa_oflag |= O_RDONLY;

  if ((oflag & RFS_O_WRONLY) == RFS_O_WRONLY)
    rfsa_oflag |= O_WRONLY;

  if ((oflag & RFS_O_RDWR) == RFS_O_RDWR)
    rfsa_oflag |= O_RDWR;

  if ((oflag & RFS_O_CREAT) == RFS_O_CREAT)
    rfsa_oflag |= O_CREAT;

  if ((oflag & RFS_O_TRUNC) == RFS_O_TRUNC)
    rfsa_oflag |= O_TRUNC;

  if ((oflag & RFS_O_APPEND) == RFS_O_APPEND)
    rfsa_oflag |= O_APPEND;

  return rfsa_oflag;
}

static char *
allocate_rfsa_path (const char *path)
{
  uint32 path_len;
  char *real_path, *rfsa_path;

  real_path = (char *) path;
  while (*real_path != 0 && *real_path == '/')
  {
    real_path++;
  }

  path_len = strlen (real_path) + strlen (RFSA_ROOT_DIR) + 1;
  rfsa_path = (char *) fs_os_malloc (path_len);
  if (rfsa_path == NULL)
    return rfsa_path;

  (void) snprintf (rfsa_path, path_len, "%s%s", RFSA_ROOT_DIR, real_path);
  return rfsa_path;
}

static void
free_rfsa_path (char *path)
{
  free (path);
}

static int
rfs_rfsa_open (const char *path, int oflag)
{
  int32 result;
  int rfsa_oflag;
  char *rfsa_path;

  rfsa_path = allocate_rfsa_path (path);
  if (rfsa_path == NULL)
  {
    result = RFS_ENOMEM;
    goto end;
  }

  rfsa_oflag = map_rfs_to_rfsa_oflag (oflag);

  result = rfsa_open (rfsa_path, rfsa_oflag);

end:
  if (rfsa_path != NULL)
  {
    free_rfsa_path (rfsa_path);
  }

  if (result < 0)
  {
    return map_rfsa_to_rfs_errno (result);
  }

  return result;
}


static int32
rfs_rfsa_read (const char *path, uint32 offset, void *buf, uint32 nbyte)
{
  int32 result = RFS_ESYSTEM;
  int fd;

  if (nbyte == 0)
  {
    return 0;
  }

  fd = rfs_rfsa_open (path, O_RDONLY);
  if (fd < 0)
  {
    return fd;
  }

  result = rfsa_seek (fd, offset, RFSA_SEEK_SET);
  if (result != 0)
  {
    result = map_rfsa_to_rfs_errno (result);
    goto End;
  }

  result = rfsa_ftell (fd);
  if (result != offset)
  {
    result = RFS_ESYSTEM;
    goto End;
  }

  result = rfsa_read (fd, buf, nbyte);

End:
  if (fd >= 0)
  {
    (void) rfsa_close (fd);
  }

  return result;
}

static int32
rfs_rfsa_write (const char *path, uint32 offset, int oflag,
                const void *buf, uint32 nbyte)
{
  int32 result = RFS_ESYSTEM;
  int fd;
  int rfsa_oflag;

  rfsa_oflag = map_rfs_to_rfsa_oflag (oflag);

  fd = rfs_rfsa_open (path, rfsa_oflag);
  if (fd < 0)
  {
    return fd;
  }

  result = rfsa_seek (fd, offset, RFSA_SEEK_SET);
  if (result != 0)
  {
    result = map_rfsa_to_rfs_errno (result);
    goto End;
  }

  result = rfsa_ftell (fd);
  if (result != offset)
  {
    result = RFS_ESYSTEM;
    goto End;
  }

  result = rfsa_write (fd, (char *)buf, nbyte);

End:
  if (fd >= 0)
  {
    (void) rfsa_close (fd);
  }

  return result;
}

static int32
rfs_rfsa_put (const char *path, void *data, uint32 length, int oflag)
{
  return rfs_rfsa_write (path, 0, oflag, data, length);
}

static int32
rfs_rfsa_get (const char *path, void *data, uint32 length)
{
  return rfs_rfsa_read (path, 0, data, length);
}

static int
rfs_rfsa_stat (const char *path, struct rfs_stat_buf *buf)
{
  int32 result;
  char *rfsa_path;

  rfsa_path = allocate_rfsa_path (path);
  if (rfsa_path == NULL)
  {
    result = RFS_ENOMEM;
    goto End;
  }

  result = rfsa_plen (rfsa_path);

End:
  if (rfsa_path != NULL)
  {
    free_rfsa_path (rfsa_path);
  }

  if (result < 0)
  {
    return map_rfsa_to_rfs_errno (result);
  }

  memset (buf, 0x0, sizeof (struct rfs_stat_buf));
  buf->st_size = result;
  return RFS_ENOERROR;
}

static int
rfs_rfsa_invalid_unlink (const char *path)
{
  (void) path;
  return RFS_EPERM;
}

int32
rfs_rfsa_put_debug (const char *path, void *data, uint32 length, int32 offset,
            	   int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
          		   struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length; (void) oflag;
  (void) cb_func; (void) cb_param, (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

int32
rfs_rfsa_get_debug (const char *path, void *data, uint32 length, int32 offset,
              	   RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
             		   struct rfs_debug_info_type *debug_info)
{
  (void) path; (void) data; (void) length;
  (void) cb_func; (void) cb_param; (void) debug_info;
  return RFS_ENOTSUPPORTED;
}

int
rfs_rfsa_set_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return RFS_ENOTSUPPORTED;
}

int
rfs_rfsa_get_config (struct rfs_config_info_type *config_info)
{
  (void) config_info;
  return -RFS_ENOTSUPPORTED;
}

int
rfs_rfsa_init (struct rfs_ops_type *base_ops)
{
  RFS_ASSERT (base_ops != NULL);
  if (base_ops == NULL)
  {
    return -1;
  }

  memset (base_ops, 0x0, sizeof (struct rfs_ops_type));

  base_ops->read = rfs_rfsa_read;
  base_ops->write = rfs_rfsa_write;
  base_ops->unlink = rfs_rfsa_invalid_unlink;
  base_ops->stat = rfs_rfsa_stat;
  base_ops->put = rfs_rfsa_put;
  base_ops->get = rfs_rfsa_get;
  base_ops->put_debug = rfs_rfsa_put_debug;
  base_ops->get_debug = rfs_rfsa_get_debug;
  base_ops->set_config = rfs_rfsa_set_config;
  base_ops->get_config = rfs_rfsa_get_config;
  return 0;
}

#endif /* FEATURE_RFS_USE_RFSA */

