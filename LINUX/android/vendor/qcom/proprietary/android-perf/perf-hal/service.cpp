/******************************************************************************
  @file    service.cpp
  @brief   Android performance HAL service

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "vendor.qti.hardware.perf@1.0-service"

#include <vendor/qti/hardware/perf/1.0/IPerf.h>
#include <hidl/LegacySupport.h>
#include "Perf.h"

using vendor::qti::hardware::perf::V1_0::implementation::Perf;
using vendor::qti::hardware::perf::V1_0::IPerf;
using android::hardware::defaultPassthroughServiceImplementation;

using android::sp;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using android::status_t;
using android::OK;

int main() {
    Perf *perfObj = new Perf();

    if (perfObj != NULL) {
        android::sp<IPerf> service = perfObj;

        configureRpcThreadpool(1, true /*callerWillJoin*/);

        if (service->registerAsService() != OK) {
            ALOGE("Cannot register IPerf HAL service");
            return 1;
        }

        ALOGE("Registered IPerf HAL service success!");
        joinRpcThreadpool();
    }
    return 0;
}
