/*=============================================================================

FILE:         STMCfg.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/lite/src/STMCfg.c#1 $
=============================================================================*/

#include "STMCfg.h"
#include "halqdss_stm_config.c"  //lite_inline 
#include "qdss_stm_config.h"


__inline void STMCfgInit()
{
   HAL_qdss_stm_HalConfigInit(QDSS_STM_BASE_CONST);
}

__inline void STMSetTimestampFreq(uint32 ts_counter_freq)
{
   HAL_qdss_stm_AccessUnLock();
   HAL_qdss_stm_SetTimeStampFreq(ts_counter_freq);
   HAL_qdss_stm_AccessLock();
}


__inline boolean STMAccessStatusNSNID(void)
{
   return HAL_qdss_stm_AuthStatusNSNID();
}

