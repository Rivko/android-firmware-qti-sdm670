#ifndef _USLEEP_DDR_LOG_H_
#define _USLEEP_DDR_LOG_H_
/*==============================================================================
  FILE:         uSleep_ddr_log.h
  
  OVERVIEW:     Contains internal variables and functions declarations for the
                uSleep logging system
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/log/uSleep_ddr_log.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include "comdef.h"
#include "ULog.h"
#include "ULogFront.h"

/*==============================================================================
                                TYPE DEFINITIONS
 =============================================================================*/
/* uSleep ddr log levels */
typedef enum uSleep_ddr_log_level
{
  USLEEP_DDR_LOG_LEVEL_TRANSITION    = 0,
  USLEEP_DDR_LOG_TOTAL_NUMBER
} uSleep_ddr_log_level;

/* Main ddr log data structure used to declare the logs */
typedef struct uSleep_ddr_log_s
{
  uSleep_ddr_log_level  level;
  const char            *name;
  uint32                size;
}uSleep_ddr_log;

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/* Macros indicating size of various uSleep ddr logs in bytes */
#define USLEEP_DDR_LOG_SIZE_TRANSITION  2048

/* Macro to ensure that the passed log level is valid. If the level is
 * valid, the condition will be TRUE else FALSE */
#define USLEEP_DDR_LOG_IS_VALID_LEVEL(level) (level < USLEEP_DDR_LOG_TOTAL_NUMBER)

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleepDDRLog_printf
 *
 * @brief Function that will log the messages.
 *
 * @param level: Level at which want to log the message.
 * @param arg_count: Number of parameteres that are being logged.
 * @param format: Format string for logging function.
 */
void uSleepDDRLog_printf(uSleep_ddr_log_level level, int arg_count, 
                         const char *format, ...);

#else

#define uSleepDDRLog_printf(level, arg_count, format, ...)

#endif /* USLEEP_ENABLE_ULOGS */

/**
 * uSleepLog_QDSSPrintf
 *
 * @brief This function is used to log an event both to QDSS, 
 *        as well as to the usleep log indicated in the first
 *        parameter.
 *  
 * @note If QDSS is not enabled, the normal log function will be called instead, 
 *       to minimize timeline impact.
 *
 * @param logLvl:   Log level severity of current message
 * @param numArg:   Number of parameters that are being logged.
 * @param str:      The format string for printing log.
 * @param eventID:  QDSS tracer ID to use.
 */
#ifdef USLEEP_ENABLE_QDSS
#include "uSleep_tracer_event_ids.h"
#include "tracer.h"

#define uSleepDDRLog_QDSSEvent(numArg, eventID, ...)                               \
  do                                                                               \
  {                                                                                \
    tracer_event_simple_vargs((tracer_event_id_t)eventID, numArg, ## __VA_ARGS__); \
  } while (0)

#define uSleepDDRLog_QDSSPrintf(logLvl, numArg, str, eventID, ...)                 \
  do                                                                               \
  {                                                                                \
    tracer_event_simple_vargs((tracer_event_id_t)eventID, numArg, ## __VA_ARGS__); \
    uSleepDDRLog_printf(logLvl, numArg, str, ## __VA_ARGS__);      \
  } while (0)

#else 

#define uSleepDDRLog_QDSSEvent(numArg, eventID, ...)
#define uSleepDDRLog_QDSSPrintf(logLvl, numArg, str, eventID, ...) \
  uSleepDDRLog_printf(logLvl, numArg, str, ## __VA_ARGS__ )
#endif /* USLEEP_ENABLE_QDSS */

#endif  /* _USLEEP_DDR_LOG_H_ */

