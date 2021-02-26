/******************************************************************************
 *   @file    PerfHintHelper.h
 *   @brief   Perf Hint helper class to help calling into perf hal
 *
 *   DESCRIPTION
 *      PerfHintHelper is a helper class which provides easy calling of
 *   perf-hal's perf_hint API.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 ********************************************************************************/

#ifndef PERFHINTHELPER_
#define PERFHINTHELPER_
#include <string>

class PerfHintHelper {
private:
    void* mLibHandle;
    /*
     * Perf Hint call signature:
     * int perf_hint(int hint, const char *appName, int duration, int type)
     * hint = HintID
     * pkg = App Name
     * duration = duration of hint
     * type = Hint Type
     */
    int (*mPerfHint)(int hint, const char* appName, int duration, int type);
public:
    PerfHintHelper();
    ~PerfHintHelper();
    void perfHint(int hintId, std::string appName = {}, int duration = 0, int hintType = -1);
};

#endif /* PERFHINTHELPER_ */
