/*=============================================================================

FILE:         halqdss_replicator.c

DESCRIPTION:  

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/replicator/hal/src/halqdss_replicator.c#1 $
==============================================================================*/
#include "halqdss_replicator.h"
#include "qdss_replicator_regs.h"


volatile uint32 QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE;


void HAL_qdss_replicator_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE = nBaseAddr;
}


void HAL_qdss_replicator_DisableTPIU(void)
{
   HWIO_OUT(QDSS_REPL64_IDFILTER1,0xff);
}

