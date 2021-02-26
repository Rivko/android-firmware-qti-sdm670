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

#ifndef CHRE_PLATFORM_PLATFORM_QC_BLE_H_
#define CHRE_PLATFORM_PLATFORM_QC_BLE_H_

#include "chre/target_platform/platform_qc_ble_base.h"

namespace chre {

class PlatformQcBle : public PlatformQcBleBase {
 public:
  /**
   * Performs platform-specific deinitialization of the PlatformQcBle instance.
   */
  ~PlatformQcBle();

  /**
   * Initializes the platform-specific QcBle implementation. This is potentially
   * called at a later stage of initialization than the constructor, so platform
   * implementations are encouraged to put any blocking initialization here.
   */
  void init();

  /**
   * Returns the set of QcBle capabilities that the platform has exposed. This
   * may return CHRE_QcBle_CAPABILITIES_NONE if QcBle is not supported.
   *
   * @return the QcBle capabilities exposed by this platform.
   */
  uint32_t getCapabilities();

  /**
   * Configures the scan monitoring function of the platform QcBle. For more info
   * see the QcBle PAL documentation. The result of this operation is
   * asynchronous and must be delivered to CHRE by invoking the
   * QcBleRequestManager::handleScanMonitorStateChange method.
   *
   * @param enable true to enable listening for scan results.
   * @return true to indicate that the request was accepted.
   */
  bool configureScanMonitor(bool enable);

  /**
   * Releases a previously published QcBle scan event. Refer to the
   * {@link chrePalQcBleApi} struct of the CHRE API for further documentation.
   *
   * @param event A pointer to an event to be released.
   */
  void releaseScanEvent(struct chreQcBleScanEvent *event);
};

}  // namespace chre

#endif  // CHRE_PLATFORM_PLATFORM_QC_BLE_H_
