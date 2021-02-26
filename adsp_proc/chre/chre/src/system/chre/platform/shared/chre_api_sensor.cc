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

#include "chre/core/event_loop_manager.h"
#include "chre/core/sensor_request.h"
#include "chre/util/time.h"
#include "chre/util/macros.h"
#include "chre_api/chre/sensor.h"

using chre::EventLoopManager;
using chre::EventLoopManagerSingleton;
using chre::Nanoseconds;
using chre::SensorMode;
using chre::SensorRequest;
using chre::SensorType;

using chre::getSensorModeFromEnum;
using chre::getSensorTypeFromUnsignedInt;

DLL_EXPORT bool chreSensorFindDefault(uint8_t sensorType, uint32_t *handle) {
  SensorType validatedSensorType = getSensorTypeFromUnsignedInt(sensorType);
  return (validatedSensorType != SensorType::Unknown
      && EventLoopManagerSingleton::get()->getSensorRequestManager()
          .getSensorHandle(validatedSensorType, handle));
}

DLL_EXPORT bool chreGetSensorInfo(uint32_t sensorHandle,
                                  struct chreSensorInfo *info) {
  CHRE_ASSERT(info);

  chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);

  bool success = false;
  if (info != nullptr) {
    success = EventLoopManagerSingleton::get()->getSensorRequestManager().
        getSensorInfo(sensorHandle, *nanoapp, info);
  }
  return success;
}

DLL_EXPORT bool chreGetSensorSamplingStatus(
    uint32_t sensorHandle, struct chreSensorSamplingStatus *status) {
  CHRE_ASSERT(status);

  bool success = false;
  if (status != nullptr) {
    success = EventLoopManagerSingleton::get()->getSensorRequestManager().
        getSensorSamplingStatus(sensorHandle, status);
  }
  return success;
}

DLL_EXPORT bool chreSensorConfigure(uint32_t sensorHandle,
                                    enum chreSensorConfigureMode mode,
                                    uint64_t interval, uint64_t latency) {
  chre::Nanoapp *nanoapp = EventLoopManager::validateChreApiCall(__func__);
  SensorMode sensorMode = getSensorModeFromEnum(mode);
  SensorRequest sensorRequest(nanoapp, sensorMode, Nanoseconds(interval),
                              Nanoseconds(latency));
  return EventLoopManagerSingleton::get()->getSensorRequestManager()
      .setSensorRequest(nanoapp, sensorHandle, sensorRequest);
}
