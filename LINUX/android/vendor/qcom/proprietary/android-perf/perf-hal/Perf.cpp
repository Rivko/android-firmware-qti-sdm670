/******************************************************************************
  @file    Perf.cpp
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <dlfcn.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <hidl/HidlTransportSupport.h>
#include "Perf.h"
#include "mp-ctl.h"
#include <cutils/properties.h>

namespace vendor {
namespace qti {
namespace hardware {
namespace perf {
namespace V1_0 {
namespace implementation {

// Methods from ::vendor::qti::hardware::perf::V1_0::IPerf follow.
Return<int32_t> Perf::perfLockAcquire(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, int32_t reserved) {
    mpctl_msg_t pMsg;
    uint32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKACQ;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;
    uint32_t size  = boostsList.size();
    if (size > MAX_ARGS_PER_REQUEST) {
       ALOGE("Maximum number of arguments alowed exceeded");
       return retVal;
    }
    std::copy(boostsList.begin(), boostsList.end(), pMsg.pl_args);
    pMsg.data = size;
    pMsg.pl_time = duration;
    pMsg.pl_handle = pl_handle;

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<void> Perf::perfLockRelease(int32_t pl_handle, int32_t reserved) {
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKREL;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;
    pMsg.pl_handle = pl_handle;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return Void();
}

Return<int32_t> Perf::perfHint(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, int32_t reserved) {
    mpctl_msg_t pMsg;
    uint32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCKHINTACQ;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;
    pMsg.hint_id = hint;
    pMsg.pl_time = userData1;
    pMsg.hint_type = userData2;
    strlcpy(pMsg.usrdata_str, userDataStr.c_str(), MAX_MSG_APP_NAME_LEN);

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<int32_t> Perf::perfProfile(int32_t pl_handle, int32_t profile, int32_t reserved) {
    mpctl_msg_t pMsg;
    uint32_t retVal = -1;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = MPCTL_CMD_PERFLOCK_PROFILE;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;
    pMsg.pl_handle = pl_handle;
    pMsg.profile = profile;

    retVal = mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return retVal;
}

Return<void> Perf::perfLockCmd(int32_t cmd, int32_t reserved) {
    mpctl_msg_t pMsg;
    memset(&pMsg, 0, sizeof(mpctl_msg_t));

    pMsg.req_type = cmd;
    pMsg.client_pid = android::hardware::IPCThreadState::self()->getCallingPid();
    pMsg.client_tid = reserved;

    mImpl.PerfGlueLayerSubmitRequest(&pMsg);
    return Void();
}

Perf::Perf() {
    Init();
}

Perf::~Perf() {
    mImpl.PerfGlueLayerExit();
}

void Perf::Init() {
    //register mp-ctl
    mImpl.LoadPerfLib("libqti-perfd.so");

    //register gamed
    if (property_get_bool("debug.vendor.qti.enable.gamed", false)) {
        mImpl.LoadPerfLib("libqti-gt-prop.so");
    }

#ifdef TARGET_USES_LM
    bool enableLM = property_get_bool("debug.vendor.qti.enable.lm", false);
    if(enableLM) {
        mImpl.LoadPerfLib("liblearningmodule.so");
    }
    bool enableMemperfd = property_get_bool("debug.vendor.qti.enable.memperfd", false);
    if(enableLM && enableMemperfd) {
        mImpl.LoadPerfLib("libmemperfd.so");
    }
#endif

    //init all modules
    mImpl.PerfGlueLayerInit();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace perf
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
