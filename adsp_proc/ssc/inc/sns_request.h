#pragma once
/*=============================================================================
  @file sns_request.h

  Client-generated configuration request.  Buffer contains a published,
  backward-compatible structure.  Requests may have a version number,
  but Sensors must appropriately handle older or newer versions.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_request
{
  /* Message ID of the sns_std_request::payload */
  uint32_t message_id;
  /* Length of the request payload */
  uint32_t request_len;
  /* Request payload; is an encoded message of type sns_std_request */
  void *request;
} sns_request;
