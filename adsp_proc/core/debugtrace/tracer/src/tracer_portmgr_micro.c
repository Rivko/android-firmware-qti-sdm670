/*=============================================================================

FILE:         tracer_portmgr_micro.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "DALStdErr.h"

#include "tracer_portmgr.h"
#include "tracer_stp.h"
#include "qurt_island.h"

/*---------------------------------------------------------------------------
 * Variable Definitions
 * ------------------------------------------------------------------------*/
boolean bPortMgrInitialized = FALSE;

struct portMgr_s gPortMgr;
struct portMgr_s gPortMgrMicro;

boolean tracer_log_all_to_ssc_qdss = FALSE;

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
STMTracePortType tracer_port_d32(void)
{
   if (tracer_log_all_to_ssc_qdss) {
         return gPortMgrMicro.d32Port;
   }
   else {
      if (0==qurt_island_get_status()) {
         /*Not in island mode */
         return gPortMgr.d32Port;
      }
      else {
         return gPortMgrMicro.d32Port;
      }
   }
}


