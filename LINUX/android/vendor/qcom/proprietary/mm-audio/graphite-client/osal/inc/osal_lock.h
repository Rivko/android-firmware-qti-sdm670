/*
 * osal_lock.h
 *
 * This header file defines lock operations helper functions for GCS.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_LOCK_H__
#define __OSAL_LOCK_H__

/* Client communicates with OSAL LOCK using a handle of this type. */
typedef void *osal_lock_t;

/*
 * osal_lock_create - Create an OSAL lock
 *
 * @ret_lock:    Pointer to OSAL lock structure
 *
 * Returns 0 if able to create a lock and errorcodes otherwise
 *
 * Prerequisites
 *    None
 */
int32_t osal_lock_create(osal_lock_t *ret_lock);

/*
 * osal_lock_destroy - Destroy an OSAL lock.
 *
 * NOTE: Client must ensure the lock will not be used after it is destroyed.
 *
 * @lock:    OSAL lock structure.
 *
 * Returns 0 if able to destroy the lock and errorcodes otherwise
 *
 * Prerequisites
 *    Lock must be created.
 */
int32_t osal_lock_destroy(osal_lock_t lock);

/*
 * osal_lock_acquire - Acquire OSAL lock.
 *
 * @lock:    OSAL lock structure.
 *
 * Returns 0 if able to acquire the lock and errorcodes otherwise
 *
 * Prerequisites
 *    Lock must be created.
 */
int32_t osal_lock_acquire(osal_lock_t lock);

/*
 * osal_lock_release - Release OSAL lock.
 *
 * @lock:    OSAL lock structure.
 *
 * Returns 0 if able to release the lock and errorcodes otherwise
 *
 * Prerequisites
 *    Lock must be created.
 */
int32_t osal_lock_release(osal_lock_t lock);

#endif /* __OSAL_LOCK_H__ */
