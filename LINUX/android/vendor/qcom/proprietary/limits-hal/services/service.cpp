/******************************************************************************
  @file  service.cpp
  @brief vendor.qti.hardware.limits@1.0 service

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
******************************************************************************/

#define LOG_TAG "Limits-SVC"
#include <cutils/log.h>
#include <hidl/HidlTransportSupport.h>
#include <hidl/LegacySupport.h>
#include <vendor/qti/hardware/limits/1.0/IThermal.h>

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::registerPassthroughServiceImplementation;
using android::OK;

using vendor::qti::hardware::limits::V1_0::IThermal;

int main()
{
    android::status_t status;
    ALOGI("vendor::qti::hardware::limits svc is starting up...\n");

    configureRpcThreadpool(1, true /* callerWillJoin */);

    status = registerPassthroughServiceImplementation<IThermal>();
    LOG_ALWAYS_FATAL_IF(status != OK,
        "Error while registering QTI IThermal service: %d", status);

    joinRpcThreadpool();

    ALOGI("vendor::qti::hardware::limits svc is terminating...\n");
    return status;
}
