#ifndef _SLEEP_LOG_INTERNAL_H_
#define _SLEEP_LOG_INTERNAL_H_
/*==============================================================================
  FILE:         sleep_log_internal.h
  
  OVERVIEW:     Contains internal variables and functions declarations. This
                file is not supposed to be used outside sleep logging codes.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/log/sleep_log_internal.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "ULog.h"
#include "sleep_log.h"

/*==============================================================================
                             DATA DECLARATIONS
 =============================================================================*/
/* Number of available logs for a given sleep log implementation. */
extern const uint32 g_sleepNumberOfLogs;

/* Array of sleep log handles. Based on the version of sleep log (low memory, multicore or normal)
 * it will contain different number of handles. */
extern ULogHandle g_sleepLogHandles[];

/* Array for the sleep log sizes. The value at index i represents the size of log indicated by
 * handle at i in g_sleepLogHandles array. */
extern uint32 g_sleepLogSizeArray[];

/* Array of the log names. Value at index i represents name of log denoted by handle at
 * index i in sleepLogHandle. */
extern const char *g_sleepLogNameArray[];

/* Array of the log locks. Value at index i represents the lock type to use for the log
 * denoted by handle at index i in sleepLogHandle. */
extern ULOG_LOCK_TYPE g_sleepLogLockTypeArray[];

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleep_log_prepare_data
 *
 * @brief Prepares the log data like names and size.
 */
void sleepLog_initializeTargetCfg(void);

#ifdef __cplusplus
}
#endif

#endif  /* _SLEEP_LOG_INTERNAL_H_ */

