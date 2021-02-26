/*!
  @file
  qcril_signal.h

  @brief
  Enables sending inter-module notifications

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#ifndef QCRIL_SIGNAL_H_INCLUDED
#define QCRIL_SIGNAL_H_INCLUDED

struct qcril_signal;
struct qcril_signal_args
{
    va_list arg_list;
};

typedef struct qcril_signal qcril_signal;
typedef struct qcril_signal_args qcril_signal_args;


typedef int (*qcril_signal_callback_t)(qcril_signal *signal, void *userdata, qcril_signal_args *args);

int qcril_signal_init(void);
qcril_signal *qcril_signal_get(const char *name);
int qcril_signal_put(qcril_signal *signal);
int qcril_signal_add_callback(qcril_signal *signal,
                              qcril_signal_callback_t cb,
                              void *userdata);
int qcril_signal_trigger(qcril_signal *signal, ...);
#endif
