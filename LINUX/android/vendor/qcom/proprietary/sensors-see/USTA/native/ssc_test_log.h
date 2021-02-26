/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file ssc_test_log.cpp
  @brief
  ssc_test_log for logging purpose.
============================================================================*/
#pragma once
#include <cstdio>

/* formatted log to stderr */
#define log_stderr(fmt, args...) do { \
  USTA_LOGI( "%s:%d, " fmt "\n", __FUNCTION__, __LINE__, ##args); \
} while (0)

/* log to stderr as-is */
#define log_stderr_raw(fmt, args...) do { \
  USTA_LOGI( "%s", ##args); \
} while (0)


