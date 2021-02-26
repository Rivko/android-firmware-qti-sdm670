/*==============================================================================
  FILE:         uSleep_ddr_log.c
  
  OVERVIEW:     This file implements the uSleep logging system for DDR logs.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/log/ddr/uSleep_ddr_log.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include "comdef.h"
#include "uSleep_ddr_log.h"

/*==============================================================================
                                GLOBAL VARIABLES
 =============================================================================*/
ULogHandle g_uSleepDDRLogHandles[USLEEP_DDR_LOG_TOTAL_NUMBER] = { NULL };

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepDDRLog_printf
 */
void uSleepDDRLog_printf(uSleep_ddr_log_level level, int arg_count, 
                         const char *format, ...)
{
  va_list args;

  if(!USLEEP_DDR_LOG_IS_VALID_LEVEL(level))
    return;

  va_start(args, format);
  ULogFront_RealTimeVprintf(g_uSleepDDRLogHandles[level], arg_count, format, args);
  va_end(args);

  return;
}

#endif /* USLEEP_ENABLE_ULOGS */
