/******************************************************************************
  @file  PerfGlueLayerTest.cpp
  @brief test module to test hal

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include "PerfGlueLayer.h"
#include "ttf.h"

class PerfGlueLayerTest: public PerfGlueLayer {

public:
    int SimpleTest();
    int SimpleTest2();
    int SimpleReloadTest();
    int SimpleNonExistantLibLoadTest();
    int MultiThreadedTest(int numthreads = 0);
    int StressTest();
    int OverNightStressTest();
    int Verify();
};

int PerfGlueLayerTests(int flags) {
    int ret = -1, ret2 = -1;
    PerfGlueLayerTest tGL;

    //tests
    if (flags & TESTS_SIMPLE) {
        ret = tGL.SimpleTest();
        ret2 = tGL.Verify();
        tGL.PerfGlueLayerExit();
        if ((ret >= 0) && (ret2 == MAX_MODULES))
            printf("SimpleTest Passed\n");
        else
           printf("SimpleTest Failed\n");

        ret = tGL.SimpleTest2();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("SimpleTest2 Passed\n");
        else
           printf("SimpleTest2 Failed\n");

        ret = tGL.SimpleReloadTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("SimpleReloadTest Passed\n");
        else
           printf("SimpleReloadTest Failed\n");

        ret = tGL.SimpleNonExistantLibLoadTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("SimpleNonExistantLibLoadTest Passed\n");
        else
           printf("SimpleNonExistantLibLoadTest Failed\n");
    }

    if (flags & TESTS_MULTITHREADED) {
        ret = tGL.MultiThreadedTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("MultiThreadedTest Passed\n");
        else
            printf("MultiThreadedTest Failed\n");
    }

    if (flags & TESTS_STRESS) {
        ret = tGL.StressTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("StressTest Passed\n");
        else
            printf("StressTest Failed\n");
    }

    if (flags & TESTS_NIGHT_STRESS) {
        ret = tGL.OverNightStressTest();
        ret2 = tGL.Verify();
        if ((ret >= 0) && (ret2 >= 0))
            printf("OverNightStressTest Passed\n");
        else
            printf("OverNightStressTest Failed\n");
    }

    tGL.PerfGlueLayerExit();

    return 0;
}

int PerfGlueLayerTest::Verify() {
    int ret = 0;

    for (int i=0; i<MAX_MODULES; i++) {
        if (!mModules[i].IsEmpty()) {
            ret++;
        }
    }
    return ret;
}

int PerfGlueLayerTest::SimpleTest() {
    int ret = -1, numReg = 0;

    PerfGlueLayerExit();

    ret = LoadPerfLib("libqti-tests-mod1.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = LoadPerfLib("libqti-tests-mod2.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod3.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod4.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod5.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = LoadPerfLib("libqti-tests-mod6.so");
    if (ret >= 0) {
        numReg++;
    }

    ret = -1;
    //possible that already registered modules with glue will lead to less successful load test libs
    if (numReg == MAX_MODULES) {
        ret = 0;
    }

    return ret;
}

int PerfGlueLayerTest::SimpleTest2() {
    int ret = -1, numReg = 0;

    PerfGlueLayerExit();

    for (int i=0; i<5; i++) {
        ret = LoadPerfLib("libqti-tests-mod1.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod2.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod3.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = LoadPerfLib("libqti-tests-mod4.so");
        if (ret >= 0) {
            numReg++;
        }
        ret = -1;
        if (numReg == MAX_MODULES) {
           ret = 0;
        }

        numReg = 0;
        PerfGlueLayerExit();
    }

    return ret;
}

int PerfGlueLayerTest::SimpleReloadTest() {
    int ret = -1, numReg = 0;

    PerfGlueLayerExit();

    for (int i=0; i<10; i++) {
        ret = LoadPerfLib("libqti-tests-mod1.so");
        if (ret >= 0) {
            numReg++;
        }
    }

    ret = -1;
    if (numReg == 1) {
       ret = 0;
    }

    PerfGlueLayerExit();

    return ret;
}

int PerfGlueLayerTest::SimpleNonExistantLibLoadTest() {
    int ret = -1, numReg = 0;

    //clear all if any registered modules
    PerfGlueLayerExit();

    for (int i=0; i<10; i++) {
        ret = LoadPerfLib("libqti-nonexist.so");
        if (ret >= 0) {
            numReg++;
        }
    }

    ret = -1;
    if (numReg == 0) {
       ret = 0;
    }

    PerfGlueLayerExit();

    return ret;
}

void *RegisterMultithreadedMods(void *data) {
    int ret = -1, numReg = 0;
    PerfGlueLayerTest *glue1 = (PerfGlueLayerTest *)data;

    if (NULL == glue1) {
        return NULL;
    }

    ret = glue1->LoadPerfLib("libqti-tests-mod1.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod2.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod3.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod4.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod5.so");
    if (ret >= 0) {
        numReg++;
    }
    ret = glue1->LoadPerfLib("libqti-tests-mod6.so");
    if (ret >= 0) {
        numReg++;
    }

    return NULL;
}

int PerfGlueLayerTest::MultiThreadedTest(int numthreads) {
   int rc, i;
   const int NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   void *status;

   PerfGlueLayerExit();

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (0 == numthreads) {
       numthreads = NUM_THREADS;
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, RegisterMultithreadedMods, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   PerfGlueLayerExit();

   return 1;
}

int PerfGlueLayerTest::StressTest() {
    int ret = 0;
    ret = MultiThreadedTest(100);

    for (int i=0; i<1000;i++) {
        ret = MultiThreadedTest(50);
    }
    return ret;
}

int PerfGlueLayerTest::OverNightStressTest() {
    int ret = 0;
    for (int i=0; i<100000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static TinyTF ttfglue = {
   "PerfGlueLayerTests", PerfGlueLayerTests,
};

