/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <iostream>
#include <thread>
#ifdef __ANDROID__
#include <private/android_filesystem_config.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>
#endif

#include "framework/Util.h"

void threadSleep(int seconds) {
  std::chrono::seconds sec(seconds);
  std::this_thread::sleep_for(sec);
}

void setThreadName(const char *threadName) {
  ignore(threadName);
#ifdef __ANDROID__
  const char *s = threadName;
  prctl(PR_SET_NAME, (unsigned long)s, 0, 0, 0);
#endif
}
