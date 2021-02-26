/******************************************************************************
  @file    EventDataPool.h
  @brief   Implementation of event data pool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __EVENT_DATA_POOL__
#define __EVENT_DATA_POOL__

#include <pthread.h>
#include <list>
#include "PriorityQueue.h"

using namespace std;

#define MAX_POOL_SIZE 128

typedef struct EventData {
    int mEvType;
    void *mEvData;
    int mReserved;

    struct EventData *mNext;
    struct EventData *mPrev;
}EventData;

typedef void *(*AllocCB)();
typedef void (*DeallocCB)(void *);

class EventDataPool {
protected:
    EventData *mAllocPool;
    EventData *mFreePool;
    int mSize; //pool size
    pthread_mutex_t mMutex;

    AllocCB mAllocCB;
    DeallocCB mDeallocCB;

    int Resize(int n);
public:
    EventDataPool();
    ~EventDataPool();
    void SetCBs(AllocCB acb, DeallocCB dcb);
    EventData* Get();
    void Return(EventData *data);
    void Dump();
};

#endif

