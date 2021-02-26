/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/StrongPointer.h>
#include "CommandListener.h"
#include "NetPerfTuner.h"
#include "NetworkPerformanceTuner.h"

#define LOG_TAG "wigignpt"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;

using vendor::qti::hardware::wigig::netperftuner::V1_0::INetPerfTuner;
using vendor::qti::hardware::wigig::netperftuner::V1_0::implementation::NetPerfTuner;

int main(int /*argc*/, char** argv) {
    android::base::InitLogging(argv,
                               android::base::LogdLogger(android::base::SYSTEM));

    LOG(INFO) << "wigignpt is starting up...";

    configureRpcThreadpool(1, true /* callerWillJoin */);

    // setup and register the network performance tuner service
    sp<NetworkPerformanceTuner> npt = NetworkPerformanceTuner::getInstance();
    CommandListener commandListener(npt);
    sp<INetPerfTuner> service = new NetPerfTuner(&commandListener);

    if (service->registerAsService("default") != android::NO_ERROR) {
        LOG(ERROR) << "Failed to register network performance tuner service";
        return -1;
    }

    if (commandListener.startListener()) {
        LOG(ERROR) << "Unable to start CommandListener: " << strerror(errno);
        return -1;
    }

    joinRpcThreadpool();

    LOG(INFO) << "wigignpt is terminating...";
    return 0;
}
