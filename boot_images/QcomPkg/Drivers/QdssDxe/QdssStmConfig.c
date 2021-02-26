/*=============================================================================

FILE:         QdssStmConfig.c

DESCRIPTION:

================================================================================
                Copyright 2015 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Drivers/QdssDxe/QdssStmConfig.c#1 $
=============================================================================*/

#include "HalQdssStm.h"

#define QDSS_STM_ATID  16

void QdssStmConfig_PreInit(uint32 ts_counter_freq)
{
  //HAL_qdss_stm_SetConfigPhysAddr();  // Set directly thru symbolic const.
  HAL_qdss_stm_SetConfigStimulusPort();
  HAL_qdss_stm_AccessUnLock();
  HAL_qdss_stm_SetTimeStampFreq(ts_counter_freq);
  HAL_qdss_stm_AccessLock();
}

void QdssStmConfig_Init()
{
  //HAL_qdss_stm_SetConfigPhysAddr();
  HAL_qdss_stm_AccessUnLock();
  if (!HAL_qdss_stm_isEnabled()) {
    HAL_qdss_stm_MasterSelectCfgAll();
    HAL_qdss_stm_PortEnableMask(0xFFFFFFFF);
    HAL_qdss_stm_PortSelectCfgAll();
    HAL_qdss_stm_TimeStampEnable();
    HAL_qdss_stm_TraceIdCfg(QDSS_STM_ATID);
    HAL_qdss_stm_SetDefaultSyncPeriod();

    HAL_qdss_stm_Enable();
  }
  HAL_qdss_stm_AccessLock();
}

void QdssStmConfig_SetTimestampFreq(uint32 ts_counter_freq)
{
  HAL_qdss_stm_AccessUnLock();
  HAL_qdss_stm_SetTimeStampFreq(ts_counter_freq);
  HAL_qdss_stm_AccessLock();
}

