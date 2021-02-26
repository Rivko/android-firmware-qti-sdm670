/*
 * Copyright (c) 2017-18 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <atomic>
#include <pthread.h>
#include <atomic>
#include <queue>
#include "sensors_log.h"
#include <wakelock_utils.h>

class sns_wakelock;

/**
 * type alias for worker's task function
 */
using worker_task = std::function<void()>;

/**
 * Implementation of a worker thread with its own task-queue.
 * Worker thread starts running when constructed and stops when
 * destroyed. Tasks can be assigned to the worker
 * asynchronously and they will be performed in order.
 */
class worker
{
public:
    /**
     * @brief creates a worker thread and starts processing tasks
     */
    worker(): _alive(true)
    {
      _mutex = PTHREAD_MUTEX_INITIALIZER;
      _cond = PTHREAD_COND_INITIALIZER;
      strlcpy(ret, "create", MY_STR_LEN);
      _unsuspendable = false;
      pthread_create(&_threadhandle, NULL, &worker::thread_execute, this);
    }

    /**
     * @brief terminates the worker thread, waits until all
     *        outstanding tasks are finished.
     *
     * @note this destructor should never be called from the worker
     *       thread itself.
     */
    ~worker()
    {
        pthread_mutex_lock( &_mutex );
        _alive = false;
        pthread_cond_signal( &_cond );
        pthread_mutex_unlock( &_mutex );
        pthread_join(_threadhandle,(void**)&ret);
        remaining_ind_mem_cleanup();
    }

    void setname(const char *name) {
        pthread_setname_np(_threadhandle , name);
    }
    static void *thread_execute(void *param)
    {
        worker *ptr = ((worker *)param);
        if (NULL != ptr)
           ptr->run();
        strlcpy(ptr->ret, "exit", MY_STR_LEN);
        pthread_exit((void*)ptr->ret);
        return NULL;
    }

    /**
     * @brief helps to make worker thread as unsuspendable.
     *      useful for wakeup sensors
     * @note name of the wakelock must be unique per thread
     */
    int make_unsuspendable(std::string S){
       if (_unsuspendable) {
        sns_loge("already set, must not set more than once");
        return -1;
      }
      _unsuspendable = true;
      _wake_lock = std::make_unique<sns_wakelock>(S);
      return 0;
    }

     /**
     * @brief add a new task for the worker to do
     *
     * Tasks are performed in order in which they are added
     *
     * @param task task to perform
     */
    void add_task(void *buf_ptr, worker_task task)
    {
        pthread_mutex_lock( &_mutex );
        if(NULL != buf_ptr){
          _ind_mem_tracker.push(buf_ptr);
        }
        if(_wake_lock)
            _wake_lock->get();
        _task_q.push(task);
        pthread_cond_signal( &_cond );
        pthread_mutex_unlock( &_mutex );
    }

    /**
    * @brief free memory pushed to worked thread while calling add_task
    *
    * This task is required to avoid the memory leaks
    *
    */
    void ind_mem_free()
    {
      pthread_mutex_lock( &_mutex );
      if(_ind_mem_tracker.size()){
        void *buf_ptr = _ind_mem_tracker.front();
        free(buf_ptr);
        _ind_mem_tracker.pop();
      }
      pthread_mutex_unlock( &_mutex );
    }

private:
    /* waits until there is something to do */
    void wait_for_signal()
    {
        pthread_mutex_lock( &_mutex );
        /* wait if queue is empty and we are alive */
        while (_task_q.empty() && _alive) {
          pthread_cond_wait( &_cond, &_mutex );
        }
        pthread_mutex_unlock( &_mutex );
    }

    /* get number of outstanding tasks */
    typename std::queue<worker_task>::size_type remaining_tasks()
    {
        pthread_mutex_lock( &_mutex );
        size_t size = _task_q.size();
        pthread_mutex_unlock( &_mutex );
        return size;
    }

    /* clear remaining ind memory which are in task queue*/
    void remaining_ind_mem_cleanup()
    {
        pthread_mutex_lock( &_mutex );
        while(_ind_mem_tracker.size()){
            void *buf_ptr = _ind_mem_tracker.front();
            free(buf_ptr);
            _ind_mem_tracker.pop();
        }
        pthread_mutex_unlock( &_mutex );
    }

    /* worker thread's mainloop */
    void run()
    {
        while (_alive) {
            wait_for_signal();
            /* finish all outstanding tasks in the queue */
            while (_alive && remaining_tasks() != 0) {
                auto& task = _task_q.front();
                try {
                    if (NULL != task)
                       task();
                } catch (const std::exception& e) {
                    /* if an unhandled exception happened when running
                       the task, just log it and move on */
                    sns_loge("task failed, %s", e.what());
                }
                /* critical section: remove item from queue */
                pthread_mutex_lock( &_mutex );
                if(_wake_lock)
                   _wake_lock->put(true);
                if (NULL != task)
                   _task_q.pop();
                pthread_mutex_unlock( &_mutex );
            }
        }
    }
    std::atomic<bool> _alive;
    std::queue<worker_task> _task_q;
    std::queue<void *> _ind_mem_tracker;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_t _threadhandle;
    const static int MY_STR_LEN = 20;
    char ret[MY_STR_LEN];
    /* flag to make the worker thread prevent target to go to suspend */
    bool _unsuspendable;
    std::unique_ptr<sns_wakelock> _wake_lock;
};
