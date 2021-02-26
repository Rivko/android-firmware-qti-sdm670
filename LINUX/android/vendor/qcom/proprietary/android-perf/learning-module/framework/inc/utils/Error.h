/******************************************************************************
  @file    Error.h
  @brief   Implementation of memory performance module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef FWK_ERRORCODES_H
#define FWK_ERRORCODES_H

enum {
    SUCCESS = 0,
    METAMETER_INVALID_ASYNC = -1,
    METAMETER_NULL_ASYNC = -2,
    METAMETER_NULL_METERMGR = -3,
    METAMETER_BUSY = -4,
    METAMETER_INVALID_SECONADARY_ASYNC = -5,
    METAMETER_INVALID_APP_FEATURE_STATE = -6,
};

#endif
