/*
 * osal_thread.c
 *
 * This file has implementation of thread related helper functions.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <errno.h>
#include "osal_thread.h"
#include "osal_log.h"

typedef struct osal_int_thread {
    pthread_t osal_handle;
    osal_thread_fn_t fn;
    void* param;
} osal_int_thread_t;

static void* osal_thread_wrapper(void *param)
{
    osal_int_thread_t *the_thread = (osal_int_thread_t *)param;

    if (the_thread != NULL)
        the_thread->fn(the_thread->param);

    return NULL;
}

int32_t osal_thread_create(osal_thread_t *ret_thread,
                          int8_t *name __unused,
                          int32_t priority __unused,
                          uint8_t *stack_base __unused,
                          int32_t stack_size __unused,
                          osal_thread_fn_t thread_fn,
                          void *thread_param)
{
    int32_t rc;
    osal_int_thread_t *the_thread;
    pthread_attr_t attr;
    void *ret_val;

    if (ret_thread == NULL) {
        OSAL_LOGE("%s: ret_thread is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_thread = (osal_int_thread_t *) malloc(sizeof(osal_int_thread_t));
    if (the_thread == NULL) {
        OSAL_LOGE("%s: Failed to allocate memory\n", __func__);
        rc = -ENOMEM;
        goto done;
    }

    rc = pthread_attr_init(&attr);
    if (rc) {
        OSAL_LOGE("%s: Failed to init attributes, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_attr;
    }

    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (rc) {
        OSAL_LOGE("%s: Failed to set detach state, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_set;
    }

    the_thread->fn = thread_fn;
    the_thread->param = thread_param;
    rc = pthread_create(&the_thread->osal_handle, &attr,
                        osal_thread_wrapper, ((void *)the_thread));
    if (rc) {
        OSAL_LOGE("%s: Failed to create thread, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_set;
    }

    rc = pthread_attr_destroy(&attr);
    if (rc) {
        OSAL_LOGE("%s: Failed to destroy attributes, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
        goto err_destoy;
    }

    *ret_thread = (osal_thread_t *)the_thread;
    goto done;

err_destoy:
    pthread_join(the_thread->osal_handle, &ret_val);

err_set:
    pthread_attr_destroy(&attr);

err_attr:
    free(the_thread);

done:
    return rc;
}

int32_t osal_thread_destroy(osal_thread_t thread)
{
    int32_t rc = 0;
    osal_int_thread_t *the_thread;
    void *ret_val;

    if (thread == NULL) {
        OSAL_LOGE("%s: Thread is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }
    the_thread = (osal_int_thread_t *)thread;
    rc = pthread_join(the_thread->osal_handle, &ret_val);
    if (rc) {
        OSAL_LOGE("%s: Failed to join the thread, rc = %d\n", __func__, rc);
        /*
         * pthread lib returns positive error code.
         * Return (-) of error received.
         */
        rc = -rc;
    }

    free(the_thread);

done:
    return rc;
}

int32_t osal_thread_close_from_thread_fn()
{
    return 0;
}
