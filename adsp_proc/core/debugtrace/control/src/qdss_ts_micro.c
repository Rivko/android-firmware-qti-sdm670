/*=============================================================================

FILE:         qdss_ts_micro.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qurt_island.h"
#include "DALStdDef.h"

void (*pfn_qdss_ts_timer_callback)(void);


/**
  @brief  QDSS TS timer call back
          We cancel the timer when enetring island mode.
          So ideally this should not get called in island mode.
          Incase it does we check and exit.

  @return NULL
 */
void* qdss_ts_timer_callback_micro(void *pCtxt,
                             uint32 unused1,void* unused2,uint32 unused3)
{
   if (1==qurt_island_get_status()) {
      /*In Island mode, bail */
      return NULL;
   }

   if (pfn_qdss_ts_timer_callback) {
      pfn_qdss_ts_timer_callback();
   }
   return NULL;
}
