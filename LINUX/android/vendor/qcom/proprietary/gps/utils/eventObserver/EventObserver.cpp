/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <errno.h>
#include <string.h>
#include "EventObserver.h"
#include <log_util.h>
#include <ulp_service.h>
#include <loc_pla.h>

void *evtRxThread(void * arg1)
{
    EventObserver *evtObsPtr = ((EventObserver *)arg1);
    evtObsPtr->evtObserverRxThread();
    return NULL;
}

EventObserver::EventObserver():
                            mObserverRole(EVENT_OBSERVER_PRODUCER),
                            mFifoFd(0),
                            mFifoFd_rw(0),
                            mSendEvent(1),
                            mObserverCb(NULL)
{
    int result;
    LOC_LOGV("TX Thread constructor called..\n");
    strlcpy(mFifoName, EVENT_OBSERVER_FIFO_NAME, strlen(EVENT_OBSERVER_FIFO_NAME)+1);
    result = createFifo();
    if (result != 0)
    {
        LOC_LOGE("%s: createFifo Failed - %s, err= %d \n", __func__, mFifoName, errno);
    }
}

EventObserver::EventObserver(eventObserver_cb systemEventCallbackFn):
                            mObserverRole(EVENT_OBSERVER_CONSUMER),
                            mFifoFd(0),
                            mFifoFd_rw(0),
                            mSendEvent(0),
                            mObserverCb(systemEventCallbackFn)
{
    int result;
    pthread_t thread_id;
    const char *msg = "evtObsRxThrd";

    LOC_LOGV("RXThread constructor called.. FIFOLen: %zu\n", strlen(EVENT_OBSERVER_FIFO_NAME));
    strlcpy(mFifoName, EVENT_OBSERVER_FIFO_NAME, strlen(EVENT_OBSERVER_FIFO_NAME)+1);
    result = createFifo();
    if (result != 0)
    {
        LOC_LOGE("%s: createFifo Failed - %s, err= %d \n", __func__, mFifoName, errno);
    }

    result = pthread_create( &thread_id, NULL, evtRxThread, (void*) this);
    if(0 != result)
    {
        LOC_LOGE("%s: RX thread creation failed - %s, err= %d \n", __func__, mFifoName, errno);
    }
}

EventObserver::~EventObserver()
{
    if(0 != mFifoFd)
    {
        close(mFifoFd);
        mFifoFd = 0;
    }
    if (0 != mFifoFd_rw)
    {
        close(mFifoFd_rw);
        mFifoFd_rw = 0;
    }
}

int EventObserver::createFifo()
{
    int result = EINVAL, fd = 0, fd_rw = 0;
    if( access( mFifoName, F_OK ) == 0 ) {
        // file exists
        LOC_LOGV("FIFO Exists..\n");
    } else {
        result = mkfifo(mFifoName, 0660);
        if (result != 0)
        {
            LOC_LOGE("%s: failed to create FIFO %s, err = %d, ignore \n", __func__, mFifoName, errno);
        }
    }

    result = chmod (mFifoName, 0660);
    if (result != 0)
    {
        LOC_LOGE("%s: failed to change mode for %s, error = %d\n", __func__, mFifoName, errno);
    }

    struct group * gps_group = getgrnam("gps");
    if (gps_group != NULL)
    {
       result = chown (mFifoName, -1, gps_group->gr_gid);
       if (result != 0)
       {
          LOC_LOGE("%s: chown for pipe failed, gid = %d, result = %d, error code = %d\n", __func__, gps_group->gr_gid, result, errno);
       }
    }
    else
    {
       LOC_LOGE("%s: getgrnam for gps failed, error code = %d\n", __func__,  errno);
    }

    // To avoid deadlock, we open for RDWR first
    fd_rw = open(mFifoName, O_RDWR);
    LOC_LOGD("%s: open pipe for read name = %s, fd_rw = %d\n", __func__, mFifoName, fd);

    if (fd_rw == -1)
    {
       LOC_LOGE("%s: open pipe for RD/WR failed: name = %s, err = %d\n", __func__, mFifoName, errno);
       return -1;
    }
    else {
        mFifoFd_rw = fd_rw;
    }

    // Open the pipe for non-blocking write/read, so if the pipe is full, the write will return instead of running into deadlock
    if (mObserverRole == EVENT_OBSERVER_PRODUCER)
    {
        //Event Producer
        fd = open(mFifoName, O_WRONLY | O_NONBLOCK);
    }
    else {
        //Event Consumer
        fd = open(mFifoName, O_RDONLY);
    }

    LOC_LOGD("%s: open pipe for write name = %s, fd = %d\n", __func__, mFifoName, fd);
    if (fd == -1)
    {
       LOC_LOGE("%s: open pipe for write failed: name = %s, err = %d\n", __func__, mFifoName, errno);
       return -1;
    }
    else
    {
       mFifoFd = fd;
    }

    return result;
}

int  EventObserver::sendSystemEvent(unsigned int SystemEvent)
{
    int writeLen = 0;
    if (ULP_LOC_EVENT_OBSERVER_STOP_EVENT_TX == SystemEvent)
    {
        mSendEvent = 0;
        LOC_LOGV("%s: STOP SENDING SYSTEM EVENT \n", __func__);
        return true;
    }
    if ((mFifoFd != 0) && (mSendEvent))
    {
        EventObserverMsg evtMsg(SystemEvent);
        writeLen = write(mFifoFd,&evtMsg, sizeof(EventObserverMsg));
        if(writeLen < 0)
        {
            LOC_LOGE("%s: Send Failed, err = %d\n", __func__, errno);
        }
    } else {
        LOC_LOGE("%s: Send Failed, mFifoFd == 0\n", __func__);
    }
    return writeLen;
}
int EventObserver::evtObserverRxThread()
{
    int result = 0, errorCount = 0;
    EventObserverMsg evtMsg(0);

    while(mFifoFd)
    {
        result = read(mFifoFd, &evtMsg, sizeof(EventObserverMsg));
        if (result <= 0)
        {
            LOC_LOGE("evtObserverRxThread read Error -- %d\n",errno);
            if(errorCount++ > FIFO_MAX_READ_ERROR)
            {
                LOC_LOGE("evtObserverRxThread MAX read error -- exit thread\n");
                break;
            }
            else{
                continue;
            }
        }
        LOC_LOGV("%s: SYSTEM EVENT = %d\n", __func__, evtMsg.eventId);

        if(NULL != mObserverCb)
        {
            /*Deliver received SystemEvent*/
            mObserverCb(evtMsg.eventId);
        }
    }
    return result;
}
