/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdlib.h>
#include <stddef.h>

#include "rpmh_events.h"
#include "inq.h"
#include "rpmh_os.h"

struct rpmh_event_s
{
    inq_link_t        link;
    uint32            event;
    //DALSYSEventHandle event;
};

static inq_t *rpmh_event_pool;


static rpmh_event_t *rpmh_create_dal_event(void)
{
    rpmh_event_t *event;
    //DALResult res;
    
    if(!rpmh_event_pool)
        rpmh_event_pool = inq_create(offsetof(rpmh_event_t, link));

    event = (rpmh_event_t *)inq_pop(rpmh_event_pool);

    // If we didn't have a saved event, make one.
    if(!event)
    {
        event = (rpmh_event_t *)malloc(sizeof(rpmh_event_t));
        rpmh_core_verify_ptr(event);
        //res = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, &event->event, NULL);
        //rpmh_core_verify(DAL_SUCCESS == res);
    }

    //DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_RESET);

    return event;
}

static void rpmh_destroy_dal_event(rpmh_event_t *event)
{
    inq_append(rpmh_event_pool, event);
}

static void rpmh_reset_dal_event(rpmh_event_t *event)
{
    //DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_RESET);
}

static void rpmh_set_dal_event(rpmh_event_t *event)
{
    //DALSYS_EventCtrl(event->event, DALSYS_EVENT_CTRL_TRIGGER);
}

static void rpmh_wait_dal_event(rpmh_event_t *event)
{
    //DALSYS_EventWait(event->event);
}

rpmh_event_t *(*rpmh_event_create)(void)       = rpmh_create_dal_event;
void (*rpmh_event_destroy)(rpmh_event_t *self) = rpmh_destroy_dal_event;
void (*rpmh_event_reset)(rpmh_event_t *self)   = rpmh_reset_dal_event;
void (*rpmh_event_set)(rpmh_event_t *self)     = rpmh_set_dal_event;
void (*rpmh_event_wait)(rpmh_event_t *self)    = rpmh_wait_dal_event;


