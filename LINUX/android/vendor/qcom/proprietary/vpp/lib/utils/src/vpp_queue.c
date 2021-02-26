/*!
 * @file vpp_queue.c
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @services
 *
 * @description
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vpp_dbg.h"
#include "vpp_core.h"
#include "vpp_queue.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
// #define DEBUG_QUEUE_DS

#ifdef DEBUG_QUEUE_DS
#define VPP_QUEUE_LOG LOGI
#else
#define VPP_QUEUE_LOG(...)
#endif

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ***********************************************************************/

/************************************************************************
 * Local functions
 ***********************************************************************/
static inline uint32_t DVP_MINU(uint32_t a, uint32_t b)
{
    return (a < b) ? a : b;
}

static inline uint32_t DVP_MOD_PLUS(uint32_t i, uint32_t del, uint32_t mod)
{
    uint32_t t = i + del;
    return DVP_MINU(t, t - mod);
}

/************************************************************************
 * Global functions
 ***********************************************************************/

int32_t vpp_queue_init(t_StVppQueue *q, uint32_t size)
{
    if (!q)
        return -1;

    memset(q, 0, sizeof(t_StVppQueue));
    q->size = size;

    VPP_QUEUE_LOG("allocated queue: %p, size: %u", q, size);

    return 0;
}

int32_t vpp_queue_term(t_StVppQueue *q)
{
    if (q)
        memset(q, 0, sizeof(t_StVppQueue));

    return 0;
}

int32_t vpp_queue_enqueue(t_StVppQueue *q)
{
    int32_t next;
    if (!q)
        return -1;

    VPP_QUEUE_LOG("enqueue: queue: %p ", q);

    // check if the queue is full
    if (q->entries == q->size)
    {
        LOGE("queue is full for q: %p", q);
        return -1;
    }

    next = DVP_MOD_PLUS(q->head, 1, q->size);
    q->head = next;
    q->entries += 1;

    return next;
}

int32_t vpp_queue_dequeue(t_StVppQueue *q)
{
    int32_t next;

    if (!q)
        return -1;

    VPP_QUEUE_LOG("dequeue: queue: %p", q);

    if (q->entries == 0)
    {
        VPP_QUEUE_LOG("trying to dequeue from empty q: %p", q);
        return -1;
    }

    next = DVP_MOD_PLUS(q->tail, 1, q->size);
    q->tail = next;
    q->entries -= 1;

    VPP_QUEUE_LOG("dequeue: returning: %d, entries=%u", next, q->entries);
    return next;
}

uint32_t vpp_queue_empty(t_StVppQueue *q)
{
    if (!q)
        return VPP_ERR_PARAM;

    return q->entries == 0 ? 1 : 0;
}

uint32_t vpp_queue_count(t_StVppQueue *q)
{
    if (!q)
        return 0;

    return q->entries;
}

int32_t vpp_queue_peek_head(t_StVppQueue *q)
{
    int32_t next;

    if (!q)
        return -1;

    VPP_QUEUE_LOG("peek_head: queue: %p", q);

    if (q->entries == 0)
    {
        VPP_QUEUE_LOG("trying to peek from empty q: %p", q);
        return -1;
    }

    next = DVP_MOD_PLUS(q->tail, 1, q->size);

    VPP_QUEUE_LOG("peek: returning: %d, entries=%u", next, q->entries);
    return next;
}

