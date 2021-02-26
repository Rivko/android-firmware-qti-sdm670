#ifndef _SLEEP_LOG_H_
#define _SLEEP_LOG_H_
/*============================================================================
  FILE:         sleep_log.h

  OVERVIEW:     This file declares the interface to the sleep module's log.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
==============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/inc/sleep_log.h#2 $
$DateTime: 2017/09/13 08:52:52 $
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "ULogFront.h"

/*==============================================================================
                             TYPE DEFINITION(S)
 =============================================================================*/
/**
 * @brief Enum for log level severity levels.
 *
 * Based on target we will have different logs for different levels. For
 *
 * Non-HLOS targerts:
 * We will have one log for each level up to SLEEP_LOG_LEVEL_DEBUG (inclusive).
 * Since ERROR and WARNING are at same level, they share the log.
 *
 * Low memory targets:
 * They can have one shared log for ERROR and WARNING levels and one shared
 * for all other levels up to DEBUG (inclusive).
 *
 * Apps:
 * Each core will have its own set of logs with one log for each level. But
 * -   ERROR and WARNING levels share a log on each core.
 * -   REQUESTS and ATS (if used) logs are shared across cores.
 */
typedef enum sleep_log_level
{
  SLEEP_LOG_LEVEL_ERROR = 0,
  SLEEP_LOG_LEVEL_WARNING = 0,
  SLEEP_LOG_LEVEL_REQUESTS,
  SLEEP_LOG_LEVEL_ATS,
  SLEEP_LOG_LEVEL_INFO,
  SLEEP_LOG_LEVEL_PROFILING,
  SLEEP_LOG_LEVEL_WAKEUPS,
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  SLEEP_LOG_LEVEL_AUTO_LATENCY,
#endif
  SLEEP_LOG_LEVEL_DEBUG,
  SLEEP_LOG_LEVEL_MAX = SLEEP_LOG_LEVEL_DEBUG,
  SLEEP_TOTAL_LOG_LEVELS
} sleep_log_level;

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepLog_initialize
 *
 * @brief Initializes the log system for sleep with all the logs being disabled
 *        initially.
 *
 * If user wants to enable a particular log level, he should call
 * sleepLog_setLevel with valid sleep log level once logs are initialized.
 */
void sleepLog_initialize(void);

/**
 * sleepLog_setLevel
 *
 * @brief Changes the current sleep log level to the one passed as
 *        argument.
 *
 * After calling this function, all the sleep log levels upto the
 * 'level' will be enabled. As of now, once the log at a particular
 * level is enabled, it cannot be disabled.
 *
 * @param level: new level for the sleep log.
 */
void sleepLog_setLevel(sleep_log_level level);

/**
 * sleepLog_printf
 *
 * @brief Generates the actual log using ULog system.
 *
 * @param level: Log level severity of current message
 * @param arg_count: Number of parameters that are being logged.
 * @param format: The format string for printing log.
 */
void sleepLog_printf(sleep_log_level level, int arg_count, const char *format, ...);

/**
 * sleepLog_QDSSPrintf
 *
 * @brief This function is used to log an event both to QDSS, 
 *        as well as to the sleep log indicated in the first
 *        parameter.
 *  
 *         NOTE that if QDSS is not enabled, sleepLog_printf
 *         will be called instead, to minimize timeline impact.
 *
 * @param level: Log level severity of current message
 * @param arg_count: Number of parameters that are being logged.
 * @param format: The format string for printing log.
 * @param tracer_id: QDSS tracer ID to use.
 */
#ifdef SLEEP_ENABLE_QDSS
#include "sleep_tracer_event_ids.h"
#include "tracer.h"
#define sleepLog_QDSSEvent( numArg, eventID, ... ) \
  do \
  { \
    tracer_event_simple_vargs( ( tracer_event_id_t )eventID, numArg, ## __VA_ARGS__ ); \
  } while (0)

#define sleepLog_QDSSPrintf( logLvl, numArg, str, eventID, ... ) \
  do \
  { \
    tracer_event_simple_vargs( ( tracer_event_id_t )eventID, numArg, ## __VA_ARGS__ ); \
    sleepLog_printf( logLvl, numArg, str, ## __VA_ARGS__ ); \
  } while (0)

#else /* !SLEEP_ENABLE_QDSS */

#define sleepLog_QDSSEvent(numArg, eventID, ...)
#define sleepLog_QDSSPrintf(logLvl, numArg, str, eventID, ...) \
 sleepLog_printf(logLvl, numArg, str, ## __VA_ARGS__ )

#endif /* !SLEEP_ENABLE_QDSS */

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_LOG_H_ */

