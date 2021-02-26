/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chre/platform/platform_qc_ble.h"

#include <cinttypes>

#include "chre/core/event_loop_manager.h"
#include "chre/platform/shared/pal_system_api.h"
#include "chre/platform/log.h"

namespace chre {

PlatformQcBle::~PlatformQcBle() {
  if (mQcBleApi != nullptr) {
    LOGD("Platform QC BLE closing");
    prePalApiCall();
    mQcBleApi->close();
    LOGD("Platform QC BLE closed");
  }
}

void PlatformQcBle::init() {
  prePalApiCall();
  mQcBleApi = chrePalQcBleGetApi(CHRE_PAL_QC_BLE_API_CURRENT_VERSION);
  if (mQcBleApi != nullptr) {
    mQcBleCallbacks.scanMonitorStatusChangeCallback =
        PlatformQcBle::scanMonitorStatusChangeCallback;
    mQcBleCallbacks.scanEventCallback =
        PlatformQcBleBase::scanEventCallback;
    if (!mQcBleApi->open(&gChrePalSystemApi, &mQcBleCallbacks)) {
      LOGE("QC BLE PAL open returned false");
      mQcBleApi = nullptr;
    }
  } else {
    LOGW("Requested QC BLE PAL (version %08" PRIx32 ") not found",
         CHRE_PAL_QC_BLE_API_CURRENT_VERSION);
  }
}

uint32_t PlatformQcBle::getCapabilities() {
  if (mQcBleApi != nullptr) {
    prePalApiCall();
    return mQcBleApi->getCapabilities();
  } else {
    return CHRE_QC_BLE_CAPABILITIES_NONE;
  }
}

bool PlatformQcBle::configureScanMonitor(bool enable) {
  if (mQcBleApi != nullptr) {
    prePalApiCall();
    return mQcBleApi->configureScanMonitor(enable);
  } else {
    return false;
  }
}

void PlatformQcBle::releaseScanEvent(struct chreQcBleScanEvent *event) {
  if (mQcBleApi != nullptr) {
    prePalApiCall();
    mQcBleApi->releaseScanEvent(event);
  }
}

void PlatformQcBleBase::scanMonitorStatusChangeCallback(bool enabled,
                                                       uint8_t errorCode) {
  EventLoopManagerSingleton::get()->getQcBleRequestManager()
      .handleScanMonitorStateChange(enabled, errorCode);
}

void PlatformQcBleBase::scanEventCallback(struct chreQcBleScanEvent *event) {
  EventLoopManagerSingleton::get()->getQcBleRequestManager()
      .handleScanEvent(event);
}

}  // namespace chre
