/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Technologies Inc. Proprietary and Confidential.

  ===========================================================================*/

#include <Library/rpm_comdef.h>
#include "rpm_events.h"
#include "inq.h"
#include "DALSys.h"
#include "CoreHeap.h"
#include "CoreVerify.h"

struct rpm_event_s
{
    inq_link_t        link;
    DALSYSEventHandle event;
};

static inq_t *rpm_event_pool;


static rpm_event_t *rpm_create_dal_event(void)
{
    rpm_event_t *event;
    DALResult res;
    
    if(!rpm_event_pool)
        rpm_event_pool = inq_create(offsetof(rpm_event_t, link));

    event = (rpm_event_t *)inq_pop(rpm_event_pool);

    // If we didn't have a saved event, make one.
    if(!event)
    {
        event = Core_Malloc(sizeof(rpm_event_t));
        CORE_VERIFY_PTR(event);
        res = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, &event->event, NULL);
        CORE_VERIFY(DAL_SUCCESS == res);
    }

    DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_RESET);

    return event;
}

static void rpm_destroy_dal_event(rpm_event_t *event)
{
    inq_append(rpm_event_pool, event);
}

static void rpm_reset_dal_event(rpm_event_t *event)
{
    DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_RESET);
}

static void rpm_set_dal_event(rpm_event_t *event)
{
    DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_TRIGGER);
}

static void rpm_wait_dal_event(rpm_event_t *event)
{
    DALSYS_EventWait(event->event);
}

rpm_event_t *(*rpm_event_create)(void)       = rpm_create_dal_event;
void (*rpm_event_destroy)(rpm_event_t *self) = rpm_destroy_dal_event;
void (*rpm_event_reset)(rpm_event_t *self)   = rpm_reset_dal_event;
void (*rpm_event_set)(rpm_event_t *self)     = rpm_set_dal_event;
void (*rpm_event_wait)(rpm_event_t *self)    = rpm_wait_dal_event;


