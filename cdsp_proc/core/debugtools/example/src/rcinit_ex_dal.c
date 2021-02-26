/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_ex_dal.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/example/src/rcinit_ex_dal.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "rcinit_dal.h"

/*===========================================================================

 FUNCTION rcinit_example_daltask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#define EXAMPLE_FILE       "rcinit_ex_dal"
#define EXAMPLE_DEVICEID   0

static struct                                                                    // clients do not need to access this, expose the communications infrastructure by API
{
   int init;
   DALSYSEventHandle hEvent0;                                                    // event 0
   DALSYSEventHandle hEvent1;                                                    // event 1
   DALSYSEventHandle hEvents[2];                                                 // handles array for EventWaitMultiple

} rcinit_example_daltask_events;

void rcinit_example_daltask_client_signal_event0(void)                           // called by other task
{
   if ((TRUE != rcinit_example_daltask_events.init) ||
       (DAL_SUCCESS != DALSYS_EventCtrl(rcinit_example_daltask_events.hEvent0, DALSYS_EVENT_CTRL_TRIGGER)))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event trigger", EXAMPLE_FILE, __LINE__);
   }
}

void rcinit_example_daltask_client_signal_event1(void)                           // called by other task
{
   if ((TRUE != rcinit_example_daltask_events.init) ||
       (DAL_SUCCESS != DALSYS_EventCtrl(rcinit_example_daltask_events.hEvent1, DALSYS_EVENT_CTRL_TRIGGER)))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event trigger", EXAMPLE_FILE, __LINE__);
   }
}

DALResult rcinit_example_daltask(DALSYSEventHandle hUnused, void* unused)
{
   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &rcinit_example_daltask_events.hEvent0, NULL))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event create", EXAMPLE_FILE, __LINE__);
   }

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &rcinit_example_daltask_events.hEvent1, NULL))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event create", EXAMPLE_FILE, __LINE__);
   }

   rcinit_example_daltask_events.hEvents[0] = rcinit_example_daltask_events.hEvent0;
   rcinit_example_daltask_events.hEvents[1] = rcinit_example_daltask_events.hEvent1;

   rcinit_example_daltask_events.init = TRUE;

   rcinit_handshake_startup();                                                   // *REQUIRED*

   for (;;)
   {
      unsigned long index;

      if (DAL_SUCCESS != DALSYS_EventMultipleWait(rcinit_example_daltask_events.hEvents,
                                                  sizeof(rcinit_example_daltask_events.hEvents) / sizeof(DALSYSEventHandle),
                                                  DALSYS_EVENT_TIMEOUT_INFINITE, &index))
      {
         DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event wait", EXAMPLE_FILE, __LINE__);
      }

      switch (index)
      {
      case 0:                                                                    // hEvent0 index handler
         if ((TRUE != rcinit_example_daltask_events.init) ||
             (DAL_SUCCESS != DALSYS_EventCtrl(rcinit_example_daltask_events.hEvent0, DALSYS_EVENT_CTRL_RESET)))
         {
            DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event reset", EXAMPLE_FILE, __LINE__);
         }
         break;

      case 1:                                                                    // hEvent1 index handler
         if ((TRUE != rcinit_example_daltask_events.init) ||
             (DAL_SUCCESS != DALSYS_EventCtrl(rcinit_example_daltask_events.hEvent1, DALSYS_EVENT_CTRL_RESET)))
         {
            DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event reset", EXAMPLE_FILE, __LINE__);
         }
         break;

      default:                                                                   // unknown event index
         DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event index", EXAMPLE_FILE, __LINE__);
         break;
      }

      // ...
   }

   /* NOTREACHED */                                                              // lint, CR421030

   rcinit_example_daltask_events.init = FALSE;

   if (DAL_SUCCESS != DALSYS_DestroyObject(&rcinit_example_daltask_events.hEvent0))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event destroy", EXAMPLE_FILE, __LINE__);
   }

   if (DAL_SUCCESS != DALSYS_DestroyObject(&rcinit_example_daltask_events.hEvent1))
   {
      DALSYS_LogEvent(EXAMPLE_DEVICEID, DALSYS_LOGEVENT_FATAL_ERROR, "%s(%d): event destroy", EXAMPLE_FILE, __LINE__);
   }

   if (hUnused) {;}                                                              // unused
   if (unused) {;}                                                               // unused

   return DAL_SUCCESS;
}
