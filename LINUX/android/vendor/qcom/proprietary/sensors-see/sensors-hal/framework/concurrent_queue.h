/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "sensors_log.h"
#include <wakelock_utils.h>

/**
 * @brief thread-safe and generic concurrent_queue implementation
 *
 */
template <typename T>
class concurrent_queue
{
public:

    /**
     * @brief concurrent_queue constructor and wakelock name
     *
     */
    concurrent_queue():_wake_lock(std::make_unique<sns_wakelock>("sensor_ssc_cq")){};
    /**
     * @brief remove and return front element from the queue
     *
     *  blocks if queue is empty
     *
     * @return T
     */
    T pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_q.empty()) {
            _cv.wait(lock);
        }
        auto item = _q.front();
        _q.pop();
        bool wakeflag = _q_wake_cnt.front();
        if (wakeflag) _wake_lock->put(false);
        _q_wake_cnt.pop();
        return item;
    }

    /**
     * @brief add an element at the end of the queue
     *
     * @param item
     */
    void push(const T& item, bool wakeup)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _q.push(item);
        if (wakeup) {
          _wake_lock->get();
          _q_wake_cnt.push(true);
        } else
          _q_wake_cnt.push(false);
        lock.unlock();
        _cv.notify_one();
    }

    /**
     * @brief returns number of elements in the queue
     *
     * @return typename std::queue<T>::size_type
     */
    typename std::queue<T>::size_type size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _q.size();
    }

    /**
     * @brief calls sns_wakelock class put function and sns_wakelock
     * object release the wakelock if no wakeup samples in queue
     *
     */
    void release_wakelock()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _wake_lock->put(true);
    }

private:
    std::queue<T> _q;
    /*same concurrent queue for both wakeup and non wakup samples
        . have  a count variable to help to call put or not */
    std::queue<bool> _q_wake_cnt;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::unique_ptr<sns_wakelock> _wake_lock;
};
