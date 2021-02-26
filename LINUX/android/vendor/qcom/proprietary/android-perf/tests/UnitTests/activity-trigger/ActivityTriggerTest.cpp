/******************************************************************************
  @file  ActivityTriggerTest.cpp
  @brief test module to test activity triggers

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
 ******************************************************************************/
#include "ttf.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>

class ActivityTriggerTest {
public:
    void *dlhandle;
    void (*atstart)(const char *, int *);
    void (*atresume)(const char *);
    void (*atpause)(const char *);
    void (*atstop)(const char *);
    void (*atinit)(void);
    void (*atdeinit)(void);
    void (*atmisc)(int, const char *, int, int, float *);

    int Load();
    int Unload();
public:
    ActivityTriggerTest();
    ~ActivityTriggerTest();

    int SimpleLoadUnloadLibTest();
    int SimpleTest();
    int SimpleTest2();
    int MultiThreadedTest(int numthreads = 0);
    int MultiThreadedTest2(int numthreads = 0);
    int StressTest();
    int OverNightStressTest();

    static void *DoTrigger(void *);
    static void *DoTrigger2(void *);
};

int ActivityTriggerTests(int flags) {
    int ret = -1;

    ActivityTriggerTest att;

    //tests
    if (flags & TESTS_SIMPLE) {
        ret = att.SimpleLoadUnloadLibTest();
        if (ret >= 0)
            printf("SimpleLoadUnloadLibTest Passed\n");
        else
           printf("SimpleLoadUnloadLibTest Failed\n");
        ret = att.SimpleTest();
        if (ret >= 0)
            printf("SimpleTest Passed\n");
        else
           printf("SimpleTest Failed\n");
        ret = att.SimpleTest2();
        if (ret >= 0)
            printf("SimpleTest2 Passed\n");
        else
           printf("SimpleTest2 Failed\n");
    }

    if (flags & TESTS_MULTITHREADED) {
        ret = att.MultiThreadedTest();
        if (ret >= 0)
            printf("MultiThreadedTest Passed\n");
        else
            printf("MultiThreadedTest Failed\n");
        ret = att.MultiThreadedTest2();
        if (ret >= 0)
            printf("MultiThreadedTest2 Passed\n");
        else
            printf("MultiThreadedTest2 Failed\n");
    }

    if (flags & TESTS_STRESS) {
        ret = att.StressTest();
        if (ret >= 0)
            printf("StressTest Passed\n");
        else
            printf("StressTest Failed\n");
    }

    if (flags & TESTS_NIGHT_STRESS) {
        ret = att.OverNightStressTest();
        if (ret >= 0)
            printf("OverNightStressTest Passed\n");
        else
            printf("OverNightStressTest Failed\n");
    }

    return ret;
}

ActivityTriggerTest::ActivityTriggerTest() {
    dlhandle = NULL;
    atstart = NULL;
    atresume = NULL;
    atpause = NULL;
    atstop = NULL;
    atinit = NULL;
    atdeinit = NULL;
    atmisc = NULL;
}

ActivityTriggerTest::~ActivityTriggerTest() {
}

int ActivityTriggerTest::Load() {
    const char *rc = NULL;

    dlhandle = dlopen("libqti-at.so", RTLD_NOW | RTLD_LOCAL);
    if (dlhandle == NULL) {
        printf("load dlopen fail\n");
        return -1;
    }

    //clear off old ones
    dlerror();

    *(void **) (&atinit) = dlsym(dlhandle, "activity_trigger_init");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atstart) = dlsym(dlhandle, "activity_trigger_start");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atresume) = dlsym(dlhandle, "activity_trigger_resume");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atpause) = dlsym(dlhandle, "activity_trigger_pause");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atstop) = dlsym(dlhandle, "activity_trigger_stop");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atmisc) = dlsym(dlhandle, "activity_trigger_misc");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    *(void **) (&atdeinit) = dlsym(dlhandle, "activity_trigger_deinit");
    if ((rc = dlerror()) != NULL) {
        dlclose(dlhandle);
        return -1;
    }

    return 0;
}

int ActivityTriggerTest::Unload() {
    int ret = 0;
    ret = dlclose(dlhandle);
    return ret;
}

int ActivityTriggerTest::SimpleLoadUnloadLibTest() {
    int ret = 0;

    for (int i=0; i<100; i++) {
        ret = Load();
        if (ret < 0) {
            ret = -1;
            break;
        }

        ret = Unload();
        if (ret != 0) {
            ret = -1;
            break;
        }
        ret = 0;
    }

    return ret;
}

int ActivityTriggerTest::SimpleTest() {
    int ret = 0;
    int flags = 0;
    float value = 0.0;

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    for (int i=0; i<100; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }

        if (atpause) {
            (*atpause)("sampleapp/sample/1.2");
        }

        if (atresume) {
            (*atresume)("sampleapp/sample/1.2");
        }

        if (atmisc) {
            (*atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (atstop) {
            (*atstop)("sampleapp/sample/1.2");
        }
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

int ActivityTriggerTest::SimpleTest2() {
    int ret = 0;
    int flags = 0;
    float value = 0.0;
    char buf[128];

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    for (int i=0; i<100; i++) {
        if (atstart) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*atstart)(buf, &flags);
        }
    }

    for (int i=0; i<100; i++) {
        if (atpause) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*atpause)(buf);
        }
    }

    for (int i=0; i<100; i++) {
        if (atresume) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*atresume)(buf);
        }
    }

    for (int i=0; i<100; i++) {
        if (atmisc) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*atmisc)(i, buf, i, i+1, &value);
        }
    }

    for (int i=0; i<100; i++) {
        if (atstop) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*atstop)(buf);
        }
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}


void *ActivityTriggerTest::DoTrigger(void *ptr) {
    int flags = 0;
    float value = 0.0;

    ActivityTriggerTest *att = (ActivityTriggerTest *) ptr;

    if (NULL == att) {
        return NULL;
    }

    for (int i=0; i<10; i++) {
        if (att->atstart) {
            (*att->atstart)("sampleapp/sample/1.2", &flags);
        }

        if (att->atpause) {
            (*att->atpause)("sampleapp/sample/1.2");
        }

        if (att->atresume) {
            (*att->atresume)("sampleapp/sample/1.2");
        }

        if (att->atmisc) {
            (*att->atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (att->atstop) {
            (*att->atstop)("sampleapp/sample/1.2");
        }
    }

    return NULL;
}

void *ActivityTriggerTest::DoTrigger2(void *ptr) {
    int flags = 0;
    float value = 0.0;
    char buf[128];

    ActivityTriggerTest *att = (ActivityTriggerTest *) ptr;

    if (NULL == att) {
        return NULL;
    }

    for (int i=0; i<5; i++) {
        if (att->atstart) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*att->atstart)(buf, &flags);
        }
    }

    for (int i=0; i<5; i++) {
        if (att->atpause) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*att->atpause)(buf);
        }
    }

    for (int i=0; i<5; i++) {
        if (att->atresume) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*att->atresume)(buf);
        }
    }

    for (int i=0; i<5; i++) {
        if (att->atmisc) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*att->atmisc)(i, buf, i, i+1, &value);
        }
    }

    for (int i=0; i<5; i++) {
        if (att->atstop) {
            snprintf(buf, 128, "sampleapp%d/sample%d/1.2", i, i);
            (*att->atstop)(buf);
        }
    }

    return NULL;
}

int ActivityTriggerTest::MultiThreadedTest(int numthreads) {
   int rc, i;
   const int NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   int ret =0;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   ret = Load();
   if (ret < 0) {
       return ret;
   }

   if (atinit) {
       (*atinit)();
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, DoTrigger, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   if (atdeinit) {
       (*atdeinit)();
   }

   ret = Unload();
   if (ret != 0) {
       ret = -1;
   }

   //success
   return ret;
}

int ActivityTriggerTest::MultiThreadedTest2(int numthreads) {
   int rc, i;
   const int NUM_THREADS = 20;
   pthread_t threads[MAX_THREADS];
   pthread_attr_t attr;
   int ret =0;
   void *status;

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   if (numthreads == 0) {
       numthreads = NUM_THREADS;
   }

   ret = Load();
   if (ret < 0) {
       return ret;
   }

   if (atinit) {
       (*atinit)();
   }

   for( i=0; i < numthreads; i++ ){
      pthread_create(&threads[i], &attr, DoTrigger2, (void *)this );
   }

   //free attribute and wait for the other threads
   pthread_attr_destroy(&attr);

   for( i=0; i < numthreads; i++ ){
      rc = pthread_join(threads[i], &status);
   }

   if (atdeinit) {
       (*atdeinit)();
   }

   ret = Unload();
   if (ret != 0) {
       ret = -1;
   }

   //success
   return ret;
}

int ActivityTriggerTest::StressTest() {
    int ret = 0;
    int flags = 0;
    float value = 0.0;
    char buf[128];

    ret = Load();
    if (ret < 0) {
        return -1;
    }

    if (atinit) {
        (*atinit)();
    }

    //same app opening
    for (int i=0; i<10; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }
    }

    //different app opening
    for (int i=0; i<10; i++) {
        snprintf(buf, 128, "sampleapp-%d/sample-%d/1.2", i, i);
        if (atstart) {
            (*atstart)(buf, &flags);
        }
    }

    //stop in loop
    for (int i=0; i<10; i++) {
        snprintf(buf, 128, "sampleapp-%d/sample-%d/1.2", i, i);
        if (atstop) {
            (*atstop)(buf);
        }
    }

    for (int i=0; i<10; i++) {
        if (atstart) {
            (*atstart)("sampleapp/sample/1.2", &flags);
        }

        if (atpause) {
            (*atpause)("sampleapp/sample/1.2");
        }

        if (atresume) {
            (*atresume)("sampleapp/sample/1.2");
        }

        if (atmisc) {
            (*atmisc)(1, "sampleapp/sample/1.2", 0, 1, &value);
        }

        if (atstop) {
            (*atstop)("sampleapp/sample/1.2");
        }
    }


    for (int i=0; i<10; i++) {
        SimpleLoadUnloadLibTest();
        SimpleTest();
        SimpleTest2();
        MultiThreadedTest(200);
        MultiThreadedTest2(200);
    }

    if (atdeinit) {
        (*atdeinit)();
    }

    ret = Unload();
    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

int ActivityTriggerTest::OverNightStressTest() {
    int ret = 0;
    for (int i=0; i<1000;i++) {
        ret = StressTest();
    }
    return ret;
}

//interface for TTF
static TinyTF ttftrig = {
   "ActivityTriggerTests", ActivityTriggerTests,
};

