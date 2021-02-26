/*==============================================================================
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include <stdlib.h>
#include <string.h>
#include <type_traits>

#include "chre/platform/memory.h"
#include "chre/platform/log.h"

#include "chre/platform/slpi/printheap.h"

extern "C" void printHeap(void)
{
  #define NALLOCS 500
  #define SALLOCS 2048
  /*{
    int n = 0xFFFFFFFF;
    void* allocs[NALLOCS];
    {
      int i;
      memset(allocs, 0, sizeof(allocs));
      for (i = 0; i < NALLOCS; i++) {
        allocs[i] = chre::memoryAlloc(SALLOCS);
        if (0 == allocs[i]) {
          break;
        }
      }
      n = i;
      for (i = 0; i < NALLOCS; i++) {
        if (allocs[i]) {
          chre::memoryFree(allocs[i]);
          allocs[i] = 0;
        }
      }
    }
    LOGE("-- %d chre allocs of %d bytes, total free %d B", n, SALLOCS, n * SALLOCS);
  }*/

  {
    int n = 0xFFFFFFFF;
    void* allocs[NALLOCS];
    {
      int i;
      memset(allocs, 0, sizeof(allocs));
      for (i = 0; i < NALLOCS; i++) {
        allocs[i] = malloc(SALLOCS);
        if (0 == allocs[i]) {
          break;
        }
      }
      n = i;
      for (i = 0; i < NALLOCS; i++) {
        if (allocs[i]) {
          free(allocs[i]);
          allocs[i] = 0;
        }
      }
    }
    LOGE("-- %d amss allocs of %d bytes, total free %d B", n, SALLOCS, n * SALLOCS);
  }
}

