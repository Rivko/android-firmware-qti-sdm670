/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include "chre_api/chre/wifi.h"

#include "chre/core/event_loop_manager.h"
#include "chre/util/macros.h"

using chre::EventLoopManager;
using chre::EventLoopManagerSingleton;

DLL_EXPORT uint32_t chreWifiGetCapabilities() {
  return chre::EventLoopManagerSingleton::get()->getWifiRequestManager()
      .getCapabilities();
}

DLL_EXPORT bool chreWifiConfigureScanMonitorAsync(bool enable,
                                                  const void *cookie) {
  chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);
  return EventLoopManagerSingleton::get()->getWifiRequestManager()
      .configureScanMonitor(nanoapp, enable, cookie);
}

DLL_EXPORT bool chreWifiRequestScanAsync(
    const struct chreWifiScanParams *params, const void *cookie) {
  chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);
  return (params == nullptr) ? false :
      EventLoopManagerSingleton::get()->getWifiRequestManager()
      .requestScan(nanoapp, params, cookie);
}

DLL_EXPORT bool chreWifiRequestRangingAsync(
    const struct chreWifiRangingParams *params, const void *cookie) {
  chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);
  return (params == nullptr) ? false :
      EventLoopManagerSingleton::get()->getWifiRequestManager()
          .requestRanging(nanoapp, params, cookie);
}
