/*!
 * @file vpp_queue.h
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @services
 *
 * @description
 */

#ifndef _VPP_QUEUE_H_
#define _VPP_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************
 * Defines
 ***********************************************************************/

/************************************************************************
 * Structures / Enumerations
 ***********************************************************************/

typedef struct {
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    uint32_t entries;
} t_StVppQueue;
/************************************************************************
 * Function Prototypes
 ***********************************************************************/

int32_t vpp_queue_init(t_StVppQueue *q, uint32_t size);
int32_t vpp_queue_term(t_StVppQueue *q);
int32_t vpp_queue_enqueue(t_StVppQueue *q);
int32_t vpp_queue_dequeue(t_StVppQueue *q);
uint32_t vpp_queue_empty(t_StVppQueue *q);
uint32_t vpp_queue_count(t_StVppQueue *q);
int32_t vpp_queue_peek_head(t_StVppQueue *q);

#ifdef __cplusplus
}
#endif

#endif /* _VPP_QUEUE_H_ */
