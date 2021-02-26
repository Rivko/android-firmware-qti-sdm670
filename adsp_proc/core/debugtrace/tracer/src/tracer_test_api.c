/*===========================================================================
  FILE: tracer_test_api.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer - Test the APIs

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_test_api.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_config.h"
#include "tracer_config_int.h"
#include "tracer_test.h"

/*-------------------------------------------------------------------------*/
/* Unit test for sanity check of tracer APIs.
   Returns: TRACER_TEST_RVAL_SUCCESS for test completed.
            TRACER_TEST_RVAL_xxxx    otherwise.
*/
int tracer_test_api(uint8 subtype)
{
   tracer_client_handle_t client_handle;

   if (TRACER_RETURN_SUCCESS != tracer_register(&client_handle,
       TRACER_ENTITY_DEFAULT, TRACER_PROTOCOL_DEFAULT))
   { return TRACER_TEST_RVAL_ERROR; }

   // Exercise control functions and report.
   tracer_cfg_entity_ctrl(TRACER_ENTITY_DEFAULT, TRACER_ENTITY_ON);

   tracer_cfg_op_ctrl(TRACER_OUTPUT_ENABLE);
   tracer_data(client_handle, TRACER_OPT_GUARANTEE,
               0, "Events enabled by default: %d", tcfg_get_event_count());

   tracer_data(client_handle, TRACER_OPT_GUARANTEE | TRACER_OPT_TIMESTAMP,
       0, "Hello, World!");

   tracer_set_owner(client_handle, 1234);
   tracer_data(client_handle, TRACER_OPT_NONE,
       0, "My thread 1234");

   tracer_event_ctrl(TRCEVT_TEST1, TRACER_EVENT_ON);
   tracer_event_ctrl(TRCEVT_TEST2_3ARGS, TRACER_EVENT_ON);

   tracer_data(client_handle, TRACER_OPT_TIMESTAMP,
               0, "Events enabled for test: %d", tcfg_get_event_count());

   tracer_event(client_handle, TRACER_EVENT_RESERVE_0); // should not print
   tracer_event(client_handle, TRCEVT_TEST1);
   tracer_event_simple(TRCEVT_TEST1);
   tracer_event_simple_vargs(TRCEVT_TEST1, 2, 0x0496, 4112);
   tracer_event_simple_vargs(TRCEVT_TEST2_3ARGS, 3, 0x53534451, 0x63617254, 0x21217265);
                                                   //SSDQ, carT, !!re
   // Return to default startup state.
   switch (subtype)
   {
      case 0:  // Disable all
         tracer_cfg_op_ctrl(TRACER_OUTPUT_DISABLE);
         tracer_cfg_entity_ctrl(TRACER_ENTITY_ALL, TRACER_ENTITY_OFF);
         tcfg_event_set_default();
         break;
      case 1:  // Enable all
         tracer_cfg_entity_ctrl(TRACER_ENTITY_ALL, TRACER_ENTITY_ON);
         tracer_event_ctrl(TRACER_EVENT_ALL, TRACER_EVENT_ON);
         tracer_cfg_op_ctrl(TRACER_OUTPUT_ENABLE);
         break;
      default:
         break;
   }
   tracer_unregister(&client_handle);

   return TRACER_TEST_RVAL_SUCCESS;
}


