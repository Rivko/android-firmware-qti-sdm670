/*============================================================================
  @file therm_log.c

  This file implements the thermal SW module's log interface.

  Copyright (c) 2011-2013,2015 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1/power/thermal/src/common/therm_log.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdarg.h>
#include <string.h>
#include "DALStdDef.h"
#include "ULog.h"
#include "ULogFront.h"
#include "CoreVerify.h"
#include <stringl/stringl.h>
#include "therm_log.h"

/*
===============================================================================

                              MACRO DEFINITION

===============================================================================
*/

/**
  Macros indicating size of various thermal log in bytes.
*/
#define THERM_ERR_WARNING_LOG_SIZE     1024
#define THERM_INFO_LOG_SIZE            2048
#define THERM_DEBUG_LOG_SIZE           4096
#define THREM_STATS_LOG_SIZE           2048
#define THREM_REF_LOG_SIZE             1024

/*
===============================================================================

                              INTERNAL DEFINITIONS

===============================================================================
*/

/**
 @brief therm_log_handles

 Array of log handles for each log level. It can be indexed
 using therm_log_level.
*/
static ULogHandle thermLogHandles[THERM_TOTAL_LOG_LEVELS] = { NULL };

/**
 @brief therm_log_level

 Current DEM Therm log level.
*/
static therm_log_level thermLogLevel;

/**
  @brief therm_enabled_log_mask

  Contains the set bit for the therm logs that have been
  enabled.
*/
static uint32 thermEnabledLogMask = 0;

/**
  @brief therm_log_is_valid_level

  Checks if the therm_log_level passed as argument is valid or
  not.

  @param level: therm_log_level we want to verify.

  @return TRUE (nonzero) if the input log level is valid, else
          FALSE (zero).
*/
static DALBOOL therm_log_is_valid_level(therm_log_level level)
{
  return (level < THERM_TOTAL_LOG_LEVELS);
}

/*
===============================================================================

                         GLOBAL FUNCTION DEFINITIONS

===============================================================================
*/
/**
  @brief therm_log_size

  Array that stores the size of various therm log levels.
*/
uint32 thermLogSize[ THERM_TOTAL_LOG_LEVELS ] =
{
  THERM_ERR_WARNING_LOG_SIZE,
  THERM_INFO_LOG_SIZE,
  THERM_DEBUG_LOG_SIZE,
  THREM_STATS_LOG_SIZE,
  THREM_REF_LOG_SIZE,
};

/**
  @brief therm_log_init

  Initializes the log system for thermal with all the logs being
  disabled initially.

  If user wants to enable a particular log level, he should call
  therm_log_set_level with valid therm_log_level once logs are initialized.
*/
void therm_log_init(void)
{
  uint32 i = 0;

  #define THERM_LOG_TITLE_MAX_BUF_LEN 80

  /* ULog Header substring. */
  char uLogBaseHeader[THERM_LOG_TITLE_MAX_BUF_LEN];

  /* List of available thermal log names */
  char *thermLogNames[ THERM_TOTAL_LOG_LEVELS ] =
  {
    "therm WarningError",
    "therm Info",
    "therm Debug",
    "therm Stats",
    "therm Ref",
  };

  for(i = 0; i < THERM_TOTAL_LOG_LEVELS ; i++)
  {
    if(!thermLogHandles[i])
    {
      /* All the modes are disabled initially */
      CORE_VERIFY(ULOG_ERR_INITINCOMPLETE ==
                            ULogFront_RealTimeInit(&thermLogHandles[i],
                            thermLogNames[i],
                            0,
                            ULOG_MEMORY_LOCAL,
                            ULOG_LOCK_OS));

      /* Preparing ULog Header messages */
      snprintf(uLogBaseHeader, THERM_LOG_TITLE_MAX_BUF_LEN,
               "Content-Type: text/tagged-log-1.0; title=%s",
               thermLogNames[i]);

      /* Setting ULog header */
      ULogCore_HeaderSet(thermLogHandles[i], uLogBaseHeader);
    }
  }
}

/**
  @brief therm_log_deinit

  Deinitializes (tears down) the log system for Thermal SW.
*/
void therm_log_deinit(void)
{
  uint32 i = 0;
  for(i = 0; i < THERM_TOTAL_LOG_LEVELS ; i++)
  {
    /* FIXME: ULog currently doesn't have a way to delete a log. */
    thermLogHandles[i] = NULL;
  }
}

/**
  @brief therm_log_get_level

  Inquire the current level where thermal log is set.

  @return Current level of thermal log as listed in
          therm_log_level enum
*/
therm_log_level therm_log_get_level(void)
{
  return thermLogLevel;
}

/**
  @brief therm_log_set_level

  Changes the current therm_log_level to the one passed as an
  argument.

  After calling this function, all the therm_log_level's upto
  the 'level' will be enabled. As of now, once the log at a
  particular level is enabled, it cannot be disabled.

  @param level: new level for the thermal SW log.
*/
void therm_log_set_level(therm_log_level level)
{
  int i = 0;

  if(!therm_log_is_valid_level(level))
    return;

  for(i=0; i <= level; i++)
  {
    if( !(thermEnabledLogMask & (1<<i)) )
    {
      /* Allocating the required log memory */
      ULogCore_Allocate(thermLogHandles[i], thermLogSize[i]);

      /* Enabling the disabled log */
      ULogCore_Enable(thermLogHandles[i]);

      thermEnabledLogMask |= (1 << i);
    }
  }

  thermLogLevel = level;
}

/**
  @brief  therm_log_printf

  Generates the actual log using ULog system.

  @param : level: Log level severity of current message
           arg_count: Number of params that are being logged.
           format: The format string for printing log.
*/
void therm_log_printf(therm_log_level level, int arg_count,
                      const char *format, ...)
{
  va_list args;

  if(!therm_log_is_valid_level(level))
    return;

  va_start(args, format);
  ULogFront_RealTimeVprintf(thermLogHandles[level], arg_count, format, args);
  va_end(args);
}
