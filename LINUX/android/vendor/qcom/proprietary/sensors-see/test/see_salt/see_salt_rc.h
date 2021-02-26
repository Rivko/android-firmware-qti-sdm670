/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file see_salt_rc.h
  @brief      salt_rc is wrapper for usta_rc
============================================================================*/
#pragma once

typedef enum salt_rc
{
  /* No error occured ; success */
  SALT_RC_SUCCESS = 0,
  /* operation failed ; cannot continue */
  SALT_RC_FAILED,
  /* descriptor unavailable */
  SALT_RC_DESC_FAILED,
  /* Memory Allocation Failed */
  SALT_RC_NO_MEMORY,
  /* proto files not found in desccriptor pool */
  SALT_RC_NO_PROTO,
  /* No request messages found */
  SALT_RC_NO_REQUEST_MSG,
  /* requested attribute not found */
  SALT_RC_ATTRIB_FAILED,
  /*Required fileds are missing in the proto files*/
  SALT_RC_REQUIIRED_FILEDS_NOT_PRESENT,
  /* required argument is null */
  SALT_RC_NULL_ARG,


}salt_rc;
