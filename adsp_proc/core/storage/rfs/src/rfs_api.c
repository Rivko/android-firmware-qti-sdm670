/***********************************************************************
 * rfs_api.c
 *
 * RFS API implementations.
 * Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * Implementation of the Remote File System Module APIs
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_api.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-05-31   rp    Port to SLPI image.
2015-11-19   mj    Add DLL_API_GLOBAL for APIs included in Shared Objects.
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-30   dks   Check writes to readonly paths.
2014-12-05   dks   Target compilation and audio compilation fixes.
2014-12-03   dks   Move rfs init to a task
2014-11-18   dks   Add auto_dir support for rfs on efs.
2014-11-17   dks   Add path prefix check to the path validation.
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-06   dks   Return error for open with encrypt and compress oflags.
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-15-04   dks   Add support for RFS over RFSA APIs.
2014-01-27   dks   Add support for RFS over RMTS APIs.
2013-12-26   rp    Add tftp-client module.
2013-11-07   dks   Create

===========================================================================*/


#include "rfs_config_i.h"
#include "rfs_api.h"
#include "rfs_api_priv.h"
#include "rfs_errno.h"
#include "rfs_ops.h"
#include "rfs_tftp.h"
#include "rfs_efs.h"
#include "rfs_nodev.h"
#include "rfs_rmts.h"
#include "rfs_rfsa.h"
#include "rfs_desc.h"
#include "rfs_assert.h"
#include "fs_os.h"
#include "fs_os_malloc.h"
#include "rcinit.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define RFS_PATH_BUFF (RFS_PATH_MAX + 1)

#define RFS_READONLY_PREFIX   "/readonly/"

char *allowed_prefix_paths[]=
{
  RFS_READONLY_PREFIX,
  "/readwrite/",
  "/shared/",
  "/ramdumps/",
  "/hlos/",
};

struct rfs_info_type
{
  int init_done;
  struct rfs_ops_type rfs_ops;
  int num_allowed_prefix_paths;
  fs_os_mutex_t rfs_crit_sect;
};

static struct rfs_info_type rfs_info;

static void
rfs_api_lock_init (void)
{
  memset (&rfs_info.rfs_crit_sect, 0,
          sizeof (rfs_info.rfs_crit_sect));
  fs_os_mutex_init (&rfs_info.rfs_crit_sect);
}

static void
rfs_api_lock (void)
{
  fs_os_mutex_lock (&rfs_info.rfs_crit_sect);
}

static void
rfs_api_unlock (void)
{
  fs_os_mutex_unlock (&rfs_info.rfs_crit_sect);
}

void
rfs_api_init (void)
{
  rfs_api_lock ();
  RFS_ASSERT (rfs_info.init_done == 1);
  rfs_api_unlock();
}

static int
rfs_check_path_prefix (const char *path, int *is_path_readonly)
{
  int i;

  if (path == NULL || path[0] == '\0')
    return RFS_EINVAL;

  if (strlen (path) > RFS_PATH_MAX)
    return RFS_ENAMETOOLONG;

  for (i = 0; i < rfs_info.num_allowed_prefix_paths; i++)
  {
    if (strncmp (path, allowed_prefix_paths[i],
                 strlen(allowed_prefix_paths[i])) == 0)
    {
      break;
    }
  }
  if (i >= rfs_info.num_allowed_prefix_paths)
  {
    return RFS_EPERM;
  }

  if (is_path_readonly != NULL)
  {
    *is_path_readonly = 0;
    if (strcmp (allowed_prefix_paths[i], RFS_READONLY_PREFIX) == 0)
    {
      *is_path_readonly = 1;
    }
  }

  return RFS_ENOERROR;
}

int
rfs_open (const char *path, int oflag, int mode)
{
  int result = RFS_ESYSTEM;
  int fd = RFS_ESYSTEM;
  struct rfs_stat_buf stat_buf;
  int is_path_readonly, file_just_created;

  (void) mode;

  rfs_api_init();

  result = rfs_check_path_prefix (path, &is_path_readonly);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  if ((oflag & RFS_O_ENCRYPT) || (oflag & RFS_O_COMPRESS))
  {
    return RFS_ENOTSUPPORTED;
  }

  fd = rfs_desc_allocate_fd (path, oflag);
  if (fd < 0)
  {
    return fd;
  }

  file_just_created = 0;

  result = rfs_info.rfs_ops.stat (path, &stat_buf);
  if (result != RFS_ENOERROR && result != RFS_ENOENT)
  {
    /* Only ENOENT can be handled correctly */
    goto End;
  }

  if ((result == RFS_ENOENT) && (oflag & RFS_O_CREAT))
  {
    if (is_path_readonly == 1)
    {
      result = RFS_EPERM;
      goto End;
    }
     /* use dummy buffer */
    result = rfs_info.rfs_ops.put (path, &stat_buf, 0, oflag);
    if (result == 0)
    {
      result = RFS_ENOERROR;
    }

    file_just_created = 1;
  }

  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  if (oflag & RFS_O_TRUNC)
  {
    if (is_path_readonly == 1)
    {
      result = RFS_EPERM;
      goto End;
    }

    if (file_just_created == 1)
    {
      result = 0; /* no need to write again */
    }
    else
    {
      /* use a dummy buffer */
      result = rfs_info.rfs_ops.put (path, &stat_buf, 0, oflag);
    }
    if (result == 0)
    {
      result = RFS_ENOERROR;
    }
    else
    {
      RFS_ASSERT (result < 0);
      if (result > 0)  /* If successful, should have written 0 bytes only.
                          force failure */
      {
        result = RFS_ESYSTEM;
      }
    }
  }

End:
  if (result != RFS_ENOERROR)
  {
    (void) rfs_desc_free_fd (fd);
    return result;
  }

  return fd;
}

int
rfs_close (int fd)
{
  rfs_api_init();

  return rfs_desc_free_fd (fd);
}

int32
rfs_read (int fd, void *buf, uint32 nbyte)
{
  char *fname;
  uint32 offset;
  int32 result;
  int oflag;

  rfs_api_init();

  fname = (char *) fs_os_malloc (RFS_PATH_BUFF);
  if (fname == NULL)
  {
    return RFS_ENOMEM;
  }

  result = rfs_desc_lookup_fd (fd, fname, RFS_PATH_BUFF, &oflag, &offset);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  /* Check if we have read permissions */
  switch (oflag & RFS_O_ACCMODE)
  {
    case RFS_O_RDONLY:
    case RFS_O_RDWR:
    {
      break;
    }

    default:
    {
      result = RFS_EBADF;
      goto End;
    }
  }

  if (nbyte == 0)
  {
    result = 0;
    goto End;
  }

  if (buf == NULL)
  {
    result = RFS_EINVAL;
    goto End;
  }

  result = rfs_info.rfs_ops.read (fname, offset, buf, nbyte);
  if (result > 0)
  {
    int update_res;
    update_res = rfs_desc_update_offset (fd, offset + result);
    if (update_res != RFS_ENOERROR)
    {
      result = update_res;
    }
  }

End:
  if (fname != NULL)
  {
    fs_os_free (fname);
  }

  return result;
}

int32
rfs_write (int fd, const void *buf, uint32 nbyte)
{
  char *fname;
  uint32 offset;
  int32 result;
  int oflag, is_path_readonly;

  rfs_api_init();

  fname = (char *) fs_os_malloc (RFS_PATH_BUFF);
  if (fname == NULL)
  {
    return RFS_ENOMEM;
  }

  result = rfs_desc_lookup_fd (fd, fname, RFS_PATH_BUFF, &oflag, &offset);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  result = rfs_check_path_prefix (fname, &is_path_readonly);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  if (is_path_readonly == 1)
  {
    result = RFS_EPERM;
    goto End;
  }

  /* Check if we have write permissions */
  switch (oflag & RFS_O_ACCMODE)
  {
    case RFS_O_WRONLY:
    case RFS_O_RDWR:
    {
      break;
    }

    default:
    {
      result = RFS_EBADF;
      goto End;
    }
  }

  if (nbyte == 0)
  {
    result = 0;
    goto End;
  }

  if (buf == NULL)
  {
    return RFS_EINVAL;
  }

  result = rfs_info.rfs_ops.write (fname, offset, oflag, buf, nbyte);
  if (result > 0)
  {
    int update_res;
    update_res = rfs_desc_update_offset (fd, offset + result);
    if (update_res != RFS_ENOERROR)
    {
      result = update_res;
    }
  }

End:
  if (fname != NULL)
  {
    fs_os_free (fname);
  }

  return result;
}

int32
rfs_seek (int fd, int32 offset, int whence)
{
  struct rfs_stat_buf stat_buf;
  int32 new_offset, curr_offset;
  int result, oflag;
  char *fname;

  rfs_api_init();

  fname = (char *) fs_os_malloc (RFS_PATH_BUFF);
  if (fname == NULL)
  {
    return RFS_ENOMEM;
  }

  result = rfs_desc_lookup_fd (fd, fname, RFS_PATH_BUFF, &oflag,
                              (uint32*)&curr_offset);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  switch (whence)
  {
    case RFS_SEEK_CUR:
      new_offset = curr_offset + offset;
      break;

    case RFS_SEEK_END:
      result = rfs_stat (fname, &stat_buf);
      if (result != RFS_ENOERROR)
      {
        goto End;
      }
      new_offset = stat_buf.st_size + offset;
      break;

    case RFS_SEEK_SET:
      new_offset = 0 + offset;
      break;

    default:
      result = RFS_EINVAL;
      goto End;
  }

  if (new_offset < 0)
  {
    result = RFS_EINVAL;
  }

  result = rfs_desc_update_offset (fd, new_offset);
  if (result == RFS_ENOERROR)
  {
    result = new_offset;
  }

End:
  if (fname != NULL)
  {
    fs_os_free (fname);
  }

  return result;
}

int
rfs_unlink (const char *path)
{
  int result, is_path_readonly;

  rfs_api_init();

  result = rfs_check_path_prefix (path, &is_path_readonly);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  if (is_path_readonly == 1)
  {
    result = RFS_EPERM;
    goto End;
  }

  result = rfs_info.rfs_ops.unlink (path);

End:
  return result;
}

int
rfs_stat (const char *path, struct rfs_stat_buf *buf)
{
  int result;

  rfs_api_init();

  result = rfs_check_path_prefix (path, NULL);
  if (result != RFS_ENOERROR)
  {
    goto End;
  }

  if (buf == NULL)
  {
    result = RFS_EINVAL;
    goto End;
  }

  result = rfs_info.rfs_ops.stat (path, buf);

End:
  return result;
}


int32
rfs_put_check_params (const char *path, void *data, uint32 length, int oflag,
                      int mode, RFS_PUT_CALLBACK_FUNC_TYPE cb_func,
                      void *cb_param, struct rfs_debug_info_type *debug_info)
{
  int result, is_path_readonly;

  (void) length;
  (void) mode;
  (void) debug_info;
  (void) cb_param;

  result = rfs_check_path_prefix (path, &is_path_readonly);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  if (is_path_readonly == 1)
  {
    return RFS_EPERM;
  }

  if ((data == NULL) && (cb_func == NULL))
  {
    return RFS_EINVAL;
  }
  if ((data != NULL) && (cb_func != NULL))
  {
    return RFS_EINVAL;
  }

  if ((oflag & RFS_O_ENCRYPT) || (oflag & RFS_O_COMPRESS))
  {
    return RFS_ENOTSUPPORTED;
  }

  return RFS_ENOERROR;
}

int32
rfs_put (const char *path, void *data, uint32 length, int oflag, int mode)
{
  int32 result;
  int dummy_data;
  struct rfs_stat_buf stat_buf;

  if (length == 0 && data == NULL)
  {
    data = &dummy_data;
  }

  rfs_api_init();

  result = rfs_put_check_params (path, data, length, oflag, mode,
                                 NULL, NULL, NULL);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  result = rfs_info.rfs_ops.stat (path, &stat_buf);
  if (result != RFS_ENOERROR && result != RFS_ENOENT)
  {
    /* Only ENOENT can be handled correctly */
    return result;
  }

  if ((result == RFS_ENOENT) && (oflag & RFS_O_CREAT) == 0)
  {
    return RFS_ENOENT;
  }

  result = rfs_info.rfs_ops.put (path, data, length, oflag);
  return result;
}

int32
rfs_put_debug(const char *path, void *data, uint32 length, int32 offset,
              int oflag, int mode, RFS_PUT_CALLBACK_FUNC_TYPE cb_func,
              void *cb_param, struct rfs_debug_info_type *debug_info)
{
  int32 result;

  rfs_api_init();

  result = rfs_put_check_params (path, data, length, oflag, mode,
                                 cb_func, cb_param, debug_info);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  result = rfs_info.rfs_ops.put_debug (path, data, length, offset, oflag,
                                       cb_func, cb_param, debug_info);
  return result;
}

int32
rfs_get_check_params (const char *path, void *data, uint32 length,
                     RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                     struct rfs_debug_info_type *debug_info)
{
  int result;
  (void) length;
  (void) cb_param;
  (void) debug_info;

  result = rfs_check_path_prefix (path, NULL);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  if ((data == NULL) && (cb_func == NULL))
  {
    return RFS_EINVAL;
  }
  if ((data != NULL) && (cb_func != NULL))
  {
    return RFS_EINVAL;
  }

  return RFS_ENOERROR;
}


int32
rfs_get (const char *path, void *data, uint32 length)
{
  int32 result;

  rfs_api_init();

  result = rfs_get_check_params (path, data, length, NULL, NULL, NULL);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  result = rfs_info.rfs_ops.get (path, data, length);
  return result;
}

int32
rfs_get_debug (const char *path, void *data, uint32 length, int32 offset,
               RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
               struct rfs_debug_info_type *debug_info)
{
  int32 result;

  rfs_api_init();

  result = rfs_get_check_params (path, data, length, cb_func, cb_param,
                                 debug_info);
  if (result != RFS_ENOERROR)
  {
    return result;
  }

  result = rfs_info.rfs_ops.get_debug (path, data, length, offset,
                                       cb_func, cb_param, debug_info);
  return result;
}

int
rfs_set_config (struct rfs_config_info_type *config_info)
{
  int result;

  rfs_api_init();

  if (config_info == NULL)
  {
    result = RFS_EINVAL;
    goto end;
  }

  result = rfs_info.rfs_ops.set_config (config_info);

 end:
  return result;
}

int
rfs_get_config (struct rfs_config_info_type *config_info)
{
  int result;

  rfs_api_init();

  if (config_info == NULL)
  {
    result = RFS_EINVAL;
    goto end;
  }

  result = rfs_info.rfs_ops.get_config (config_info);

 end:
  return result;
}


void
rfs_init (void)
{
  int result = -1;

  memset (&rfs_info, 0, sizeof (rfs_info));

  rfs_api_lock_init ();
  rfs_api_lock ();

  rcinit_handshake_startup ();

  rfs_desc_init ();

  rfs_info.num_allowed_prefix_paths = sizeof (allowed_prefix_paths) /
                                      sizeof (allowed_prefix_paths[0]);
#if defined (FEATURE_RFS_USE_NO_DEV)
  result = rfs_nodev_init (&rfs_info.rfs_ops);
#elif defined (FEATURE_RFS_USE_TFTP)
  result = rfs_tftp_init (&rfs_info.rfs_ops);
#elif defined (FEATURE_RFS_USE_RMTS)
  result = rfs_rmts_init (&rfs_info.rfs_ops);
#elif defined (FEATURE_RFS_USE_RFSA)
  result = rfs_rfsa_init (&rfs_info.rfs_ops);
#elif defined (FEATURE_RFS_USE_EFS)
  result = rfs_efs_init (&rfs_info.rfs_ops);
#endif
  if (result != 0)
  {
    result = rfs_nodev_init (&rfs_info.rfs_ops);
    RFS_ASSERT (result == 0);
  }

  rfs_info.init_done = 1;

  rfs_api_unlock ();
}
