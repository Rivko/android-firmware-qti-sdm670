#ifndef HALQDSS_LPA_REPLICATOR_H
#define HALQDSS_LPA_REPLICATOR_H
/*=============================================================================

FILE:         halqdss_lpa_replicator.h

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/lpa_replicator/hal/inc/halqdss_lpa_replicator.h#1 $
==============================================================================*/
#if defined (ENABLE_LPA_REPLICATOR_QDSS) || defined (ENABLE_LPA_REPLICATOR_SWITCH)

#include "HALcomdef.h"

void HAL_qdss_lpa_replicator_HalConfigInit(uint32 nBaseAddr);

void HAL_qdss_ConfigSWAOReplicator(void);

void HAL_qdss_ConfigCXReplicator(void);

#endif

#endif //HALQDSS_LPA_REPLICATOR_H
