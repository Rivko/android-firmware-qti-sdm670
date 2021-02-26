/*
===========================================================================

Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.

FILE:         rpmclient_wl.c

DESCRIPTION:
  This is the implementation for rpm client wookloop thread.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


#include "DALSys.h"
#include "CoreVerify.h"

extern void rpmclient_smdl_read(void);

#define RPM_WORKLOOP_PRIORITY   (239)
#define RPM_WORKLOOP_MAX_EVENTS (10)

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

typedef struct
{
    // Handle for ISR WorkLoop
    DALSYSWorkLoopHandle    handle;
    // Event for wait on ISR
    DALSYSEventHandle       event;
    // Event for trigger infinite loop in the WorkLoop
    DALSYSEventHandle       start_event;

} rpmclient_workloop_t;

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
#ifdef RPMCLIENT_IRQ_NOT_SAFE_WORKAROUND
static rpmclient_workloop_t rpm_workloop;
#endif

/* -----------------------------------------------------------------------
**                           STATIC FUNCTIONS
** ----------------------------------------------------------------------- */

DALResult rpmclient_workloop
(
   DALSYSEventHandle        hEvent,
   void *pCtx
)
{
#ifdef RPMCLIENT_IRQ_NOT_SAFE_WORKAROUND
    DALSYS_EventCtrl(rpm_workloop.event, DALSYS_EVENT_CTRL_RESET);

    while ( 1 )
    {
        DALSYS_EventWait(rpm_workloop.event);
        DALSYS_EventCtrl(rpm_workloop.event, DALSYS_EVENT_CTRL_RESET);

        rpmclient_smdl_read();
    }
#endif
return DAL_SUCCESS;
}

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

void rpmclient_init_workloop
(
   DALSYSEventHandle       *hEvent
)
{
#ifdef RPMCLIENT_IRQ_NOT_SAFE_WORKAROUND

    rpm_workloop.handle      = NULL;
    rpm_workloop.event       = NULL;
    rpm_workloop.start_event = NULL;
    *hEvent                  = NULL;


    DALSYS_InitMod(NULL);

    if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                                          &(rpm_workloop.event),
                                          NULL))
        ERR_FATAL("Couldn't Create RPM workloop EVENT", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                          &(rpm_workloop.start_event),
                                          NULL))
        ERR_FATAL("Couldn't Create RPM workloop Start EVENT", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_RegisterWorkLoopEx("rpmclient_workloop",
                                                 WCN_WORKLOOP_STACK_SIZE,
                                                 RPM_WORKLOOP_PRIORITY,
                                                 RPM_WORKLOOP_MAX_EVENTS,
                                                 &(rpm_workloop.handle), NULL))
        ERR_FATAL("Couldn't Register RPM workloop", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_AddEventToWorkLoop(rpm_workloop.handle,
                                                 rpmclient_workloop,
                                                 (void *)NULL,
                                                 rpm_workloop.start_event,
                                                 NULL))
        ERR_FATAL("Couldn't Add RPM workloop Start EVENT", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_EventCtrl(rpm_workloop.start_event,
                                        DALSYS_EVENT_CTRL_TRIGGER))
        ERR_FATAL("Couldn't Trigger RPM workloop Start EVENT", 0, 0, 0);

    *hEvent = rpm_workloop.event;
#endif
}

