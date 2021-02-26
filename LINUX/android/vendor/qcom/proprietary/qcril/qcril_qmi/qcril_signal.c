/*!
  @file
  qcril_signal.c

  @brief
  Enables sending inter-module notifications

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "qcril_signal.h"
#include "simple_list.h"
#include "ref_counted.h"

static simple_list *signal_list;
static pthread_mutex_t signal_list_mutex;

typedef struct qcril_signal_callback_entry
{
    qcril_signal *signal;
    qcril_signal_callback_t cb;
    void *userdata;
} qcril_signal_callback_entry;

struct qcril_signal
{
    REF_COUNTED_DECL;
    char *name;
    simple_list *callback_list;
    pthread_mutex_t callback_list_mutex;
};

qcril_signal *list;
pthread_mutexattr_t mutex_attr;


int qcril_signal_init(void)
{
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&signal_list_mutex, &mutex_attr);
    signal_list = simple_list_new();

    return 0;
}

static void qcril_signal_delete(void *strct, ref_counted *rfcnt)
{
    //qcril_signal *s = (qcril_signal *)strct;

    if (!strct || !rfcnt)
    {
        return;
    }

    /*TODO: Delete list contents and free*/
    // free(strct->name);
}

static qcril_signal *qcril_signal_new(const char *name)
{
    qcril_signal *ret = NULL;

    if (!name)
    {
        return ret;
    }

    ret = calloc(1, sizeof(qcril_signal));
    if (ret)
    {
        pthread_mutex_init(&ret->callback_list_mutex, &mutex_attr);
        REF_COUNTED_INIT(ret, qcril_signal_delete);
        ret->callback_list = simple_list_new();
        ret->name = strdup(name);
    }

    return ret;
}

qcril_signal *qcril_signal_get(const char *name)
{
    qcril_signal *sig = NULL;
    simple_list_iter *it;

    pthread_mutex_lock(&signal_list_mutex);

    for(it = simple_list_first(signal_list);
        it;
        it = simple_list_iter_next(it))
    {
        sig = (qcril_signal *)simple_list_iter_get_value(it);
        if (sig)
        {
            if (!strcmp(name, sig->name))
            {
                break;
            }
        }
    }

    if (!sig)
    {
        sig = qcril_signal_new(name);
        simple_list_append(signal_list, sig);
    }

    REF_COUNTED_INCR(sig);
    simple_list_iter_done(it);
    pthread_mutex_unlock(&signal_list_mutex);

    return sig;
}
int qcril_signal_add_callback
(
    qcril_signal *signal,
    qcril_signal_callback_t cb,
    void *userdata
)
{
    int ret = 0;
    qcril_signal_callback_entry *cbe;

    if (!signal || ! cb)
    {
        return ret;
    }

    cbe = calloc(1, sizeof(qcril_signal_callback_entry));
    if (cbe)
    {
        cbe->cb = cb;
        cbe->userdata = userdata;
        cbe->signal = REF_COUNTED_INCR(signal);
        simple_list_append(signal->callback_list, cbe);
    }
    return ret;
}

int qcril_signal_trigger(qcril_signal *signal, ...)
{
    int ret = 0;
    qcril_signal_callback_entry *cbe;
    simple_list_iter *it;
    qcril_signal_args args;

    if (!signal)
    {
        return ret;
    }

    memset(&args, 0, sizeof(args));

    for(it = simple_list_first(signal->callback_list);
        it;
        it = simple_list_iter_next(it))
    {
        cbe = (qcril_signal_callback_entry *)simple_list_iter_get_value(it);
        if (cbe && cbe->cb)
        {
            va_start(args.arg_list, signal);
            cbe->cb(cbe->signal, cbe->userdata, &args);
            va_end(args.arg_list);
        }
    }

    return ret;
}
