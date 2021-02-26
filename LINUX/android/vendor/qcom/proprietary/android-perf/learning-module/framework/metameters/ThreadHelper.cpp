/******************************************************************************
  @file    ThreadHelper.cpp
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "ThreadHelper.h"
#include "inc/MeterFwk.h"

#define LOG_TAG_TH "ThreadHelper"
#define MAX_THREAD_NAME_LENGTH 15

using namespace std;

ThreadHelper::ThreadHelper(string name,bool detachable)
    :threadName(name),
    isDetachable(detachable) {

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_wait, NULL);
    update = false;
    threadCreated = false;
    pendingUpdates = 0;
    exitThread = false;
}

ThreadHelper::~ThreadHelper() {
    DEBUGD(LOG_TAG_TH,"CleanUp Begin:(Meter,Helper,Name)=(%p,%p,%s)\n",meter, this, getThreadName().c_str());
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_wait);
    DEBUGD(LOG_TAG_TH,"CleanUp Finish:(Meter,Helper,Name)=(%p,%p,%s)\n",meter, this, getThreadName().c_str());
}


void ThreadHelper::destroyThread() {
    lock();
    exitThread = true;
    unlock();

    wakeUp();

    if(!isDetachable) {
        DEBUGD(LOG_TAG_TH,"pthread_join ThreadName: %s\n",getThreadName().c_str());
        int rc = pthread_join(th,NULL);
        if(rc!=0) {
            DEBUGE(LOG_TAG_TH,"pthread_join Fail ThreadName: %s\n",getThreadName().c_str());
        } else {
            DEBUGD(LOG_TAG_TH,"pthread_join Success ThreadName: %s\n",getThreadName().c_str());
        }
    }
    return;
}

int ThreadHelper::createThread(Meter* mtr) {
    int retValue = -1;
    meter = mtr;

    if(!mtr) {
        return retValue;
    }

    retValue = pthread_create(&th,NULL,thread_callback,this);
    if (retValue != 0) {
        DEBUGE(LOG_TAG_TH,"Failed to create thread with Detachable state ThreadName: %s\n",getThreadName().c_str());
        return retValue;
    }

    if(isDetachable) {
        retValue = pthread_detach(th);
        if (retValue != 0) {
            DEBUGE(LOG_TAG_TH,"Failed to detach thread: %s\n",getThreadName().c_str());
            return retValue;
        }
    }


    return retValue;
}

int ThreadHelper::lock() {
    return pthread_mutex_lock(&mutex);
}
int ThreadHelper::unlock() {
    return pthread_mutex_unlock(&mutex);
}
const atomic<bool>& ThreadHelper::isReady() {
    return threadCreated;
}

void ThreadHelper::wakeUp() {
    int tStatus = 0;

    lock();
    update = true;
    pendingUpdates++;
    DEBUGD(LOG_TAG_TH,"(Meter,Helper,Name,pending)=(%p,%p,%s,%d)\n",meter, this, getThreadName().c_str(),pendingUpdates);
    tStatus=pthread_cond_signal(&cond_wait);
    if(tStatus != 0) {
        DEBUGE(LOG_TAG_TH,"Failed to send conditional signal ThreadName: %s\n",getThreadName().c_str());
    }
    unlock();

}

void* ThreadHelper::thread_callback(void* helper) {
    if(!helper) {
        return NULL;
    }

    ThreadHelper *threadHelper = (ThreadHelper *)helper;
    bool shouldExit = threadHelper->exitThread;

    string tname = threadHelper->getThreadName().substr(0,MAX_THREAD_NAME_LENGTH-1);
    if (pthread_setname_np(pthread_self(), tname.empty() ? "lm-unknown" : tname.c_str()) != 0) {
        DEBUGW(LOG_TAG_TH,"Failed to setname ThreadName: %s",threadHelper->getThreadName().c_str());
    }

    while(!shouldExit) {
        int tStatus = 0;
        tStatus = threadHelper->lock();
        if(tStatus != 0) {
            DEBUGE(LOG_TAG_TH,"lock Failed - Error %d\n",tStatus);
        }
        //shouldExit = threadHelper->exitThread;
        while(!threadHelper->update) {
            DEBUGD(LOG_TAG_TH,"Waiting:(Meter,Helper,Name,pending)=(%p,%p,%s,%d)\n",
            threadHelper->meter, threadHelper, threadHelper->getThreadName().c_str(),threadHelper->pendingUpdates);
            threadHelper->threadCreated = true;
            tStatus=pthread_cond_wait(&threadHelper->cond_wait,&threadHelper->mutex);
            if(tStatus != 0) {
                DEBUGE(LOG_TAG_TH,"Conditional Wait Failed: %d\n",tStatus);
            }
        }
        //wakeupUpdate = threadHelper->readWakeupUpdateFromQ();
        threadHelper->pendingUpdates--;
        shouldExit = threadHelper->exitThread;
        if(0 != threadHelper->pendingUpdates) {
            DEBUGE(LOG_TAG_TH,"More than one pending updates!!! This should not happen. MeterMgr should avoid this case.\n");
        }
        threadHelper->update = false;
        DEBUGD(LOG_TAG_TH,"Starting:(Meter,Helper,Name,pending)=(%p,%p,%s,%d) \n",
            threadHelper->meter, threadHelper, threadHelper->getThreadName().c_str(), threadHelper->pendingUpdates);
        tStatus = threadHelper->unlock();
        if(tStatus != 0) {
            DEBUGE(LOG_TAG_TH,"Unlock Failed - Error %d\n",tStatus);
        }
        if(!shouldExit) {
            threadHelper->meter->start();
            if((threadHelper->meter->getMeterFwk() != NULL)
                && (threadHelper->meter->getMeterFwk()->getReceiver()!=NULL)) {
                    threadHelper->meter->getMeterFwk()->getReceiver()->notifyReceiver(threadHelper->meter);
            }
        }
        DEBUGD(LOG_TAG_TH,"Returned from Start:(Meter,Helper,Name,pending)=(%p,%p,%s,%d) \n",
            threadHelper->meter, threadHelper, threadHelper->getThreadName().c_str(), threadHelper->pendingUpdates);
    }
    return NULL;
}

