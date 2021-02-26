/***********************************************************************
 * rfs_utils.c
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

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_utils.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-05   vm    Compile server for TN Apps
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-10-20   dks   Remove rfs and tftp dependency
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-26   rp    Bring in changes from target-unit-testing.
2014-08-04   rp    Create

===========================================================================*/

#include "rfs_utils.h"
#include "rfs_api.h"
#include "rfs_assert.h"
#include "rfs_msg.h"
#include "rfs_errno.h"
#include "fs_os_malloc.h"
#include "stringl/stringl.h"
#include <stdlib.h>
#include <string.h>


/* TODO: Increment Block number for each block */
static void
rfs_util_fill_buf (uint8 *buf, uint32 buf_size, const char *filename,
                   uint32 block_no, uint32 id)
{
  char temp_buf[100];
  uint32 cur_buf_size, temp_buf_size, ctr;

  memset (buf, 0, buf_size);

  cur_buf_size = ctr = 0;
  while (cur_buf_size < buf_size)
  {
    memset (temp_buf, 0, sizeof (temp_buf));
    snprintf (temp_buf, sizeof (temp_buf),
              "%s-%d-%d-%d\n", filename, (int)block_no,
              (int)id, (int)ctr++);
    strlcat ((char *)buf, temp_buf, buf_size);
    temp_buf_size = strlen (temp_buf);
    cur_buf_size += temp_buf_size;
  }
}

int
rfs_util_verify_file_in_chunks (const char *filename, const uint8 *file_buf,
                                uint32 file_size, uint32 read_size,
                                uint32 read_size_incr)
{
  int fd = -1, result = -1;
  uint8 *cur_buf, *verify_buf = NULL;
  uint32 cur_file_size, cur_read_size, remaining_size;
  struct rfs_stat_buf stat_buf;

  RFS_MSG_STRING_2 ("rfs_util_verify_file: %s %d", filename, file_size);
  RFS_MSG_STRING_2 ("rfs_util_verify_file : %d %d", read_size, read_size_incr);

  verify_buf = fs_os_malloc (file_size);
  if (verify_buf == NULL)
  {
    RFS_MSG_STRING_1 ("%d, malloc failed", (int)file_size);
    goto Error;
  }

  result = rfs_stat (filename, &stat_buf);
  RFS_ASSERT (result == 0);
  RFS_ASSERT ((int32)stat_buf.st_size == file_size);

  memset (verify_buf, 0, file_size);

  fd = rfs_open (filename, RFS_O_RDONLY, 0644);
  if (fd < 0)
  {
    goto End;
  }

  cur_file_size = 0;
  cur_read_size = read_size;
  while (cur_file_size < file_size)
  {
    remaining_size = file_size - cur_file_size;
    cur_read_size += read_size_incr;
    if (cur_read_size > remaining_size)
    {
      cur_read_size = remaining_size;
    }

    cur_buf = &verify_buf[cur_file_size];
    result = rfs_read (fd, cur_buf, cur_read_size);
    if (result != cur_read_size)
    {
      RFS_MSG_STRING_1 ("%d, rfs_read failed", result);
      goto Error;
    }

    cur_file_size += cur_read_size;

    result = memcmp (file_buf, verify_buf, cur_file_size);
    RFS_ASSERT (result == 0);
  }

  result = memcmp (file_buf, verify_buf, file_size);
  RFS_ASSERT (result == 0);
  result = 0;
  goto End;

Error:
    result = -1;

End:
  if (verify_buf != NULL)
  {
    fs_os_free (verify_buf);
  }

  if (fd >= 0)
  {
    (void) rfs_close (fd);
  }

  return result;
}

int
rfs_util_verify_file (const char *filename, const uint8 *file_buf,
                      uint32 file_size)
{
  int result;

  result = rfs_util_verify_file_in_chunks (filename, file_buf,
                                    file_size, file_size, 0);
  RFS_ASSERT (result == 0);
  return result;
}


int
rfs_util_create_file (const char *filename, uint32 file_size,
                      uint32 write_size, uint32 write_size_incr,
                      uint32 buf_seed,
                      int verify_file_after_each_write)
{
  uint32 cur_file_size, cur_write_size, remaining_size, block_no;
  uint8 *file_buf = NULL, *cur_buf;
  int fd = -1, result = -1;

  RFS_MSG_STRING_2 ("rfs_util_create_file: %s %d", filename, file_size);
  RFS_MSG_STRING_2 ("rfs_util_create_file : %d %d",write_size,write_size_incr);

  file_buf = fs_os_malloc (file_size);
  if (file_buf == NULL)
  {
    RFS_MSG_STRING_1 ("%d, malloc failed", (int)file_size);
    goto Error;
  }

  memset (file_buf, 0, file_size);

  fd = rfs_open (filename, RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC, 0644);
  if (fd < 0)
  {
    RFS_MSG_STRING_1 ("%d, rfs_open failed", fd);
    goto Error;
  }

  RFS_MSG_STRING_1 ("%d, rfs_open success", fd);

  cur_file_size = block_no = 0;
  cur_write_size = write_size;
  while (cur_file_size < file_size)
  {
    remaining_size = file_size - cur_file_size;
    cur_write_size += write_size_incr;
    if (cur_write_size > remaining_size)
    {
      cur_write_size = remaining_size;
    }

    cur_buf = &file_buf[cur_file_size];
    rfs_util_fill_buf (cur_buf, cur_write_size, filename, ++block_no, buf_seed);
    result = rfs_write (fd, cur_buf, cur_write_size);
    if (result != cur_write_size)
    {
      RFS_MSG_STRING_1 ("%d, rfs_write failed", result);
      goto Error;
    }

    cur_file_size += cur_write_size;

    if (verify_file_after_each_write)
    {
      result = rfs_util_verify_file_in_chunks (filename, file_buf,
                      cur_file_size, write_size, write_size_incr);
      if (result != 0)
      {
        RFS_MSG_STRING_1 ("file verification failed. file=%s", filename);
        RFS_MSG_STRING_1 ("file verification failed. size=%d", cur_file_size);
        goto Error;
      }
    }
  }

  RFS_MSG_STRING_1 ("%s create file success", filename);

  result = 0;
  goto End;

Error:
result = -1;

End:
  if (file_buf != NULL)
  {
    fs_os_free (file_buf);
  }

  if (fd >= 0)
  {
    (void) rfs_close (fd);
  }

  return result;
}

int
rfs_util_create_file_from_buf (const char *filename, uint8 *file_buf,
                               uint32 file_size, uint32 write_size,
                               uint32 write_size_incr)
{
  uint32 cur_file_size, cur_write_size, remaining_size;
  uint8 *cur_buf;
  int fd = -1, result = -1;

  fd = rfs_open (filename, RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC, 0644);
  if (fd < 0)
  {
    RFS_MSG_STRING_1 ("%d, rfs_open failed", fd);
    goto Error;
  }

  cur_file_size = 0;
  cur_write_size = write_size;
  while (cur_file_size < file_size)
  {
    remaining_size = file_size - cur_file_size;
    cur_write_size += write_size_incr;
    if (cur_write_size > remaining_size)
    {
      cur_write_size = remaining_size;
    }

    cur_buf = &file_buf[cur_file_size];
    result = rfs_write (fd, cur_buf, cur_write_size);
    if (result != cur_write_size)
    {
      RFS_MSG_STRING_1 ("%d, rfs_write failed", result);
      goto Error;
    }

    cur_file_size += cur_write_size;
  }

  result = 0;
  goto End;

Error:
result = -1;

End:
  if (fd >= 0)
  {
    (void) rfs_close (fd);
  }

  return result;
}



#define RFS_UTIL_PUT_GET_CB_PARAM_START_MAGIC  0xDEADBEEF
#define RFS_UTIL_PUT_GET_CB_PARAM_END_MAGIC    0xCAFEABBA

struct rfs_util_put_get_cb_param_type
{
  uint32 start_magic;
  uint32 file_size;
  uint32 block_size;
  uint8 *block_buf;
  uint32 buf_seed;
  uint32 num_blocks;
  uint32 verify_file;
  uint32 end_magic;
};

static int32
rfs_util_put_cb (void *param, uint16 block_number, uint8 **data_buf)
{
  struct rfs_util_put_get_cb_param_type *cb_param;
  uint32 last_buf_bytes = 0, copy_buf_size = 0;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  RFS_ASSERT (param != NULL);
  RFS_ASSERT (data_buf != NULL);
  if ((param == NULL) ||
      (data_buf == NULL))
  {
    return 0;
  }

  cb_param = (struct rfs_util_put_get_cb_param_type *)param;
  RFS_ASSERT (cb_param->start_magic == RFS_UTIL_PUT_GET_CB_PARAM_START_MAGIC);
  RFS_ASSERT (cb_param->end_magic == RFS_UTIL_PUT_GET_CB_PARAM_END_MAGIC);

  copy_buf_size = cb_param->block_size;
  if (((uint32)block_number == cb_param->num_blocks))
  {
    last_buf_bytes = cb_param->file_size % cb_param->block_size;
    copy_buf_size = last_buf_bytes;
  }

  memset (cb_param->block_buf, (int)(block_number + cb_param->buf_seed),
      	  copy_buf_size);
  *data_buf = cb_param->block_buf;

  return copy_buf_size;
}


int
rfs_util_put (const char *filename, uint32 file_size, uint32 buf_seed,
              int oflag, int mode, int verify_file,
              struct rfs_debug_info_type *debug_info)
{
  int result = -1;
  struct rfs_util_put_get_cb_param_type cb_param;
  struct rfs_config_info_type config_info;
  uint8 *block_buf = NULL;
  char* seeded_filename = NULL;

  RFS_MSG_STRING_2 ("rfs_util_put: %s %d", filename, file_size);

  /* assuming seed is a max of 4 chars + 1 char for '\0' */
  if (strlen(filename) + 5 >= RFS_PATH_MAX)
  {
    RFS_MSG_STRING_2 ("file name len %d > max %d", (uint32)strlen(filename) + 5,
                      RFS_PATH_MAX);
    result = -RFS_ENAMETOOLONG;
    goto End;
  }

  seeded_filename = fs_os_malloc (RFS_PATH_MAX);
  if (seeded_filename == NULL)
  {
    RFS_MSG_STRING_1 ("malloc of size %d failed ", RFS_PATH_MAX);
    result = -RFS_ENOMEM;
    goto End;
  }

  snprintf (seeded_filename, RFS_PATH_MAX, "%s-%d",
            filename, (int)buf_seed);

  result = rfs_get_config (&config_info);
  if (result != 0)
  {
    RFS_MSG_STRING_1 ("rfs_get_config failed %d", result);
    result = -RFS_EINVAL;
    goto End;
  }

  block_buf = fs_os_malloc (config_info.block_size);
  if (block_buf == NULL)
  {
    RFS_MSG_STRING_1 ("malloc of size %d failed", config_info.block_size);
    result = -RFS_ENOMEM;
    goto End;
  }

  memset (&cb_param, 0, sizeof (cb_param));
  cb_param.start_magic = RFS_UTIL_PUT_GET_CB_PARAM_START_MAGIC;
  cb_param.end_magic = RFS_UTIL_PUT_GET_CB_PARAM_END_MAGIC;
  cb_param.file_size = file_size;
  cb_param.block_size = config_info.block_size;
  cb_param.block_buf = block_buf;
  cb_param.buf_seed = buf_seed;
  cb_param.num_blocks = (file_size / config_info.block_size) + 1;
  cb_param.verify_file = (uint32) verify_file;

  result = rfs_put_debug (seeded_filename, NULL, file_size, -1, oflag, mode,
                          rfs_util_put_cb, &cb_param, debug_info);
  if (result != file_size)
  {
    RFS_MSG_STRING_1 ("%d, rfs_util_put failed", result);
    goto End;
  }

  if (verify_file)
  {
    result = rfs_util_get (filename, file_size, buf_seed, verify_file, NULL);
    if (result < 0)
    {
      RFS_MSG_STRING_1 ("%d, rfs_util_put verification failed", result);
      goto End;
    }
  }

  result = (int)file_size;

End:
  if (block_buf != NULL)
  {
    fs_os_free (block_buf);
  }

  if (seeded_filename != NULL)
  {
    fs_os_free (seeded_filename);
  }

  return result;
}

static int32
rfs_util_get_cb (void *param, uint16 block_number,
                 uint8 *data_buf, uint32 data_buf_size)
{
  struct rfs_util_put_get_cb_param_type *cb_param;
  uint32 last_buf_bytes = 0, copy_buf_size = 0;
  int result;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  RFS_ASSERT (param != NULL);
  RFS_ASSERT (data_buf != NULL);
  if ((param == NULL) ||
      (data_buf == NULL))
  {
    return 0;
  }

  cb_param = (struct rfs_util_put_get_cb_param_type *)param;
  RFS_ASSERT (cb_param->start_magic == RFS_UTIL_PUT_GET_CB_PARAM_START_MAGIC);
  RFS_ASSERT (cb_param->end_magic == RFS_UTIL_PUT_GET_CB_PARAM_END_MAGIC);

  copy_buf_size = data_buf_size;
  if (cb_param->verify_file == 0)
  {
    goto End;
  }

  if (((uint32)block_number == cb_param->num_blocks))
  {
    last_buf_bytes = cb_param->file_size % cb_param->block_size;
    copy_buf_size = last_buf_bytes;
    RFS_DEBUG_ASSERT (data_buf_size == copy_buf_size);
  }

  memset (cb_param->block_buf, (int)(block_number + cb_param->buf_seed),
          copy_buf_size);
  result = memcmp (cb_param->block_buf, data_buf, copy_buf_size);
  if (result != 0)
  {
    return -1; // todo: return better error-code
  }

End:
  return copy_buf_size;
}


int
rfs_util_get (const char *filename, uint32 file_size, uint32 buf_seed,
              int verify_file, struct rfs_debug_info_type *debug_info)
{
  int result = -1;
  struct rfs_util_put_get_cb_param_type cb_param;
  struct rfs_config_info_type config_info;
  uint8 *block_buf = NULL;
  char* seeded_filename = NULL;

  RFS_MSG_STRING_2 ("rfs_util_get: %s %d", filename, file_size);

  /* assuming seed is a max of 4 chars + 1 char for '\0' */
  if (strlen(filename) + 5 >= RFS_PATH_MAX)
  {
    RFS_MSG_STRING_2 ("file name len %d > max %d", (uint32)strlen(filename) + 5,
                      RFS_PATH_MAX);
    result = -RFS_ENAMETOOLONG;
    goto End;
  }

  seeded_filename = fs_os_malloc (RFS_PATH_MAX);
  if (seeded_filename == NULL)
  {
    RFS_MSG_STRING_1 ("malloc of size %d failed ", RFS_PATH_MAX);
    result = -RFS_ENOMEM;
    goto End;
  }

  snprintf (seeded_filename, RFS_PATH_MAX, "%s-%d",
            filename, (int)buf_seed);

  result = rfs_get_config (&config_info);
  if (result != 0)
  {
    RFS_MSG_STRING_1 ("rfs_get_config failed %d", result);
    result = -RFS_EINVAL;
    goto End;
  }

  block_buf = fs_os_malloc (config_info.block_size);
  if (block_buf == NULL)
  {
    RFS_MSG_STRING_1 ("malloc of size %d failed ", config_info.block_size);
    result = -RFS_ENOMEM;
    goto End;
  }

  memset (&cb_param, 0, sizeof (cb_param));
  cb_param.start_magic = RFS_UTIL_PUT_GET_CB_PARAM_START_MAGIC;
  cb_param.end_magic = RFS_UTIL_PUT_GET_CB_PARAM_END_MAGIC;
  cb_param.file_size = file_size;
  cb_param.block_size = config_info.block_size;
  cb_param.block_buf = block_buf;
  cb_param.buf_seed = buf_seed;
  cb_param.num_blocks = (file_size / config_info.block_size) + 1;
  cb_param.verify_file = (uint32)verify_file;

  result = rfs_get_debug (seeded_filename, NULL, file_size, -1,
                          rfs_util_get_cb, &cb_param, debug_info);
  if (result != file_size)
  {
    RFS_MSG_STRING_1 ("%d, rfs_util_get failed", result);
    goto End;
  }

  result = (int)file_size;

End:
  if (block_buf != NULL)
  {
    fs_os_free (block_buf);
  }

  if (seeded_filename != NULL)
  {
    fs_os_free (seeded_filename);
  }

  return result;
}

int
rfs_util_write (const char *filename, uint32 offset, uint32 write_size,
                uint32 buf_seed, int oflag, int mode,
                struct rfs_debug_info_type *debug_info)
{
  int result = -1;
  uint8 *block_buf = NULL;

  RFS_MSG_STRING_3("rfs_util_write: %s %d %d", filename, offset, write_size);

  block_buf = malloc (write_size);
  if (block_buf == NULL)
  {
    RFS_MSG_STRING_1("rfs_util_write malloc of size %d failed", write_size);
    result = -RFS_ENOMEM;
    goto End;
  }

  memset (block_buf, (uint8) buf_seed, write_size);
  result = rfs_put_debug (filename, block_buf, write_size, offset,
                          oflag, mode, NULL, NULL, debug_info);
  if (result != write_size)
  {
    RFS_MSG_STRING_1 ("rfs_util_write put_debug failed %d", result);
    goto End;
  }

  RFS_MSG_STRING_1 ("rfs_util_write success %d", result);

End:
  if (block_buf != NULL)
  {
    free (block_buf);
  }
  return result;
}

int
rfs_util_read (const char *filename, uint32 offset, uint32 read_size,
               struct rfs_debug_info_type *debug_info)
{
  int result = -1;
  uint8 *block_buf = NULL;

  RFS_MSG_STRING_3("rfs_util_read: %s %d %d", filename, offset, read_size);

  block_buf = malloc (read_size);
  if (block_buf == NULL)
  {
    RFS_MSG_STRING_1("rfs_util_read malloc of size %d failed", read_size);
    result = -RFS_ENOMEM;
    goto End;
  }

  result = rfs_get_debug (filename, block_buf, read_size, offset,
                          NULL, NULL, debug_info);
  if (result != read_size)
  {
    RFS_MSG_STRING_1 ("rfs_util_read get_debug failed %d", result);
    goto End;
  }

  RFS_MSG_STRING_1 ("rfs_util_read success %d", result);

End:
  if (block_buf != NULL)
  {
    free (block_buf);
  }

  return result;
}
