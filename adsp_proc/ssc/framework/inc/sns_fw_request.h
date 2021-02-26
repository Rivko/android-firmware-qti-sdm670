#pragma once
/*=============================================================================
  @file sns_fw_request.h

  Client-generated configuration request.  Buffer contains a published,
  backward-compatible structure (possibly QMI-decoded).  Requests may
  have a version number, but Sensors must appropriately
  handle older or newer versions.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include "sns_isafe_list.h"
#include "sns_request.h"

/*=============================================================================
  Macros and constants
  ===========================================================================*/

// Sanity check for a request.
#define SNS_STREAM_SERVICE_REQUEST_SANITY 0x53AC53AC

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_fw_request
{
  sns_request request;

  /* List entry item; on sns_client_req_list::client_requests */
  sns_isafe_list_item list_entry;
  /* The data stream on which the request arrived */
  struct sns_fw_data_stream *stream;
  /* Sanity Check */
  uint32_t sanity;
} sns_fw_request;

/*=============================================================================
  Functions
  ===========================================================================*/

/**
 * Validate a request
 *
 * @param[i] request to be validated
 *
 * @return true if request is valid. false otherwise
 */
bool sns_data_stream_validate_request(sns_fw_request *request);
