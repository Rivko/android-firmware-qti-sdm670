/*==============================================================================
  FILE:         uSleep_log_init.c
  
  OVERVIEW:     This file initializes the uSleep logging system.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/log/ddr/usleep_log_init.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef USLEEP_ENABLE_ULOGS

#include <stdarg.h>
#include <string.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_log.h"
#include "uSleep_ddr_log.h"
#include "ULog.h"
#include "CoreString.h"

/*==============================================================================
                          VARIABLE DECLARATIONS
 =============================================================================*/
/* Island mode logs */
uSleep_log g_uSleepLogs[USLEEP_LOG_TOTAL_NUMBER] = 
{
  {USLEEP_LOG_LEVEL_PROFILING, "uSleep Island", USLEEP_LOG_SIZE_PROFILE},
};

/* Normal mode logs */
uSleep_ddr_log g_uSleepDDRLogs[USLEEP_DDR_LOG_TOTAL_NUMBER] = 
{
  {USLEEP_DDR_LOG_LEVEL_TRANSITION, "uSleep Normal", USLEEP_DDR_LOG_SIZE_TRANSITION},
};

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
extern char g_uSleepLogBuffers[USLEEP_TOTAL_LOG_BUFFER_SIZE];
extern ULogHandle g_uSleepDDRLogHandles[USLEEP_DDR_LOG_TOTAL_NUMBER];

/*==============================================================================
                       GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepLog_initialize
 */
void uSleepLog_initialize(void)
{
  uint32            nLog;
  uint32            level;
  uint32            effectiveSize;
  micro_ULogResult  status;
  char              uLogBaseHeader[80];
  uint32            indexLoc = 0;
  
  /* Init uSleep Island logs */
  for(nLog = 0; nLog < USLEEP_LOG_TOTAL_NUMBER; nLog++)
  {
    level = g_uSleepLogs[nLog].level;

    if(USLEEP_LOG_IS_VALID_LEVEL(level) && 0 == g_uSleepLogHandles[level])
    {
      CORE_VERIFY(indexLoc < USLEEP_TOTAL_LOG_BUFFER_SIZE);

      /* Create island logs */
      status = micro_ULog_CreateLockableLog(&g_uSleepLogHandles[level],
                                            g_uSleepLogs[level].name,
                                            &g_uSleepLogBuffers[indexLoc],
                                            &effectiveSize,
                                            g_uSleepLogs[level].size);
    
      CORE_VERIFY(MICRO_ULOG_SUCCESS == status);

      /* Enabling the log */
      micro_ULog_Enable(g_uSleepLogHandles[level]);

      /* Prepare for next log location in global buffer */
      indexLoc += g_uSleepLogs[level].size;
    }
  }

  /* Init uSleep DDR logs */
  for(nLog = 0; nLog < USLEEP_DDR_LOG_TOTAL_NUMBER; nLog++)
  {
    if(NULL == g_uSleepDDRLogHandles[nLog])
    {
      /* All the modes are disabled initially */
      CORE_VERIFY(ULOG_ERR_INITINCOMPLETE == 
                            ULogFront_RealTimeInit(&g_uSleepDDRLogHandles[nLog],
                            g_uSleepDDRLogs[nLog].name,
                            0,
                            ULOG_MEMORY_LOCAL,
                            ULOG_LOCK_OS));

      /* Preparing ULog Header messages */
      core_snprintf(uLogBaseHeader, 80, 
                    "Content-Type: text/dyn-uSleep-log-1.0; title=:%s",
                    g_uSleepDDRLogs[nLog].name);

      /* Setting ULog header */
      ULogCore_HeaderSet(g_uSleepDDRLogHandles[nLog], uLogBaseHeader);

      /* Allocating the required log memory */
      ULogCore_Allocate(g_uSleepDDRLogHandles[nLog], g_uSleepDDRLogs[nLog].size);

      /* Enabling the disabled log */
      ULogCore_Enable(g_uSleepDDRLogHandles[nLog]);
    }
  }

  return;
}

#endif /* USLEEP_ENABLE_ULOGS */

