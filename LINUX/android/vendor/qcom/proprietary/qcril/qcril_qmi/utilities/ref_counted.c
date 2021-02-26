/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "ref_counted.h"
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

struct ref_counted_struct
{
    pthread_mutex_t ref_cnt_mutex;
    unsigned long ref_cnt;
    ref_counted_delete_cb delete_cb;
};

void ref_counted_default_free(void *strct, ref_counted *rfcnt)
{
    if (!strct || !rfcnt) return;

    free(strct);
}
void ref_counted_init(void *strct, ref_counted **ref_cnt, ref_counted_delete_cb delete_cb)
{
    pthread_mutexattr_t attr;
    ref_counted *rfcnt;

    if (!strct || !ref_cnt)
    {
        return;
    }

    rfcnt = calloc(1, sizeof(ref_counted));
    if(rfcnt)
    {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&rfcnt->ref_cnt_mutex, &attr);

        rfcnt->ref_cnt = 1;
        rfcnt->delete_cb = delete_cb;
    }

    if (ref_cnt)
    {
        *ref_cnt = rfcnt;
    }
}

void * ref_counted_incr(void *strct, ref_counted *rfcnt)
{
    void *ret = NULL;

    if (!strct || !rfcnt)
    {
        return ret;
    }

    pthread_mutex_lock(&rfcnt->ref_cnt_mutex);
    ++rfcnt->ref_cnt;
    pthread_mutex_unlock(&rfcnt->ref_cnt_mutex);
    return strct;
}

unsigned long ref_counted_decr(void *strct, ref_counted *rfcnt)
{
    ref_counted_delete_cb delete_cb;
    int deleted = 0;
    unsigned long ret = 0;

    if (!strct || !rfcnt)
    {
        return ret;
    }

    pthread_mutex_lock(&rfcnt->ref_cnt_mutex);

    if ( (ret = --rfcnt->ref_cnt) == 0 )
    {
        deleted = 1;
    }

    pthread_mutex_unlock(&rfcnt->ref_cnt_mutex);
    if ( deleted )
    {
        delete_cb = rfcnt->delete_cb;
        pthread_mutex_destroy(&rfcnt->ref_cnt_mutex);
        if (delete_cb)
        {
            delete_cb(strct, rfcnt);
        }
        free(rfcnt);
    }
    return ret;
}
