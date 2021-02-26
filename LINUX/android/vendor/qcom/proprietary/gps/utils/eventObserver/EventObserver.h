/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef _EVENT_OBSERVER_H_
#define _EVENT_OBSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include<grp.h>

#define FIFO_PATHNAME_MAX   200
#define FIFO_MAX_READ_ERROR 1000
#define EVENT_OBSERVER_FIFO_NAME  "/data/vendor/location/mq/eventObserver_q"
typedef void (*eventObserver_cb)(unsigned int systemEvent);

typedef enum {
    EVENT_OBSERVER_PRODUCER = 1,
    EVENT_OBSERVER_CONSUMER,
    EVENT_OBSERVER_MAX
} EventObserverRole_T;

struct EventObserverMsg {
    const int eventId;
    inline EventObserverMsg(int id) : eventId(id)
    {
    }
    virtual ~EventObserverMsg()
    {
    }
};

class EventObserver {
private:
    EventObserverRole_T mObserverRole;
    char                mFifoName[FIFO_PATHNAME_MAX];
    int                 mFifoFd;
    int                 mFifoFd_rw;
    int                 mSendEvent;
    eventObserver_cb    mObserverCb;

public:
    EventObserver();
    EventObserver(eventObserver_cb systemEventCallbackFn);
    ~EventObserver();

    int          createFifo();
    int          sendSystemEvent(unsigned int SystemEvent);
    int          evtObserverRxThread();
};

#endif //_EVENT_OBSERVER_H_
