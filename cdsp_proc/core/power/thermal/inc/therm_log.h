#ifndef THERM_LOG_H
#define THERM_LOG_H

/*============================================================================
  FILE:         therm_log.h

  OVERVIEW:     This file declares the interface to the thermal module's log.

                Copyright (c) 2011,2015,2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/thermal/inc/therm_log.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/

/**
 * @brief Enum for log level severity levels
 */
typedef enum therm_log_level
{
  THERM_LOG_LEVEL_ERROR = 0,
  THERM_LOG_LEVEL_WARNING = THERM_LOG_LEVEL_ERROR,
  THERM_LOG_LEVEL_INFO,
  THERM_LOG_LEVEL_DEBUG,
  THERM_LOG_LEVEL_STATS,
  THERM_LOG_LEVEL_REF,
  THERM_LOG_LEVEL_MAX = THERM_LOG_LEVEL_REF,
  THERM_TOTAL_LOG_LEVELS
} therm_log_level;

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
  @brief therm_log_init

  Initializes the log system for thermal with all the logs being
  disabled initially.

  If user wants to enable a particular log level, he should call
  therm_log_set_level with valid therm_log_level once logs are initialized.
*/
void therm_log_init(void);

/**
  @brief therm_log_deinit

  Deinitializes (tears down) the log system for Thermal SW.
*/
void therm_log_deinit(void);

/**
  @brief therm_log_get_level

  Inquire the current level where thermal log is set.

  @return Current level of thermal log as listed in
          therm_log_level enum
*/
therm_log_level therm_log_get_level(void);

/**
  @brief therm_log_set_level

  Changes the current therm_log_level to the one passed as an
  argument.

  After calling this function, all the therm_log_level's up to
  the 'level' will be enabled. As of now, once the log at a
  particular level is enabled, it cannot be disabled.

  @param level: new level for the thermal SW log.
*/
void therm_log_set_level(therm_log_level level);

/**
  @brief  therm_log_printf

  Generates the actual log using ULog system.

  @param : level: Log level severity of current message
           arg_count: Number of params that are being logged.
           format: The format string for printing log.
*/
void therm_log_printf(therm_log_level level, int arg_count, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* THERM_LOG_H */
