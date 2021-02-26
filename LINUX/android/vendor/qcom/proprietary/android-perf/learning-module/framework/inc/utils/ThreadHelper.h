/******************************************************************************
  @file    ThreadHelper.h
  @brief   Implementation of Android Framework Learning Module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef FWK_THREADHELPER_H
#define FWK_THREADHELPER_H

#include <pthread.h>
#include <atomic>
#include <errno.h>

#include "Meter.h"
#include "DebugLog.h"

class ThreadHelper{
    pthread_mutex_t mutex;
    pthread_cond_t cond_wait;
    pthread_t th;
    std::string threadName;
    bool isDetachable;

    bool update;
    std::atomic_bool threadCreated;
    Meter *meter;
    int pendingUpdates;
    bool exitThread;
    int lock();
    int unlock();
    std::string getThreadName(){ return threadName; }
    static void* thread_callback(void* helper);

public:
    ThreadHelper(std::string name, bool isDetachable);
    ~ThreadHelper();

    int createThread(Meter* meter);
    void wakeUp();
    const std::atomic_bool& isReady();
    void destroyThread();
};

#endif
