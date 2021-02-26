/*===========================================================================
  FILE: tracer_test.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer - Test

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_test.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "comdef.h"
#include "tracer_test.h"
#include "tracer_event_ids.h"
#include "tracer_stp.h"
#include "tracer_config.h"
#include "tracer_portmgr.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Test types
#define TRACER_TEST_TYPE_API         1  // Basic sanity test.
#define TRACER_TEST_TYPE_PERF        2  // Performance measures.
#define TRACER_TEST_TYPE_CTRL_CB     3  // Output CTRL CB
/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

extern int tracer_test_api(uint8 subtype);

/*-------------------------------------------------------------------------*/
int tracer_test_perf(uint8 type, uint16 option, uint32 mask,
                     uint32 count1, uint32 count2)
{
   return TRACER_TEST_RVAL_UNSUPPORTED;
}

/*-------------------------------------------------------------------------*/

boolean trcevt_test1_is_enabled;

void tracer_test_cb(tracer_op_state_t state)
{
   trcevt_test1_is_enabled = tracer_is_event_enabled(TRCEVT_TEST1);
}


/*-------------------------------------------------------------------------*/


int tracer_test_ctrl_cb(uint8 type, uint16 option, uint32 mask,
                         uint32 count1, uint32 count2)
{
   tracer_cmdresp_t rval = TRACER_CMDRESP_SUCCESS;
   switch (type)  {
   case 1:
      rval=tracer_register_output_ctrl_cb(tracer_test_cb);
      break;
   case 2:
      rval=tracer_unregister_output_ctrl_cb(tracer_test_cb);
      break;
   case 3:
      if (trcevt_test1_is_enabled) {
         tracer_stp_send_event(tracer_port_d32(),TRCEVT_TEST1);
         rval = TRACER_CMDRESP_SUCCESS;
      }
      else {
         rval = TRACER_CMDRESP_S_NO_OUTPUT;
      }
      break;
   default:
      rval =TRACER_TEST_RVAL_UNSUPPORTED;
      break;
   }
   return (int) rval;
}


/*-------------------------------------------------------------------------*/
int tracer_test(uint8 type, uint8 subtype,
                uint16 option, uint32 mask,
                uint32 count1, uint32 count2)
{
   int rval;
   switch (type)
   {
      case TRACER_TEST_TYPE_API:
         rval = tracer_test_api(subtype);
         break;
      case TRACER_TEST_TYPE_PERF:
         rval = tracer_test_perf(subtype, option, mask, count1, count2);
         break;
      case TRACER_TEST_TYPE_CTRL_CB:
         rval = tracer_test_ctrl_cb(subtype, option, mask, count1, count2);
         break;
      default:
         rval = TRACER_TEST_RVAL_UNSUPPORTED;
         break;
   }
   return rval;
}

