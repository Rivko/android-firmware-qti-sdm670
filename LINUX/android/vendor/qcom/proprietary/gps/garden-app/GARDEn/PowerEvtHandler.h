/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef _POWER_EVENT_HANDLER_H_
#define _POWER_EVENT_HANDLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "EventObserver.h"

class PowerEvtHandler {
private:
    int                 mSuspend;
    int                 mPwrDeviceFd;
    pthread_t           mThreadId;
    EventObserver       *mProducerEventObserver;

public:
    PowerEvtHandler();
    ~PowerEvtHandler();

    void         suspendOrResume();
    int          powerEvtHandlerThread();
    int          timeDiffUs(struct timeval *press, struct timeval *release);
};

#endif //_POWER_EVENT_HANDLER_H_
