/******************************************************************************
  @file    EventDataPool.cpp
  @brief   Implementation of event data pool

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "EventQueue.h"
#include "stdlib.h"

#define LOG_TAG "ANDR-PERF-DATA-POOL"
#include <cutils/log.h>

using namespace std;

//event data memory pool
EventDataPool::EventDataPool() {
    mAllocPool = NULL;
    mFreePool = NULL;
    mAllocCB = NULL;
    mDeallocCB = NULL;
    mSize = 0;
    pthread_mutex_init(&mMutex, NULL);
    pthread_mutex_lock(&mMutex);
    Resize(MAX_POOL_SIZE);
    pthread_mutex_unlock(&mMutex);
}

//if not registered callbacks memory management of msg data lies with user
void EventDataPool::SetCBs(AllocCB acb, DeallocCB dcb) {
    pthread_mutex_lock(&mMutex);
    mAllocCB = acb;
    mDeallocCB = dcb;
    pthread_mutex_unlock(&mMutex);
}

EventDataPool::~EventDataPool() {
    //freeup free pool, alloc pool
    EventData *pCur = NULL, *tmp = NULL;

    for (pCur = mFreePool; pCur;) {
        tmp = pCur;
        pCur = pCur->mNext;
        if (mDeallocCB) {
            mDeallocCB(tmp->mEvData);
        }
        delete tmp;
    }

    for (pCur = mAllocPool; pCur;) {
        tmp = pCur;
        pCur = pCur->mNext;
        if (mDeallocCB) {
            mDeallocCB(tmp->mEvData);
        }
        delete tmp;
    }

    mAllocPool = NULL;
    mFreePool = NULL;
    pthread_mutex_destroy(&mMutex);
}

int EventDataPool::Resize(int n) {
    EventData *pCur = NULL;
    int i =0;

    //if free items still available in the pool then do not resize
    pCur = mFreePool;

    if (NULL != pCur) {
        return mSize;
    }

    //resize
    for (i = 0; i < n; i++) {
        EventData *pCur = new EventData;
        if (NULL != pCur) {
            pCur->mEvType = 0;
            pCur->mEvData = NULL;
            pCur->mReserved = 0;
            pCur->mPrev = NULL;
            pCur->mNext = mFreePool;

            if(NULL != mFreePool) {
                mFreePool->mPrev = pCur;
            }
            mFreePool = pCur;
        } else {
            break;
        }
    }

    mSize += i;
    QLOGI("New size of Event Data Pool:%d\n", mSize);
    pCur = mFreePool;

    if (NULL == pCur) {
        return -1;
    }

    return mSize;
}

EventData* EventDataPool::Get() {
    bool bAlloc = false;
    int ret = -1;

    pthread_mutex_lock(&mMutex);
    //all allocated, no free items
    if (NULL == mFreePool) {
        ret = Resize(mSize);
        if (ret <= 0) {
            QLOGE("EventDataPool Resize didn't work");
            pthread_mutex_unlock(&mMutex);
            return NULL;
        }
    }

    //remove from free pool if only type of data matches
    EventData *pCur = mFreePool;
    mFreePool = pCur->mNext;
    if(NULL != mFreePool) {
        mFreePool->mPrev = NULL;
    }
    if (NULL == pCur->mEvData) {
        //no data in event data, so allocate
        bAlloc = true;
    }

    if (bAlloc && mAllocCB) {
        pCur->mEvData = mAllocCB();
    }

    //add it to alloc pool
    pCur->mNext = mAllocPool;
    if(NULL != mAllocPool) {
        mAllocPool->mPrev = pCur;
    }
    mAllocPool = pCur;

    pthread_mutex_unlock(&mMutex);
    return pCur;
}

void EventDataPool::Return(EventData *data) {
    EventData *tmp;
    EventData *pCur = data;

    if (NULL == pCur) {
        return;
    }

    QLOGI("call to Return of eventpool data:%u", data);

    pthread_mutex_lock(&mMutex);
    //remove from alloc pool
    tmp = pCur->mPrev;
    if(NULL != tmp) {
        tmp->mNext =  pCur->mNext;
    } else {
        mAllocPool = pCur->mNext;
    }

    if (NULL !=  pCur->mNext) {
        pCur->mNext->mPrev = tmp;
    }

    //add it to free pool
    pCur->mNext = mFreePool;
    pCur->mPrev = NULL;
    if(NULL != mFreePool) {
        mFreePool->mPrev = pCur;
    }
    mFreePool = pCur;

    pthread_mutex_unlock(&mMutex);
    return;
}
void EventDataPool::Dump() {

    EventData *pCur = NULL;
    pthread_mutex_lock(&mMutex);
    for (pCur = mFreePool; pCur;) {
        if (pCur != NULL) {
            QLOGI("Freepool: pcur:%u pcurdata:%u", pCur, pCur->mEvData);
        }
        pCur = pCur->mNext;
    }

    for (pCur = mAllocPool; pCur;) {
        if (pCur != NULL) {
            QLOGI("Allocpool: pcur:%u pcurdata:%u", pCur, pCur->mEvData);
        }
        pCur = pCur->mNext;
    }
    pthread_mutex_unlock(&mMutex);
}
