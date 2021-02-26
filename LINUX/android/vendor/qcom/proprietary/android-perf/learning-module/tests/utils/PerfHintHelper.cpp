/******************************************************************************
 *   @file    PerfHintHelper.cpp
 *   @brief   Implementation of perf hint helper
 *
 *   DESCRIPTION
 *      PerfHintHelper is a helper class which implements dynamic loading of
 *   perf client library and provides call to perf_hint API.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 ********************************************************************************/

#include <dlfcn.h>
#include <cutils/properties.h>
#include "DebugLog.h"
#include "PerfHintHelper.h"

#define LOG_TAG_PHH "PerfHint"

#define EXTLIB_PROPERTY_NAME "ro.vendor.extension_library"

PerfHintHelper::PerfHintHelper() {
    char perfClientLibPath[PROPERTY_VALUE_MAX] = {0};

    if(property_get(EXTLIB_PROPERTY_NAME, perfClientLibPath, NULL)) {
        mLibHandle = dlopen(perfClientLibPath, RTLD_NOW);
        if (mLibHandle) {
            mPerfHint = (int (*)(int, const char *, int, int))dlsym(mLibHandle, "perf_hint");
            if (!mPerfHint) {
                DEBUGE(LOG_TAG_PHH,"Unable to get perf_hint function handle.\n");
            }
        } else {
            DEBUGE(LOG_TAG_PHH,"Unable to open %s: %s\n", perfClientLibPath, dlerror());
        }
    }
}

PerfHintHelper::~PerfHintHelper() {
    if (mLibHandle) {
        if (dlclose(mLibHandle)) {
            ALOGE("Error occurred while closing Perf Client library.");
        }
    }
}

void PerfHintHelper::perfHint(int hintId, std::string appName, int hintDuration, int hintType){
    if (mPerfHint) {
        /*
         * Perf Hint call signature:
         * int perf_hint(int hint, const char *pkg, int duration, int type)
         * hint = HintID
         * pkg = App Name
         * duration = duration of hint
         * type = Hint Type
         */
        DEBUGE(LOG_TAG_PHH,"Calling perf_hint hint_id=0x%x\n", hintId);
        mPerfHint(hintId, appName.c_str(), hintDuration, hintType);
    }
}
