/*
 * osal_cond.h
 *
 * This file defines conditional variable helper functions.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_COND_H__
#define __OSAL_COND_H__

/* Client communicates with osal condition using a handle of this type. */
typedef void *osal_cond_t;

 /*
  * osal_cond_create - Create an osal condition structure
  *
  * @ret_cond:    Pointer to osal condition structure
  *
  * Returns 0 if success and error code otherwise
  *
  * Prerequisites
  *      None
  */
 int32_t osal_cond_create(osal_cond_t *ret_cond);

 /*
  * osal_cond_destroy - Destroy an osal condition structure
  *
  * NOTE: Client must ensure condition will not be used after it is destroyed.
  *
  * @cond:    Osal condition structure.
  *
  * Returns 0 if success and error code otherwise
  *
  * Prerequisites
  *      Condition must be created.
  */
 int32_t osal_cond_destroy(osal_cond_t cond);

 /*
  * osal_cond_wait - Wait on osal condition.
  *
  * @cond:    Osal condition structure.
  *
  * Returns 0 if it waits on condition and error code otherwise
  *
  * Prerequisites
  *      Condition must be created.
  */
 int32_t osal_cond_wait(osal_cond_t cond);

 /*
  * osal_cond_timedwait - Wait on osal condition till timeout happens.
  *
  * @cond:             Osal cond structure.
  * @timeout_in_ms:    Time to wait in ms
  * Returns 0 if it waits on condition and error code otherwise
  *
  * Prerequisites
  *      Condition must be created.
  */
 int32_t osal_cond_timedwait(osal_cond_t cond, uint32_t timeout_in_ms);

 /*
  * osal_cond_signal - Wait on osal condition.
  *
  * @cond:    Osal condition structure.
  *
  * Returns 0 if signal to condition is success and error code otherwise
  *
  * Prerequisites
  *      Condition must be created.
  */
 int32_t osal_cond_signal(osal_cond_t cond);

#endif /* __OSAL_COND_H__ */
