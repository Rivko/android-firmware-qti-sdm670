/******************************************************************************
  @file    client.cpp
  @brief   Android performance PerfLock library

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2014,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <shared_mutex>

#define LOG_TAG         "ANDR-PERF"
#include <cutils/log.h>
#include <cutils/properties.h>

#include "client.h"
#include "mp-ctl.h"
#include <vendor/qti/hardware/perf/1.0/IPerf.h>

#define FAILED                  -1
#define SUCCESS                 0

using android::hardware::Return;
using android::hardware::Void;
using android::hardware::hidl_death_recipient;
using android::hardware::hidl_vec;
using android::hardware::hidl_string;
using android::hidl::base::V1_0::IBase;
using vendor::qti::hardware::perf::V1_0::IPerf;
using ::android::sp;
using ::android::wp;
using std::unique_lock;
using std::shared_lock;
using std::shared_timed_mutex;

static sp<IPerf> gPerfHal = NULL;
static shared_timed_mutex gPerf_lock;

struct PerfDeathRecipient : virtual public hidl_death_recipient
{
    virtual void serviceDied(uint64_t /*cookie*/, const wp<IBase>& /*who*/) override {
        unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
        gPerfHal = NULL;
        ALOGE("IPerf serviceDied");
    }
};
static sp<PerfDeathRecipient> perfDeathRecipient = NULL;

static void getPerfServiceAndLinkToDeath() {
    shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
    if (gPerfHal == NULL) {
        read_lock.unlock();
        {
            unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
            if (gPerfHal == NULL) {
                gPerfHal = IPerf::tryGetService();
                if (gPerfHal != NULL) {
                    perfDeathRecipient = new PerfDeathRecipient();
                    android::hardware::Return<bool> linked = gPerfHal->linkToDeath(perfDeathRecipient, 0);
                    if (!linked || !linked.isOk()) {
                        gPerfHal = NULL;
                        ALOGE("Unable to link to gPerfHal death notifications!");
                    }
                } else {
                    ALOGE("IPerf::tryGetService failed!");
                }
            }
        }
    }
}

static int processIntReturn(const Return<int32_t> &intReturn, const char* funcName) {
    int ret = -1;
    if (!intReturn.isOk()) {
        unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
        gPerfHal = NULL;
        ALOGE("%s() failed: perf HAL service not available.", funcName);
    } else {
        ret = intReturn;
    }
    return ret;
}

static void processVoidReturn(const Return<void> &voidReturn, const char* funcName) {
    if (!voidReturn.isOk()) {
        unique_lock<shared_timed_mutex> write_lock(gPerf_lock);
        gPerfHal = NULL;
        ALOGE("%s() failed: perf HAL service not available.", funcName);
    }
}

int perf_lock_acq(int handle, int duration, int list[], int numArgs)
{
    int rc = -1;
    pid_t client_tid = 0;
    std::vector<int32_t> boostsList;

    QLOGI("inside perf_lock_acq of client");
    if (duration < 0)
        return FAILED;

    if (numArgs > OPTIMIZATIONS_MAX) {
        numArgs = OPTIMIZATIONS_MAX;
    }

    client_tid = gettid();
    boostsList.assign(list, (list + numArgs));
    getPerfServiceAndLinkToDeath();
    {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfLockAcquire(handle, duration, boostsList, client_tid);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfLockAcquire");
        }
    }
    return rc;
}

int perf_lock_rel(int handle)
{
    pid_t client_tid = 0;

    QLOGI("inside perf_lock_rel of client");
    if (handle == 0)
        return FAILED;

    client_tid = gettid();
    getPerfServiceAndLinkToDeath();
    {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<void> voidReturn = gPerfHal->perfLockRelease(handle, client_tid);
            read_lock.unlock();
            processVoidReturn(voidReturn, "perfLockRelease");
            return SUCCESS;
        }
    }
    return FAILED;
}

int perf_hint(int hint, const char *pkg, int duration, int type)
{
    int rc = -1;
    pid_t client_tid = 0;

    QLOGI("inside perf_hint of client");
    client_tid = gettid();
    getPerfServiceAndLinkToDeath();
    {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfHint(hint, pkg, duration, type, client_tid);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfHint");
        }
    }
    return rc;
}

int perf_lock_use_profile(int handle, int profile)
{
    int rc = -1;
    pid_t client_tid = 0;

    QLOGI("inside perf_lock_use_profile of client");
    client_tid = gettid();
    getPerfServiceAndLinkToDeath();
    {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<int32_t> intReturn = gPerfHal->perfProfile(handle, profile, client_tid);
            read_lock.unlock();
            rc = processIntReturn(intReturn, "perfProfile");
        }
    }
    return rc;

}

void perf_lock_cmd(int cmd)
{
    pid_t client_tid = 0;

    QLOGI("inside perf_lock_cmd of client");
    client_tid = gettid();
    getPerfServiceAndLinkToDeath();
    {
        shared_lock<shared_timed_mutex> read_lock(gPerf_lock);
        if (gPerfHal != NULL) {
            Return<void> voidReturn = gPerfHal->perfLockCmd(cmd, client_tid);
            read_lock.unlock();
            processVoidReturn(voidReturn, "perfLockCmd");
        }
    }
}
