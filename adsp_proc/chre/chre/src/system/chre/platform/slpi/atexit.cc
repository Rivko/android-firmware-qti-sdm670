/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "chre/platform/log.h"

#include <inttypes.h>


// implement at exit callback handling locally due to missing implementation
// in some targets that don't support c++11

struct atexits_t {
  void(*func)(void *);
  void* obj;
  void* dso;
};

#define N_ATEXITS 10

struct atexits_t atexits[N_ATEXITS] = {0};


extern "C" void __cxa_finalize(void* dso)
{
	for (int i = N_ATEXITS - 1; i >= 0; i--) {
    if (atexits[i].func && dso == atexits[i].dso) {
      //LOGD("atexit calling func %p obj %p dso %p", atexits[i].func, atexits[i].obj, atexits[i].dso);
      atexits[i].func(atexits[i].obj);
      atexits[i].func = 0;
		}
	}
}

extern "C" void __cxa_atexit(void(*func)(void *), void *obj, void *dso)
{
  //LOGD("atexit register func %p obj %p dso %p", func, obj, dso);
  for (int i = 0; i < N_ATEXITS; i++) {
    if (0 == atexits[i].func) {
      atexits[i].func = func;
      atexits[i].obj = obj;
      atexits[i].dso = dso;
      return;
    }
  }
  LOGE("Failed to register atexit function, no more room");
}

