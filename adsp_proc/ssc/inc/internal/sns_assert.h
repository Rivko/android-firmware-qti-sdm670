#pragma once
/*=============================================================================
  @file sns_assert.h

  Abort the program is the expression evaluates to false.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <stdbool.h>
#include "err.h"

#include "sns_printf.h"
#include "sns_time.h"

#if defined SSC_TARGET_HEXAGON
#   define SNS_ASSERT(value) if(!(value))\
      ERR_FATAL( #value ,0,0,0)
#elif defined SSC_TARGET_X86
#include <assert.h>
#   define SNS_ASSERT(value) assert(value)
#endif

#define SNS_VERBOSE_ASSERT(value, msg, ...) if(!(value))\
  { \
    SNS_PRINTF(FATAL, sns_fw_printf, msg, ##__VA_ARGS__); \
    sns_busy_wait(sns_convert_ns_to_ticks(5000000000ULL)); \
    SNS_ASSERT(value);\
  } (void)0
