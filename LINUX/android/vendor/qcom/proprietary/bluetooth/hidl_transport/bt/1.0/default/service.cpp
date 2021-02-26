//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#define LOG_TAG "android.hardware.bluetooth@1.0-service"

#include <android/hardware/bluetooth/1.0/IBluetoothHci.h>
#include <hidl/LegacySupport.h>
#ifdef QCOM_FM_SUPPORTED
#include <vendor/qti/hardware/fm/1.0/IFmHci.h>
#endif
#ifdef QCOM_ANT_SUPPORTED
#include <com/qualcomm/qti/ant/1.0/IAntHci.h>
#endif
// Generated HIDL files
using android::hardware::bluetooth::V1_0::IBluetoothHci;

#ifdef QCOM_FM_SUPPORTED
using vendor::qti::hardware::fm::V1_0::IFmHci;
#endif

#ifdef QCOM_ANT_SUPPORTED
using com::qualcomm::qti::ant::V1_0::IAntHci;
#endif

using android::hardware::defaultPassthroughServiceImplementation;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::registerPassthroughServiceImplementation;
using android::OK;

int main() {
    configureRpcThreadpool(1, true /*callerWillJoin*/);
    ALOGI("registering BT & FM services");

    android::status_t status;

    status = registerPassthroughServiceImplementation<IBluetoothHci>();

    if (status != OK)
        ALOGI("Error while registering BT service: %d", status);

#ifdef QCOM_FM_SUPPORTED
    status = registerPassthroughServiceImplementation<IFmHci>();

    if (status != OK)
        ALOGI("Error while registering FM service: %d", status);
#endif

#ifdef QCOM_ANT_SUPPORTED
   status = registerPassthroughServiceImplementation<IAntHci>();
   if (status != OK)
      ALOGI("Error while registering Ant service: %d", status);
#endif

    joinRpcThreadpool();

    return status;
}
