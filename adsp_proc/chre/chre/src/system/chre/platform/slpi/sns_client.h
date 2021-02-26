#pragma once
/*=============================================================================
  @file sns_client.h

  Client library for SEE communication via QSockets or QMI.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_client.pb.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

struct sns_client;

/**
 * Indication callback function.
 *
 * @param[i] msg Encoded message of type sns_client_event_msg
 */
typedef void (*sns_client_ind)(struct sns_client *client,
    void *msg, uint32_t msg_len, void *cb_data);

/**
 * Response callback function.
 *
 * @param[i] error Error code as received from service
 */
typedef void (*sns_client_resp)(struct sns_client *client,
    sns_std_error error, void *cb_data);

/**
 * Error callback function.
 *
 * @param[i] msg Encoded message of type sns_client_event_msg
 */
typedef void (*sns_client_error)(struct sns_client *client,
    sns_std_error error, void *cb_data);

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
 * Initialize a new client connection to the service.
 *
 * @return
 * 0  - Success
 * -1 - Unable to find service
 * -2 - Maximum client count reached
 */
int sns_client_init(struct sns_client **client, uint32_t timeout,
    sns_client_ind ind_cb, void *ind_cb_data,
    sns_client_error error_cb, void *error_cb_data);

/**
 * Deinitialize an existing client connection.  Blocking function.
 * No response or indication callbacks will be received after function returns.
 *
 * @return 0 upon success; <0 upon error
 */
int sns_client_deinit(struct sns_client *client);

/**
 * Send a request on the client connection.
 *
 * @return
 * 0  - Success
 * -1 - Encoding failure
 * -2 - Transport layer failure
 */
int sns_client_send(struct sns_client *client, sns_client_request_msg *msg,
    sns_client_resp resp_cb, void *resp_cb_data);
