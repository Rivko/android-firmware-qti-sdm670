/*=============================================================================

FILE:         halqdss_replicator.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "halqdss_replicator.h"
#include "qdss_replicator_regs.h"


volatile uint32 QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE;


void HAL_qdss_replicator_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP_REG_BASE = nBaseAddr;
}

void  HAL_qdss_replicator_AccessUnLock(void)
{
   HWIO_OUT(QDSS_REPL64_LAR,0xC5ACCE55);   
}

void  HAL_qdss_replicator_AccessLock(void)
{ 
   HWIO_OUT(QDSS_REPL64_LAR,(~0xC5ACCE55));   
}


boolean HAL_qdss_replicator_IsArm0Enabled(void)
{
   if (0xFF == (0xFF & HWIO_IN(QDSS_REPL64_IDFILTER0))) {
      //All ATIDs on this ARM are blocked
      return FALSE;
   }
   else {
      return TRUE;
   }
}

boolean HAL_qdss_replicator_IsArm1Enabled(void)
{
   if (0xFF == (0xFF & HWIO_IN(QDSS_REPL64_IDFILTER1))) {
      //All ATIDs on this ARM are blocked
      return FALSE;
   }
   else {
      return TRUE;
   }
}





