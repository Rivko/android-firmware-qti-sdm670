/***********************************************************************
 * tftp_config_list.h
 *
 * Util file to create list of configuration values.
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/client/src/tftp_config_list.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-01-05   dks   Create

===========================================================================*/

#include "tftp_client_config.h"

/* Block sizes */
static const int tftp_server_default_data_block_size =
                             TFTP_SERVER_DEFAULT_DATA_BLOCK_SIZE;
static const int tftp_max_data_block_size = TFTP_MAX_DATA_BLOCK_SIZE;
static const int tftp_client_data_block_size = TFTP_CLIENT_DATA_BLOCK_SIZE;

/* Timeout */
static const int tftp_server_default_timeout_in_ms =
                               TFTP_SERVER_DEFAULT_TIMEOUT_IN_MS;
static const int tftp_max_timeout_in_ms = TFTP_MAX_TIMEOUT_IN_MS;
static const int tftp_client_timeout_in_ms = TFTP_CLIENT_TIMEOUT_IN_MS;

/* Window size */
static const int tftp_server_default_window_size =
                                TFTP_SERVER_DEFAULT_WINDOW_SIZE;
static const int tftp_max_window_size = TFTP_MAX_WINDOW_SIZE;
static const int tftp_client_window_size = TFTP_CLIENT_WINDOW_SIZE;

/* Pkt counts */
static const int tftp_max_pkt_retry_count = TFTP_MAX_PKT_RETRY_COUNT;
static const int tftp_max_wrong_pkt_count = TFTP_MAX_WRONG_PKT_COUNT;

static const int tftp_client_max_clients = TFTP_CLIENT_MAX_CLIENTS;
static const int tftp_client_server_instance_id =
                                    TFTP_CLIENT_SERVER_INSTANCE_ID;
