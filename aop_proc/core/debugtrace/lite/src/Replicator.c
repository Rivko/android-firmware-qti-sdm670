/*=============================================================================

FILE:         Replicator.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/


#include "halqdss_replicator.c"
#include "qdss_replicator_config.h"

void ReplicatorInit()
{
   HAL_qdss_replicator_HalConfigInit(QDSS_SWAO_REPLICATOR_BASE_PHYS);
}

boolean ReplicatorIsCXArmEnabled(void)
{
   return HAL_qdss_replicator_IsArm0Enabled();
}
