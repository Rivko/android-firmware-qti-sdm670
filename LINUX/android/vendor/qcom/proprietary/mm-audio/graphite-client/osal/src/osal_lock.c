/*
 * osal_lock.c
 *
 * This library contains locking operations helper functions for GCS.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "osal_lock.h"
#include "osal_log.h"

/* Internal lock definition */
typedef struct osal_int_lock {
    pthread_mutex_t mutex;
} osal_int_lock_t;

int32_t osal_lock_create(osal_lock_t *ret_lock)
{
    int32_t rc;
    osal_int_lock_t* the_lock;

    if (ret_lock == NULL) {
        OSAL_LOGE("%s: ret_lock is NULL\n", __func__);
        return -EINVAL;
    }

    the_lock = ((osal_int_lock_t *) malloc(sizeof(osal_int_lock_t)));
    if (the_lock == NULL) {
        OSAL_LOGE("%s: failed to allocate memory for lock\n", __func__);
        rc = -ENOMEM;
        goto exit;
    }

    rc = pthread_mutex_init(&the_lock->mutex, NULL);
    if (rc) {
        OSAL_LOGE("%s: Failed to create lock\n", __func__);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto fail;
    }

    *ret_lock = the_lock;
    return 0;

fail:
    free(the_lock);

exit:
    return rc;
}


int32_t osal_lock_destroy(osal_lock_t lock)
{
    int32_t rc = 0;
    osal_int_lock_t *the_lock = lock;

    if (the_lock == NULL) {
        OSAL_LOGE("%s: lock is NULL\n", __func__);
        return -EINVAL;
    }

    rc = pthread_mutex_destroy(&the_lock->mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to destroy lock\n", __func__);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto exit;
    }
    free(the_lock);

exit:
    return rc;
}


int32_t osal_lock_acquire(osal_lock_t lock)
{
    int32_t rc;
    osal_int_lock_t *the_lock = lock;

    if (the_lock == NULL) {
        OSAL_LOGE("%s: lock is NULL\n", __func__);
        return -EINVAL;
    }

    rc = pthread_mutex_lock(&the_lock->mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to acquire lock\n", __func__);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        return rc;
    }

    return rc;
}


int32_t osal_lock_release(osal_lock_t lock)
{
    int32_t rc;
    osal_int_lock_t *the_lock = lock;

    if (the_lock == NULL) {
        OSAL_LOGE("%s: lock is NULL\n", __func__);
        return -EINVAL;
    }

    rc = pthread_mutex_unlock(&the_lock->mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to release lock\n", __func__);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        return rc;
    }

    return rc;
}
