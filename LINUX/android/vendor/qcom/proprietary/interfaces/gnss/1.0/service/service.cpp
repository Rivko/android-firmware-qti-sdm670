/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "LocSvc_HIDL_vendor.qti.gnss@1.0-service"

#include <android/hardware/gnss/1.0/IGnss.h>
#include <vendor/qti/gnss/1.0/ILocHidlGnss.h>

#include <hidl/LegacySupport.h>

using android::hardware::gnss::V1_0::IGnss;
using android::hardware::configureRpcThreadpool;
using android::hardware::registerPassthroughServiceImplementation;
using android::hardware::joinRpcThreadpool;

using android::status_t;
using android::OK;

using vendor::qti::gnss::V1_0::ILocHidlGnss;

int main(int /* argc */, char* /* argv */ []) {

    ALOGI("%s", __FUNCTION__);

    configureRpcThreadpool(1, true);
    status_t status;

    status = registerPassthroughServiceImplementation<IGnss>();
    // LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering gnss hal service: %d", status);
    if (status != OK) {
        ALOGE("Error while registering gnss hal service: %d", status);
        return -1;
    }

    status = registerPassthroughServiceImplementation<ILocHidlGnss>("gnss_vendor");
    // LOG_ALWAYS_FATAL_IF(status != OK, "Error while registering gnss vendor hal service: %d", status);
    if (status != OK) {
        ALOGE("Error while registering gnss vendor hal service: %d", status);
        return -1;
    }

    joinRpcThreadpool();
    return 0;
}
