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

#include "chre/platform/platform_sensor.h"

#include "sns_std_sensor.pb.h"
#include "stringl.h"

#include <cmath>

#include "chre_api/chre/sensor.h"
#include "chre/core/event_loop_manager.h"
#include "chre/core/sensor.h"
#include "chre/platform/assert.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/log.h"
#include "chre/platform/shared/platform_sensor_util.h"
#include "chre/platform/slpi/see/see_client.h"
#include "chre/platform/slpi/see/see_helper.h"
#include "chre/platform/system_time.h"

#ifdef CHREX_SENSOR_SUPPORT
#include "chre/extensions/platform/slpi/see/vendor_data_types.h"
#endif  // CHREX_SENSOR_SUPPORT

#ifndef CHRE_SEE_NUM_TEMP_SENSORS
// There are usually more than one 'sensor_temperature' sensors in SEE.
// Define this in the variant-specific makefile to avoid missing sensors in
// sensor discovery.
#error "CHRE_SEE_NUM_TEMP_SENSORS is not defined"
#endif

namespace chre {
namespace {

//! A class that implements SeeHelperCallbackInterface.
class SeeHelperCallback : public SeeHelperCallbackInterface {
  void onSamplingStatusUpdate(
      UniquePtr<SeeHelperCallbackInterface::SamplingStatusData>&& status)
      override;

  void onSensorDataEvent(
      SensorType sensorType, UniquePtr<uint8_t>&& eventData) override;

  void onHostWakeSuspendEvent(bool awake) override;
};

//! A struct to facilitate sensor discovery
struct SuidAttr {
  sns_std_suid suid;
  SeeAttributes attr;
};

//! The list of SEE platform sensor data types that CHRE intends to support.
//! The standardized strings are defined in sns_xxx.proto.
const char *kSeeDataTypes[] = {
  "accel",
  "gyro",
  "mag",
  "pressure",
  "ambient_light",
  "proximity",
  "motion_detect",
  "stationary_detect",
};

/**
 * Obtains the sensor type given the specified data type and whether the sensor
 * is runtime-calibrated or not.
 */
SensorType getSensorTypeFromDataType(const char *dataType, bool calibrated) {
  SensorType sensorType;
  if (strcmp(dataType, "accel") == 0) {
    if (calibrated) {
      sensorType = SensorType::Accelerometer;
    } else {
      sensorType = SensorType::UncalibratedAccelerometer;
    }
  } else if (strcmp(dataType, "gyro") == 0) {
    if (calibrated) {
      sensorType = SensorType::Gyroscope;
    } else {
      sensorType = SensorType::UncalibratedGyroscope;
    }
  } else if (strcmp(dataType, "mag") == 0) {
    if (calibrated) {
      sensorType = SensorType::GeomagneticField;
    } else {
      sensorType = SensorType::UncalibratedGeomagneticField;
    }
  } else if (strcmp(dataType, "pressure") == 0) {
    sensorType = SensorType::Pressure;
  } else if (strcmp(dataType, "ambient_light") == 0) {
    sensorType = SensorType::Light;
  } else if (strcmp(dataType, "proximity") == 0) {
    sensorType = SensorType::Proximity;
  } else if (strcmp(dataType, "motion_detect") == 0) {
    sensorType = SensorType::InstantMotion;
  } else if (strcmp(dataType, "stationary_detect") == 0) {
    sensorType = SensorType::StationaryDetect;
#ifdef CHREX_SENSOR_SUPPORT
  } else if (strcmp(dataType, kVendorDataTypes[0]) == 0) {
    sensorType = SensorType::VendorType0;
#endif  // CHREX_SENSOR_SUPPORT
  } else {
    sensorType = SensorType::Unknown;
  }
  return sensorType;
}

void seeSensorDataEventFree(uint16_t eventType, void *eventData) {
  memoryFree(eventData);

  // Remove all requests if it's a one-shot sensor and only after data has been
  // delivered to all clients.
  SensorType sensorType = getSensorTypeForSampleEventType(eventType);
  if (sensorTypeIsOneShot(sensorType)) {
    EventLoopManagerSingleton::get()->getSensorRequestManager()
        .removeAllRequests(sensorType);
  }
}

/**
 * Posts a CHRE_EVENT_SENSOR_SAMPLING_CHANGE event to the specified Nanoapp.
 *
 * @param instaceId The instance ID of the nanoapp with an open request.
 * @param sensorHandle The handle of the sensor.
 * @param status A reference of the sampling status to be posted.
 */
void postSamplingStatusEvent(uint32_t instanceId, uint32_t sensorHandle,
                             const struct chreSensorSamplingStatus& status) {
  auto *event = memoryAlloc<struct chreSensorSamplingStatusEvent>();
  if (event == nullptr) {
    LOG_OOM();
  } else {
    event->sensorHandle = sensorHandle;
    event->status = status;

    EventLoopManagerSingleton::get()->getEventLoop().postEventOrFree(
        CHRE_EVENT_SENSOR_SAMPLING_CHANGE, event, freeEventDataCallback,
        kSystemInstanceId, instanceId);
  }
}

/**
 * Updates the sampling status.
 */
void updateSamplingStatus(
    const SeeHelperCallbackInterface::SamplingStatusData& update) {
  Sensor *sensor = EventLoopManagerSingleton::get()->getSensorRequestManager()
      .getSensor(update.sensorType);
  struct chreSensorSamplingStatus prevStatus;

  if (sensor != nullptr && !sensorTypeIsOneShot(update.sensorType)
      && sensor->getSamplingStatus(&prevStatus)) {
    struct chreSensorSamplingStatus newStatus = prevStatus;

    if (update.enabledValid) {
      newStatus.enabled = update.status.enabled;
    }
    if (update.intervalValid) {
      newStatus.interval = update.status.interval;
    }
    if (update.latencyValid) {
      newStatus.latency = update.status.latency;
    }

    if (newStatus.enabled != prevStatus.enabled
        || newStatus.interval != prevStatus.interval
        || newStatus.latency != prevStatus.latency) {
      sensor->setSamplingStatus(newStatus);

      // Only post to Nanoapps with an open request.
      uint32_t sensorHandle = getSensorHandleFromSensorType(update.sensorType);
      const DynamicVector<SensorRequest>& requests =
          EventLoopManagerSingleton::get()->getSensorRequestManager()
          .getRequests(update.sensorType);
      for (const auto& req : requests) {
        if (req.getNanoapp() != nullptr) {
          postSamplingStatusEvent(req.getNanoapp()->getInstanceId(),
                                  sensorHandle, newStatus);
        }
      }
    }
  }
}

void SeeHelperCallback::onSamplingStatusUpdate(
    UniquePtr<SeeHelperCallbackInterface::SamplingStatusData>&& status) {
  auto callback = [](uint16_t /* type */, void *data) {
    auto cbData = UniquePtr<SeeHelperCallbackInterface::SamplingStatusData>(
        static_cast<SeeHelperCallbackInterface::SamplingStatusData *>(data));
    updateSamplingStatus(*cbData);
  };

  // Schedule a deferred callback to handle sensor status change in the main
  // thread.
  EventLoopManagerSingleton::get()->deferCallback(
      SystemCallbackType::SensorStatusUpdate, status.release(), callback);
}

void SeeHelperCallback::onSensorDataEvent(
    SensorType sensorType, UniquePtr<uint8_t>&& eventData) {
  // Schedule a deferred callback to update on-change sensor's last event in
  // the main thread.
  if (sensorTypeIsOnChange(sensorType)) {
    updateLastEvent(sensorType, eventData.get());
  }

  uint16_t eventType = getSampleEventTypeForSensorType(sensorType);
  EventLoopManagerSingleton::get()->getEventLoop().postEventOrFree(
      eventType, eventData.get(), seeSensorDataEventFree);
  eventData.release();
}

void SeeHelperCallback::onHostWakeSuspendEvent(bool awake) {
  if (EventLoopManagerSingleton::isInitialized()) {
    EventLoopManagerSingleton::get()->getEventLoop()
        .getPowerControlManager().onHostWakeSuspendEvent(awake);
  }
}

/**
 * Allocates memory and specifies the memory size for an on-change sensor to
 * store its last data event.
 *
 * @param sensorType The sensorType of this sensor.
 * @param eventSize A non-null pointer to indicate the memory size allocated.
 * @return Pointer to the memory allocated.
 */
ChreSensorData *allocateLastEvent(SensorType sensorType, size_t *eventSize) {
  CHRE_ASSERT(eventSize);

  *eventSize = 0;
  ChreSensorData *event = nullptr;
  if (sensorTypeIsOnChange(sensorType)) {
    SensorSampleType sampleType = getSensorSampleTypeFromSensorType(sensorType);
    switch (sampleType) {
      case SensorSampleType::ThreeAxis:
        *eventSize = sizeof(chreSensorThreeAxisData);
        break;
      case SensorSampleType::Float:
        *eventSize = sizeof(chreSensorFloatData);
        break;
      case SensorSampleType::Byte:
        *eventSize = sizeof(chreSensorByteData);
        break;
      case SensorSampleType::Occurrence:
        *eventSize = sizeof(chreSensorOccurrenceData);
        break;
      default:
        CHRE_ASSERT_LOG(false, "Unhandled sample type");
        break;
    }

    event = static_cast<ChreSensorData *>(memoryAlloc(*eventSize));
    if (event == nullptr) {
      *eventSize = 0;
      FATAL_ERROR("Failed to allocate last event memory for SensorType %" PRIu8,
                  static_cast<uint8_t>(sensorType));
    }
  }
  return event;
}

/**
 * Constructs and initializes a sensor, and adds it to the sensor list.
 *
 * @param suid The SUID of the sensor as provided by SEE.
 * @param sensorType The sensor type of the sensor.
 * @param calibrated Whether the sensor is runtime-calibrated or not.
 * @param attr A reference to SeeAttrbutes.
 * @param sensor The sensor list.
 */
void addSensor(SensorType sensorType, const sns_std_suid& suid,
               const SeeAttributes& attr, DynamicVector<Sensor> *sensors) {
  // Concatenate vendor and name with a space in between.
  char sensorName[kSensorNameMaxLen];
  strlcpy(sensorName, attr.vendor, sizeof(sensorName));
  strlcat(sensorName, " ", sizeof(sensorName));
  strlcat(sensorName, attr.name, sizeof(sensorName));

  // Override one-shot sensor's minInterval to default
  uint64_t minInterval = sensorTypeIsOneShot(sensorType) ?
      CHRE_SENSOR_INTERVAL_DEFAULT : static_cast<uint64_t>(
          ceilf(Seconds(1).toRawNanoseconds() / attr.maxSampleRate));

  // Allocates memory for on-change sensor's last event.
  size_t lastEventSize;
  ChreSensorData *lastEvent = allocateLastEvent(sensorType, &lastEventSize);

  // Constructs and initializes PlatformSensorBase.
  Sensor sensor;
  sensor.initBase(sensorType, minInterval, sensorName, lastEvent,
                  lastEventSize, attr.passiveRequest);

  if (!sensors->push_back(std::move(sensor))) {
    FATAL_ERROR("Failed to allocate new sensor: out of memory");
  }

  bool prevRegistered;
  bool registered = getSeeHelper()->registerSensor(
      sensorType, suid, &prevRegistered);
  if (!registered && prevRegistered) {
    LOGW("SUID has been previously registered");
  } else if (!registered) {
    FATAL_ERROR("Failed to register SUID/SensorType mapping.");
  }
}

/**
 * Compare SEE reported stream type attribute to the expected one. Some SEE
 * sensors may support more than one stream type.
 */
bool isStreamTypeCorrect(SensorType sensorType, uint8_t streamType) {
  bool success = true;
  if ((sensorTypeIsContinuous(sensorType)
       && streamType != SNS_STD_SENSOR_STREAM_TYPE_STREAMING)
      || (sensorTypeIsOnChange(sensorType)
          && streamType != SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE)
      || (sensorTypeIsOneShot(sensorType)
          && streamType != SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT)) {
    success = false;
    LOGW("Inconsistent sensor type %" PRIu8 " and stream type %" PRIu8,
         static_cast<uint8_t>(sensorType), streamType);
  }
  return success;
}

/**
 * Obtains the list of SUIDs and their attributes that support the specified
 * data type.
 */
bool getSuidAndAttrs(const char *dataType, DynamicVector<SuidAttr> *suidAttrs,
                     uint8_t minNumSuids) {
  DynamicVector<sns_std_suid> suids;
  bool success = getSeeHelper()->findSuidSync(dataType, &suids, minNumSuids);
  if (!success) {
    LOGE("Failed to find sensor '%s'", dataType);
  } else {
    LOGD("Num of SUIDs found for '%s': %zu", dataType, suids.size());

    for (const auto& suid : suids) {
      SeeAttributes attr;
      if (!getSeeHelper()->getAttributesSync(suid, &attr)) {
        success = false;
        LOGE("Failed to get attributes of SUID 0x%" PRIx64 " %" PRIx64,
             suid.suid_high, suid.suid_low);
      } else {
        LOGI("%s %s, hw id %" PRId64 ", max ODR %f Hz, stream type %" PRIu8
             " passive %d",
             attr.vendor, attr.name, attr.hwId, attr.maxSampleRate,
             attr.streamType, attr.passiveRequest);
        SuidAttr sensor = {
          .suid = suid,
          .attr = attr,
        };
        if (!suidAttrs->push_back(sensor)) {
          success = false;
          LOG_OOM();
        }
      }
    }
  }
  return success;
}

// Check whether two sensors with the specified attrtibutes belong to the same
// sensor hardware module.
bool sensorHwMatch(const SeeAttributes& attr0, const SeeAttributes& attr1) {
  // When HW ID is absent, it's default to 0 and won't be a factor.
  return ((strncmp(attr0.vendor, attr1.vendor, kSeeAttrStrValLen) == 0)
          && (strncmp(attr0.name, attr1.name, kSeeAttrStrValLen) == 0)
          && (attr0.hwId == attr1.hwId));
}

}  // anonymous namespace

PlatformSensor::~PlatformSensor() {
  if (mLastEvent != nullptr) {
    LOGD("Releasing lastEvent: sensor %s, size %zu",
         getSensorTypeName(getSensorType()), mLastEventSize);
    memoryFree(mLastEvent);
  }
}

void PlatformSensor::init() {
  SeeHelperSingleton::init();

  static SeeHelperCallback seeHelperCallback;
  if (!getSeeHelper()->init(&seeHelperCallback)) {
    FATAL_ERROR("Failed to initialize SEE helper");
  }
}

void PlatformSensor::deinit() {
  SeeHelperSingleton::deinit();
}

bool PlatformSensor::getSensors(DynamicVector<Sensor> *sensors) {
  CHRE_ASSERT(sensors);

  DynamicVector<SuidAttr> tempSensors;
  if (!getSuidAndAttrs("sensor_temperature", &tempSensors,
                       CHRE_SEE_NUM_TEMP_SENSORS)) {
      FATAL_ERROR("Failed to get temperature sensor UID and attributes");
  }

#ifndef CHREX_SENSOR_SUPPORT
  const char *kVendorDataTypes[] = {};
#endif  // CHREX_SENSOR_SUPPORT
  constexpr size_t kNumSeeTypes = ARRAY_SIZE(kSeeDataTypes);
  constexpr size_t kNumVendorTypes = ARRAY_SIZE(kVendorDataTypes);
  for (size_t i = 0; i < kNumSeeTypes + kNumVendorTypes; i++) {
    const char *dataType = (i < kNumSeeTypes)
        ? kSeeDataTypes[i] : kVendorDataTypes[i - kNumSeeTypes];

    SensorType sensorType = getSensorTypeFromDataType(
        dataType, false /* calibrated */);
    if (sensorType == SensorType::Unknown) {
      LOGE("Unknown sensor type found for '%s'", dataType);
      continue;
    }

    DynamicVector<SuidAttr> primarySensors;
    if (!getSuidAndAttrs(dataType, &primarySensors, 1 /* minNumSuids */)) {
      FATAL_ERROR("Failed to get primary sensor UID and attributes");
    } else {
      for (const auto& primarySensor : primarySensors) {
        sns_std_suid suid = primarySensor.suid;
        SeeAttributes attr = primarySensor.attr;

        // Some sensors support both continuous and on-change streams.
        // If there are more than one SUIDs that support the data type,
        // choose the first one that has the expected stream type.
        if (isStreamTypeCorrect(sensorType, attr.streamType)) {
          addSensor(sensorType, suid, attr, sensors);

          // Check if this sensor has a runtime-calibrated version.
          SensorType calibratedType = getSensorTypeFromDataType(
              dataType, true /* calibrated */);
          if (calibratedType != sensorType) {
            addSensor(calibratedType, suid, attr, sensors);
          }

          // Check if this sensor has a secondary temperature sensor.
          SensorType temperatureType = getTempSensorType(sensorType);
          if (temperatureType != SensorType::Unknown) {
            bool tempFound = false;
            for (const auto& tempSensor : tempSensors) {
              sns_std_suid tempSuid = tempSensor.suid;
              SeeAttributes tempAttr = tempSensor.attr;

              if (sensorHwMatch(attr, tempAttr)) {
                LOGD("Found matching temperature sensor type");
                tempFound = true;
                addSensor(temperatureType, tempSuid, tempAttr, sensors);
                break;
              }
            }
            if (!tempFound) {
              LOGW("Temperature sensor type %" PRIu8 " not found!",
                   static_cast<uint8_t>(temperatureType));
            }
          }
          break;
        }
      }
    }
  }
  return true;
}

bool PlatformSensor::applyRequest(const SensorRequest& request) {
  SeeSensorRequest req = {
    .sensorType = getSensorType(),
    .enable = (request.getMode() != SensorMode::Off),
    .passive = sensorModeIsPassive(request.getMode()),
    .samplingRateHz = static_cast<float>(
        kOneSecondInNanoseconds / request.getInterval().toRawNanoseconds()),
    // Override batch period to 0 for non-continuous sensors to ensure one
    // sample per batch.
    .batchPeriodUs = !sensorTypeIsContinuous(mSensorType) ? 0
        : static_cast<uint32_t>(request.getLatency().toRawNanoseconds()
                                / kOneMicrosecondInNanoseconds),
  };

  if (req.enable && req.passive && !mPassiveSupported) {
    LOGD("Promoting sensor %" PRIu8 " passive request to active",
         static_cast<uint8_t>(getSensorType()));
  }

  bool success = SeeHelperSingleton::get()->makeRequest(req);

  // TODO: remove setSamplingStatus when .latency is available in status update
  // from SEE.
  if (success) {
    struct chreSensorSamplingStatus status;
    if (getSamplingStatus(&status)) {

      // If passive request is not supported by this SEE sensor, it won't be
      // dynamically enabled/disabled and its status stays the same as set here.
      if (!mPassiveSupported) {
        status.enabled = req.enable;
      }
      status.latency = req.batchPeriodUs * kOneMicrosecondInNanoseconds;
      setSamplingStatus(status);
    }
  }
  return success;
}

SensorType PlatformSensor::getSensorType() const {
  return mSensorType;
}

uint64_t PlatformSensor::getMinInterval() const {
  return mMinInterval;
}

const char *PlatformSensor::getSensorName() const {
  return mSensorName;
}

PlatformSensor::PlatformSensor(PlatformSensor&& other) {
  // Our move assignment operator doesn't assume that "this" is initialized, so
  // we can just use that here.
  *this = std::move(other);
}

PlatformSensor& PlatformSensor::operator=(PlatformSensor&& other) {
  // Note: if this implementation is ever changed to depend on "this" containing
  // initialized values, the move constructor implemenation must be updated.
  mSensorType = other.mSensorType;
  mMinInterval = other.mMinInterval;
  memcpy(mSensorName, other.mSensorName, kSensorNameMaxLen);

  mLastEvent = other.mLastEvent;
  other.mLastEvent = nullptr;

  mLastEventSize = other.mLastEventSize;
  other.mLastEventSize = 0;

  mLastEventValid = other.mLastEventValid;
  mSamplingStatus = other.mSamplingStatus;
  mPassiveSupported = other.mPassiveSupported;

  return *this;
}

ChreSensorData *PlatformSensor::getLastEvent() const {
  return mLastEventValid ? mLastEvent : nullptr;
}

bool PlatformSensor::getSamplingStatus(
    struct chreSensorSamplingStatus *status) const {
  CHRE_ASSERT(status);

  memcpy(status, &mSamplingStatus, sizeof(*status));
  return true;
}

void PlatformSensorBase::initBase(
    SensorType sensorType,uint64_t minInterval, const char *sensorName,
    ChreSensorData *lastEvent, size_t lastEventSize, bool passiveSupported) {
  mSensorType = sensorType;
  mMinInterval = minInterval;
  memcpy(mSensorName, sensorName, kSensorNameMaxLen);
  mLastEvent = lastEvent;
  mLastEventSize = lastEventSize;

  mSamplingStatus.enabled = false;
  mSamplingStatus.interval = CHRE_SENSOR_INTERVAL_DEFAULT;
  mSamplingStatus.latency = CHRE_SENSOR_LATENCY_DEFAULT;

  mPassiveSupported = passiveSupported;
}

void PlatformSensorBase::setLastEvent(const ChreSensorData *event) {
  memcpy(mLastEvent, event, mLastEventSize);
  mLastEventValid = true;
}

void PlatformSensorBase::setSamplingStatus(
    const struct chreSensorSamplingStatus& status) {
  mSamplingStatus = status;
}

}  // namespace chre
