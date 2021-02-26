/*=============================================================================

FILE:         halqdss_lpa_replicator.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/lpa_replicator/hal/src/halqdss_lpa_replicator.c#1 $
==============================================================================*/
#include "halqdss_lpa_replicator.h"
#include "hwioqdss_lpa_replicator_regs.h"

#if defined (ENABLE_LPA_REPLICATOR_QDSS) || defined (ENABLE_LPA_REPLICATOR_SWITCH)
/* Move it to xml file */
volatile uint32 QDSS_SWAO_REPLIC_CXATBREPLICATOR_64WP_REG_BASE; 

void HAL_qdss_lpa_replicator_HalConfigInit(uint32 nBaseAddr)
{ 
   QDSS_SWAO_REPLIC_CXATBREPLICATOR_64WP_REG_BASE = nBaseAddr;
} 
 
/*-------------------------------------------------------------------------*/

void HAL_qdss_ConfigSWAOReplicator(void)
{
    /* Confiuring LPASS Replicator Leg to switch to AOP in LPI mode */
    HWIO_OUT(QDSS_SWAO_REPLIC_IDFILTER1,0xFFFFFFFF);
}

/*-------------------------------------------------------------------------*/

void HAL_qdss_ConfigCXReplicator(void)
{
    /* Confiuring LPASS Replicator Leg to switch to Main QDSS in CX ON mode */
    HWIO_OUT(QDSS_SWAO_REPLIC_IDFILTER1,0x00000000);
}
 
#endif

