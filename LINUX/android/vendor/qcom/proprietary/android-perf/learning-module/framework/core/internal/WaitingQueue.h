/******************************************************************************
 *   @file    WaitingQueue.h
 *   @brief   Generic Waiting Queue Template
 *
 *   DESCRIPTION
 *      A waiting queue template which waits on the queue while it's empty
 *   and pops the queue when an element is pushed to the queue.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/
#ifndef WAITINGQUEUE_H
#define WAITINGQUEUE_H

#include <mutex>
#include <queue>
#include <string>
#include <condition_variable>
#include "DebugLog.h"

#define TAG_WQ "WQ"

template<typename T>
class WaitingQueue {
public:
    T pop() {
        std::unique_lock<std::mutex> mlock(mMutex);
        DEBUGV(TAG_WQ, "%s pop() size=%d", mQueueName.c_str(), getSize());
        /*
         * Use while(empty()) instead of if(empty()) to take care of
         * spurious wakeups and wakeup after queue clear due to flood/flush
         */
        while (mQueue.empty()) {
            mCond.wait(mlock);
        }
        auto item = mQueue.front();
        mQueue.pop();

        DEBUGV(TAG_WQ, "%s pop() done size=%d", mQueueName.c_str(), getSize());
        return item;
    }

    void push(const T& item) {
        /* scoping for the unique_lock */
        {
            std::unique_lock<std::mutex> mlock(mMutex);
            /* If threshold is set and flooding is detected, clear the queue */
            if(mFlushThreshold != 0 && getSize() >= mFlushThreshold) {
                DEBUGW(TAG_WQ,
                        "%s push() size=%d mFlushThreshold=%d, Reached threshold => flooding detected, clearing Queue",
                        mQueueName.c_str(), getSize(), mFlushThreshold);
                while (!mQueue.empty()) {
                    mQueue.pop();
                }
            } else {
                DEBUGV(TAG_WQ, "%s push() size=%d", mQueueName.c_str(), getSize());
                mQueue.push(item);
                DEBUGV(TAG_WQ, "%s push() done size=%d", mQueueName.c_str(), getSize());
            }
        }
        mCond.notify_one();
    }

    WaitingQueue(std::string queueName, int threshold = 0) {
        mQueueName = queueName;
        mFlushThreshold = threshold;
    }

private:
    std::queue<T> mQueue;
    std::string mQueueName;
    std::mutex mMutex;
    std::condition_variable mCond;
    /* Queue Flush threshold - if non-zero, the queue would be flushed
     * when it's size reaches this threshold
     */
    int mFlushThreshold;

    int getSize() {
        return mQueue.size();
    }

    int getName() {
        return mQueueName;
    }
};

#endif /* WAITINGQUEUE_H */
