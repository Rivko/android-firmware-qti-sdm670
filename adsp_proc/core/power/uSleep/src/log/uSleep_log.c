/*==============================================================================
  FILE:         uSleep_log.c
  
  OVERVIEW:     This file implements the uSleep logging system.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/log/uSleep_log.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include "comdef.h"
#include "uSleep_log.h"

/*==============================================================================
                          VARIABLE DECLARATIONS
 =============================================================================*/
/* Array of sleep log handles. */
micro_ULogHandle g_uSleepLogHandles[USLEEP_LOG_TOTAL_NUMBER] = { NULL };

/* Pre-allocated log buffers required for island mode */
char g_uSleepLogBuffers[USLEEP_TOTAL_LOG_BUFFER_SIZE];

/*==============================================================================
                       GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepLog_printf
 */
void uSleepLog_printf(uSleep_log_level level, int arg_count, 
                      const char *format, ...)
{
  va_list args;

  if(!USLEEP_LOG_IS_VALID_LEVEL(level))
    return;

  va_start(args, format);
  micro_ULog_RealTimeVprintf(g_uSleepLogHandles[level], arg_count, format, args);
  va_end(args);

  return;
}

#endif /* USLEEP_ENABLE_ULOGS */

