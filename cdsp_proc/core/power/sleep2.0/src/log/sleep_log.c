/*==============================================================================
  FILE:         sleep_log.c
  
  OVERVIEW:     This file implements the sleep module's log interface.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/log/sleep_log.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdarg.h>
#include <string.h>
#include "comdef.h"
#include "ULog.h"
#include "ULogFront.h"
#include "CoreVerify.h"
#include "CoreString.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleep_log.h"
#include "sleep_log_internal.h"

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
/* Bitwise enum type for supported sleep features */
typedef enum sleep_features
{
  SLEEP_FEATURE_NONE              = 0x0000,
  SLEEP_FEATURE_QDSS              = 0x0001,
  SLEEP_FEATURE_UNUSED_2          = 0x0002,
  SLEEP_FEATURE_SYNTH_BACKOFF_ADJ = 0x0004,
  SLEEP_FEATURE_FREQUENCY_SCALING = 0x0008,
  SLEEP_FEATURE_DISABLE_MODES     = 0x0010,
  SLEEP_FEATURE_USLEEP            = 0x0020,
  SLEEP_FEATURE_UNUSED40          = 0x0040,
}sleep_features;

/*==============================================================================
                        GLOBAL VARIABLE DECLARATIONS
 =============================================================================*/
/* Holds the enable status of the various sleep features */
static volatile sleep_features g_sleepEnabledFeatures = SLEEP_FEATURE_NONE;

/*==============================================================================
                          INTERNAL DEFINITIONS
 =============================================================================*/
/* Contains the set bit for the sleep logs that have been enabled (currently
 * limited to total of 32 logs. Will need a different data type in case of 
 * more logs). */
static uint32 g_sleepEnabledLogMask = 0;

/* Macro to ensure that the passed log level is valid. If the level is valid, the
 * condition will be TRUE else FALSE. */
#define SLEEP_LOG_IS_VALID_LEVEL(level) (level < SLEEP_TOTAL_LOG_LEVELS)

/* Maximum lenght of ulog header string */
#define MAX_BUF_LEN 80

/*==============================================================================
                       GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepLog_initialize
 */
void sleepLog_initialize(void)
{
  uint32 i;

  /* ULog Header substring. */
  char uLogBaseHeader[MAX_BUF_LEN];

  /* Preparing handle lookup table/map if any. */
  sleepLog_initializeTargetCfg();
  
  for(i = 0; i < g_sleepNumberOfLogs ; i++)
  {
    if(!g_sleepLogHandles[i])
    {
      /* All the modes are disabled initially */
      CORE_VERIFY(ULOG_ERR_INITINCOMPLETE == 
                            ULogFront_RealTimeInit(&g_sleepLogHandles[i],
                            g_sleepLogNameArray[i],
                            0,
                            ULOG_MEMORY_LOCAL,
                            g_sleepLogLockTypeArray[i]));

      /* Preparing ULog Header messages */
      core_snprintf(uLogBaseHeader, MAX_BUF_LEN, 
                     "Content-Type: text/dyn-sleep-log-1.0; title=:%s",
                     g_sleepLogNameArray[i]);

      /* Setting ULog header */
      ULogCore_HeaderSet(g_sleepLogHandles[i], uLogBaseHeader);
    }
  }

  return;
}

/**
 * sleepLog_setLevel
 *
 * @brief Sets the current sleep log level to a valid input sleep log level.
 *
 * In case of invalid input log level, current level won't be changed.
 *
 * @param level Level where we want to set sleep log at.
 */
void sleepLog_setLevel(sleep_log_level level)
{
  sleep_log_level nLevel;

  if(!SLEEP_LOG_IS_VALID_LEVEL(level))
    return;

  for(nLevel = (sleep_log_level)0; nLevel <= level; nLevel++)
  {
    if( !(g_sleepEnabledLogMask & (1 << nLevel)) )
    {
      /* Allocating the required log memory */
      ULogCore_Allocate(g_sleepLogHandles[nLevel], g_sleepLogSizeArray[nLevel]);

      /* Enabling the disabled log */
      ULogCore_Enable(g_sleepLogHandles[nLevel]);

      g_sleepEnabledLogMask |= (1 << nLevel);
    }
  }

  /* Set internal sleep options */
#ifdef SLEEP_ENABLE_QDSS
  g_sleepEnabledFeatures |= SLEEP_FEATURE_QDSS;
#endif

#ifdef SLEEP_FEATURE_UNUSED_2
  g_sleepEnabledFeatures |= SLEEP_FEATURE_UNUSED_2;
#endif

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  g_sleepEnabledFeatures |= SLEEP_FEATURE_SYNTH_BACKOFF_ADJ;
#endif

#ifdef SLEEP_ENABLE_FREQUENCY_SCALING
  g_sleepEnabledFeatures |= SLEEP_FEATURE_FREQUENCY_SCALING;
#endif

#ifdef FEATURE_DISABLE_SLEEP_MODES
  g_sleepEnabledFeatures |= SLEEP_FEATURE_DISABLE_MODES;
#endif

#ifdef USLEEP_ISLAND_MODE_ENABLE
  g_sleepEnabledFeatures |= SLEEP_FEATURE_USLEEP;
#endif

  return;
}

/**
 * sleepLog_printf
 *
 * @brief Actual function that will log the messages.
 *
 * @param level: Level at which want to log the message.
 * @param arg_count: Number of parameteres that are being logged.
 * @param format: Format string for logging function.
 */
void sleepLog_printf(sleep_log_level level, int arg_count, 
                      const char *format, ...)
{
  va_list args;

  if(!SLEEP_LOG_IS_VALID_LEVEL(level))
    return;
  
  va_start(args, format);
  ULogFront_RealTimeVprintf(g_sleepLogHandles[level], arg_count, format, args);
  va_end(args);

  return;
}

