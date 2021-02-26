/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __IZAT_MANAGER_UTILS_H__
#define __IZAT_MANAGER_UTILS_H__

#include <log_util.h>
#include <base_util/postcard.h>
#include "comdef.h"
#include "climits"

using namespace qc_loc_fw;

#define MAX_THREAD_NAME_LEN 15

typedef void* (*pthread_func) (void*);

int GetTimeSinceBoot(int64 & time_since_bootup);
pthread_t create_thread_callback(const char* threadName, void (*start)(void *), void* arg);
int copyInCard(InPostcard * const inCardIn, InPostcard * const inCardOut);

inline int64 convertNanoSecsToMilliSecs(int64 nsecs) {
    return nsecs / 1000000;
}

inline int64 convertMilliSecsToNanoSecs(int64 msecs) {
    return msecs * 1000000;
}

inline int64 convertNanoSecsToSecs(int64 nsecs) {
    return nsecs / 1000000000;
}

inline int64 convertSecsToNanoSecs(int64 secs) {
    return secs * 1000000000;
}

inline uint32_t capTouint32(uint64 x) { if (x > UINT_MAX) { return UINT_MAX; } return x;}


#endif
