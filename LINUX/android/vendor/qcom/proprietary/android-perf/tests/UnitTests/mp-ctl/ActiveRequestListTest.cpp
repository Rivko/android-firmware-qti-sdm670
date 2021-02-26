/******************************************************************************
  @file  ActiveRequestList.cpp
  @brief test module to test

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include "Request.h"
#include "ActiveRequestList.h"
#include "ttf.h"

class ActiveRequestListTest {
public:
    static void *DoMultithreadedWork(void *lst);
    int BasicTests(ActiveRequestList *arl);
    int MultithreadedBasicTests(ActiveRequestList *arl);

    int SimpleTest();
    int SimpleTest2();
    int MultiThreadedTest(int numthreads = 0);
    int StressTest();
    int OverNightStressTest();
};

int ActiveRequestListTests(int flags) {
    int ret = -1;
    ActiveRequestListTest aRLt;

    //tests
    if (flags & TESTS_SIMPLE) {
        ret = aRLt.SimpleTest();
        if (ret >= 0)
            printf("SimpleTest Passed\n");
        else
           printf("SimpleTest Failed\n");
        ret = aRLt.SimpleTest2();
        if (ret >= 0)
            printf("SimpleTest2 Passed\n");
        else
           printf("SimpleTest2 Failed\n");
    }

    if (flags & TESTS_MULTITHREADED) {
        ret = aRLt.MultiThreadedTest();
        if (ret >= 0)
            printf("MultiThreadedTest Passed\n");
        else
            printf("MultiThreadedTest Failed\n");
    }

    if (flags & TESTS_STRESS) {
        ret = aRLt.StressTest();
        if (ret >= 0)
            printf("StressTest Passed\n");
        else
            printf("StressTest Failed\n");
    }

    if (flags & TESTS_NIGHT_STRESS) {
        ret = aRLt.OverNightStressTest();
        if (ret >= 0)
            printf("OverNightStressTest Passed\n");
        else
            printf("OverNightStressTest Failed\n");
    }

    return ret;
}

#define MAX_REQUESTS 250

int ActiveRequestListTest::BasicTests(ActiveRequestList *arl) {
    int ret = -1, i =0;
    Request *req[MAX_REQUESTS];

    if (NULL == arl) {
        return ret;
    }

    for (int i=0; i<MAX_REQUESTS; i++) {
        req[i] = NULL;
    }

    for (int i = 0; i<MAX_REQUESTS; i++) {
        req[i] = new Request(2000, (pid_t)(1000+i), (pid_t) (1000+i), REGULAR_CLIENT);
        arl->Add(req[i], i+1);
    }

    ret = 0;
    if (MAX_REQUESTS != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    //verify handles
    ret = 0;
    for (i=0; i<MAX_REQUESTS; i++) {
        if (NULL == arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    //verify active reqs
    for (i=0; i<MAX_REQUESTS; i++) {
        if (NULL == arl->IsActive(i+1, req[i])) {
            ret = -1;
            break;
        }
    }

    if (ret < 0) {
        return ret;
    }

    if (MAX_REQUESTS != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    //verify remove
    for (i=0; i<(MAX_REQUESTS-100); i++) {
        arl->Remove(req[i]);
    }
    if (100 != arl->GetNumActiveRequests()) {
        ret = -1;
    }
    if (ret < 0) {
        return ret;
    }
    for (i=0; i<(MAX_REQUESTS-100); i++) {
        if (NULL != arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    for (i=(MAX_REQUESTS-100); i<MAX_REQUESTS; i++) {
        if (NULL == arl->DoesExists(i+1)) {
            ret = -1;
            break;
        }
    }
    if (ret < 0) {
        return ret;
    }

    //verify reset
    arl->Reset();
    if (0 != arl->GetNumActiveRequests()) {
        ret = -1;
    }

    if (ret < 0) {
        return ret;
    }

    return ret;
}

int ActiveRequestListTest::MultithreadedBasicTests(ActiveRequestList *arl) {
    int ret = -1, i =0;
    Request *req[MAX_REQUESTS];

    if (NULL == arl) {
        return ret;
    }

    for (int i=0; i<MAX_REQUESTS; i++) {
        req[i] = NULL;
    }

    for (int i = 0; i<MAX_REQUESTS; i++) {
        req[i] = new Request(2000, (pid_t)(1000+i), (pid_t) (1000+i), REGULAR_CLIENT);
        arl->Add(req[i], i+1);
    }

    for (i=0; i<MAX_REQUESTS; i++) {
        arl->Remove(req[i]);
    }
    return 0;
}

int ActiveRequestListTest::SimpleTest() {
    int ret = -1;
    ActiveRequestList *arl = new ActiveRequestList;
    if (NULL != arl) {
        ret = BasicTests(arl);
        delete arl;
    }
    return ret;
}

int ActiveRequestListTest::SimpleTest2() {
    int ret = -1;
    ActiveRequestList *arl = NULL;

    for (int i=0; i<100; i++) {
        arl = new ActiveRequestList;
        if (NULL != arl) {
            ret = BasicTests(arl);
            delete arl;
            if (ret < 0) {
                break;
            }
        }
    }

    return ret;
}


typedef struct {
    ActiveRequestList *aRL;
    ActiveRequestListTest *aRLT;
    int result;
}ArlIOStruct;

void *ActiveRequestListTest::DoMultithreadedWork(void *lst) {
    int ret = -1;
    ArlIOStruct *tmp = (ArlIOStruct *) lst;
    if (NULL == tmp) {
        return NULL;
    }
    ActiveRequestList *arl = tmp->aRL;
    ActiveRequestListTest *arlt = tmp->aRLT;
    if ((NULL != arl) && (NULL != arlt)) {
        ret = arlt->MultithreadedBasicTests(arl);
        tmp->result = ret;
    }

    return NULL;
}

int ActiveRequestListTest::MultiThreadedTest(int numthreads) {
   int ret = -1, i = 0;
   const int NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   ArlIOStruct structs[MAX_THREADS];
   ActiveRequestList *arl = NULL;
   pthread_attr_t attr;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   arl = new ActiveRequestList;

   for( i=0; i < numthreads; i++ ){
      structs[i].result = -1;
      structs[i].aRL = arl;
      structs[i].aRLT = this;
      pthread_create(&threads[i], &attr, DoMultithreadedWork, (void *)&structs[i]);
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      pthread_join(threads[i], &status);
   }

   if (NULL != arl) {
       ret  = 0;
       if (0 != arl->GetNumActiveRequests()){
           ret =-1;
       }
       delete arl;
   }

   return ret;
}

int ActiveRequestListTest::StressTest() {
    int ret = 0;
    ret = MultiThreadedTest(100);

    for (int i=0; i<1000;i++) {
        ret = MultiThreadedTest(50);
    }
    return ret;
}

int ActiveRequestListTest::OverNightStressTest() {
    int ret = 0;
    for (int i=0; i<100000;i++) {
        ret = SimpleTest();
        ret = SimpleTest2();
        ret = MultiThreadedTest(100);

        for (int j=0; j<1000;j++) {
            ret = MultiThreadedTest(50);
        }
    }
    return ret;
}

//interface for TTF
static TinyTF ttfarl = {
   "ActiveRequestListTests", ActiveRequestListTests,
};

