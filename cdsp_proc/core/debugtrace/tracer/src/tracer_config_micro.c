/*=============================================================================

FILE:         tracer_config_micro.c

DESCRIPTION:  Minimal tracer_config functions and data that are required to be 
              in micro image to facilitate SW events 

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_config_int.h"

#include "tracer_portmgr.h"
#include "tracer_stp.h"
#include "tracer_ost.h"
#include "tracer_config_micro.h"

boolean bTracerMutexInitialized = FALSE;
volatile boolean bTracerInitialized  = FALSE;
boolean bTracerEventSimpleInitialized  = FALSE;
tracer_config_t tracer_config;


/*-------------------------------------------------------------------------*/
boolean tcfg_is_event_on(tracer_event_id_t _eid)
{
   if (bTracerInitialized)
   {
      if (TRACER_EVENT_ID_MAX > _eid)
      {
         return ((1 == GET_ARRAY_BIT(event_op_ctrl_t,
                       tracer_config.event_op_ctrl, _eid))? TRUE : FALSE);
      }
   }
   return FALSE;
}

/*-------------------------------------------------------------------------*/
boolean tcfg_is_op_enabled(void)
{
   if (bTracerInitialized)
   {
      return (TRACER_OUTPUT_ENABLE == tracer_config.output_enabled);
   }
   else
   {
      return FALSE;
   }
}

/*-------------------------------------------------------------------------*/
boolean tcfg_is_event_initialized(void)
{
   return (bTracerEventSimpleInitialized);
}
