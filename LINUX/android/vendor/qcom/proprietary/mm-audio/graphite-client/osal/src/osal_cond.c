/*
 * osal_cond.c
 *
 * This file defines conditional variable implementaion.
 *
 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "osal_cond.h"
#include "osal_log.h"

/* Internal condition definition */
typedef struct osal_int_cond {
    pthread_mutex_t osal_mutex;
    pthread_cond_t osal_cond;
} osal_int_cond_t;

int32_t osal_cond_create(osal_cond_t *ret_cond)
{
    int32_t rc;
    osal_int_cond_t *the_cond;

    if (ret_cond == NULL) {
        OSAL_LOGE("%s: cond is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_cond = (osal_int_cond_t *)malloc(sizeof(osal_int_cond_t));
    if (the_cond == NULL) {
        OSAL_LOGE("%s: failed to allocate cond memory\n", __func__);
        rc = -ENOMEM;
        goto done;
    }

    rc = pthread_mutex_init(&the_cond->osal_mutex, NULL);
    if (rc) {
        OSAL_LOGE("%s: Failed to init mutex, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_mutex;
    }

    rc = pthread_cond_init(&the_cond->osal_cond, NULL);
    if (rc) {
        OSAL_LOGE("%s: Failed to init cond, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_cond;
    }

    *ret_cond = the_cond;
    rc = pthread_mutex_lock(&the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to acquire lock, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_cond;
    }
    goto done;

err_cond:
    pthread_mutex_destroy(&the_cond->osal_mutex);

err_mutex:
    free(the_cond);

done:
   return rc;
}

int32_t osal_cond_destroy(osal_cond_t cond)
{
    int32_t rc;
    osal_int_cond_t *the_cond;

    if (cond == NULL) {
        OSAL_LOGE("%s: cond is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_cond = (osal_int_cond_t *)cond;
    rc = pthread_cond_destroy(&the_cond->osal_cond);
    if (rc) {
        OSAL_LOGE("%s: Failed to destroy cond, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    rc = pthread_mutex_destroy(&the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to destroy mutex, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    free(cond);

done:
   return rc;
}

int32_t osal_cond_wait(osal_cond_t cond)
{
    int32_t rc;
    osal_int_cond_t *the_cond;

    if (cond == NULL) {
        OSAL_LOGE("%s: condition is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_cond = (osal_int_cond_t *)cond;

    rc = pthread_cond_wait(&the_cond->osal_cond, &the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to wait on cond, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    rc = pthread_mutex_unlock(&the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to unlock, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

done:
    return rc;
}

int32_t osal_cond_timedwait(osal_cond_t cond, uint32_t timeout_in_ms)
{
    int32_t rc, rc1;
    osal_int_cond_t *the_cond;
    struct timespec osal_ts;

    if (cond == NULL) {
        OSAL_LOGE("%s: cond is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    clock_gettime(CLOCK_REALTIME, &osal_ts);
    osal_ts.tv_sec += (timeout_in_ms / 1000);
    osal_ts.tv_nsec = 0;

    the_cond = (osal_int_cond_t *)cond;

    rc = pthread_cond_timedwait(&the_cond->osal_cond, &the_cond->osal_mutex, &osal_ts);
    if (rc) {
        OSAL_LOGE("%s: Failed to wait on cond, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    rc1 = pthread_mutex_unlock(&the_cond->osal_mutex);
    if (rc1) {
        OSAL_LOGE("%s: Failed to unlock, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc1;
    }

done:
    return rc;
}

int32_t osal_cond_signal(osal_cond_t cond)
{
    int32_t rc;
    osal_int_cond_t *the_cond;

    if (cond == NULL) {
        OSAL_LOGE("%s: cond is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_cond = (osal_int_cond_t *)cond;
    rc = pthread_mutex_lock(&the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to acquire lock, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto done;
    }

    rc = pthread_cond_signal(&the_cond->osal_cond);
    if (rc) {
        OSAL_LOGE("%s: Failed to signal on cond, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    rc = pthread_mutex_unlock(&the_cond->osal_mutex);
    if (rc) {
        OSAL_LOGE("%s: Failed to unlock, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

done:
    return rc;
}
