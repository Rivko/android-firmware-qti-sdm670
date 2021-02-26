/******************************************************************************
  @file  EventQueueTest.cpp
  @brief EventQueue unit tests

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "EventDataPool.h"
#include "EventQueue.h"
#include "ttf.h"

#define MAX_NUM_THREADS 100
#define PASS_CODE (-2)

//callbacks for eventqueue
typedef struct msg {
    int num;
}msg_t;

static void *Alloccb() {
    void *mem = (void *) new msg_t;
    return mem;
}

static void Dealloccb(void *mem) {
    if (NULL != mem) {
        delete (msg_t *)mem;
    }
}

typedef struct data {
    int i;
    EventQueue *eq;
    int result;
}data;

class EventQueueTest: public EventQueue {
public:
    int MultiThreadedTest(int numthreads = 0);
    int StressTest();
    int OverNightStressTest();
    static void *WaitThread(void *d);
    static void *WakeupThread(void *d);
};

int EventQueueTests(int flags) {
    int ret = 0;
    EventQueueTest eq;

    if (flags & TESTS_SIMPLE) {
        //simple test
        ret = eq.MultiThreadedTest(1);
        if (ret >= 0)
           printf("SimpleTest Passed\n");
        else
           printf("SimpleTest Failed\n");
    }

    if (flags & TESTS_MULTITHREADED) {
        //multithreaded test
        ret = eq.MultiThreadedTest(20);
        if (ret >= 0)
            printf("MultiThreadedTest Passed\n");
        else
            printf("MultiThreadedTest Failed\n");
    }

    if (flags & TESTS_STRESS) {
        //stress test
        ret = eq.StressTest();
        if (ret >= 0)
            printf("StressTest Passed\n");
        else
            printf("StressTest Failed\n");
    }

    if (flags & TESTS_NIGHT_STRESS) {
        ret = eq.OverNightStressTest();
        if (ret >= 0)
            printf("OverNightStressTest Passed\n");
        else
            printf("OverNightStressTest Failed\n");
    }

    return ret;
}


void *EventQueueTest::WaitThread(void *d) {
    int ret = -1;
    data *dat = (data *)d;

    if (NULL == dat) {
        return reinterpret_cast<void*>(ret);
    }

    EventQueue *evqueue = (EventQueue *) dat->eq;
    if (NULL == evqueue) {
        return reinterpret_cast<void*>(ret);
    }
    
    //wait for perflock commands
    EventData *evData = evqueue->Wait();

    if (evData && (evData->mEvType == dat->i)) {
        ret = 0;
        dat->result = PASS_CODE ;
    }

    return reinterpret_cast<void*>(ret);
}

void *EventQueueTest::WakeupThread(void *d) {
    int ret = -1;
    data *dat = (data *)d;
    EventData *evData = NULL;

    if (NULL == dat) {
        return reinterpret_cast<void*>(ret);
    }

    EventQueue *evqueue = (EventQueue *) dat->eq;
    if (NULL == evqueue) {
        return reinterpret_cast<void*>(ret);
    }

    evData = evqueue->GetDataPool().Get();
    if (NULL == evData) {
        printf("event data pool ran empty\n");
        return reinterpret_cast<void*>(ret);
    }

    evData->mEvType = dat->i;

    evqueue->Wakeup(evData);
    ret = 0;

    return reinterpret_cast<void*>(ret);
}

int EventQueueTest::MultiThreadedTest(int numthreads) {
    int rc, i;
    const int NUM_THREADS = 20;
    pthread_t threads[MAX_THREADS];
    pthread_t threads2[MAX_THREADS];
    pthread_attr_t attr;
    void *status;
    data d[MAX_NUM_THREADS];
    int ret = 0;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (numthreads == 0) {
        numthreads = NUM_THREADS;
    }

    for( i=0; i < NUM_THREADS; i++ ){
        d[i].i = i;
        d[i].eq = new EventQueue();
        if (d[i].eq) {
            d[i].eq->GetDataPool().SetCBs(Alloccb, Dealloccb);
        }
        d[i].result = 0;
        pthread_create(&threads[i], &attr, WaitThread, (void *)&d[i]);
    }

    for( i=0; i < NUM_THREADS; i++ ){
        pthread_create(&threads2[i], &attr, WakeupThread, (void *)&d[i]);
    }

    //free attribute and wait for the other threads
    pthread_attr_destroy(&attr);

    for( i=0; i < NUM_THREADS; i++ ){
        rc = pthread_join(threads2[i], &status);
    }

    for( i=0; i < NUM_THREADS; i++ ){
        rc = pthread_join(threads[i], &status);
    }

    for( i=0; i < NUM_THREADS; i++ ){
        if (d[i].result != PASS_CODE) {
            ret=-1;
            break;
        }
    }

    return ret;
}

int EventQueueTest::StressTest() {
    int ret = 0;
    ret = MultiThreadedTest(1);
    if (ret < 0) {
        return ret;
    }
    for (int i=0; i<1000;i++) {
        ret = MultiThreadedTest(100);
        if (ret < 0) {
            break;
        }
    }
    return ret;
}

int EventQueueTest::OverNightStressTest() {
    int ret = 0;
    for (int i=0; i<100000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface TTF
static TinyTF ttf2 = {
   "EventQueueTests", EventQueueTests,
};

