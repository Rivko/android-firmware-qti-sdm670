/***********************************************************************
 * tftp_file_modem.c
 *
 * NHHLOS File IO Abstraction.
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/src/tftp_file_modem.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-05-31   rp    Port to SLPI image.
2015-05-06   dks   Add OS abstraction layer
2015-01-06   vm    Compile TFTP server on TN Apps.
2014-12-30   dks   Fixes to config and log module.
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NON-HLOS builds"
#endif

#include "tftp_file.h"

#ifdef TFTP_TN_BUILD
#include "tftp_os.h"
#include "tftp_assert.h"
#include "tftp_malloc.h"
#include "tftp_string.h"
#include "tftp_log.h"
#include "fs_estdio.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_fcntl.h"
#include <stdlib.h>
#include <stdio.h>

struct tftp_file_modem_handle
{
  EFS_EFILE* file_handle;
  char *stream_buf;
};

#define TFTP_FILE_STREAM_BUF_SIZE (32*1024)
volatile static uint32 tftp_modem_stream_buf_size = TFTP_FILE_STREAM_BUF_SIZE;

struct tftp_modem_file_handle
{
  EFS_EFILE *file_handle;
  char *stream_buf;
};

static void
tftp_file_modem_alloc_stream (struct tftp_modem_file_handle *modem_handle)
{
  int result;

  TFTP_ASSERT (modem_handle != NULL);
  if (modem_handle == NULL)
  {
    return;
  }

  modem_handle->stream_buf = tftp_malloc (tftp_modem_stream_buf_size);
  if (modem_handle->stream_buf == NULL)
  {
    TFTP_LOG_ERR ("Malloc failed for stream buf");
  }
  else
  {
    result = efs_esetvbuf (modem_handle->file_handle,
                           modem_handle->stream_buf,
                           EFS_IOFBF, tftp_modem_stream_buf_size);
    TFTP_ASSERT (result == 0);
    if (result != 0)
    {
      TFTP_LOG_ERR ("efs_esetvbuf failed, errno: %d", efs_errno,
                    strerror(efs_errno));
    }
  }
}

int32
tftp_file_open (tftp_file_handle *handle, const char *path, const char *mode)
{
  struct tftp_modem_file_handle *modem_handle;

  modem_handle = tftp_malloc (sizeof (struct tftp_modem_file_handle));
  if (modem_handle == NULL)
  {
    TFTP_LOG_ERR ("open malloc failed");
    return -ENOMEM;
  }

  modem_handle->file_handle = efs_efopen (path, mode);
  if (modem_handle->file_handle == NULL)
  {
    TFTP_LOG_ERR ("efs_efopen failed, errno: %d", efs_errno,
                  strerror(efs_errno));
    *handle = NULL;
    return -(efs_errno);
  }

  tftp_file_modem_alloc_stream (modem_handle);
  *handle = (void *) modem_handle;

   return 0;
}

int32
tftp_file_open_stream (tftp_file_handle *handle, int32 fd, const char *mode)
{
  int32 result;
  EFS_EFILE *efs_handle;
  struct tftp_modem_file_handle *modem_handle;

  *handle = NULL;

  modem_handle = tftp_malloc (sizeof (struct tftp_modem_file_handle));
  if (modem_handle == NULL)
  {
    TFTP_LOG_ERR ("open stream : malloc failed");
    return -ENOMEM;
  }

  efs_handle = efs_efdopen (fd, mode);
  TFTP_ASSERT (efs_handle != NULL);
  if(efs_handle == NULL)
  {
    result = -(efs_errno);
    tftp_free (modem_handle);
  }
  else
  {
    modem_handle->file_handle = efs_handle;
    tftp_file_modem_alloc_stream (modem_handle);
    *handle = (void *) modem_handle;
    result = 0;
  }

  return result;
}

int32
tftp_file_read (tftp_file_handle handle, void *buffer, int32 buffer_size)
{
  int32 return_val;
  size_t remaining_bytes, read_bytes, result;
  EFS_EFILE *efs_handle;
  uint8 *data_buffer = (uint8 *) buffer;

  TFTP_ASSERT (handle != NULL);
  if (handle == NULL)
  {
    return -EINVAL;
  }

  if (buffer_size > INT32_MAX)
  {
    return_val = -(EFBIG);
    return return_val;
  }

  if (buffer_size == 0)
  {
    return 0;
  }

  efs_handle = ((struct tftp_modem_file_handle*)handle)->file_handle;

  read_bytes = 0;
  remaining_bytes = buffer_size;
  while (remaining_bytes > 0)
  {
    result = efs_efread (data_buffer, 1, buffer_size, efs_handle);
    if (result == 0)
    {
      break;
    }
    data_buffer += result;
    read_bytes += result;
    remaining_bytes -= result;
  }

  if (read_bytes == 0)
  {
    return_val = (-efs_errno);
  }
  else
  {
    return_val = (int32) read_bytes;
  }

  return return_val;
}

int32
tftp_file_write(tftp_file_handle handle, const void *buffer, int32 buffer_size)
{
  int32 return_val;
  size_t remaining_bytes, written_bytes, result;
  EFS_EFILE *efs_handle;
  uint8 *data_buffer = (uint8 *) buffer;
  uint8 *buffer_end;

  TFTP_ASSERT (handle != NULL);
  if (handle == NULL)
  {
    return -EINVAL;
  }

  TFTP_ASSERT (buffer_size >= 0);
  if (buffer_size == 0)
  {
    return 0;
  }

  TFTP_ASSERT (buffer != NULL);
  if ((buffer == NULL) ||
      (buffer_size < 0))
  {
    return -EINVAL;
  }

  if (buffer_size > INT32_MAX)
  {
    return -EFBIG;
  }

  efs_handle = ((struct tftp_modem_file_handle*)handle)->file_handle;

  buffer_end = (uint8 *) buffer + buffer_size;
  written_bytes = 0;
  remaining_bytes = buffer_size;
  while (remaining_bytes > 0)
  {
    TFTP_ASSERT (data_buffer + remaining_bytes <= buffer_end);

    result = efs_efwrite (data_buffer, 1, remaining_bytes, efs_handle);

    if (result == 0)
    {
      break;
    }
    data_buffer += result;
    written_bytes += result;
    remaining_bytes -= result;
  }

  if (written_bytes == 0)
  {
    return_val = (-efs_errno);
  }
  else
  {
    return_val = (int32) written_bytes;
  }

  return return_val;
}

int32
tftp_file_seek (tftp_file_handle handle, int32 offset, int whence)
{
  int32 result;
  EFS_EFILE *efs_handle;

  TFTP_ASSERT (handle != NULL);
  if (handle == NULL)
  {
    return -EINVAL;
  }

  efs_handle = ((struct tftp_modem_file_handle*)handle)->file_handle;

  if (whence == SEEK_SET)
  {
    result = efs_eftell (efs_handle);
    if (result == offset)
    {
      return 0;
    }
  }

  result = (int32) efs_efseek (efs_handle, (long)offset, whence);

  if (result != 0)
  {
    result = -(efs_errno);
  }
  return result;
}

int32
tftp_file_close (tftp_file_handle handle)
{
  int32 result;
  struct tftp_modem_file_handle *modem_handle;

  TFTP_ASSERT (handle != NULL);
  if (handle == NULL)
  {
    return -EINVAL;
  }

  modem_handle = (struct tftp_modem_file_handle*)handle;

  result = (int32) efs_efclose (modem_handle->file_handle);
  if (result != 0)
  {
    return -(efs_errno);
  }

  if (modem_handle->stream_buf != NULL)
  {
    tftp_free (modem_handle->stream_buf);
  }

  tftp_free (modem_handle);

  return 0;
}

int32
tftp_file_get_file_size (const char *path)
{
  EFS_EFILE *efs_handle;
  long file_size;
  int32 result;

  efs_handle = efs_efopen (path, "rb"); /* Open in read mode */
  if(efs_handle == NULL)
  {
    result = -(efs_errno);
    return result;
  }

  result = efs_efseek (efs_handle, 0, SEEK_END); // Seek to end of file
  if (result != 0)
  {
    result = -(efs_errno);
    goto cleanup;
  }

  file_size = efs_eftell (efs_handle);
  if (file_size == -1)
  {
    result = -(efs_errno);
    goto cleanup;
  }

  if (file_size > INT32_MAX)
  {
    result = -(EFBIG);
    goto cleanup;
  }
  result  = (int32) file_size;


cleanup:
    efs_efclose (efs_handle);
    return result;
}

#else /* TFTP_TN_BUILD */

int32
tftp_file_open (tftp_file_handle *handle, const char *path, const char *mode)
{
  (void) handle; (void) path; (void) mode;
  return 0;
}

int32
tftp_file_close (tftp_file_handle handle)
{
  (void) handle;
  return 0;
}

int32
tftp_file_write(tftp_file_handle handle, const void *buffer, int32 buffer_size)
{
  (void) handle; (void) buffer; (void) buffer_size;
  return buffer_size;
}


#endif /* TFTP_TN_BUILD */

