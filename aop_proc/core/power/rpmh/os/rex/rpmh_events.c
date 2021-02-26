/*===========================================================================

  Copyright (c) 2016, 2017 Qualcomm Technologies, Inc.  
  All Rights Reserved.
  Proprietary and Confidential- Qualcomm Technologies, Inc

  ===========================================================================*/

#include <stdlib.h>
#include <stddef.h>

#include "rpmh_events.h"
#include "inq.h"
#include "rpmh_os.h"
#include "rex.h"

typedef enum
{
  RPMH_SIG_CMD_FINISHED = 0x8000000,
  RPMH_SIG_MAX
} signal_type;

struct rpmh_event_s
{
    inq_link_t        link;
    rex_tcb_type      *task;
};

static inq_t *rpmh_event_pool;


static rpmh_event_t *rpmh_create_rex_event(void)
{
    rpmh_event_t *event;
    
    if(!rpmh_event_pool)
        rpmh_event_pool = inq_create(offsetof(rpmh_event_t, link));

    event = (rpmh_event_t *)inq_pop(rpmh_event_pool);

    if(!event)
    {
        event = (rpmh_event_t *)malloc(sizeof(rpmh_event_t));
        rpmh_core_verify_ptr(event);
    }

    return event;
}

static void rpmh_destroy_rex_event(rpmh_event_t *event)
{
    inq_append(rpmh_event_pool, event);
}

static void rpmh_reset_rex_event(rpmh_event_t *event)
{
  rex_clr_sigs(event->task, RPMH_SIG_CMD_FINISHED);
}

static void rpmh_set_rex_event(rpmh_event_t *event)
{
  rex_set_sigs(event->task, RPMH_SIG_CMD_FINISHED);
}

static void rpmh_wait_rex_event(rpmh_event_t *event)
{
    event->task = rex_self();
    rpmh_unlock (FALSE); 
    rex_wait(RPMH_SIG_CMD_FINISHED);
    rpmh_lock (FALSE); 
    rex_clr_sigs(event->task, RPMH_SIG_CMD_FINISHED);
}

rpmh_event_t *(*rpmh_event_create)(void)       = rpmh_create_rex_event;
void (*rpmh_event_destroy)(rpmh_event_t *self) = rpmh_destroy_rex_event;
void (*rpmh_event_reset)(rpmh_event_t *self)   = rpmh_reset_rex_event;
void (*rpmh_event_set)(rpmh_event_t *self)     = rpmh_set_rex_event;
void (*rpmh_event_wait)(rpmh_event_t *self)    = rpmh_wait_rex_event;


