#ifndef HALQDSS_CSR_H
#define HALQDSS_CSR_H

/*=============================================================================

FILE:         halqdss_csr.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/csr/hal/inc/halqdss_csr.h#1 $
==============================================================================*/
#include "HALcomdef.h"

void HAL_qdss_csr_HalConfigInit(uint32 nBaseAddr);
void HAL_qdss_csr_AccessUnLock(void);
void HAL_qdss_csr_AccessLock(void);
void HAL_qdss_csr_SetTimestampGenerator(uint8 enable);
void HAL_qdss_csr_LoadTimestampCounter(uint64 timeval);
uint64 HAL_qdss_csr_GetTimestampCounter(void);
void HAL_qdss_csr_SetHeartbeatPulse(uint32 mask);
void HAL_qdss_csr_SWAOPowerUp(void);
void HAL_qdss_csr_SWAOPowerDown(void);



#endif //HALQDSS_CSR_H
