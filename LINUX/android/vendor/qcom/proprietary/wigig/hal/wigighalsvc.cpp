/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/StrongPointer.h>
#include "SuppTunnelProvider.h"

#define LOG_TAG "wigighalsvc"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

using vendor::qti::hardware::wigig::supptunnel::V1_0::ISuppTunnelProvider;
using vendor::qti::hardware::wigig::supptunnel::V1_0::implementation::SuppTunnelProvider;

int main(int /*argc*/, char** argv) {
    android::base::InitLogging(argv,
                               android::base::LogdLogger(android::base::SYSTEM));

    LOG(INFO) << "wigighalsvc is starting up...";

    configureRpcThreadpool(1, true /* callerWillJoin */);

    // setup and register the supplicant tunnel service
    sp<ISuppTunnelProvider> service = new SuppTunnelProvider();

    if (service->registerAsService("default") != android::NO_ERROR) {
        LOG(ERROR) << "Failed to register supplicant tunnel service";
        return -1;
    }

    joinRpcThreadpool();

    LOG(INFO) << "wigighalsvc is terminating...";
    return 0;
}
