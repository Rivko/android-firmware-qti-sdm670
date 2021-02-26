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

#include "chre/core/sensor_request_manager.h"

#include "chre/core/event_loop_manager.h"
#include "chre/platform/fatal_error.h"
#include "chre_api/chre/version.h"
#include "chre/util/system/debug_dump.h"

namespace chre {
namespace {

bool isSensorRequestValid(const Sensor& sensor,
                          const SensorRequest& sensorRequest) {
  bool isRequestContinuous = sensorModeIsContinuous(
      sensorRequest.getMode());
  bool isRequestOneShot = sensorModeIsOneShot(sensorRequest.getMode());
  uint64_t requestedInterval = sensorRequest.getInterval().toRawNanoseconds();
  SensorType sensorType = sensor.getSensorType();

  bool success = true;
  if (requestedInterval < sensor.getMinInterval()) {
    success = false;
    LOGE("Requested interval %" PRIu64 " < sensor's minInterval %" PRIu64,
         requestedInterval, sensor.getMinInterval());
  } else if (isRequestContinuous) {
    if (sensorTypeIsOneShot(sensorType)) {
      success = false;
      LOGE("Invalid continuous request for a one-shot sensor.");
    }
  } else if (isRequestOneShot) {
    if (!sensorTypeIsOneShot(sensorType)) {
      success = false;
      LOGE("Invalid one-shot request for a continuous sensor.");
    }
  }
  return success;
}

}  // namespace

SensorRequestManager::SensorRequestManager() {
  mSensorRequests.resize(mSensorRequests.capacity());

  DynamicVector<Sensor> sensors;
  sensors.reserve(8);  // Avoid some initial reallocation churn
  if (!PlatformSensor::getSensors(&sensors)) {
    LOGE("Failed to query the platform for sensors");
  } else if (sensors.empty()) {
    LOGW("Platform returned zero sensors");
  } else {
    for (size_t i = 0; i < sensors.size(); i++) {
      SensorType sensorType = sensors[i].getSensorType();
      size_t sensorIndex = getSensorTypeArrayIndex(sensorType);

      if (sensorType == SensorType::Unknown) {
        LOGE("Invalid sensor type");
      } else if (sensors[i].getMinInterval() == 0) {
        LOGE("Invalid sensor minInterval: %s", getSensorTypeName(sensorType));
      } else {
        mSensorRequests[sensorIndex].sensor = std::move(sensors[i]);
        LOGD("Found sensor: %s", getSensorTypeName(sensorType));
      }
    }
  }
}

SensorRequestManager::~SensorRequestManager() {
  SensorRequest nullRequest = SensorRequest();
  for (size_t i = 0; i < mSensorRequests.size(); i++) {
    // Disable sensors that have been enabled previously.
    if (mSensorRequests[i].sensor.has_value()) {
      mSensorRequests[i].sensor->setRequest(nullRequest);
    }
  }
}

bool SensorRequestManager::getSensorHandle(SensorType sensorType,
                                           uint32_t *sensorHandle) const {
  CHRE_ASSERT(sensorHandle);

  bool sensorHandleIsValid = false;
  if (sensorType == SensorType::Unknown) {
    LOGW("Querying for unknown sensor type");
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    sensorHandleIsValid = mSensorRequests[sensorIndex].sensor.has_value();
    if (sensorHandleIsValid) {
      *sensorHandle = getSensorHandleFromSensorType(sensorType);
    }
  }

  return sensorHandleIsValid;
}

bool SensorRequestManager::setSensorRequest(Nanoapp *nanoapp,
    uint32_t sensorHandle, const SensorRequest& sensorRequest) {
  CHRE_ASSERT(nanoapp);

  // Validate the input to ensure that a valid handle has been provided.
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to configure an invalid sensor handle");
    return false;
  }

  // Ensure that the runtime is aware of this sensor type.
  size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
  SensorRequests& requests = mSensorRequests[sensorIndex];
  if (!requests.sensor.has_value()) {
    LOGW("Attempting to configure non-existent sensor");
    return false;
  }

  const Sensor& sensor = requests.sensor.value();
  if (!isSensorRequestValid(sensor, sensorRequest)) {
    return false;
  }

  size_t requestIndex;
  uint16_t eventType = getSampleEventTypeForSensorType(sensorType);
  bool nanoappHasRequest = (requests.find(nanoapp, &requestIndex) != nullptr);

  bool success;
  bool requestChanged;
  if (sensorRequest.getMode() == SensorMode::Off) {
    if (nanoappHasRequest) {
      // The request changes the mode to off and there was an existing request.
      // The existing request is removed from the multiplexer. The nanoapp is
      // unregistered from events of this type if this request was successful.
      success = requests.remove(requestIndex, &requestChanged);
      if (success) {
        nanoapp->unregisterForBroadcastEvent(eventType);
      }
    } else {
      // The sensor is being configured to Off, but is already Off (there is no
      // existing request). We assign to success to be true and no other
      // operation is required.
      requestChanged = false;
      success = true;
    }
  } else if (!nanoappHasRequest) {
    // The request changes the mode to the enabled state and there was no
    // existing request. The request is newly created and added to the
    // multiplexer. The nanoapp is registered for events if this request was
    // successful.
    success = requests.add(sensorRequest, &requestChanged);
    if (success) {
      nanoapp->registerForBroadcastEvent(eventType);

      // Deliver last valid event to new clients of on-change sensors
      if (sensorTypeIsOnChange(sensor.getSensorType())
          && sensor.getLastEvent() != nullptr) {
        EventLoopManagerSingleton::get()->getEventLoop()
            .postEvent(getSampleEventTypeForSensorType(sensorType),
                       sensor.getLastEvent(), nullptr, kSystemInstanceId,
                       nanoapp->getInstanceId());
      }
    }
  } else {
    // The request changes the mode to the enabled state and there was an
    // existing request. The existing request is updated.
    success = requests.update(requestIndex, sensorRequest, &requestChanged);
  }

  if (requestChanged) {
    // TODO: Send an event to nanoapps to indicate the rate change.
  }

  return success;
}

bool SensorRequestManager::getSensorInfo(uint32_t sensorHandle,
                                         const Nanoapp& nanoapp,
                                         struct chreSensorInfo *info) const {
  CHRE_ASSERT(info);

  bool success = false;

  // Validate the input to ensure that a valid handle has been provided.
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to access sensor with an invalid handle %" PRIu32,
         sensorHandle);
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (!mSensorRequests[sensorIndex].sensor.has_value()) {
      LOGW("Attempting to get sensor info for unsupported sensor handle %"
           PRIu32, sensorHandle);
    } else {
      // Platform-independent properties.
      info->sensorType = getUnsignedIntFromSensorType(sensorType);
      info->isOnChange = sensorTypeIsOnChange(sensorType);
      info->isOneShot  = sensorTypeIsOneShot(sensorType);
      info->unusedFlags = 0;

      // Platform-specific properties.
      const Sensor& sensor = mSensorRequests[sensorIndex].sensor.value();
      info->sensorName = sensor.getSensorName();

      // minInterval was added in CHRE API v1.1 - do not attempt to populate for
      // nanoapps targeting v1.0 as their struct will not be large enough
      if (nanoapp.getTargetApiVersion() >= CHRE_API_VERSION_1_1) {
        info->minInterval = sensor.getMinInterval();
      }

      success = true;
    }
  }

  return success;
}

bool SensorRequestManager::removeAllRequests(SensorType sensorType) {
  bool success = false;
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to remove all requests of an invalid sensor type");
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    SensorRequests& requests = mSensorRequests[sensorIndex];
    uint16_t eventType = getSampleEventTypeForSensorType(sensorType);

    for (const SensorRequest& request : requests.multiplexer.getRequests()) {
      Nanoapp *nanoapp = request.getNanoapp();
      nanoapp->unregisterForBroadcastEvent(eventType);
    }

    success = requests.removeAll();
  }
  return success;
}

Sensor *SensorRequestManager::getSensor(SensorType sensorType) {
  Sensor *sensorPtr = nullptr;
  if (sensorType == SensorType::Unknown
      || sensorType >= SensorType::SENSOR_TYPE_COUNT) {
    LOGW("Attempting to get Sensor of an invalid SensorType %d",
         static_cast<int>(sensorType));
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (mSensorRequests[sensorIndex].sensor.has_value()) {
      sensorPtr = &mSensorRequests[sensorIndex].sensor.value();
    }
  }
  return sensorPtr;
}

bool SensorRequestManager::getSensorSamplingStatus(
    uint32_t sensorHandle, struct chreSensorSamplingStatus *status) const {
  CHRE_ASSERT(status);

  bool success = false;
  SensorType sensorType = getSensorTypeFromSensorHandle(sensorHandle);
  if (sensorType == SensorType::Unknown) {
    LOGW("Attempting to access sensor with an invalid handle %" PRIu32,
         sensorHandle);
  } else {
    size_t sensorIndex = getSensorTypeArrayIndex(sensorType);
    if (mSensorRequests[sensorIndex].sensor.has_value()) {
      success = mSensorRequests[sensorIndex].sensor->getSamplingStatus(status);
    }
  }
  return success;
}

const DynamicVector<SensorRequest>& SensorRequestManager::getRequests(
    SensorType sensorType) const {
  size_t sensorIndex = 0;
  if (sensorType == SensorType::Unknown
      || sensorType >= SensorType::SENSOR_TYPE_COUNT) {
    LOGW("Attempting to get requests of an invalid SensorType");
  } else {
    sensorIndex = getSensorTypeArrayIndex(sensorType);
  }
  return mSensorRequests[sensorIndex].multiplexer.getRequests();
}

bool SensorRequestManager::logStateToBuffer(char *buffer, size_t *bufferPos,
                                            size_t bufferSize) const {
  bool success = debugDumpPrint(buffer, bufferPos, bufferSize, "\nSensors:\n");
  for (uint8_t i = 0; i < static_cast<uint8_t>(SensorType::SENSOR_TYPE_COUNT);
       i++) {
    SensorType sensor = static_cast<SensorType>(i);
    if (sensor != SensorType::Unknown) {
      for (const auto& request : getRequests(sensor)) {
        uint32_t instanceId = (request.getNanoapp() != nullptr) ?
            request.getNanoapp()->getInstanceId() : kInvalidInstanceId;
        success &= debugDumpPrint(buffer, bufferPos, bufferSize, " %s: mode=%d"
                                  " interval(ns)=%" PRIu64 " latency(ns)=%"
                                  PRIu64 " nanoappId=%" PRIu32 "\n",
                                  getSensorTypeName(sensor), request.getMode(),
                                  request.getInterval().toRawNanoseconds(),
                                  request.getLatency().toRawNanoseconds(),
                                  instanceId);
      }
    }
  }

  return success;
}

const SensorRequest *SensorRequestManager::SensorRequests::find(
    const Nanoapp *nanoapp, size_t *index) const {
  CHRE_ASSERT(index);

  const auto& requests = multiplexer.getRequests();
  for (size_t i = 0; i < requests.size(); i++) {
    const SensorRequest& sensorRequest = requests[i];
    if (sensorRequest.getNanoapp() == nanoapp) {
      *index = i;
      return &sensorRequest;
    }
  }

  return nullptr;
}

bool SensorRequestManager::SensorRequests::add(const SensorRequest& request,
                                               bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(sensor.has_value());

  size_t addIndex;
  bool success = true;
  if (!multiplexer.addRequest(request, &addIndex, requestChanged)) {
    *requestChanged = false;
    success = false;
    LOG_OOM();
  } else if (*requestChanged) {
    success = sensor->setRequest(multiplexer.getCurrentMaximalRequest());
    if (!success) {
      // Remove the newly added request since the platform failed to handle it.
      // The sensor is expected to maintain the existing request so there is no
      // need to reset the platform to the last maximal request.
      multiplexer.removeRequest(addIndex, requestChanged);

      // This is a roll-back operation so the maximal change in the multiplexer
      // must not have changed. The request changed state is forced to false.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::remove(size_t removeIndex,
                                                  bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(sensor.has_value());

  bool success = true;
  multiplexer.removeRequest(removeIndex, requestChanged);
  if (*requestChanged) {
    success = sensor->setRequest(multiplexer.getCurrentMaximalRequest());
    if (!success) {
      LOGE("SensorRequestManager failed to remove a request");

      // If the platform fails to handle this request in a debug build there is
      // likely an error in the platform. This is not strictly a programming
      // error but it does make sense to use assert semantics when a platform
      // fails to handle a request that it had been sent previously.
      CHRE_ASSERT(false);

      // The request to the platform to set a request when removing has failed
      // so the request has not changed.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::update(size_t updateIndex,
                                                  const SensorRequest& request,
                                                  bool *requestChanged) {
  CHRE_ASSERT(requestChanged != nullptr);
  CHRE_ASSERT(sensor.has_value());

  bool success = true;
  SensorRequest previousRequest = multiplexer.getRequests()[updateIndex];
  multiplexer.updateRequest(updateIndex, request, requestChanged);
  if (*requestChanged) {
    success = sensor->setRequest(multiplexer.getCurrentMaximalRequest());
    if (!success) {
      // Roll back the request since sending it to the sensor failed. The
      // request will roll back to the previous maximal. The sensor is
      // expected to maintain the existing request if a request fails so there
      // is no need to reset the platform to the last maximal request.
      multiplexer.updateRequest(updateIndex, previousRequest, requestChanged);

      // This is a roll-back operation so the maximal change in the multiplexer
      // must not have changed. The request changed state is forced to false.
      *requestChanged = false;
    }
  }

  return success;
}

bool SensorRequestManager::SensorRequests::removeAll() {
  CHRE_ASSERT(sensor.has_value());

  bool requestChanged;
  multiplexer.removeAllRequests(&requestChanged);

  bool success = true;
  if (requestChanged) {
    SensorRequest maximalRequest = multiplexer.getCurrentMaximalRequest();
    success = sensor->setRequest(maximalRequest);
    if (!success) {
      LOGE("SensorRequestManager failed to remove all request");

      // If the platform fails to handle this request in a debug build there is
      // likely an error in the platform. This is not strictly a programming
      // error but it does make sense to use assert semantics when a platform
      // fails to handle a request that it had been sent previously.
      CHRE_ASSERT(false);
    }
  }
  return success;
}

}  // namespace chre
