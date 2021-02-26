/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "IzatSvc_Utils"

#include <stdlib.h>
#include <string.h>
#include "Utils.h"
#include <time.h>
#include <pthread.h>
#include "IzatDefines.h"
int GetTimeSinceBoot(int64 & time_since_bootup)
{
  int result = 0;
  struct timespec tp;

  do {
#ifdef CLOCK_BOOTIME
    BREAK_IF_NON_ZERO(1, clock_gettime(CLOCK_BOOTTIME, &tp));
#else
    BREAK_IF_NON_ZERO(2, clock_gettime(CLOCK_MONOTONIC, &tp));
#endif

    time_since_bootup = convertSecsToNanoSecs(tp.tv_sec);
    time_since_bootup += tp.tv_nsec;

    result = 0;
  } while (0);

  if (result != 0) {
    LOC_LOGE("Error in getting current time");
  }

  return result;
}

pthread_t create_thread_callback(const char* threadName, void (*start)(void *), void* arg)
{
  pthread_t threadId;
  pthread_attr_t attr;

  pthread_attr_init(&attr);

  if (threadName != NULL &&
          !pthread_create(
                  &threadId, &attr,
                  reinterpret_cast <pthread_func>(start), arg) ) {
    char lname[MAX_THREAD_NAME_LEN+1];
    memcpy(lname, threadName, MAX_THREAD_NAME_LEN);
    lname[MAX_THREAD_NAME_LEN] = 0;
    pthread_setname_np(threadId,lname);
    return threadId;
  }
  else{
      // Destroy attr since thread creation failed
      pthread_attr_destroy(&attr);
  }
  return 0;
}

int copyInCard(InPostcard * const inCardIn, InPostcard * const inCardOut) {
    int result = 0;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, inCardOut);
        // Make a copy of the incoming postcard
        unsigned char * inCardBufferPtr = const_cast<unsigned char *>(inCardIn->getBuffer()->getBuffer());
        BREAK_IF_ZERO(2, inCardBufferPtr);
        size_t inCardBufferSize = inCardIn->getBuffer()->getSize();
        BREAK_IF_ZERO(3, inCardBufferSize);
        unsigned char * copyBufferPtr = new unsigned char[inCardBufferSize];
        BREAK_IF_ZERO(4, copyBufferPtr);
        memcpy(static_cast<void *>(copyBufferPtr),
               static_cast<void *>(inCardBufferPtr),
               inCardBufferSize);

        if(inCardOut->init(
                static_cast<const void *>(copyBufferPtr),
                inCardBufferSize, true) != 0){
            // Free allocated memory
            LOC_LOGE("copyInCard(): inCardOut->init() failed");
            delete[] copyBufferPtr;
        }

    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

