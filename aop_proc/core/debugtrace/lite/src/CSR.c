/*=============================================================================

FILE:         CSR.c

DESCRIPTION:  QDSS CSR releated

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/lite/src/CSR.c#1 $
==============================================================================*/

#include "halqdss_csr.c"
#include "qdss_csr_config.h"
#include "time_service.h"

void CSRInit()
{
   HAL_qdss_csr_HalConfigInit(QDSS_SWAOCSR_REG_BASE_PHYS);
}


void CSRTraceEnable()
{
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_SWAOPowerUp();
   HAL_qdss_csr_LoadTimestampCounter(time_service_now());
   HAL_qdss_csr_SetHeartbeatPulse(0x1) ;
   HAL_qdss_csr_SetTimestampGenerator(1);
   HAL_qdss_csr_AccessLock();
}


void CSRTraceDisable()
{
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_SetTimestampGenerator(0);
   HAL_qdss_csr_SWAOPowerDown();
   HAL_qdss_csr_AccessLock();
}

