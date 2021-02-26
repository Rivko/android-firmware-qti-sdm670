/**
 * @file platform_sensor_qcom.cc
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <algorithm>
#include <cinttypes>
#include <stdio.h>
#include <stdlib.h>

#include "chre/platform/platform_sensor.h"

extern "C" {

#include "fixed_point.h"
#include "qmi_client.h"
#include "timetick.h"

#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>

#include "qurt.h"
#include "uTimetick.h"

}  // extern "C"

#include "chre_api/chre/sensor.h"
#include "chre/core/event_loop_manager.h"
#include "chre/core/sensor.h"
#include "chre/platform/assert.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/log.h"
#include "chre/platform/system_time.h"
#include "chre/util/macros.h"
#include "chre/platform/mutex.h"
#include "chre/platform/condition_variable.h"

#include "ssc_util.h"
#include "ssc_sensor.h"

namespace chre {

extern const char *SSC_DATATYPE_ACCEL;
extern const char *SSC_DATATYPE_ANY_MOT;
extern const char *SSC_DATATYPE_NO_MOT;
extern const char *SSC_DATATYPE_GYRO;
extern const char *SSC_DATATYPE_MAG;
extern const char *SSC_DATATYPE_PRESSURE;
extern const char *SSC_DATATYPE_LIGHT;
extern const char *SSC_DATATYPE_PROX;
extern const char *SSC_DATATYPE_ACCELTEMP;
extern const char *SSC_DATATYPE_GYROTEMP;
extern const char *SSC_DATATYPE_UNCAL_ACCEL;
extern const char *SSC_DATATYPE_UNCAL_GYRO;
extern const char *SSC_DATATYPE_UNCAL_MAG;
extern const char *SSC_DATATYPE_PEDOMETER;
extern const char *SSC_DATATYPE_CMC;

// list of sensor object created internally to keep track of individual sensor information
DynamicVector<ssc_sensor> sensor_list;

// Signal used to ping CHRE_DRV_SNS thread when msg is received from SEE
sns_signal_s sns_msg_signal;
// Signal used to ping platform_sensor to proceed with next init step
sns_signal_s sns_ssc_sensor_init_signal;
// Mutex to protect sns_msg_list
Mutex sns_msg_list_mutex;
// List of sensor msg received from SEE
sns_msg_list_s sns_msg_list;

// Mutex and conditional variable used to hold initialization process
// until suid and attributes are received.
// TODO : this may block the whole chre init process if unexpected
//        result comes back from SEE. needs improvement
Mutex init_mutex;
//ConditionVariable init_cond_var;

#define CHRE_DRV_SNS_THREAD_PRIO 0x8C

namespace {

/**
 * List of sensor type string to be used for querying sensor suid
 * When this list is updated, NUM_SSC_SENSORS must reflect the
 * number of element in this array
 */
typedef struct sensor_type_info {
  const char *sensor_type;
  SensorType chre_sensor_type;
  decode_fn_t decode_fn;
  sns_std_sensor_stream_type stream_type;
} sensor_type_info;

sensor_type_info sensor_type_list[] =
{
  {SSC_DATATYPE_ACCEL,         SensorType::Accelerometer,                 decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_ANY_MOT,       SensorType::InstantMotion,                 decode_mcmd_event,          SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_NO_MOT,        SensorType::StationaryDetect,              decode_mcmd_event,          SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_GYRO,          SensorType::Gyroscope,                     decode_calibration_event,   SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_MAG,           SensorType::GeomagneticField,              decode_calibration_event,   SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_PRESSURE,      SensorType::Pressure,                      decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_LIGHT,         SensorType::Light,                         decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_PROX,          SensorType::Proximity,                     decode_prox_event,          SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
  {SSC_DATATYPE_ACCELTEMP,     SensorType::AccelerometerTemperature,      decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_GYROTEMP,      SensorType::GyroscopeTemperature,          decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_UNCAL_ACCEL,   SensorType::UncalibratedAccelerometer,     decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_UNCAL_GYRO,    SensorType::UncalibratedGyroscope,         decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
  {SSC_DATATYPE_UNCAL_MAG,     SensorType::UncalibratedGeomagneticField,  decode_sensor_event,        SNS_STD_SENSOR_STREAM_TYPE_STREAMING},
//  {SSC_DATATYPE_PEDOMETER,     SensorType::Unknown,                       decode_pedo_event,          SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
//  {SSC_DATATYPE_CMC,           SensorType::Unknown,                       decode_cmc_event,           SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE},
};

/* Thread related variable */
#define SEE_CLIENT_STACK_SIZE  (0x2000)
static qurt_thread_attr_t see_client_thread_attr;
static qurt_thread_t see_client_tcb;
static char see_client_stack[SEE_CLIENT_STACK_SIZE];

static sns_signal_s sns_init_signal;
#define CHRE_DRV_SNS_INIT_DONE 0x00000001U

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
  ChreSensorData *event = nullptr;

  *eventSize = 0;
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
    if (nullptr == event) {
      *eventSize = 0;
      FATAL_ERROR("Failed to allocate last event memory for SensorType %d",
                  static_cast<int>(sensorType));
    }
  }
  return event;
}

/**
 * Posts a CHRE_EVENT_SENSOR_SAMPLING_CHANGE event to the specified Nanoapp.
 *
 * @param instaceId The instance ID of the nanoapp with an open request
 * @param eventRef A reference of the sampling status event to be posted.
 */
void postSamplingStatusEvent(uint32_t instanceId, uint32_t sensorHandle,
                             const struct chreSensorSamplingStatus& status) {
  // PEND: add a generic reference counted pointer class and use it for Event
  // to share across interested nanoapps.
  auto *event = memoryAlloc<struct chreSensorSamplingStatusEvent>();

  if (event == nullptr) {
    LOGE("Failed to allocate memory for sampling status change event");
  } else {
    event->sensorHandle = sensorHandle;
    memcpy(&event->status, &status, sizeof(event->status));

    if (!EventLoopManagerSingleton::get()->getEventLoop().postEvent(
            CHRE_EVENT_SENSOR_SAMPLING_CHANGE, event, freeEventDataCallback,
            kSystemInstanceId, instanceId)) {
      LOGE("Failed to post sampling status change event");
    }
  }
}

/**
 * Updates the sampling status after the QMI sensor request is accepted by SMGR.
 */
void updateSamplingStatus(Sensor *sensor, const SensorRequest& request) {
  // With SMGR's implementation, sampling interval will be filtered to be the
  // same as requeted. Latency can be shorter if there were other SMGR clients
  // with proc_type also set to SNS_PROC_SSC_V01.
  // If the request is passive, 'enabled' may change over time and needs to be
  // updated.
  if (sensor != nullptr) {
    bool postUpdate = false;
    struct chreSensorSamplingStatus& status = sensor->samplingStatus;
    bool enabled = (request.getMode() != SensorMode::Off);

    if (status.enabled != enabled) {
      postUpdate = true;
      status.enabled = enabled;
    }
    if (!sensorTypeIsOneShot(sensor->getSensorType())) {
      if (status.interval != request.getInterval().toRawNanoseconds()) {
        postUpdate = true;
        status.interval = request.getInterval().toRawNanoseconds();
      }
      if (status.latency != request.getLatency().toRawNanoseconds()) {
        postUpdate = true;
        status.latency = request.getLatency().toRawNanoseconds();
      }
    }

    if (postUpdate) {
      uint32_t sensorHandle = getSensorHandleFromSensorType(
          sensor->getSensorType());

      // Only post to Nanoapps with an open request.
      auto& requests = EventLoopManagerSingleton::get()->
          getSensorRequestManager().getRequests(sensor->getSensorType());
      for (const auto& req : requests) {
        if (req.getNanoapp() != nullptr) {
          postSamplingStatusEvent(req.getNanoapp()->getInstanceId(),
                                  sensorHandle, status);
        }
      }
    }
  }
}

/**
 * NOTE : This is for Passive Mode support only.
 *        Need to revisit since SEE does not support passive mode currently
 *
 * Determines whether a requet is allowed. A passive request is not always
 * allowed.
 *
 * @param sensorType The SensorType of this request
 * @param request The intended sensor request
 * @return true if the request is allowed.
 */
bool isRequestAllowed(SensorType sensorType, const SensorRequest& request) {
  bool allowed = false;

  const Sensor *sensor = EventLoopManagerSingleton::get()
      ->getSensorRequestManager().getSensor(sensorType);
  if (sensor != nullptr) {
    // If it's an ACTIVE or an OFF request, it's always allowed.
    allowed = true;
    if (sensorModeIsPassive(request.getMode())) {
      LOGW("Passive mode not supported");
      allowed = false;
      /*
      size_t numNonChreClients = getNumNonChreClients(sensor->sensorId);
      SensorMode mode = getMergedMode(sensor->sensorId, sensorType, request);
      allowed = (numNonChreClients > 0 || sensorModeIsActive(mode));
      LOGD("sensorType %d allowed %d: mergedMode %d, numNonChreClients %zu",
           static_cast<size_t>(sensorType), allowed, static_cast<int>(mode),
           numNonChreClients);
      */
    }

    // If sensor is not available from SEE, do not allow the request
    if(!sensor_list[sensor->sensorId].available()) {
      allowed = false;
    }
  }
  return allowed;
}

/**
 * Send the appropriate request message(s) to the SSC/SEE according to the
 * configuration specified by request.
 *
 * @param sensorType The sensor type of the request.
 * @param request The sensor request to be made.
 * @return true if the request has been accepted.
 */
bool makeRequest(SensorType sensorType, const SensorRequest& request) {
  bool success = false;

  Sensor *sensor = EventLoopManagerSingleton::get()->getSensorRequestManager()
      .getSensor(sensorType);
  if (nullptr == sensor) {
    LOGE("Invalid sensorType %d", static_cast<size_t>(sensorType));
  } else {
    // Do not make a QMI off request if the sensor is off. Otherwise, SMGR
    // returns an error.
    if (SensorMode::Off == request.getMode()) {
      success = sensor->isSensorOff;
    }

    if (!success) {
      LOGW("Request with interval %" PRIu64 ", latency %" PRIu64,
          request.getInterval().toRawNanoseconds(),
          request.getLatency().toRawNanoseconds());

      if(request.getMode() == SensorMode::Off) {
        sensor_list[sensor->sensorId].disable();
        success = true;
      } else {
        success = sensor_list[sensor->sensorId].enable(request.getLatency(), request.getInterval());
      }
    }
  }

  // PEND: handle makeQmiRequest failures
  if (success) {
    // Update internal states if request was accepted by SMGR.
    sensor->isSensorOff = (request.getMode() == SensorMode::Off);

    if (SensorMode::Off == request.getMode()) {
      sensor->lastEventValid = false;
    }

    updateSamplingStatus(sensor, request);
  }
  return success;
}

}  // anonymous namespace

/**
 * Create chre sensor object based on the information from SEE
 * and add it to the sensors vector
 *
 * @param[o] sensors A pointer to sensors vector
 */
void addSensor(DynamicVector<Sensor> *sensors) {
  // All sensor will be added at this point.
  // However, if individual sensor is not connected to SEE sensor
  // request will be rejected

  for (int i = 0; i < sensor_list.size(); i++) {
    if(SensorType::Unknown != sensor_list[i].get_chre_sensor_type()) {
      Sensor sensor;
      sensor.sensorId = i;
      sensor.dataType = 0; //dummy
      sensor.calType = 0; //dummy

      if(nullptr != sensor_list[i].get_sensor_name()) {
        strlcpy(sensor.sensorName, sensor_list[i].get_sensor_name(), sizeof(sensor.sensorName));
      } else {
        strlcpy(sensor.sensorName, sensor_type_list[i].sensor_type, sizeof(sensor.sensorName));
      }

      // Override one-shot sensor's minInterval to default
      SensorType sensorType = getSensorTypeFromSensorId(i);

      sensor.minInterval = sensorTypeIsOneShot(sensorType) ?
          1 : 10;
      /*
      sensor.minInterval = sensorTypeIsOneShot(sensorType) ?
          CHRE_SENSOR_INTERVAL_DEFAULT : static_cast<uint64_t>(
              Seconds(1).toRawNanoseconds() / sensorInfo.MaxSampleRate);
      */

      // Allocates memory for on-change sensor's last event.
      sensor.lastEvent = allocateLastEvent(sensorType, &sensor.lastEventSize);

      sensor.isSensorOff = true;
      sensor.samplingStatus.enabled = false;
      sensor.samplingStatus.interval = CHRE_SENSOR_INTERVAL_DEFAULT;
      sensor.samplingStatus.latency = CHRE_SENSOR_LATENCY_DEFAULT;

      if (!sensors->push_back(std::move(sensor))) {
        FATAL_ERROR("Failed to allocate new sensor: out of memory");
      }
    }
  }
}

/**
 * Main thread for processing all indication received from SEE
 *
 * @param[i]  unused  Pointer to user data (not used)
 *
 * @return    None
 */
static void see_client_thread(void* unused) {
  qurt_anysignal_set(&sns_init_signal.signal, CHRE_DRV_SNS_INIT_DONE);

  while(1) {
    uint32_t signal_result = 0;

    // block on signal wait
    qurt_anysignal_wait(&sns_msg_signal.signal, sns_msg_signal.flags);
    signal_result = qurt_anysignal_get(&sns_msg_signal.signal);
    qurt_anysignal_clear(&sns_msg_signal.signal, signal_result);

    if (signal_result & CHRE_SNS_DRV_SEE_MSG) {
      // handle message
      while(1) {
        sns_msg_node_s *node;
        sns_msg_list_mutex.lock();
        node = sns_msg_list_pop_first(&sns_msg_list);
        sns_msg_list_mutex.unlock();
        if (0 == node) {
          break;
        }

        if (node->sensor_ptr) {
          node->sensor_ptr->handle_msg(&node->payload[0], node->payload_len);
        }
        free(node);
      }
    }
    if (signal_result & CHRE_SNS_DRV_EXIT) {
      break;
    }
  }
  see_client_tcb = NULL;
  // should only reach here once CHRE_SNS_DRV_EXIT signal is received
  LOGW("CHRE_DRV_SNS end of thread...");
}

PlatformSensor::~PlatformSensor() {
  if (lastEvent != nullptr) {
    LOGD("Releasing lastEvent: 0x%p, id %" PRIu8 ", type %" PRIu8 ", cal %"
         PRIu8 ", size %zu",
         lastEvent, sensorId, dataType, calType, lastEventSize);
    memoryFree(lastEvent);
  }
}

void PlatformSensor::init() {
  int err;
  char thread_name[] = "CHRE_DRV_SNS";

  qurt_anysignal_init(&sns_init_signal.signal);
  sns_init_signal.flags = CHRE_DRV_SNS_INIT_DONE;

  qurt_anysignal_init(&sns_msg_signal.signal);
  sns_msg_signal.flags = CHRE_SNS_DRV_SEE_MSG | CHRE_SNS_DRV_EXIT;

  qurt_anysignal_init(&sns_ssc_sensor_init_signal.signal);
  sns_ssc_sensor_init_signal.flags = CHRE_SNS_DRV_INIT_NEXT;

  sns_msg_list.head = nullptr;
  sns_msg_list.tail = nullptr;
  sns_msg_list.num_data = 0;

  qurt_thread_attr_init(&see_client_thread_attr);
  qurt_thread_attr_set_name(&see_client_thread_attr, thread_name);
  qurt_thread_attr_set_priority(&see_client_thread_attr, CHRE_DRV_SNS_THREAD_PRIO);
  qurt_thread_attr_set_stack_size(&see_client_thread_attr,
                                  SEE_CLIENT_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&see_client_thread_attr,
                                  &see_client_stack[0]);
  /*
  //re-enable this when micro image support is available
  qurt_thread_attr_set_tcb_partition(&see_client_thread_attr,
                                     QURT_THREAD_ATTR_TCB_PARTITION_TCM);
  */
  err = qurt_thread_create(&see_client_tcb, &see_client_thread_attr,
                            see_client_thread, nullptr);

  if(QURT_EOK != err) {
    LOGE("Failed to create CHRE_DRV_SNS thread");
  }

  qurt_anysignal_wait(&sns_init_signal.signal, sns_init_signal.flags);

  /*
  const uint64_t one_second_in_ticks = 19200000ULL;
  for(int i = 12; i > 0; i--)
  {
    LOGE("init countdown %d ", i);
    // sns_busy_wait is implemented as a sleep()
    timer_sleep(one_second_in_ticks, T_TICK, TRUE);
  }
  */

  for(int i = 0; i<ARR_SIZE(sensor_type_list); i++) {
    ssc_sensor sensor_to_add = ssc_sensor(sensor_type_list[i].sensor_type,
                                          sensor_type_list[i].chre_sensor_type,
                                          sensor_type_list[i].decode_fn,
                                          sensor_type_list[i].stream_type);
    sensor_to_add.set_sensor_id(i);
    sensor_list.push_back(std::move(sensor_to_add));
  }

  LOGE("PlatformSensor init done");
}

void PlatformSensor::deinit() {
  // TOD: Close Client connection(s)?
  int status;
  int result = 0xbeef;

  for(int i = 0; i < sensor_list.size(); i++) {
    sensor_list[i].disable();
  }

  qurt_anysignal_set(&sns_msg_signal.signal, CHRE_SNS_DRV_EXIT);

  qurt_anysignal_destroy(&sns_init_signal.signal);
  qurt_anysignal_destroy(&sns_ssc_sensor_init_signal.signal);

  sns_msg_list_mutex.lock();
  sns_msg_node_s* node;
  while(0 != (node = sns_msg_list_pop_first(&sns_msg_list))) {
    free(node);
  }
  sns_msg_list_mutex.unlock();

  if (see_client_tcb) {
    LOGW("CHRE_DRV_SNS thread join");
    result = qurt_thread_join(see_client_tcb, &status);
  }
  qurt_anysignal_destroy(&sns_msg_signal.signal);
  LOGW("PlatformSensor deinit done, join result %d", result);
}

bool PlatformSensor::getSensors(DynamicVector<Sensor> *sensors) {
  CHRE_ASSERT(sensors);

  bool rv = true;
  uint32_t sig_result;

  LOGW("getSensors");
  init_mutex.lock();

  for(int i = 0; i < sensor_list.size(); i++) {
    LOGW("Reuqest SUID for %i", i);
    sensor_list[i].query_suid();
    qurt_anysignal_wait(&sns_ssc_sensor_init_signal.signal, sns_ssc_sensor_init_signal.flags);
    sig_result = qurt_anysignal_get(&sns_ssc_sensor_init_signal.signal);
    qurt_anysignal_clear(&sns_ssc_sensor_init_signal.signal, sig_result);
  }

  LOGW("Add Sensor to DynamicVector");
  addSensor(sensors);

  init_mutex.unlock();

  for(int i = 0; i < sensor_list.size(); i++) {
    LOGW("see_type %s chre_type %i", sensor_list[i].get_see_datatype(), sensor_list[i].get_chre_sensor_type());
    LOGW("stream_type %i", (int)sensor_list[i].get_stream_type());
  }

  return rv;
}

bool PlatformSensor::applyRequest(const SensorRequest& request) {
  // Adds a sensor monitor the first time this sensor is requested.
  // not supported yet on SDM 845
  // addSensorMonitor(this->sensorId);

  // Determines whether a (passive) request is allowed at this point.
  bool requestAllowed = isRequestAllowed(getSensorType(), request);
  SensorRequest offRequest;
  bool success;

  if(!requestAllowed) {
    LOGW("Request allowed %d", requestAllowed);
  }

  // If request is not allowed, turn off the sensor. Otherwise, make request.
  success = makeRequest(getSensorType(), requestAllowed ? request : offRequest);
  return success;
}

SensorType PlatformSensor::getSensorType() const {
  return getSensorTypeFromSensorId(this->sensorId);
}

uint64_t PlatformSensor::getMinInterval() const {
  return minInterval;
}

const char *PlatformSensor::getSensorName() const {
  return sensorName;
}

PlatformSensor::PlatformSensor(PlatformSensor&& other) {
  // Our move assignment operator doesn't assume that "this" is initialized, so
  // we can just use that here
  *this = std::move(other);
}

PlatformSensor& PlatformSensor::operator=(PlatformSensor&& other) {
  // Note: if this implementation is ever changed to depend on "this" containing
  // initialized values, the move constructor implemenation must be updated
  sensorId = other.sensorId;
  dataType = other.dataType;
  calType = other.calType;
  memcpy(sensorName, other.sensorName, MAX_SENSOR_NAME_SIZE);
  minInterval = other.minInterval;

  lastEvent = other.lastEvent;
  other.lastEvent = nullptr;

  lastEventSize = other.lastEventSize;
  other.lastEventSize = 0;

  lastEventValid = other.lastEventValid;
  isSensorOff = other.isSensorOff;
  samplingStatus = other.samplingStatus;

  return *this;
}

ChreSensorData *PlatformSensor::getLastEvent() const {
  return (this->lastEventValid) ? this->lastEvent : nullptr;
}

bool PlatformSensor::getSamplingStatus(
    struct chreSensorSamplingStatus *status) const {
  CHRE_ASSERT(status);

  bool success = false;
  if (status != nullptr) {
    success = true;
    memcpy(status, &samplingStatus, sizeof(*status));
  }
  return success;
}

void PlatformSensorBase::setLastEvent(const ChreSensorData *event) {
  memcpy(this->lastEvent, event, this->lastEventSize);
  this->lastEventValid = true;
}

}  // namespace chre
