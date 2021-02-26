/***********************************************************************
 * tftp_client.c
 *
 * Qualcomm TFTP Client implemenation.
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Implemenation of the qualcomm tftp client APIs. These APIs can be hooked
 * to RFS APIs to provide POSIX like Filesystem implemenation to FS clients.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/client/src/tftp_client.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-06-05   rp    Improve TFTP log stats.
2015-11-09   rp    remove the boot up tftp-file-transfer.
2015-08-18   rp    Do not assert when server sends unknown error-code
2015-07-30   mj    Use tftp_strlen instead of strlen.
2015-05-01   dks   Add rsize extended option to read partial files.
2015-04-30   dks   Collect debug_info from rfs_read and rfs_write APIs.
2015-01-02   dks   Fix rfs_put API to perform stat at API layer.
2014-12-30   dks   Fixes to config and log module
2014-12-10   dks   Separate client and server configs.
2014-12-03   dks   Move rfs init to a task
2014-10-20   dks   Remove rfs and tftp dependency
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-09-19   dks   Add hooks to extract performance numbers.
2014-08-26   rp    Bring in changes from target-unit-testing.
2014-07-28   rp    Fix tftp client slot alloc and free.
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-30   nr    Support connected sockets and muti-poll.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Switch to IPCRouter sockets.
2014-01-20   dks   Fix compilation on windows builds.
2014-01-20   rp    Add support to do partial-file reads.
2014-01-16   rp    Handle error-pkt received from server.
2014-01-13   dks   Add wrappers for rex critical section APIs.
2013-12-26   rp    Add tftp-client module.
2013-11-24   nr    Support to set timeout and remote addr for an open conn.
2013-12-06   rp    Correct the TFTP_MAX_DATA_BLOCK_SIZE macro name.
2013-12-06   rp    Improve OPACK logic.
2013-12-05   rp    Add new debug-log interface.
2013-11-24   rp    Support TFTP extension options.
2013-11-21   nr    Abstract the OS layer across the OS's.
2013-11-15   dks   Add new tftp APIs to support RFS APIs.
2013-11-14   rp    Create

===========================================================================*/

#include "tftp_client_config.h"
#include "tftp_client.h"
#include "tftp_client_i.h"
#include "tftp_protocol.h"
#include "tftp_assert.h"
#include "tftp_malloc.h"
#include "tftp_string.h"
#include "tftp_log.h"
#include "tftp_threads.h"
#include "tftp_os.h"

#ifdef TFTP_SIMULATOR_BUILD
  #include "sim_framework.h"
#endif

#ifdef FEATURE_TFTP_SERVER_BUILD
  #error "Incorrect build configuration. Client is enabled in Server build."
#endif

/*----------------------------------------------------------------------------
  tftp-client : Remember each client's instance.
----------------------------------------------------------------------------*/
struct tftp_client_handle_type;

struct tftp_client_slot_type
{
  int is_in_use;
  struct tftp_client_handle_type *client_handle;
  uint32 age;
};

/*----------------------------------------------------------------------------
  tftp-client : Main Instance struct.
----------------------------------------------------------------------------*/
struct tftp_client_info_type
{
  int is_server_running;
  int is_partital_files_supported;

  struct tftp_client_internal_config_type client_internal_cfg;

  struct tftp_client_slot_type client_slots[TFTP_CLIENT_MAX_CLIENTS];
  uint32 clients_age_count;
};
static struct tftp_client_info_type tftp_client_info;


static enum tftp_client_result_type tftp_client_detect_server (void);


/*----------------------------------------------------------------------------
  tftp-client : Main critical-section
----------------------------------------------------------------------------*/
static tftp_mutex_handle tftp_client_cs;

static void
tftp_client_lock_init (void)
{
  uint32 result;
  memset (&tftp_client_cs, 0, sizeof (tftp_client_cs));
  result = tftp_thread_mutex_init (&tftp_client_cs);
  TFTP_ASSERT (result == 0);
  // todo : return failure form this function if lock-init fails
}

static void
tftp_client_lock (void)
{
  uint32 result;
  result = tftp_thread_lock (&tftp_client_cs);
  TFTP_ASSERT (result == 0);
  //todo : return failure from this function if lock fails
}

static void
tftp_client_unlock (void)
{
  uint32 result;
  result = tftp_thread_unlock (&tftp_client_cs);
  TFTP_ASSERT (result == 0);
  //todo : return failure from this fucntion if unlock fails
}


/*----------------------------------------------------------------------------
  tftp-client : Each client's instance.
----------------------------------------------------------------------------*/
struct tftp_client_handle_debug_info_type
{
  uint64 total_start_time_tick;
  uint64 total_end_time_tick;
  uint64 total_api_time;

  uint64 sock_open_start_time_tick;
  uint64 sock_open_end_time_tick;
  uint64 sock_open_time;

  uint64 req_start_time_tick;
  uint64 req_end_time_tick;
  uint64 req_time;

  uint64 data_xfer_start_time_tick;
  uint64 data_xfer_end_time_tick;
  uint64 data_xfer_time;
};

struct tftp_client_handle_type
{
  struct tftp_client_info_type *client_info;
  struct tftp_protocol_info proto;
  struct tftp_client_handle_debug_info_type debug_info;
};


struct tftp_client_slot_type*
tftp_client_allocate_slot (void)
{
  struct tftp_client_slot_type *list_node = NULL;
  uint32 i;

  tftp_client_lock ();

  for ( i = 0; i < TFTP_CLIENT_MAX_CLIENTS; ++i )
  {
    list_node = &tftp_client_info.client_slots[i];
    if (list_node->is_in_use == 0)
    {
      list_node->is_in_use = 1;
      list_node->age = tftp_client_info.clients_age_count++;
      break;
    }
  }

  if (i == TFTP_CLIENT_MAX_CLIENTS)
  {
    list_node = NULL;
  }

  tftp_client_unlock ();

  return list_node;
}

void
tftp_client_free_slot (struct tftp_client_slot_type *list_node)
{
  TFTP_ASSERT (list_node != NULL);
  if (list_node == NULL)
  {
    return; // todo : return failure from this function
  }

  tftp_client_lock ();
  TFTP_ASSERT (list_node->is_in_use == 1);
  list_node->is_in_use = 0;
  tftp_client_unlock ();
}

static enum tftp_client_result_type
tftp_client_map_proto_res_to_client_res (enum tftp_protocol_result proto_res)
{
  enum tftp_client_result_type client_res;

  switch (proto_res)
  {
    case TFTP_PROTOCOL_SUCCESS:
      client_res = TFTP_CLIENT_RESULT_SUCCESS;
      break;

    case TFTP_PROTOCOL_TIMEOUT:
      client_res = TFTP_CLIENT_RESULT_TIME_OUT;
      break;

    case TFTP_PROTOCOL_REMOTE_HANGUP:
    case TFTP_PROTOCOL_CONN_ERROR:
      client_res = TFTP_CLIENT_RESULT_NO_SERVER;
      break;
    case TFTP_PROTOCOL_PKT_ERROR:
      client_res = TFTP_CLIENT_RESULT_PKT_ERROR;
      break;

    case TFTP_PROTOCOL_WRONG_PACKET:
      client_res = TFTP_CLIENT_RESULT_RECD_WRONG_PKT;
      break;

    case TFTP_PROTOCOL_ERROR_PKT_RECD:
      client_res = TFTP_CLIENT_RESULT_ERROR_PKT_RECD;
      break;

    case TFTP_PROTOCOL_FILE_NOT_FOUND:
      client_res = TFTP_CLIENT_RESULT_ENOENT;
      break;

    case TFTP_PROTOCOL_ACCESS_VIOLATION:
      client_res = TFTP_CLIENT_RESULT_EPERM;
      break;

    case TFTP_PROTOCOL_DISK_FULL:
      client_res = TFTP_CLIENT_RESULT_ENOSPC;
      break;

    case TFTP_PROTOCOL_FILE_ALREADY_EXISTS:
      client_res = TFTP_CLIENT_RESULT_EEXIST;
      break;

    case TFTP_PROTOCOL_ENOSPC:
      client_res = TFTP_CLIENT_RESULT_ENOSPC;
      break;

    case TFTP_PROTOCOL_INVALID:
      client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
      break;

    case TFTP_PROTOCOL_ILLEGAL_FTP_OPERATION:
    case TFTP_PROTOCOL_UNKNOWN_TRANSFER_ID:
    case TFTP_PROTOCOL_NO_SUCH_USER:
    case TFTP_PROTOCOL_INVALID_OPTIONS:
    case TFTP_PROTOCOL_UNDEFINED_ERROR:
    case TFTP_PROTOCOL_EOT:
    default:
      client_res = TFTP_CLIENT_RESULT_UNKNOWN_ERROR;
       break;
  }

  return client_res;
}

static uint32
tftp_client_get_window_size (void)
{
  uint32 window_size = TFTP_CLIENT_WINDOW_SIZE;

  #ifdef TFTP_SIMULATOR_BUILD
    window_size = sim_framework_get_window_size ();
  #endif

  return window_size;
}

static void
tftp_client_read_build_config (struct tftp_client_info_type *client_info)
{
  struct tftp_client_internal_config_type *cli_config;

  TFTP_ASSERT (client_info != NULL);
  if (client_info == NULL)
  {
    return; // todo return failure
  }

  memset (&client_info->client_internal_cfg, 0,
          sizeof (client_info->client_internal_cfg));
  cli_config = &client_info->client_internal_cfg;

  cli_config->config.window_size = tftp_client_get_window_size ();
  cli_config->config.block_size = TFTP_CLIENT_DATA_BLOCK_SIZE;
  cli_config->config.timeout_in_ms = TFTP_CLIENT_TIMEOUT_IN_MS;
  cli_config->config.max_pkt_retry_count = TFTP_MAX_PKT_RETRY_COUNT;
  cli_config->config.max_wrong_pkt_count = TFTP_MAX_WRONG_PKT_COUNT;

  cli_config->server_addr.addr_type = TFTP_CONNECTION_IPCR_ADDR_TYPE_NAME;
  cli_config->server_addr.u.name_addr.service_id = TFTP_SERVER_SERVICE_ID;
  cli_config->server_addr.u.name_addr.instance_id =
                                           TFTP_CLIENT_SERVER_INSTANCE_ID;
}

static void
tftp_client_copy_build_config (struct tftp_client_handle_type *client_handle)
{
  struct tftp_client_internal_config_type *cli_config;
  struct tftp_protocol_info *proto;

  TFTP_ASSERT (client_handle != NULL);
  if (client_handle == NULL)
  {
    return; // todo return failure
  }

  cli_config = &client_handle->client_info->client_internal_cfg;
  proto = &client_handle->proto;

  memset(proto, 0, sizeof (struct tftp_protocol_info));

  proto->block_size = cli_config->config.block_size;
  proto->window_size = cli_config->config.window_size;
  proto->timeout_in_ms = cli_config->config.timeout_in_ms;
  proto->max_pkt_retry_count = cli_config->config.max_pkt_retry_count;
  proto->max_wrong_pkt_count = cli_config->config.max_wrong_pkt_count;

  tftp_memscpy (&proto->remote_addr, sizeof (proto->remote_addr),
                &cli_config->server_addr, sizeof (proto->remote_addr));
}

static void
tftp_client_populate_req_options(struct tftp_client_handle_type *client_handle,
                                 int append_mode, uint8 do_seek,
                                 uint32 seek_posn, int want_tsize,
                                 uint32 read_size)
{
  struct tftp_protocol_info *proto;
  struct tftp_pkt_options_type *options;
  uint32 idx = 0;

  TFTP_ASSERT (client_handle != NULL);
  if (client_handle == NULL)
  {
    return; // todo return failure
  }

  proto = &client_handle->proto;
  options = &proto->options;

  options->options[idx].option_id = TFTP_PKT_OPTION_ID_BLOCK_SIZE;
  options->options[idx].option_value = proto->block_size;
  options->options[idx].is_valid = 1;
  ++idx;

  options->options[idx].option_id = TFTP_PKT_OPTION_ID_TIMEOUT_IN_MS;
  options->options[idx].option_value = proto->timeout_in_ms;
  options->options[idx].is_valid = 1;
  ++idx;

  if (append_mode == 1)
  {
    options->options[idx].option_id = TFTP_PKT_OPTION_ID_APPEND;
    options->options[idx].option_value = seek_posn;
    options->options[idx].is_valid = 1;
    ++idx;
  }
  else if (do_seek == 1)
  {
    options->options[idx].option_id = TFTP_PKT_OPTION_ID_SEEK;
    options->options[idx].option_value = seek_posn;
    options->options[idx].is_valid = 1;
    ++idx;
  }

  if (want_tsize > 0)
  {
    options->options[idx].option_id = TFTP_PKT_OPTION_ID_TSIZE;
    options->options[idx].option_value = 0;
    options->options[idx].is_valid = 1;
    ++idx;
  }

  options->options[idx].option_id = TFTP_PKT_OPTION_ID_WINDOW_SIZE;
  options->options[idx].option_value = proto->window_size;
  options->options[idx].is_valid = 1;
  ++idx;

  if (read_size != 0)
  {
    options->options[idx].option_id = TFTP_PKT_OPTION_ID_RSIZE;
    options->options[idx].option_value = read_size;
    options->options[idx].is_valid = 1;
    ++idx;
  }

  options->extended_options_count = idx;
  options->are_all_options_valid = 1;
}


enum tftp_protocol_result
tftp_protocol_parse_and_apply_oack (struct tftp_protocol_info *proto)
{
  enum tftp_connection_result con_res;
  enum tftp_protocol_result proto_res = TFTP_PROTOCOL_SUCCESS;
  struct tftp_pkt_oack_pkt_fields *oack_pkt;
  uint32 option_idx, option_val;

  TFTP_ASSERT (proto->rx_pkt.opcode == TFTP_PKT_OPCODE_TYPE_OACK);
  oack_pkt = &proto->rx_pkt.pkt_fields.oack_pkt;
  TFTP_ASSERT (oack_pkt->options.extended_options_count >= 1);

  if (!oack_pkt->options.are_all_options_valid)
  {
    proto_res = TFTP_PROTOCOL_PKT_ERROR;
    goto End;
  }

  for ( option_idx = 0;
        option_idx < oack_pkt->options.extended_options_count;
        ++option_idx)
  {
    option_val = oack_pkt->options.options[option_idx].option_value;

    switch (oack_pkt->options.options[option_idx].option_id)
    {
      //TODO: Validate options
      case TFTP_PKT_OPTION_ID_BLOCK_SIZE:
        proto->block_size = option_val;
        break;

      case TFTP_PKT_OPTION_ID_TSIZE:
        proto->tsize = option_val;
        break;

      case TFTP_PKT_OPTION_ID_SEEK:
        proto->seek_offset = option_val;
        break;

      case TFTP_PKT_OPTION_ID_APPEND:
        proto->do_append = option_val;
        break;

      case TFTP_PKT_OPTION_ID_WINDOW_SIZE:
        proto->window_size = option_val;
        break;

      case TFTP_PKT_OPTION_ID_TIMEOUT_IN_MS:
        proto->timeout_in_ms = option_val;
        con_res = tftp_connection_set_timeout (&proto->connection_hdl,
                                                proto->timeout_in_ms);
        TFTP_ASSERT (con_res == TFTP_CONNECTION_RESULT_SUCCESS);
        // todo : handle the failure when assert is turned off.
        break;

     case TFTP_PKT_OPTION_ID_RSIZE:
        proto->rsize = option_val;
        break;

      case TFTP_PKT_OPTION_ID_TIMEOUT_IN_SECS:
      case TFTP_PKT_OPTION_ID_MAX:
      default:
        TFTP_ASSERT (0);
        // todo : Handle these cases.
        break;
    }
  }

End:
  return proto_res;
}


static enum tftp_client_result_type
tftp_put_helper (const char *filename, int append_mode, uint8 do_seek,
                 uint32 seek_posn, const void *buf, uint32 in_buf_size,
                 uint32 *out_buf_size, tftp_client_send_data_cb_type cb_func,
                 void *cb_func_param, struct rfs_debug_info_type *dbg_info)
{
  struct tftp_client_slot_type *client_slot = NULL;
  struct tftp_client_handle_type *client_handle = NULL;
  struct tftp_protocol_info *proto = NULL;
  struct tftp_client_handle_debug_info_type *debug_info = NULL;
  struct tftp_connection_debug_info_type *con_debug_info = NULL;
  enum tftp_protocol_result proto_res;
  struct tftp_pkt_type *rx_pkt;
  enum tftp_pkt_error_code err_code = TFTP_PKT_ERROR_CODE_NOT_DEFINED;
  const char *err_msg = NULL;
  enum tftp_client_result_type client_res;
  char *fname;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (filename != NULL);
  TFTP_ASSERT (out_buf_size != NULL);
  if ((filename == NULL) ||
      (out_buf_size == NULL))
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }

  TFTP_ASSERT (buf != NULL || cb_func != NULL);
  if ((buf == NULL) &&
      (cb_func == NULL))
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }
  if (buf == NULL)
  {
    TFTP_ASSERT (cb_func != NULL);
  }
  if (cb_func == NULL)
  {
    TFTP_ASSERT (buf != NULL);
  }

  fname = strrchr (filename, '/');
  if (fname == NULL)
  {
    fname = (char *) filename;
  }
  TFTP_LOG_INFO ("TFTP_PUT : %s of size %d", filename, in_buf_size);

  /*--------------------------------------------------------------------------
    Init with the server if not done already.
  --------------------------------------------------------------------------*/
  client_res = tftp_client_detect_server ();
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    client_res = TFTP_CLIENT_RESULT_NO_SERVER;
    goto End;
  }

  /*--------------------------------------------------------------------------
    Get a free client-slot, and hook it up with a new client-instance.
  --------------------------------------------------------------------------*/
  client_slot = tftp_client_allocate_slot ();
  if (client_slot == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS;
    goto End;
  }

  client_handle = tftp_malloc (sizeof (struct tftp_client_handle_type));
  if (client_handle == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_ENOMEM;
    goto End;
  }

  memset (client_handle, 0, sizeof (struct tftp_client_handle_type));
  client_slot->client_handle = client_handle;
  client_handle->client_info = &tftp_client_info;
  proto = &client_handle->proto;
  con_debug_info = &proto->connection_hdl.debug_info;
  debug_info = &client_handle->debug_info;

  debug_info->total_start_time_tick = tftp_timetick_get ();

  /*--------------------------------------------------------------------------
    Copy the global-config-settings to this client-instance.
  --------------------------------------------------------------------------*/
  tftp_client_copy_build_config (client_handle);

  debug_info->sock_open_start_time_tick = tftp_timetick_get ();

  proto_res = tftp_protocol_open_connection (&client_handle->proto);

  debug_info->sock_open_end_time_tick = tftp_timetick_get ();
  debug_info->sock_open_time = tftp_timetick_diff_in_usec (
                                     debug_info->sock_open_start_time_tick,
                                     debug_info->sock_open_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    Populate the OACK options
  --------------------------------------------------------------------------*/
  tftp_client_populate_req_options (client_handle, append_mode, do_seek,
                                    seek_posn, 0, 0);

  /*--------------------------------------------------------------------------
    Sent the Write-Request to the Server.
  --------------------------------------------------------------------------*/
  debug_info->req_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_send_wrq_helper (proto, filename);
  debug_info->req_end_time_tick = tftp_timetick_get ();
  debug_info->req_time = tftp_timetick_diff_in_usec (
    debug_info->req_start_time_tick, debug_info->req_end_time_tick);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_update_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_connect_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    For WRQ, we will either receive OACK or ACK for block-0
    On receipt of OACK, we have to send ACK for block-0.
  --------------------------------------------------------------------------*/
  rx_pkt = &proto->rx_pkt;
  TFTP_ASSERT ((rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_ACK) ||
               (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK));
  if (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK)
  {
    proto_res = tftp_protocol_parse_and_apply_oack (proto);
    if (proto_res != TFTP_PROTOCOL_SUCCESS)
    {
      client_res = tftp_client_map_proto_res_to_client_res (proto_res);
      err_code = TFTP_PKT_ERROR_CODE_INVALID_OPTIONS;
      err_msg = "Error in OACK-Options";
      goto Error;
    }
  }

  /*--------------------------------------------------------------------------
    Send the whole buffer.
  --------------------------------------------------------------------------*/
  *out_buf_size = 0;
  debug_info->data_xfer_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_send_data (proto, buf, in_buf_size, out_buf_size,
                                      (tftp_protocol_send_data_cb_type)cb_func,
                                       cb_func_param);
  debug_info->data_xfer_end_time_tick = tftp_timetick_get ();
  debug_info->data_xfer_time = tftp_timetick_diff_in_usec (
                                       debug_info->data_xfer_start_time_tick,
                                       debug_info->data_xfer_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  client_res = TFTP_CLIENT_RESULT_SUCCESS;

  goto End;

Error:
  tftp_protocol_send_error_packet (proto, err_code, err_msg);

End:
  if (proto != NULL)
  {
    TFTP_LOG_DBG ("WRQ stats : bytes, blocks, blocksize = [%d, %d, %d]",
                   proto->debug_info.total_bytes,
                   proto->debug_info.total_blocks,
                   proto->block_size);
    TFTP_LOG_DBG ("WRQ stats : Timedout-pkts = %d, Wrong-pkts = %d",
                  proto->debug_info.total_timeouts,
                  proto->debug_info.total_wrong_pkts);

    if ((debug_info != NULL) && (con_debug_info != NULL))
    {
      debug_info->total_end_time_tick = tftp_timetick_get ();
      debug_info->total_api_time = tftp_timetick_diff_in_usec (
                                        debug_info->total_start_time_tick,
                                        debug_info->total_end_time_tick);

      TFTP_LOG_DBG ("WRQ time stats : Total : [API, TX, RX] = [%u, %u, %u]",
                    debug_info->total_api_time,
                    con_debug_info->total_send_time_delta,
                    con_debug_info->total_recv_time_delta);

      TFTP_LOG_DBG ("WRQ time stats : [Open, Req, Xfer] = [%u, %u, %u]",
                    debug_info->sock_open_time,
                    debug_info->req_time,
                    debug_info->data_xfer_time);


      TFTP_LOG_DBG ("WRQ time stats : Tx [Min, Max] = [%u, %u]",
                    con_debug_info->min_send_time_delta,
                    con_debug_info->max_send_time_delta);

      TFTP_LOG_DBG ("WRQ time stats : Rx [Min, Max] = [%u, %u]",
                    con_debug_info->min_recv_time_delta,
                    con_debug_info->max_recv_time_delta);

      if (dbg_info != NULL)
      {
        dbg_info->block_size = proto->block_size;
        dbg_info->total_blocks = proto->debug_info.total_blocks;
        dbg_info->total_api_time = debug_info->total_api_time;
        dbg_info->sock_open_time = debug_info->sock_open_time;
        dbg_info->req_time = debug_info->req_time;
        dbg_info->data_xfer_time = debug_info->data_xfer_time;
        dbg_info->total_tx_time = con_debug_info->total_send_time_delta;
        dbg_info->total_rx_time = con_debug_info->total_recv_time_delta;
        dbg_info->min_tx_time = con_debug_info->min_send_time_delta;
        dbg_info->max_tx_time = con_debug_info->max_send_time_delta;
        dbg_info->min_rx_time = con_debug_info->min_recv_time_delta;
        dbg_info->max_rx_time = con_debug_info->max_recv_time_delta;
      }
    }

    (void)tftp_protocol_close_connection(proto);
  }

  if (client_handle != NULL)
  {
    tftp_free (client_handle);
    client_handle = NULL;
  }

  if (client_slot != NULL)
  {
    tftp_client_free_slot (client_slot);
    client_slot = NULL;
  }


  return client_res;
}


static enum tftp_client_result_type
tftp_get_helper (const char *filename, int is_partial, uint8 do_seek,
                 uint32 seek_posn, void *buf, uint32 in_buf_size,
                 uint32 *out_buf_size, tftp_client_recv_data_cb_type cb_func,
                 void *cb_func_param, struct rfs_debug_info_type *dbg_info)
{
  struct tftp_client_slot_type *client_slot = NULL;
  struct tftp_client_handle_type *client_handle = NULL;
  struct tftp_protocol_info *proto = NULL;
  struct tftp_client_handle_debug_info_type *debug_info = NULL;
  struct tftp_connection_debug_info_type *con_debug_info = NULL;
  enum tftp_protocol_result proto_res;
  struct tftp_pkt_type *rx_pkt;
  struct tftp_pkt_data_pkt_fields *data_pkt;
  uint32 recd_buf_size;
  enum tftp_pkt_error_code err_code = TFTP_PKT_ERROR_CODE_NOT_DEFINED;
  const char *err_msg = NULL;
  enum tftp_client_result_type client_res;
  char *fname;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (filename != NULL);
  TFTP_ASSERT (out_buf_size != NULL);
  if ((filename == NULL) ||
      (out_buf_size == NULL))
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }

  TFTP_ASSERT (buf != NULL || cb_func != NULL);
  if ((buf == NULL) &&
      (cb_func == NULL))
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }
  if (buf == NULL)
  {
    TFTP_ASSERT (cb_func != NULL);
  }
  if (cb_func == NULL)
  {
    TFTP_ASSERT (buf != NULL);
  }

  *out_buf_size = 0;

  fname = strrchr (filename, '/');
  if (fname == NULL)
  {
    fname = (char *) filename;
  }
  TFTP_LOG_INFO ("TFTP_GET : %s of size %d", filename, in_buf_size);

  /*--------------------------------------------------------------------------
    Init with the server if not done already.
  --------------------------------------------------------------------------*/
  client_res = tftp_client_detect_server ();
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    client_res = TFTP_CLIENT_RESULT_NO_SERVER;
    goto End;
  }

  /*--------------------------------------------------------------------------
    Get a free client-slot, and hook it up with a new client-instance.
  --------------------------------------------------------------------------*/
  client_slot = tftp_client_allocate_slot ();
  if (client_slot == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS;
    goto End;
  }

  client_handle = tftp_malloc (sizeof (struct tftp_client_handle_type));
  if (client_handle == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_ENOMEM;
    goto End;
  }

  memset (client_handle, 0, sizeof (struct tftp_client_handle_type));
  client_slot->client_handle = client_handle;
  client_handle->client_info = &tftp_client_info;
  proto = &client_handle->proto;
  con_debug_info = &proto->connection_hdl.debug_info;
  debug_info = &client_handle->debug_info;

  debug_info->total_start_time_tick = tftp_timetick_get ();

  /*--------------------------------------------------------------------------
    Copy the global-config-settings to this client-instance.
  --------------------------------------------------------------------------*/
  tftp_client_copy_build_config (client_handle);

  debug_info->sock_open_start_time_tick = tftp_timetick_get ();

  proto_res = tftp_protocol_open_connection (&client_handle->proto);

  debug_info->sock_open_end_time_tick = tftp_timetick_get ();
  debug_info->sock_open_time = tftp_timetick_diff_in_usec (
                                     debug_info->sock_open_start_time_tick,
                                     debug_info->sock_open_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    Populate the OACK options
  --------------------------------------------------------------------------*/
  tftp_client_populate_req_options (client_handle, 0, do_seek, seek_posn, 0,
                                    in_buf_size);

  /*--------------------------------------------------------------------------
    Sent the Write-Request to the Server.
  --------------------------------------------------------------------------*/
  debug_info->req_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_send_rrq_helper (proto, filename);
  debug_info->req_end_time_tick = tftp_timetick_get ();
  debug_info->req_time =
                  tftp_timetick_diff_in_usec (debug_info->req_start_time_tick,
                                              debug_info->req_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_update_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_connect_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    For RRQ, we will either receive OACK or DATA-block-0.
    On receipt of OACK, we have to send ACK for block-0 and wait for
    DATA-block-0.
  --------------------------------------------------------------------------*/
  rx_pkt = &proto->rx_pkt;
  data_pkt = &rx_pkt->pkt_fields.data_pkt;
  TFTP_ASSERT ((rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_DATA) ||
               (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK));
  switch (rx_pkt->opcode)
  {
    case TFTP_PKT_OPCODE_TYPE_OACK:
    {
      proto_res = tftp_protocol_parse_and_apply_oack (proto);
      if (proto_res != TFTP_PROTOCOL_SUCCESS)
      {
        client_res = tftp_client_map_proto_res_to_client_res (proto_res);
        err_code = TFTP_PKT_ERROR_CODE_INVALID_OPTIONS;
        err_msg = "Error in OACK-Options";
        goto Error;
      }

      proto_res = tftp_protocol_send_ack_wait_for_data (proto);
      if (proto_res != TFTP_PROTOCOL_SUCCESS)
      {
        client_res = tftp_client_map_proto_res_to_client_res (proto_res);
        goto End;
      }
    }
    break;

    case TFTP_PKT_OPCODE_TYPE_DATA:
    {
      TFTP_ASSERT (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_DATA);
      TFTP_ASSERT (data_pkt->block_number == 1);
    }
    break;

    default:
    {
      TFTP_ASSERT (0);
    }
    break;
  }

  /*--------------------------------------------------------------------------
    Receive all the data.
  --------------------------------------------------------------------------*/
  recd_buf_size = 0;

  debug_info->data_xfer_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_recv_data (proto, buf, in_buf_size, &recd_buf_size,
                                     (tftp_protocol_recv_data_cb_type) cb_func,
                                      NULL, cb_func_param);
  debug_info->data_xfer_end_time_tick = tftp_timetick_get ();
  debug_info->data_xfer_time = tftp_timetick_diff_in_usec (
                                       debug_info->data_xfer_start_time_tick,
                                       debug_info->data_xfer_end_time_tick);

  switch (proto_res)
  {
    case TFTP_PROTOCOL_SUCCESS:
    case TFTP_PROTOCOL_INSUFFICIENT_BUFFER:
    case TFTP_PROTOCOL_ENOSPC:
    {
      TFTP_ASSERT (in_buf_size >= recd_buf_size);
      *out_buf_size = recd_buf_size;
      client_res = TFTP_CLIENT_RESULT_SUCCESS;
      if (proto_res == TFTP_PROTOCOL_INSUFFICIENT_BUFFER)
      {
        err_code = TFTP_PKT_ERROR_CODE_END_OF_TRANSFER;
        err_msg = "End of transfer";
        goto Error;
      }
    }
    break;

    default:
    {
      client_res = tftp_client_map_proto_res_to_client_res (proto_res);
      goto End;
    }
  }

  goto End;

Error:
  tftp_protocol_send_error_packet (proto, err_code, err_msg);

End:
  if (proto != NULL)
  {
    TFTP_LOG_DBG ("RRQ stats : Block Size %d, Total-blocks %d",
                   proto->block_size, proto->debug_info.total_blocks);
    TFTP_LOG_DBG ("RRQ stats : Total-bytes = %d : %d",
                   proto->debug_info.total_bytes, in_buf_size);
    TFTP_LOG_DBG ("RRQ stats : Timedout-pkts = %d, Wrong-pkts = %d",
                  proto->debug_info.total_timeouts,
                  proto->debug_info.total_wrong_pkts);

    if (debug_info != NULL)
    {
      debug_info->total_end_time_tick = tftp_timetick_get ();
      debug_info->total_api_time = tftp_timetick_diff_in_usec (
                                        debug_info->total_start_time_tick,
                                        debug_info->total_end_time_tick);

      TFTP_LOG_DBG ("RRQ time stats : Total = %u",
                    debug_info->total_api_time);

      TFTP_LOG_DBG ("RRQ time stats : Total : [API, TX, RX] = [%u, %u, %u]",
                    debug_info->total_api_time,
                    con_debug_info->total_send_time_delta,
                    con_debug_info->total_recv_time_delta);

      TFTP_LOG_DBG ("RRQ time stats : [Open, Req, Xfer] = [%u, %u, %u]",
                    debug_info->sock_open_time,
                    debug_info->req_time,
                    debug_info->data_xfer_time);


      TFTP_LOG_DBG ("RRQ time stats : Tx [Min, Max] = [%u, %u]",
                    con_debug_info->min_send_time_delta,
                    con_debug_info->max_send_time_delta);

      TFTP_LOG_DBG ("RRQ time stats : Rx [Min, Max] = [%u, %u]",
                    con_debug_info->min_recv_time_delta,
                    con_debug_info->max_recv_time_delta);

      if (dbg_info != NULL)
      {
        dbg_info->block_size = proto->block_size;
        dbg_info->total_blocks = proto->debug_info.total_blocks;
        dbg_info->total_api_time = debug_info->total_api_time;
        dbg_info->sock_open_time = debug_info->sock_open_time;
        dbg_info->req_time = debug_info->req_time;
        dbg_info->data_xfer_time = debug_info->data_xfer_time;
        dbg_info->total_tx_time = con_debug_info->total_send_time_delta;
        dbg_info->total_rx_time = con_debug_info->total_recv_time_delta;
        dbg_info->min_tx_time = con_debug_info->min_send_time_delta;
        dbg_info->max_tx_time = con_debug_info->max_send_time_delta;
        dbg_info->min_rx_time = con_debug_info->min_recv_time_delta;
        dbg_info->max_rx_time = con_debug_info->max_recv_time_delta;
      }
    }

    (void)tftp_protocol_close_connection(proto);
  }

  if (client_handle != NULL)
  {
    tftp_free (client_handle);
    client_handle = NULL;
  }

  if (client_slot != NULL)
  {
    tftp_client_free_slot (client_slot);
    client_slot = NULL;
  }


  return client_res;
}

enum tftp_client_result_type
tftp_unlink (const char *filename)
{
  struct tftp_client_slot_type *client_slot = NULL;
  struct tftp_client_handle_type *client_handle = NULL;
  struct tftp_protocol_info *proto = NULL;
  struct tftp_client_handle_debug_info_type *debug_info = NULL;
  enum tftp_protocol_result proto_res;
  struct tftp_pkt_type *rx_pkt;
  enum tftp_client_result_type client_res;
  struct tftp_pkt_options_type *options;
  char *fname;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (filename != NULL);
  if (filename == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }

  fname = strrchr (filename, '/');
  if (fname == NULL)
  {
    fname = (char *) filename;
  }
  TFTP_LOG_INFO ("TFTP_UNLINK : %s", filename);

  /*--------------------------------------------------------------------------
    Init with the server if not done already.
  --------------------------------------------------------------------------*/
  client_res = tftp_client_detect_server ();
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    client_res = TFTP_CLIENT_RESULT_NO_SERVER;
    goto End;
  }

  /*--------------------------------------------------------------------------
    Get a free client-slot, and hook it up with a new client-instance.
  --------------------------------------------------------------------------*/
  client_slot = tftp_client_allocate_slot ();
  if (client_slot == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS;
    goto End;
  }

  client_handle = tftp_malloc (sizeof (struct tftp_client_handle_type));
  if (client_handle == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_ENOMEM;
    goto End;
  }

  memset (client_handle, 0, sizeof (struct tftp_client_handle_type));
  client_slot->client_handle = client_handle;
  client_handle->client_info = &tftp_client_info;
  proto = &client_handle->proto;
  debug_info = &client_handle->debug_info;

  debug_info->total_start_time_tick = tftp_timetick_get ();

  /*--------------------------------------------------------------------------
    Copy the global-config-settings to this client-instance.
  --------------------------------------------------------------------------*/
  tftp_client_copy_build_config (client_handle);

  debug_info->sock_open_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_open_connection (proto);
  debug_info->sock_open_end_time_tick = tftp_timetick_get ();
  debug_info->sock_open_time = tftp_timetick_diff_in_usec (
                                     debug_info->sock_open_start_time_tick,
                                     debug_info->sock_open_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    Populate the OACK options with just unlink
  --------------------------------------------------------------------------*/
  options = &proto->options;

  options->options[0].option_id = TFTP_PKT_OPTION_ID_UNLINK;
  options->options[0].option_value = 0;
  options->options[0].is_valid = 1;
  options->extended_options_count = 1;
  options->are_all_options_valid = 1;

  /*--------------------------------------------------------------------------
    Sent the Write-Request to the Server.
  --------------------------------------------------------------------------*/
  debug_info->req_start_time_tick = tftp_timetick_get ();
  proto_res = tftp_protocol_send_wrq_helper (proto, filename);
  debug_info->req_end_time_tick = tftp_timetick_get ();
  debug_info->req_time = tftp_timetick_diff_in_usec (
                                    debug_info->req_start_time_tick,
                                    debug_info->req_end_time_tick);

  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    For WRQ, we will either receive OACK or ACK for block-0
    On receipt of OACK, we have to send ACK for block-0.
  --------------------------------------------------------------------------*/
  rx_pkt = &proto->rx_pkt;
  TFTP_ASSERT ((rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_ACK) ||
               (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK));

  /*
   * Since unlink was the only option passed. A lack of OACK indicates that
   * unlink is not supported on this server.
   */
  if (rx_pkt->opcode != TFTP_PKT_OPCODE_TYPE_OACK)
  {
    tftp_protocol_send_error_packet (proto,TFTP_PKT_ERROR_CODE_INVALID_OPTIONS,
                                     "Expected OACK for UNLINK");

    client_res = TFTP_CLIENT_RESULT_NOT_SUPPORTED;
  }

End:
  if (proto != NULL)
  {
    if (debug_info != NULL)
    {
      debug_info->total_end_time_tick = tftp_timetick_get ();
      debug_info->total_api_time = tftp_timetick_diff_in_usec (
                                        debug_info->total_start_time_tick,
                                        debug_info->total_end_time_tick);
      TFTP_LOG_DBG ("Unlink time stats : Total = %u",
                    debug_info->total_api_time);

      TFTP_LOG_DBG ("Unlink time stats : WRQ = %u",
                    debug_info->req_time);

      TFTP_LOG_DBG ("Unlink time stats : Conn open = %u",
                    debug_info->sock_open_time);
    }

    (void)tftp_protocol_close_connection(proto);
  }

  if (client_handle != NULL)
  {
    tftp_free (client_handle);
    client_handle = NULL;
  }

  if (client_slot != NULL)
  {
    tftp_client_free_slot (client_slot);
    client_slot = NULL;
  }

  return client_res;
}

enum tftp_client_result_type
tftp_put (const char *filename, const void *buf, uint32 in_buf_size,
          uint32 *out_buf_size, tftp_client_send_data_cb_type cb_func,
          void *cb_param, struct rfs_debug_info_type *debug_info)
{
  return tftp_put_helper (filename, 0, 0, 0, buf, in_buf_size, out_buf_size,
                          cb_func, cb_param, debug_info);
}

enum tftp_client_result_type
tftp_get (const char *filename, void *buf, uint32 in_buf_size,
          uint32 *out_buf_size, tftp_client_recv_data_cb_type cb_func,
          void *cb_param, struct rfs_debug_info_type *debug_info)
{
  return tftp_get_helper (filename, 0, 0, 0, buf, in_buf_size, out_buf_size,
                          cb_func, cb_param, debug_info);
}

enum tftp_client_result_type
tftp_read (const char *filename, uint32 offset,
           void *buf, uint32 in_buf_size, uint32 *out_buf_size,
           struct rfs_debug_info_type *debug_info)
{
  if (tftp_client_info.is_server_running)
  {
    if (!tftp_client_info.is_partital_files_supported)
    {
      return TFTP_CLIENT_RESULT_NOT_SUPPORTED;
    }
  }

  return tftp_get_helper (filename, 1, 1, offset, buf, in_buf_size,
                          out_buf_size, NULL, NULL, debug_info);
}

enum tftp_client_result_type
tftp_write (const char *filename, uint32 offset, int append_mode,
            const void *buf, uint32 in_buf_size, uint32 *out_buf_size,
            struct rfs_debug_info_type *debug_info)
{
  if (tftp_client_info.is_server_running)
  {
    if (!tftp_client_info.is_partital_files_supported)
    {
      return TFTP_CLIENT_RESULT_NOT_SUPPORTED;
    }
  }

  return tftp_put_helper (filename, append_mode, 1, offset, buf,
                          in_buf_size, out_buf_size, NULL, NULL, debug_info);
}

enum tftp_client_result_type
tftp_stat (const char *filename, uint32 *filesize)
{
  struct tftp_client_slot_type *client_slot = NULL;
  struct tftp_client_handle_type *client_handle = NULL;
  struct tftp_protocol_info *proto = NULL;
  enum tftp_protocol_result proto_res;
  struct tftp_pkt_type *rx_pkt;
  enum tftp_pkt_error_code err_code;
  const char *err_msg = NULL;
  enum tftp_client_result_type client_res;
  char *fname;

  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (filename != NULL);
  TFTP_ASSERT (filesize != NULL);
  if ((filename == NULL) ||
      (filesize == NULL))
  {
    client_res = TFTP_CLIENT_RESULT_INVALID_ARG;
    goto End;
  }

  fname = strrchr (filename, '/');
  if (fname == NULL)
  {
    fname = (char *)filename;
  }
  TFTP_LOG_INFO ("TFTP_STAT : %s", filename);

  *filesize = 0;

  /*--------------------------------------------------------------------------
    Init with the server if not done already.
  --------------------------------------------------------------------------*/
  client_res = tftp_client_detect_server ();
  if (client_res != TFTP_CLIENT_RESULT_SUCCESS)
  {
    client_res = TFTP_CLIENT_RESULT_NO_SERVER;
    goto End;
  }

  /*--------------------------------------------------------------------------
    Get a free client-slot, and hook it up with a new client-instance.
  --------------------------------------------------------------------------*/
  client_slot = tftp_client_allocate_slot ();
  if (client_slot == NULL)
  {
    return TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS;
  }

  client_handle = tftp_malloc (sizeof (struct tftp_client_handle_type));
  if (client_handle == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_ENOMEM;
    goto End;
  }

  memset (client_handle, 0, sizeof (struct tftp_client_handle_type));
  client_slot->client_handle = client_handle;
  client_handle->client_info = &tftp_client_info;

  proto = &client_handle->proto;

  /*--------------------------------------------------------------------------
    Copy the global-config-settings to this client-instance.
  --------------------------------------------------------------------------*/
  tftp_client_copy_build_config (client_handle);

  proto_res = tftp_protocol_open_connection (&client_handle->proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    Populate the OACK options
  --------------------------------------------------------------------------*/
  tftp_client_populate_req_options (client_handle, 0, 0, 0, 1, 0);

  /*--------------------------------------------------------------------------
    Sent the Read-Request to the Server.
  --------------------------------------------------------------------------*/
  proto_res = tftp_protocol_send_rrq_helper (proto, filename);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_update_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_connect_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    For RRQ, we will either receive OACK or DATA-block-0.
    On receipt of OACK, we have to send ACK for block-0 and wait for
    DATA-block-0.
  --------------------------------------------------------------------------*/
  rx_pkt = &proto->rx_pkt;
  TFTP_ASSERT ((rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_DATA) ||
               (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK));


  if (rx_pkt->opcode != TFTP_PKT_OPCODE_TYPE_OACK)
  {
    client_res = TFTP_CLIENT_RESULT_NOT_SUPPORTED;
    err_code = TFTP_PKT_ERROR_CODE_NOT_DEFINED;
    err_msg = "Need OACK support.";
    goto Error;
  }

  proto_res = tftp_protocol_parse_and_apply_oack (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    err_code = TFTP_PKT_ERROR_CODE_INVALID_OPTIONS;
    err_msg = "Error in OACK-Options";
    goto Error;
  }

  *filesize = proto->tsize;
  client_res = TFTP_CLIENT_RESULT_SUCCESS;

  err_code = TFTP_PKT_ERROR_CODE_END_OF_TRANSFER;
  err_msg = "End of Transfer";

Error:
  (void) tftp_protocol_send_error_packet (proto, err_code, err_msg);

End:

  if (proto != NULL)
  {
    TFTP_LOG_DBG ("RRQ stats : total-blocks = %d : total-bytes = %d"
                  " timedout-pkts = %d, wrong-pkts = %d",
                  proto->debug_info.total_blocks,
                  proto->debug_info.total_bytes,
                  proto->debug_info.total_timeouts,
                  proto->debug_info.total_wrong_pkts);

    (void)tftp_protocol_close_connection(proto);
  }

  if (client_slot != NULL)
  {
    tftp_client_free_slot (client_slot);
    client_slot = NULL;
  }

  if (client_handle != NULL)
  {
    tftp_free (client_handle);
    client_handle = NULL;
  }

  return client_res;
}


int
tftp_client_config_set (struct tftp_client_config_type *config)
{
  int result = -1;
  struct tftp_client_config_type *tftp_cfg;
  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (config != NULL);
  if (config == NULL)
  {
    return result;
  }

  tftp_client_lock ();

  tftp_cfg = &tftp_client_info.client_internal_cfg.config;

  if (config->block_size > TFTP_MAX_DATA_BLOCK_SIZE)
  {
    TFTP_LOG_ERR("Cannot set to block size %d > max %d", config->block_size,
                  TFTP_MAX_DATA_BLOCK_SIZE);
  }
  else
  {
    tftp_cfg->block_size = config->block_size;
    result = 0;
  }

  if (config->window_size > TFTP_MAX_WINDOW_SIZE)
  {
    TFTP_LOG_ERR("Cannot set to window size %d > max %d", config->window_size,
                 TFTP_MAX_WINDOW_SIZE);
  }
  else
  {
    tftp_cfg->window_size = config->window_size;
    result = 0;
  }

  if (config->timeout_in_ms > TFTP_MAX_TIMEOUT_IN_MS)
  {
    TFTP_LOG_ERR("Cannot set to timeout %d > max %d", config->timeout_in_ms,
                 TFTP_MAX_TIMEOUT_IN_MS);
  }
  else
  {
    tftp_cfg->timeout_in_ms = config->timeout_in_ms;
    result = 0;
  }

  if (config->max_pkt_retry_count != (~0))
  {
    tftp_cfg->max_pkt_retry_count = config->max_pkt_retry_count;
    result = 0;
  }

  if (config->max_wrong_pkt_count != (~0))
  {
    tftp_cfg->max_wrong_pkt_count = config->max_wrong_pkt_count;
    result = 0;
  }

  TFTP_LOG_DBG ("Block Size: %d",
    tftp_client_info.client_internal_cfg.config.block_size);
  TFTP_LOG_DBG ("Window Size: %d",
    tftp_client_info.client_internal_cfg.config.window_size);
  TFTP_LOG_DBG ("Timeout in MS: %d",
    tftp_client_info.client_internal_cfg.config.timeout_in_ms);
  TFTP_LOG_DBG ("Max Pkt Retry Count: %d",
    tftp_client_info.client_internal_cfg.config.max_pkt_retry_count);
  TFTP_LOG_DBG ("Max Wrong Pkt Count: %d",
    tftp_client_info.client_internal_cfg.config.max_wrong_pkt_count);

  tftp_client_unlock ();

  return result;
}

int
tftp_client_config_get (struct tftp_client_config_type *config)
{
  /*--------------------------------------------------------------------------
    Validate the input args.
  --------------------------------------------------------------------------*/
  TFTP_ASSERT (config != NULL);
  if (config == NULL)
  {
    return -1;
  }

  tftp_client_lock ();

  tftp_memscpy (config, sizeof (struct tftp_client_config_type),
                &tftp_client_info.client_internal_cfg.config,
                sizeof (tftp_client_info.client_internal_cfg.config));

  tftp_client_unlock ();

  return 0;
}

int
tftp_client_is_partial_files_supported (void)
{
  int is_partial_files_supported = 0;

  tftp_client_lock ();

  is_partial_files_supported = tftp_client_info.is_partital_files_supported;

  tftp_client_unlock ();

  return is_partial_files_supported;
}

static enum tftp_client_result_type
tftp_client_detect_server (void)
{
  const char *filename = TFTP_SERVER_TEMP_FILEPATH;
  const uint8 *buf = NULL;
  struct tftp_client_slot_type *client_slot = NULL;
  struct tftp_client_handle_type *client_handle = NULL;
  struct tftp_protocol_info *proto = NULL;
  enum tftp_protocol_result proto_res;
  struct tftp_pkt_type *rx_pkt;
  enum tftp_pkt_error_code err_code = TFTP_PKT_ERROR_CODE_NOT_DEFINED;
  const char *err_msg = NULL;
  enum tftp_client_result_type client_res;
  uint32 in_buf_size, sent_size = 0;

  tftp_client_lock ();

  /*--------------------------------------------------------------------------
    If we have already connected with the server once, then return success.
  --------------------------------------------------------------------------*/
  if (tftp_client_info.is_server_running)
  {
    client_res = TFTP_CLIENT_RESULT_SUCCESS;
    goto End;
  }

  /*--------------------------------------------------------------------------
    Get a free client-slot, and hook it up with a new client-instance.
  --------------------------------------------------------------------------*/
  client_slot = tftp_client_allocate_slot ();
  if (client_slot == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_TOO_MANY_REQUESTS;
    goto End;
  }

  client_handle = tftp_malloc (sizeof (struct tftp_client_handle_type));
  if (client_handle == NULL)
  {
    client_res = TFTP_CLIENT_RESULT_ENOMEM;
    goto End;
  }

  memset (client_handle, 0, sizeof (struct tftp_client_handle_type));
  client_slot->client_handle = client_handle;
  client_handle->client_info = &tftp_client_info;
  proto = &client_handle->proto;
  rx_pkt = &proto->rx_pkt;

  /*--------------------------------------------------------------------------
    Copy the global-config-settings to this client-instance.
  --------------------------------------------------------------------------*/
  tftp_client_copy_build_config (client_handle);

  proto_res = tftp_protocol_open_connection (&client_handle->proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  /*--------------------------------------------------------------------------
    Populate the OACK options
  --------------------------------------------------------------------------*/
  tftp_client_populate_req_options (client_handle, 0, 0, 0, 0, 0);

  /*--------------------------------------------------------------------------
    Sent the Write-Request to the Server.
  --------------------------------------------------------------------------*/
  proto_res = tftp_protocol_send_wrq_helper (proto, filename);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_update_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  proto_res = tftp_protocol_connect_remote_addr (proto);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  tftp_client_info.is_server_running = 1;

  /*--------------------------------------------------------------------------
    For WRQ, we will either receive OACK or ACK for block-0
    On receipt of OACK, we have to send ACK for block-0.
  --------------------------------------------------------------------------*/
  rx_pkt = &proto->rx_pkt;
  TFTP_ASSERT ((rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_ACK) ||
               (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK));
  if (rx_pkt->opcode == TFTP_PKT_OPCODE_TYPE_OACK)
  {
    proto_res = tftp_protocol_parse_and_apply_oack (proto);
    if (proto_res != TFTP_PROTOCOL_SUCCESS)
    {
      client_res = tftp_client_map_proto_res_to_client_res (proto_res);
      err_code = TFTP_PKT_ERROR_CODE_INVALID_OPTIONS;
      err_msg = "Error in OACK-Options";
      goto Error;
    }

    //todo : confirm if seek_offset is present in the oack?
    tftp_client_info.is_partital_files_supported = 1;
  }

  /*--------------------------------------------------------------------------
    Send the whole buffer.
  --------------------------------------------------------------------------*/
  buf = (const uint8 *)TFTP_SERVER_TEMP_FILEPATH_DATA;
  in_buf_size = tftp_strlen (TFTP_SERVER_TEMP_FILEPATH_DATA);
  proto_res = tftp_protocol_send_data (proto, buf, in_buf_size, &sent_size,
                                        NULL, 0);
  if (proto_res != TFTP_PROTOCOL_SUCCESS)
  {
    client_res = tftp_client_map_proto_res_to_client_res (proto_res);
    goto End;
  }

  client_res = TFTP_CLIENT_RESULT_SUCCESS;

  goto End;

Error:
  tftp_protocol_send_error_packet (proto, err_code, err_msg);

End:
  if (proto != NULL)
  {
     TFTP_LOG_DBG ("WRQ stats : sent_size = %u total-blocks = %d "
                    "total-bytes = %d timedout-pkts = %d, wrong-pkts = %d",
                    sent_size,
                    proto->debug_info.total_blocks,
                    proto->debug_info.total_bytes,
                    proto->debug_info.total_timeouts,
                    proto->debug_info.total_wrong_pkts);

    (void)tftp_protocol_close_connection(proto);
  }

  if (client_slot != NULL)
  {
    tftp_client_free_slot (client_slot);
    client_slot = NULL;
  }

  if (client_handle != NULL)
  {
    tftp_free (client_handle);
    client_handle = NULL;
  }

  tftp_client_unlock ();

  return client_res;
}


enum tftp_client_result_type
tftp_client_init (void)
{

  tftp_client_lock_init ();
  tftp_client_lock ();

  memset (&tftp_client_info, 0, sizeof (tftp_client_info));

  tftp_malloc_init ();
  tftp_os_init ();
  tftp_log_init ();
  tftp_utils_init ();
  tftp_connection_init ();

  tftp_client_read_build_config (&tftp_client_info);

  tftp_client_unlock ();

  return TFTP_CLIENT_RESULT_SUCCESS;
}
