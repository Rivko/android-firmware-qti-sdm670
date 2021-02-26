#pragma once
/*=============================================================================
  @file sns_rc.h

  Common return codes used by the Sensor Framework.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

typedef enum sns_rc
{
  /* No error occurred; success. */
  SNS_RC_SUCCESS = 0,
  /* Unfixable or internal error occurred. */
  SNS_RC_FAILED,
  /* This API is not supported or is not implemented. */
  SNS_RC_NOT_SUPPORTED,
  /* Function argument contains invalid data type,
    * e.g., unknown message ID, unknown registry group, or unexpected
    * Sensor UID. */
  SNS_RC_INVALID_TYPE,
  /* Sensor[Instance] in bad or invalid state; is unable to recover. */
  SNS_RC_INVALID_STATE,
  /* One or more argument values were outside of the valid range */
  SNS_RC_INVALID_VALUE,
  /* This operation is not available at this time */
  SNS_RC_NOT_AVAILABLE,
  /* This action was rejected due to the current policy settings */
  SNS_RC_POLICY,
  /* All sensors of the library are in an invalid state and cannot recover from it. */
  SNS_RC_INVALID_LIBRARY_STATE,
} sns_rc;
