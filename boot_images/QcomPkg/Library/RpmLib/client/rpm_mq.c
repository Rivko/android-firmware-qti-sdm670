/*===========================================================================

  Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/
/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     --------------------------------------------------------
  08/07/15     rli     Merge with legacy: remove oldest message shortcircuit
  06/05/15     sj/rli  Remove faulty dot in %0.8 format specifier
  01/14/15     rli     Glink update
=============================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include "rpmclient.h"
#include "rpm_mq.h"
#include "rpm_events.h"
#include "rpm_wire.h"
#include "inq.h"

#include "DALSys.h"
#include "ULogFront.h"
#include "CoreHeap.h"
#include "CoreVerify.h"
#include "rpm_synchandle.h"

extern RPMSyncHandle    rpm_lock;
extern ULogHandle       rpm_log;
bool rpm_is_up(void);

#ifdef PRE_SIL
static volatile bool that_rpm_guy_is_crazy = true;
#else
static volatile bool that_rpm_guy_is_crazy = false;
#endif

unsigned rpm_threads_waiting = 0;

const unsigned RPM_CONCURRENT_MESSAGE_LIMIT = 2;
const unsigned RPM_PREALLOCATED_MESSAGE_HEADERS = 50;

extern rpm_resource_t *rpm_find_resource(rpm_resource_type, unsigned);

typedef struct rpm_msg_s
{
    unsigned          msg_id;
    rpm_set_type      set;
    rpm_resource_type resource;
    unsigned          id;
    rpm_resource_t   *resource_data;
    rpm_event_t      *completion_event;

    inq_link_t        link;
} rpm_msg_t;

struct rpm_mq_s
{
    unsigned next_msg_id;

    inq_t *completed;
    inq_t *in_flight;
    inq_t *enqueued;
    inq_t *free_msgs;
};

static rpm_msg_t *new_msg(rpm_mq_t *self)
{
    rpm_msg_t *msg = (rpm_msg_t *)inq_pop(self->free_msgs);
    if(msg)
        return msg;

    msg = Core_Malloc(sizeof(rpm_msg_t));
    CORE_VERIFY_PTR(msg);
    memset(msg, 0, sizeof(rpm_msg_t));
    return msg;
}

static void delete_msg(rpm_mq_t *self, rpm_msg_t *msg)
{
    // The completion event is owned by the calling context, so it isn't cleaned up here.
    memset(msg, 0, sizeof(rpm_msg_t));
    inq_append(self->free_msgs, msg);
}

void rpm_mq_update_resource_pointers(rpm_mq_t *self)
{
    //Iterate through all the queues and re-lookup the resource pointers.
    //This should be done when a new resource is added because the resource table may have moved
    inq_iterator_t it_enq, it_inf, it_comp;
    rpm_msg_t     *msg;

    inq_it_begin(self->enqueued, &it_enq);
    while((msg = (rpm_msg_t *)inq_it_deref(&it_enq)) != NULL)
    {
        msg->resource_data = rpm_find_resource(msg->resource, msg->id);
        inq_it_next(&it_enq);
    }

    inq_it_begin(self->in_flight, &it_inf);
    while((msg = (rpm_msg_t *)inq_it_deref(&it_inf)) != NULL)
    {
        msg->resource_data = rpm_find_resource(msg->resource, msg->id);
        inq_it_next(&it_inf);
    }

    inq_it_begin(self->completed, &it_comp);
    while((msg = (rpm_msg_t *)inq_it_deref(&it_comp)) != NULL)
    {
        msg->resource_data = rpm_find_resource(msg->resource, msg->id);
        inq_it_next(&it_comp);
    }
}

rpm_mq_t *rpm_mq_create(void)
{
    unsigned i;
    rpm_mq_t *self = Core_Malloc(sizeof(rpm_mq_t));

    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(rpm_mq_t));

    self->next_msg_id = 1;
    self->completed   = inq_create(offsetof(rpm_msg_t, link));
    self->in_flight   = inq_create(offsetof(rpm_msg_t, link));
    self->enqueued    = inq_create(offsetof(rpm_msg_t, link));
    self->free_msgs   = inq_create(offsetof(rpm_msg_t, link));

    for(i = 0; i < RPM_PREALLOCATED_MESSAGE_HEADERS; i++)
    {
        rpm_msg_t *msg = Core_Malloc(sizeof(rpm_msg_t));
        CORE_VERIFY_PTR(msg);
        delete_msg(self, msg);
    }

    return self;
}

void rpm_mq_destroy(rpm_mq_t *self)
{
    void *msg;

    while((msg = inq_pop(self->completed)) != NULL) Core_Free(msg);
    inq_destroy(self->completed);

    while((msg = inq_pop(self->in_flight)) != NULL) Core_Free(msg);
    inq_destroy(self->in_flight);

    while((msg = inq_pop(self->enqueued)) != NULL) Core_Free(msg);
    inq_destroy(self->enqueued);

    while((msg = inq_pop(self->free_msgs)) != NULL) Core_Free(msg);
    inq_destroy(self->free_msgs);

    Core_Free(self);
}

static unsigned get_msg_id(rpm_mq_t *self)
{
    unsigned msg_id = self->next_msg_id++;
    if(msg_id)
        return msg_id;
    else
        return get_msg_id(self);
}

static unsigned _rpm_mq_send(rpm_mq_t *self, rpm_msg_t *msg)
{
    unsigned in_flight = 1;

    inq_append(self->in_flight, msg);

    // Gentle little hack to allow the RPM communication to short-circuit
    // during bringup scenarios.
    if(!rpm_is_up() || that_rpm_guy_is_crazy)
    {
        ULOG_RT_PRINTF_2(rpm_log, "rpm_message_autocomplete (rpm_is_up: %s) (override: %s)", rpm_is_up() ? "yes" : "no", that_rpm_guy_is_crazy ? "set" : "clear");
        rpm_mq_completed(self, msg->msg_id);
        in_flight = 0;
    }
    else
    {
        ULOG_RT_PRINTF_3(rpm_log, "rpm_sending_message (resource: 0x%08x) (id: 0x%08x) (msg_id: 0x%08x)", msg->resource, msg->id, msg->msg_id);
        if(!rpm_wire_send(msg->msg_id, msg->set, msg->resource, msg->id, msg->resource_data))
        {
            ULOG_RT_PRINTF_1(rpm_log, "\trpm_redundant_message (msg_id: 0x%08x)", msg->msg_id);
            rpm_mq_completed(self, msg->msg_id);
            in_flight = 0;
        }
    }

    return in_flight;
}

unsigned rpm_mq_get_oldest_in_flight_msg_id(rpm_mq_t *self)
{
    rpm_msg_t *oldest_in_flight = inq_head(self->in_flight);
    if(!oldest_in_flight)
    {
        return 0;
    }
    else
    {
        return oldest_in_flight->msg_id;
    }
}

unsigned rpm_mq_put(rpm_mq_t *self, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *resource_data)
{
    rpm_msg_t *msg    = new_msg(self);
    unsigned   msg_id = get_msg_id(self);

    msg->msg_id   = msg_id;
    msg->set      = set;
    msg->resource = resource;
    msg->id       = id;
    msg->resource_data = resource_data;

    // If its a sleep set message or we're not at our message limit yet, we can send immediately.
    if(set == RPM_ACTIVE_SET && (inq_size(self->in_flight) >= RPM_CONCURRENT_MESSAGE_LIMIT))
    {
        ULOG_RT_PRINTF_3(rpm_log, "rpm_holding_message (resource: 0x%08x) (id: 0x%08x) (msg_id: 0x%08x)", resource, id, msg->msg_id);
        inq_append(self->enqueued, msg);
    }
    else
    {
        if(!_rpm_mq_send(self, msg))
        {
            // Msg is not in flight so we can try to send other msgs
            rpm_mq_flush_limit(self);
        }
    }

    return msg_id;
}

static rpm_event_t *get_event(rpm_mq_t *self, rpm_msg_t *msg)
{
    if(!msg->completion_event)
        msg->completion_event = rpm_event_create();

    return msg->completion_event;
}

static unsigned find_and_wait(rpm_mq_t *self, inq_t *q, unsigned msg_id)
{
    inq_iterator_t it;
    rpm_msg_t     *msg;

    inq_it_begin(q, &it);

    while((msg = (rpm_msg_t *)inq_it_deref(&it)) != NULL)
    {
        if(msg_id == msg->msg_id)
        {
            rpm_event_t *completion_event = get_event(self, msg);
            ULOG_RT_PRINTF_2(rpm_log, "rpm_msg_wait (msg_id: 0x%08x) (event: 0x%08x)", msg_id, PtrToUlong(completion_event));

            rpm_threads_waiting++;
            RPM_SyncExit(rpm_lock);
            rpm_event_wait(completion_event);
            RPM_SyncEnter(rpm_lock);
            rpm_threads_waiting--;

            ULOG_RT_PRINTF_2(rpm_log, "rpm_msg_wait_complete (msg_id: 0x%08x) (event: 0x%08x)", msg_id, PtrToUlong(completion_event));
            rpm_event_destroy(completion_event);
            return 1;
        }

        inq_it_next(&it);
    }

    return 0;
}

static unsigned msg_age(rpm_mq_t *self, rpm_msg_t *msg)
{
    if(!msg)
      return 0;
    else
      return (self->next_msg_id - msg->msg_id);
}

void rpm_mq_wait(rpm_mq_t *self, unsigned msg_id)
{
    if(!msg_id)
        return;

    // The message should be in our queues somewhere; find it and wait.
    if(!find_and_wait(self, self->completed, msg_id))
        if(!find_and_wait(self, self->in_flight, msg_id))
            find_and_wait(self, self->enqueued,  msg_id);
}

static unsigned pull_from_queue(inq_t *q, unsigned msg_id, rpm_msg_t **msg)
{
    unsigned       was_head = 1;
    inq_iterator_t it;

    inq_it_begin(q, &it);

    while((*msg = (rpm_msg_t *)inq_it_deref(&it)) != NULL)
    {
        if(msg_id == (*msg)->msg_id)
        {
            inq_it_delete(&it);
            return was_head;
        }

        was_head = 0;
        inq_it_next(&it);
    }

    // We shouldn't ever make it this far; if the message isn't in the queue,
    // something has gone horribly wrong.
    ERR_FATAL("Inconsistent rpm mq state.", 0, 0, 0);
    //KW fix:
    return was_head;
}

static unsigned get_oldest_in_flight_age(rpm_mq_t *self)
{
    return msg_age(self, inq_head(self->in_flight));
}

static void retire_msg(rpm_mq_t *self, rpm_msg_t *completed)
{
    CORE_VERIFY_PTR(completed);
    ULOG_RT_PRINTF_1(rpm_log, "rpm_retire_msg (msg_id: 0x%08x)", completed->msg_id);

    if(completed->completion_event)
        rpm_event_set(completed->completion_event);
    delete_msg(self, completed);
}

static void rpm_mq_send_msg_at_head(rpm_mq_t *self)
{
    rpm_msg_t *outgoing = (rpm_msg_t *)inq_pop(self->enqueued);
    if(outgoing)//KW
    {
        _rpm_mq_send(self, outgoing);
    }
}

void rpm_mq_completed(rpm_mq_t *self, unsigned msg_id)
{
    rpm_msg_t *completed;
    unsigned   was_oldest = pull_from_queue(self->in_flight, msg_id, &completed);

    ULOG_RT_PRINTF_1(rpm_log, "rpm_mq_completed (msg_id: 0x%08x)", msg_id);

    // If the message we just pulled off was not the oldest one in flight, we
    // cannot retire the message right away.  Instead, we'll place it in the
    // completed queue (in-order); we'll retire it when the messages in front
    // of it are retired.
    if(!was_oldest)
    {
        rpm_msg_t     *msg;
        inq_iterator_t it;
        unsigned       completed_age;

        inq_it_begin(self->completed, &it);

        completed_age = msg_age(self, completed);
        while((msg = (rpm_msg_t *)inq_it_deref(&it)) != NULL)
        {
            if(msg_age(self, msg) < completed_age)
            {
                inq_it_insert(&it, completed);
                break;
            }

            inq_it_next(&it);
        }

        if(!msg)
        {
            // Nothing newer in the completed queue already; just append.
            inq_append(self->completed, completed);
        }
    }
    else
    {
        inq_iterator_t it;
        rpm_msg_t     *unblocked;
        unsigned       oldest_in_flight_age;

        // If we just pulled the oldest message off, we can retire it straight away.
        retire_msg(self, completed);

        // Also retire anything else that might be unblocked.
        inq_it_begin(self->completed, &it);

        oldest_in_flight_age = get_oldest_in_flight_age(self);
        while(inq_head(self->completed) &&
              msg_age(self, inq_head(self->completed)) > oldest_in_flight_age)
        {
            unblocked = (rpm_msg_t *)inq_pop(self->completed);
            CORE_VERIFY_PTR(unblocked);
            if ( unblocked ) //KW fix
            {
                retire_msg(self, unblocked);
            }
        }
    }

}

static unsigned find_msg_id(rpm_mq_t *self, inq_t *q, unsigned msg_id)
{
    inq_iterator_t it;
    rpm_msg_t     *msg;

    inq_it_begin(q, &it);

    while((msg = (rpm_msg_t *)inq_it_deref(&it)) != NULL)
    {
        if(msg_id == msg->msg_id)
        {
            return 1;
        }
        inq_it_next(&it);
    }

    return 0;
}

unsigned rpm_mq_is_in_flight(rpm_mq_t *self, unsigned msg_id)
{
    if(find_msg_id(self, self->completed, msg_id) ||
       find_msg_id(self, self->in_flight, msg_id) ||
       find_msg_id(self, self->enqueued,  msg_id))
    {
        return 1;
    }

    return 0;
}

bool rpm_mq_check_empty(rpm_mq_t *self)
{
    return (0 == inq_size(self->in_flight));
}

unsigned rpm_mq_check_num_in_flight(rpm_mq_t *self)
{
    return inq_size(self->in_flight);
}

void rpm_mq_flush(rpm_mq_t *self)
{
    unsigned num_queued = inq_size(self->enqueued);

    while(num_queued--)
    {
        rpm_mq_send_msg_at_head(self);
    }
}

void rpm_mq_flush_limit(rpm_mq_t *self)
{
    // Send out a new message, if we have them waiting to go out.
    while(inq_head(self->enqueued) && (inq_size(self->in_flight) < RPM_CONCURRENT_MESSAGE_LIMIT))
    {
        rpm_mq_send_msg_at_head(self);
    }
}

bool is_standalone(void)
{
  return that_rpm_guy_is_crazy;
}
