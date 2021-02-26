/***********************************************************************
 * rfs_tftp.c
 *
 * RFS wrapper on top of TFTP.
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *
 * RFS implementation on top of TFTP. This maps the RFS APIs and data
 * structres to TFTP APIs and data structures and vice versa.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/rfs/src/rfs_tftp.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-06   dks   Add OS abstraction layer
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-10   dks   Separate client and server configs.
2014-12-03   dks   Move rfs init to a task
2014-10-20   dks   Remove rfs and tftp dependency
2014-10-29   dks   Move descriptor allocation logic to rfs_api layer.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-09-19   dks   Add hooks to extract performance numbers.
2014-07-29   dks   Fix locks for accessing fds
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-01-20   dks   Fix compilation on windows builds.
2014-01-13   dks   Add wrappers for rex critical section APIs.
2013-12-26   rp    Add tftp-client module.
2013-11-24   rp    Support TFTP extension options.
2013-11-12   dks   Create

===========================================================================*/

#include "rfs_config_i.h"

#ifdef FEATURE_RFS_USE_TFTP

#include "rfs_tftp.h"
#include "rfs_ops.h"
#include "rfs_fcntl.h"
#include "rfs_types.h"
#include "rfs_errno.h"
#include "rfs_assert.h"
#include "tftp_client.h"
#include "fs_os.h"
#include "rfs_msg.h"
#include "err.h"
#include "rfs_assert.h"
#include <stringl/stringl.h>

#include <string.h>

static int
rfs_tftp_map_tftp_to_rfs_error (enum tftp_client_result_type client_res)
{
  switch (client_res)
  {
    case TFTP_CLIENT_RESULT_SUCCESS:
      return RFS_ENOERROR;

    case TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS:
      return RFS_EBUSY;

    case TFTP_CLIENT_RESULT_ENOMEM:
      return RFS_ENOMEM;

    case TFTP_CLIENT_RESULT_ENOENT:
      return RFS_ENOENT;

    case TFTP_CLIENT_RESULT_EPERM:
      return RFS_EPERM;

    case TFTP_CLIENT_RESULT_ENOSPC:
      return RFS_ENOSPC;

    case TFTP_CLIENT_RESULT_EEXIST:
      return RFS_EEXISTS;

    case TFTP_CLIENT_RESULT_TIME_OUT:
      return RFS_ETIMEOUT;

    case TFTP_CLIENT_RESULT_NO_SERVER:
      return RFS_ENODEV;

    case TFTP_CLIENT_RESULT_INVALID_ARG:
      return RFS_EINVAL;

    case TFTP_CLIENT_RESULT_ECONFIG:
    default:
      return RFS_ESYSTEM;
  }
}

int32
rfs_tftp_read (const char* filename, uint32 offset, void *buf, uint32 nbyte)
{
  int32 read_size;
  uint32 out_buf_size;
  enum tftp_client_result_type result;

  if (nbyte == 0)
  {
    return 0;
  }

  result = tftp_read (filename, offset, buf, nbyte, &out_buf_size, NULL);
  if (result !=  TFTP_CLIENT_RESULT_SUCCESS)
  {
    read_size = rfs_tftp_map_tftp_to_rfs_error (result);
  }
  else
  {
    read_size = (int32)out_buf_size;
  }

  return read_size;
}

int32
rfs_tftp_write (const char *filename, uint32 offset, int oflag,
                const void *buf, uint32 nbyte)
{
  int32 wrote_size;
  uint32 out_buf_size;
  int append_mode = 0;
  enum tftp_client_result_type client_res;

  if ((oflag & RFS_O_APPEND) == RFS_O_APPEND)
  {
    append_mode = 1;
  }
  else
  {
    append_mode = 0;
  }

  client_res = tftp_write (filename, offset, append_mode, buf, nbyte,
                           &out_buf_size, NULL);
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    wrote_size = rfs_tftp_map_tftp_to_rfs_error (client_res);
  }
  else
  {
    wrote_size = (int32)out_buf_size;
  }

  return wrote_size;
}

int
rfs_tftp_unlink (const char *path)
{
  int result = 0;
  uint32 file_size;
  enum tftp_client_result_type client_res;

  client_res = tftp_stat (path, &file_size);
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    result = rfs_tftp_map_tftp_to_rfs_error (client_res);
    goto End;
  }

  client_res = tftp_unlink (path);
  result = rfs_tftp_map_tftp_to_rfs_error (client_res);

End:
  if(result == RFS_ENOENT)
  {
    result = RFS_ENOERROR;
  }
  return result;
}


int
rfs_tftp_stat (const char *path, struct rfs_stat_buf *buf)
{
  int result;
  uint32 size;
  enum tftp_client_result_type client_res;

  if (buf == NULL)
  {
    return RFS_EINVAL;
  }

  client_res = tftp_stat (path, &size);
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    result = rfs_tftp_map_tftp_to_rfs_error (client_res);
    return result;
  }

  memset (buf, 0x0, sizeof (struct rfs_stat_buf));
  buf->st_size = size;

  return 0;
}

int32
rfs_tftp_put_helper (const char *path, void *buf, uint32 length, int32 offset,
                     int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func,
                     void *cb_param, struct rfs_debug_info_type *debug_info)
{
  int32 result;
  uint32 out_buf_size;
  enum tftp_client_result_type client_res;
  (void) oflag;

  if ((buf == NULL) && (cb_func == NULL))
  {
    return RFS_EINVAL;
  }
  if ((buf != NULL) && (cb_func != NULL))
  {
    return RFS_EINVAL;
  }

  if (offset == -1)
  {
    client_res = tftp_put (path, buf, length, &out_buf_size,
                           (tftp_client_send_data_cb_type) cb_func,
                           cb_param, debug_info);
  }
  else
  {
    client_res = tftp_write (path, offset, 0, buf, length, &out_buf_size,
                             debug_info);
  }
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    result = rfs_tftp_map_tftp_to_rfs_error (client_res);
    return result;
  }

  return (int32)out_buf_size;
}

int32
rfs_tftp_get_helper (const char *path, void *buf, uint32 length, int32 offset,
                     RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                     struct rfs_debug_info_type *debug_info)
{
  int32 result;
  uint32 out_buf_size;
  enum tftp_client_result_type client_res;

  if ((buf == NULL) && (cb_func == NULL))
  {
    return RFS_EINVAL;
  }
  if ((buf != NULL) && (cb_func != NULL))
  {
    return RFS_EINVAL;
  }

  if (length == 0)
  {
    return 0;
  }

  if (offset == -1)
  {
    client_res = tftp_get (path, buf, length, &out_buf_size,
                           (tftp_client_recv_data_cb_type) cb_func,
                           cb_param, debug_info);
  }
  else
  {
    client_res = tftp_read (path, offset, buf, length, &out_buf_size,
                            debug_info);
  }

   if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
   {
     result = rfs_tftp_map_tftp_to_rfs_error (client_res);
     return result;
   }

  return (int32)out_buf_size;
}



int32
rfs_tftp_put (const char *path, void *buf, uint32 length, int oflag)
{
  return rfs_tftp_put_helper (path, buf, length, -1, oflag, NULL, NULL, NULL);
}

int32
rfs_tftp_put_debug (const char *path, void *buf, uint32 length, int32 offset,
                    int oflag, RFS_PUT_CALLBACK_FUNC_TYPE cb_func,
                    void *cb_param, struct rfs_debug_info_type *debug_info)
{
  return rfs_tftp_put_helper (path, buf, length, offset, oflag,
                              cb_func, cb_param, debug_info);
}

int32
rfs_tftp_get (const char *path, void *buf, uint32 length)
{
  return rfs_tftp_get_helper (path, buf, length, -1, NULL, NULL, NULL);
}

int32
rfs_tftp_get_debug (const char *path, void *buf, uint32 length, int32 offset,
                    RFS_GET_CALLBACK_FUNC_TYPE cb_func, void *cb_param,
                    struct rfs_debug_info_type *debug_info)
{
  return rfs_tftp_get_helper (path, buf, length, offset,
                              cb_func, cb_param, debug_info);
}

int
rfs_tftp_set_config (struct rfs_config_info_type *rfs_cfg)
{
  struct tftp_client_config_type tftp_cfg;
  int result = -1;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  RFS_ASSERT (rfs_cfg != NULL);
  if (rfs_cfg == NULL)
  {
    goto End;
  }

  memset (&tftp_cfg, 0, sizeof (tftp_cfg));
  result = tftp_client_config_get (&tftp_cfg);
  if (result != 0)
  {
    goto End;
  }
  tftp_cfg.block_size = rfs_cfg->block_size;
  tftp_cfg.window_size = rfs_cfg->window_size;
  tftp_cfg.timeout_in_ms = rfs_cfg->timeout_in_ms;
  tftp_cfg.max_pkt_retry_count = rfs_cfg->max_pkt_retry_count;
  tftp_cfg.max_wrong_pkt_count = rfs_cfg->max_wrong_pkt_count;

  result = tftp_client_config_set (&tftp_cfg);

End:
  return result;
}

int
rfs_tftp_get_config (struct rfs_config_info_type *rfs_cfg)
{
  struct tftp_client_config_type tftp_cfg;
  int result;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  RFS_ASSERT (rfs_cfg != NULL);
  if (rfs_cfg == NULL)
  {
    return -1;
  }

  memset (&tftp_cfg, 0, sizeof (tftp_cfg));
  result = tftp_client_config_get (&tftp_cfg);
  if (result != 0)
  {
    goto End;
  }

  rfs_cfg->block_size = tftp_cfg.block_size;
  rfs_cfg->window_size = tftp_cfg.window_size;
  rfs_cfg->timeout_in_ms = tftp_cfg.timeout_in_ms;
  rfs_cfg->max_pkt_retry_count = tftp_cfg.max_pkt_retry_count;
  rfs_cfg->max_wrong_pkt_count = tftp_cfg.max_wrong_pkt_count;

  result = 0;

 End:
  return result;
}

int
rfs_tftp_init (struct rfs_ops_type *base_ops)
{
  int result = -1;
  enum tftp_client_result_type client_res;

  RFS_ASSERT (base_ops != NULL);
  if (base_ops == NULL)
  {
    return -1;
  }

  client_res = tftp_client_init ();
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    goto End;
  }

  memset (base_ops, 0x0, sizeof (struct rfs_ops_type));

  base_ops->read = rfs_tftp_read;
  base_ops->write = rfs_tftp_write;
  base_ops->unlink = rfs_tftp_unlink;
  base_ops->stat = rfs_tftp_stat;
  base_ops->put = rfs_tftp_put;
  base_ops->get = rfs_tftp_get;
  base_ops->put_debug = rfs_tftp_put_debug;
  base_ops->get_debug = rfs_tftp_get_debug;
  base_ops->set_config = rfs_tftp_set_config;
  base_ops->get_config = rfs_tftp_get_config;

  result = 0;

End:
  return result;
}

#endif /* FEATURE_RFS_USE_TFTP */
