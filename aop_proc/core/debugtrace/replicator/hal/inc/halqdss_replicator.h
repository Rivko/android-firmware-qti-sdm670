/*=============================================================================

FILE:         halqdss_replicator.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef HALQDSS_REPLICATOR_H
#define HALQDSS_REPLICATOR_H

#include "HALcomdef.h"

void HAL_qdss_replicator_HalConfigInit(uint32 nBaseAddr);
void  HAL_qdss_replicator_AccessUnLock(void);
void  HAL_qdss_replicator_AccessLock(void);
boolean HAL_qdss_replicator_IsArm0Enabled(void);
boolean HAL_qdss_replicator_IsArm1Enabled(void);


#endif //HALQDSS_REPLICATOR_H
