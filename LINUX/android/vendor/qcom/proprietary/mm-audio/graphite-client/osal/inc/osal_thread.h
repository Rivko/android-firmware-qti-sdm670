/*
 * osal_thread.h
 *
 * This file defines thread related helper functions.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_THREAD_H__
#define __OSAL_THREAD_H__

/* Client communicates with osal thread using a handle of this type. */
typedef void* osal_thread_t;
/*
 * osal thread callback function definition.
 *
 * @param:   The client supplied data pointer when creating a thread.
 *
 */
typedef void (*osal_thread_fn_t) (void *param);

/*
 * osal_thread_create - Create osal thread
 *
 * @ret_thread:    Pointer to osal thread structure.
 * @name:          Thread name. Considered maximum length is of 12
 *                 characters. If null, default name will be given.
 * @priority:      Thread priority can be specified as numeric values
 *                 in the range 0-255, with 0 representing the highest
 *                 priority.
 * @stack_base:    Pointer to allocated stack base. If null, stack
 *                 will be allocated by osal and will have size of
 *                 8192 bytes.
 * @stack_size:    Size of allocated stack, if allocated stack is
 *                 provided by client.
 * @thread_fn:     Callback function.
 * @thread_param:  Void pointer to reference thread parameter.
 *
 * Returns 0 if thread is created or error code upon failure
 *
 * Prerequisites
 *      None
 */
int32_t osal_thread_create(osal_thread_t *ret_thread,
                           int8_t *name,
                           int32_t priority,
                           uint8_t *stack_base,
                           int32_t stack_size,
                           osal_thread_fn_t thread_fn,
                           void *thread_param);

/*
 * osal_thread_destroy - Destroy osal thread.
 *
 * NOTE: Client must ensure thread will not be used after it is destroyed.
 *
 * @thread:       osal thread structure.
 *
 * Returns 0 if thread is destroyed or error code upon failure
 *
 * Prerequisites
 *      Thread must be created.
 */
int32_t osal_thread_destroy(osal_thread_t thread);
/*
 * osal_thread_close_from_thread_fn - Close osal thread from thread function.
 *
 * NOTE: Client must ensure thread will not be used after it is closed.
 *
 * Returns 0 if thread is closed or error code upon failure
 *
 * Prerequisites
 *      Thread must be created.
 */
int32_t osal_thread_close_from_thread_fn();

#endif /* __OSAL_THREAD_H__ */
