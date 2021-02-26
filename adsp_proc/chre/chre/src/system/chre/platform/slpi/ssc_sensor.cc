/**
 * @file ssc_sensor.cc
 *
 * Defines representation of SEE sensors with its property.
 * All communication to SEE should be done using ssc_sensor object.
 * This includes querying for sensor information, enabling sensor stream,
 * and disabling sensor stream.
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include <cinttypes>
#include <stdio.h>
#include <stdlib.h>

extern "C" {

#include "fixed_point.h"
#include "timetick.h"

#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>

// nano pb headers
#include "sns_client.h"
#include "sns_osa.h"

// message definitions headers
#include "sns_std.pb.h"
#include "sns_client.pb.h"
#include "sns_resampler.pb.h"

}  // extern "C"

#include "chre/core/event_loop_manager.h"
#include "chre/core/sensor.h"
#include "chre/platform/assert.h"
#include "chre/platform/fatal_error.h"
#include "chre/platform/log.h"

#include "ssc_sensor.h"
#include "ssc_util.h"

namespace chre {
/**
 * Extern variables defined in paltform_sensor_qcom.cc
 */
extern DynamicVector<ssc_sensor> sensor_list;

extern sns_signal_s sns_msg_signal;
extern sns_signal_s sns_ssc_sensor_init_signal;

extern Mutex sns_msg_list_mutex;
extern sns_msg_list_s sns_msg_list;

//extern ConditionVariable init_cond_var;

// resampler suid that will be used for all streaming sensors
sns_std_suid suid_resampler;

/* uid lookup suid for querying sensor suid */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* flag used to query resampler suid only once */
static bool is_suid_resampler_queried = false;

/**
 * Sensor type string to be used for requesting
 * SUID of each sensors
 */
const char *SSC_DATATYPE_RESAMPLER = "resampler";

const char *SSC_DATATYPE_ACCEL = "accel";
const char *SSC_DATATYPE_ANY_MOT = "mcmd";
const char *SSC_DATATYPE_NO_MOT = "mcmd";
const char *SSC_DATATYPE_GYRO = "gyro_cal";
const char *SSC_DATATYPE_MAG = "mag_cal";
const char *SSC_DATATYPE_PRESSURE = "pressure";
const char *SSC_DATATYPE_LIGHT = "ambient_light";
const char *SSC_DATATYPE_PROX = "proximity";
const char *SSC_DATATYPE_ACCELTEMP = "sensor_temperature";
const char *SSC_DATATYPE_GYROTEMP = "sensor_temperature";
const char *SSC_DATATYPE_UNCAL_ACCEL = "accel";
const char *SSC_DATATYPE_UNCAL_GYRO = "gyro";
const char *SSC_DATATYPE_UNCAL_MAG = "mag";
const char *SSC_DATATYPE_PEDOMETER = "pedometer";
const char *SSC_DATATYPE_CMC = "cmc";

// PEND: this is a little funky, should probably be ssc_util
/**
 * Insert the node at the end of the list
 *
 * @param[i] list  list that the node would be stored
 * @param[i] node to be inserted
 */
void sns_msg_list_push(sns_msg_list_s *list, sns_msg_node_s *node)
{
  node->next = nullptr;
  if(list->head == nullptr) {
    list->head = node;
    list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }
  list->num_data++;
}

/**
 * Returns the pointer to first node in the list
 *
 * @param[i] list pop from
 */
sns_msg_node_s *sns_msg_list_pop_first(sns_msg_list_s *list)
{
  sns_msg_node_s *node = nullptr;
  if(list->num_data) {
    node = list->head;
    list->head = node->next;
    list->num_data--;
    if(list->num_data == 0) {
      list->tail = nullptr;
    }
  }

  return node;
}

/**
 * Returns the number of nodes in the list
 */
int sns_msg_list_size(sns_msg_list_s *list)
{
  return list->num_data;
}

/**
 * Iterate through sensor_list vector to find index of sensor object
 * containing given suid
 *
 * @param[i] suid SUID of specific sensor to search for
 * @return index of sensor_list vector as sensor id
 */
int getSensorIdFromSuid(sns_std_suid suid) {
  for(int i = 0; i < sensor_list.size(); i++) {
    sns_std_suid cur_suid = sensor_list[i].get_suid();
    if (cur_suid.suid_low == suid.suid_low &&
        cur_suid.suid_high == suid.suid_high) {
      return i;
    }
  }
  return -1;
}

/**
 * Returns CHRE sensor type given index of sensor_list
 *
 * @param[i] sensorId Sensor Id representing index of sensor_list vector
 * @return CHRE sensor type
 */
SensorType getSensorTypeFromSensorId(uint8_t sensorId) {
  return sensor_list[sensorId].get_chre_sensor_type();
}

/* See sns_qsocket_client_resp */
static void
resp_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);
  SNS_LOG(WARN, "Received response rc %i", error);
}

/**
 * Populate QSocket request message and encode message body
 *
 * @param[i] client_hndl Client handle to use for communication to SEE
 * @param[i] payload Payload to be encoded into request message body
 * @param[i] suid Destination SUID to request the service for
 * @param[i] msg_id Message ID for this request
 * @param[i] has_batching True if request need to enable batching, false otherwise
 * @param[i] batching_period Batching period for this request
 *
 * return true on success, false otherwise
 */
static bool send_basic_req(sns_client *client_hndl, sns_buffer_arg const *payload,
    sns_std_suid suid, uint32_t msg_id, bool has_batching, uint32_t batching_period) {
  sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
  int err = 1;

  pb_req_msg.suid = suid;
  pb_req_msg.msg_id = msg_id;
  pb_req_msg.request.has_batching = has_batching;
  pb_req_msg.request.batching.batch_period = batching_period;
  pb_req_msg.susp_config.client_proc_type = SNS_STD_CLIENT_PROCESSOR_SSC;

  if(payload->buf != nullptr) {
    pb_req_msg.request.payload.funcs.encode = &encode_payload;
    pb_req_msg.request.payload.arg = (void*)payload;
  } else {
    LOGW("payload buf is null");
    pb_req_msg.request.payload.arg = nullptr;
  }

  LOGW("sns_client_send with msg_id %i", msg_id);

  if(NULL != client_hndl) {
    err = sns_client_send(client_hndl, &pb_req_msg, &resp_cb, NULL);
  }
  if(0 != err) {
    SNS_LOG(ERROR, "sns_client_send error %i", err);
    return false;
  }

  return true;
}

/**
 * Send attribute request for specific sensor
 *
 * @param[i] client_hndl Client handle to use for communication to SEE
 * @param[i] suid Suid of sensor service to query attribute for
 *
 * @return true on success, false otherwise
 */
static bool send_attr_req(sns_client *client_hndl, sns_std_suid *suid)
{
  bool rv = false;

  LOGW("send_attr_req");
  sns_buffer_arg payload = (sns_buffer_arg) { .buf = NULL, .buf_len = 0 };
  rv = send_basic_req(client_hndl, &payload, *suid,
      SNS_STD_MSGID_SNS_STD_ATTR_REQ, false, 0);

  return rv;
}

/**
 * Send suid request
 *
 * @param[i] client_hndl Client handle to use for communication to SEE
 * @param[i] data_type Data type in string to specific sensor
 *
 * @return true on success, false otherwise
 */
static bool send_suid_req(sns_client *client_hndl, char const *data_type)
{
  LOGW("send_suid_req for %s", data_type);
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_suid_req(data_type, &encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(client_hndl, &payload, suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ,
                        false, 0);
    free(encoded_req);
  }

  return rv;
}

/**
 * A helper function that frees event data once the data is sent to clients
 *
 * @param[i] eventType The event type of this data to be freed
 * @param[i] eventData A non-null pointer to the sensor's CHRE event data.
 */
void sensorDataEventFree(uint16_t eventType, void *eventData) {
  // Events are allocated using the simple memoryAlloc/memoryFree platform
  // functions.
  // PEND: Consider using a MemoryPool.
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
 * A helper function that updates the last event of a sensor in the main thread.
 * Platform should call this function only for an on-change sensor.
 *
 * @param[i] sensorType The SensorType of the sensor.
 * @param[i] eventData A non-null pointer to the sensor's CHRE event data.
 */
void updateLastEvent(SensorType sensorType, const void *eventData) {
  CHRE_ASSERT(eventData);

  auto *header = static_cast<const chreSensorDataHeader *>(eventData);
  if (header->readingCount != 1) {
    // PEND: better error handling when SMGR behavior changes.
    // SMGR delivers one sample per report for on-change sensors.
    LOGE("%" PRIu16 " samples in an event for on-change sensor %d",
         header->readingCount, static_cast<int>(sensorType));
  } else {
    struct CallbackData {
      SensorType sensorType;
      const ChreSensorData *event;
    };
    auto *callbackData = memoryAlloc<CallbackData>();
    if (callbackData == nullptr) {
      LOGE("Failed to allocate deferred callback memory");
    } else {
      callbackData->sensorType = sensorType;
      callbackData->event = static_cast<const ChreSensorData *>(eventData);

      auto callback = [](uint16_t /* type */, void *data) {
        auto *cbData = static_cast<CallbackData *>(data);

        Sensor *sensor = EventLoopManagerSingleton::get()
            ->getSensorRequestManager().getSensor(cbData->sensorType);

        // Mark last event as valid only if the sensor is enabled. Event data
        // may arrive after sensor is disabled.
        if (sensor != nullptr
            && sensor->getRequest().getMode() != SensorMode::Off) {
          sensor->setLastEvent(cbData->event);
        }
        memoryFree(cbData);
      };

      // Schedule a deferred callback.
      EventLoopManagerSingleton::get()->deferCallback(
          SystemCallbackType::SensorLastEventUpdate, callbackData, callback);
    }  // if (callbackData == nullptr)
  }
}

/**
 * Populate sensor data header for sensor type
 *
 * @param[i] sensorType CHRE sensor type of the data
 * @param[o] header Populated data header
 * @param[i] timestamp Timestamp of payload
 * @param[i] sampleCount Number of samples in payload
 */
void populateSensorDataHeader(
    SensorType sensorType, chreSensorDataHeader *header,
    uint64_t timestamp, uint16_t sampleCount) {
  memset(header->reserved, 0, sizeof(header->reserved));
  header->baseTimestamp = timestamp;
  header->sensorHandle = getSensorHandleFromSensorType(sensorType);
  header->readingCount = sampleCount;
}
/**
 * Populate sensor event for ThreeAxisData
 *
 * @param[o] data Allocated buffer for the data
 * @param[i] list Pointer to sensor data list
 * @param[i] num_data Number of data
 * @param[i] timestamp Timestamp of the first data
 */
void populateThreeAxisEvent(
    SensorType sensorType, chreSensorThreeAxisData *data,
    sensor_data_list *list, int num_data, uint64_t timestamp) {
  populateSensorDataHeader(sensorType, &data->header, timestamp, num_data);
  uint64_t base_timestamp = timestamp;

  for(int i = 0; i < num_data; i++) {
    sensor_data_node *node = sensor_data_pop(list);
    if(node != nullptr) {
      data->readings[i].timestampDelta = node->data.ts - base_timestamp;
      base_timestamp = node->data.ts;

      // Convert from SMGR's NED coordinate to Android coordinate.
      data->readings[i].x = node->data.value[0];
      data->readings[i].y = node->data.value[1];
      data->readings[i].z = node->data.value[2];

      free(node);
    }
  }
}

/**
 * Populate sensor event for Float
 *
 * @param[o] data Allocated buffer for the data
 * @param[i] list Pointer to sensor data list
 * @param[i] num_data Number of data
 * @param[i] timestamp Timestamp of the first data
 */
void populateFloatEvent(
    SensorType sensorType, chreSensorFloatData *data,
    sensor_data_list *list, int num_data, uint64_t timestamp) {
  populateSensorDataHeader(sensorType, &data->header, timestamp, num_data);
  uint64_t base_timestamp = timestamp;

  for(int i = 0; i < num_data; i++) {
    sensor_data_node *node = sensor_data_pop(list);
    if(node != nullptr) {
      data->readings[i].timestampDelta = node->data.ts - base_timestamp;
      base_timestamp = node->data.ts;

      data->readings[i].value = node->data.value[0];
      free(node);
    }
  }
}

/**
 * Populate sensor event for Byte
 *
 * @param[o] data Allocated buffer for the data
 * @param[i] list Pointer to sensor data list
 * @param[i] num_data Number of data
 * @param[i] timestamp Timestamp of the first data
 */
void populateByteEvent(
    SensorType sensorType, chreSensorByteData *data,
    sensor_data_list *list, int num_data, uint64_t timestamp) {
  populateSensorDataHeader(sensorType, &data->header, timestamp, num_data);
  uint64_t base_timestamp = timestamp;

  for(int i = 0; i < num_data; i++) {
    sensor_data_node *node = sensor_data_pop(list);
    if(node != nullptr) {
      data->readings[i].timestampDelta = node->data.ts - base_timestamp;
      base_timestamp = node->data.ts;

      // Zero out fields invalid and padding0.
      data->readings[i].value = 0;
      data->readings[i].isNear = node->data.value[0] ? 1: 0;

      free(node);
    }
  }
}

/**
 * Populate sensor event for Occurence
 *
 * @param[o] data Allocated buffer for the data
 * @param[i] tiemstamp Timestamp of event data
 */
void populateOccurrenceEvent(SensorType sensorType,
    chreSensorOccurrenceData *data, uint64_t timestamp) {
  populateSensorDataHeader(sensorType, &data->header, timestamp, 1);
  data->readings[0].timestampDelta = 0;
}

/**
 * Allocate and populate sensor event based on sensor type.
 * This will allocate appropriate memory for header and payload of
 * the event based on sensorType. If the data is batched, payload
 * is allocated memory of (size of event) * (number of events)
 *
 * @param[i] sensorType Chre sensor type for specific event
 * @param[i] list Pointer to sensor data list
 *
 * @return buffer for specific event
 */
void *allocateAndPopulateEvent(SensorType sensorType, sensor_data_list *list) {
  SensorSampleType sampleType = getSensorSampleTypeFromSensorType(sensorType);
  size_t memorySize = sizeof(chreSensorDataHeader);
  int num_data = list->num_data;
  uint64_t timestamp = list->head->data.ts;

  switch (sampleType) {
    case SensorSampleType::ThreeAxis: {
      memorySize += num_data * sizeof(chreSensorThreeAxisData::chreSensorThreeAxisSampleData);
      auto *event =
          static_cast<chreSensorThreeAxisData *>(memoryAlloc(memorySize));
      if (event != nullptr) {
        populateThreeAxisEvent(sensorType, event, list, num_data, timestamp);
      }
      return event;
    }

    case SensorSampleType::Float: {
      memorySize += num_data * sizeof(chreSensorFloatData::chreSensorFloatSampleData);
      auto *event =
          static_cast<chreSensorFloatData *>(memoryAlloc(memorySize));
      if (event != nullptr) {
        populateFloatEvent(sensorType, event, list, num_data, timestamp);
      }
      return event;
    }

    case SensorSampleType::Byte: {
      memorySize += num_data * sizeof(chreSensorByteData::chreSensorByteSampleData);
      auto *event =
          static_cast<chreSensorByteData *>(memoryAlloc(memorySize));
      if (event != nullptr) {
        populateByteEvent(sensorType, event, list, num_data, timestamp);
      }
      return event;
    }

    case SensorSampleType::Occurrence: {
      memorySize += num_data * sizeof(chreSensorOccurrenceData::chreSensorOccurrenceSampleData);
      auto *event =
          static_cast<chreSensorOccurrenceData *>(memoryAlloc(memorySize));
      if (event != nullptr) {
        populateOccurrenceEvent(sensorType, event, timestamp);
      }
      return event;
    }

    default:
      LOGW("Unhandled sensor data %" PRIu8, static_cast<uint8_t>(sensorType));
      return nullptr;
  }
}

// ssc_sensor member functions
ssc_sensor::ssc_sensor(char const *see_datatype, SensorType chre_sensor_type,
                       decode_fn_t decode_fn, sns_std_sensor_stream_type stream_type) {
  _available = false;
  _suid.suid_high = 0;
  _suid.suid_low = 0;
  _see_datatype = see_datatype;
  _chre_sensor_type = chre_sensor_type;
  _stream_type = stream_type;
  _sensor_id = -1;
  _sensor_name = nullptr;
  _batching = false;
  _sensor_data.head = nullptr;
  _sensor_data.tail = nullptr;
  _sensor_data.num_data = 0;
  _decode_fn = decode_fn;
  _sensor_client = nullptr;

  _cal_bias[0] = 0;
  _cal_bias[1] = 0;
  _cal_bias[2] = 0;
}

ssc_sensor::~ssc_sensor() {
  this->disable();

  if(_sensor_name != nullptr) {
    free(_sensor_name);
  }
}

void ssc_sensor::set_suid(sns_std_suid suid) {
  _suid = suid;
}

void ssc_sensor::set_stream_type(sns_std_sensor_stream_type stream_type) {
  _stream_type = stream_type;
}

void ssc_sensor::set_sensor_id(int sensor_id) {
  _sensor_id = sensor_id;
}

void ssc_sensor::set_sensor_name(char *sensor_name, uint8_t len) {
  _sensor_name = (char *)malloc(len+1);
  if(nullptr != _sensor_name) {
    strlcpy(_sensor_name, sensor_name, sizeof(_sensor_name));
  } else {
    LOGE("Unable to allocate memory for sensor name");
  }
}

void ssc_sensor::set_sensor_cal_bias(float x, float y, float z) {
  _cal_bias[0] = x;
  _cal_bias[1] = y;
  _cal_bias[2] = z;
}

/**
 * Handle a received indication message from the Sensor Service.
 *
 * @param[i] client Client connection on which this indication was received.
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 */
static void
ind_cb(struct sns_client *client, void *msg, uint32_t msg_len,
    void *cb_data)
{
  UNUSED_VAR(client);
  sns_msg_node_s *node;

  node = (sns_msg_node_s*)malloc(sizeof(sns_msg_node_s) + msg_len);
  if(0 == node) {
    LOGE("Failed to malloc node, dropping samples");
    return;
  }

  if(nullptr != cb_data) {
    node->sensor_ptr = (ssc_sensor*)cb_data;
  } else {
    node->sensor_ptr = nullptr;
  }
  node->payload_len = msg_len;
  memcpy(&node->payload[0], msg, msg_len);

  sns_msg_list_mutex.lock();
  sns_msg_list_push(&sns_msg_list, node);
  sns_msg_list_mutex.unlock();

  qurt_anysignal_set(&sns_msg_signal.signal, CHRE_SNS_DRV_SEE_MSG);
}

/**
 * QSocket/QMI Error callback.  This would be our notification if the connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See sns_client_error
 */
static void
error_cb(struct sns_client *client, sns_std_error error, void *cb_data) {
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);

  SNS_LOG(ERROR, "Error callback %i", error);
}

void ssc_sensor::store_data(float x, float y, float z, uint64_t ts) {
  sensor_data_node *node = (sensor_data_node *)malloc(sizeof(sensor_data_node));
  if(nullptr == node) {
    LOGE("Unable to allocate memory for data node");
  } else {
    node->data.value[0] = x + _cal_bias[0];
    node->data.value[1] = y + _cal_bias[1];
    node->data.value[2] = z + _cal_bias[2];
    node->data.ts = ts * 1000 / 19200 * 1000;

    sensor_data_push(&_sensor_data, node);
  }
}

bool ssc_sensor::send_data() {
  bool rv = false;

  if(0 == _sensor_data.num_data) {
    return rv;
  }
  void *eventData = allocateAndPopulateEvent(_chre_sensor_type, &_sensor_data);

  if (nullptr == eventData) {
    LOGE("Dropping %d events due to allocation failure", _sensor_data.num_data);
    int num_data = _sensor_data.num_data;
    for(int i = 0; i < num_data; i++) {
      sensor_data_node *node = sensor_data_pop(&_sensor_data);
      free(node);
    }
    rv = false;
  } else {
    // Schedule a deferred callback to update on-change sensor's last
    // event in the main thread.
    if (sensorTypeIsOnChange(_chre_sensor_type)) {
      updateLastEvent(_chre_sensor_type, eventData);
    }

    EventLoopManagerSingleton::get()->getEventLoop().postEvent(
        getSampleEventTypeForSensorType(_chre_sensor_type), eventData,
        sensorDataEventFree);

    rv = true;
  }

  return rv;
}

void ssc_sensor::open_connection() {
  int err;
  if(nullptr == this->_sensor_client) {
    // PEND: Lower timeout limit?
    err = sns_client_init(&this->_sensor_client, 15000, &ind_cb,
        this, &error_cb, nullptr);
    if(0 != err) {
      SNS_LOG(ERROR, "sns_client_init error %i", err);
    }
  }
}

void ssc_sensor::query_suid() {
  this->open_connection();

  if(!is_suid_resampler_queried) {
    is_suid_resampler_queried = true;
    send_suid_req(this->_sensor_client, SSC_DATATYPE_RESAMPLER);
  }

  send_suid_req(this->_sensor_client, _see_datatype);
}

bool ssc_sensor::handle_msg(void *msg_payload, uint32_t msg_payload_len) {

  pb_istream_t stream;
  pb_istream_t stream_cpy;
  sns_client_event_msg event = sns_client_event_msg_init_default;

  if(msg_payload_len == 0) {
    LOGE("no payload, dropping message");
    return false;
  }

  stream = pb_istream_from_buffer((const pb_byte_t *)msg_payload, msg_payload_len);
  stream_cpy = stream;

  if(!pb_decode(&stream_cpy, sns_client_event_msg_fields, &event)) {
    LOGE("Error decoding event list: %s", PB_GET_ERROR(&stream));
    return false;
  }

  // Decode SUID message
  if(event.suid.suid_high == suid_lookup.suid_high && event.suid.suid_low == suid_lookup.suid_low) {
    sns_std_suid sensor_uid;
    event.events.funcs.decode = &decode_suid_events;
    event.events.arg = (void *)&sensor_uid;

    if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) {
      LOGE("Error decoding event list: %s", PB_GET_ERROR(&stream));
      qurt_anysignal_set(&sns_ssc_sensor_init_signal.signal, CHRE_SNS_DRV_INIT_NEXT);
      return false;
    }

    // sensor_uid should have been decoded at this point
    if(is_suid_resampler_queried) {
      if(sensor_uid.suid_high != 0 || sensor_uid.suid_low != 0) {
        send_attr_req(this->_sensor_client, &sensor_uid);
      }
    }

    // PEND : find better place to do this
    qurt_anysignal_set(&sns_ssc_sensor_init_signal.signal, CHRE_SNS_DRV_INIT_NEXT);

  } else {
    // Decode sensor event message
    pb_event_arg event_arg;
    event_arg.sensor_ptr = this;
    event_arg.sensor_id = _sensor_id;
    event_arg.msg_id = 0;
    event_arg.ts = 0;
    event_arg.suid.suid_high = event.suid.suid_high;
    event_arg.suid.suid_low = event.suid.suid_low;

    event.events.funcs.decode = &decode_events;
    event.events.arg = (void *)&event_arg;

    if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) {
      return false;
    }

    // Send any data which was accumulated during the decode process
    this->send_data();
  }

  return true;
}

bool ssc_sensor::enable(Nanoseconds latency, Nanoseconds interval) {
  bool rv = false;

  this->open_connection();

  if(SNS_STD_SENSOR_STREAM_TYPE_STREAMING == _stream_type) {
    void *encoded_req = nullptr;
    size_t encoded_req_len;

    float sampling_rate;

    /* Capping the lower boundary to 0.2Hz since GTS test waits for 10 seconds to
       to finish the test. May need to revisit once GTS test is updated */
    if(interval.toRawNanoseconds() > Seconds(1).toRawNanoseconds()) {
      sampling_rate = (float)1000000000 / interval.toRawNanoseconds();
      if(sampling_rate < 0.2) {
        sampling_rate = 0.2;
      }
    } else {
      sampling_rate = Seconds(1).toRawNanoseconds() / interval.toRawNanoseconds();
    }

    LOGW("Send resampler request %f" , sampling_rate);

    encoded_req_len = get_encoded_resampler_enable_req(&encoded_req, &_suid,
        sampling_rate);

    if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
              { .buf = encoded_req, .buf_len = encoded_req_len };

    bool batching_enable;
    uint64_t batching_period = latency.toRawNanoseconds()/1000;

    if(latency.toRawNanoseconds() == 0) {
      batching_enable = false;
    } else {
      batching_enable = true;
    }

    _batching = batching_enable;
    LOGW("batching period %" PRIu64, batching_period);

    rv = send_basic_req(this->_sensor_client, &payload, suid_resampler,
          SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG, batching_enable, batching_period);
      free(encoded_req);
    }
  } else if(SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE == _stream_type) {
    LOGW("Send on change request");
    void *encoded_req = nullptr;
    size_t encoded_req_len;
    uint32_t msg_id;

    if(_chre_sensor_type == SensorType::InstantMotion) {
      encoded_req_len = get_encoded_mcmd_enable_req(&encoded_req, SNS_MCMD_TYPE_MOTION);
      msg_id = SNS_MCMD_MSGID_SNS_MCMD_CONFIG;
    } else if(_chre_sensor_type == SensorType::StationaryDetect) {
      encoded_req_len = get_encoded_mcmd_enable_req(&encoded_req, SNS_MCMD_TYPE_STATIONARY);
      msg_id = SNS_MCMD_MSGID_SNS_MCMD_CONFIG;
    } else {
      encoded_req_len = get_encoded_std_enable_req(&encoded_req, 0);
      msg_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
    }

    if(NULL != encoded_req) {
      sns_buffer_arg payload = (sns_buffer_arg)
        { .buf = encoded_req, .buf_len = encoded_req_len };
      rv = send_basic_req(this->_sensor_client, &payload, _suid, msg_id, false, 0);
      free(encoded_req);
    }

    // PEND : find a better way to do this calibration sensor streaming request.
    // did not make these into function as it will need some better way to handle this
    // and these loops should be removed.
    bool send_stream_reqeust = false;
    sns_std_suid stream_suid;
    /*
    if(strcmp(_see_datatype, SSC_DATATYPE_ACCEL) == 0) {
      for(int i = 0; i < sensor_list.size(); i++) {
        if(strcmp(sensor_list[i].get_see_datatype(),SSC_DATATYPE_UNCAL_ACCEL) == 0) {
          stream_suid = sensor_list[i].get_suid();
          send_stream_reqeust = true;
          break;
        }
      }
    }
    */
    if(strcmp(_see_datatype, SSC_DATATYPE_GYRO) == 0) {
      for(int i = 0; i < sensor_list.size(); i++) {
        if(strcmp(sensor_list[i].get_see_datatype(),SSC_DATATYPE_UNCAL_GYRO) == 0) {
          stream_suid = sensor_list[i].get_suid();
          send_stream_reqeust = true;
          break;
        }
      }
    } else if(strcmp(_see_datatype, SSC_DATATYPE_MAG) == 0) {
      for(int i = 0; i < sensor_list.size(); i++) {
        if(strcmp(sensor_list[i].get_see_datatype(),SSC_DATATYPE_UNCAL_MAG) == 0) {
          stream_suid = sensor_list[i].get_suid();
          send_stream_reqeust = true;
          break;
        }
      }
    }

    if(send_stream_reqeust) {
      LOGW("Send resampler request for calibrated sensor");

      // PEND : duplicated code, move into function
      float sampling_rate;

      /* Capping the lower boundary to 0.2Hz since GTS test waits for 10 seconds to
         to finish the test. May need to revisit once GTS test is updated */
      if(interval.toRawNanoseconds() > Seconds(1).toRawNanoseconds()) {
        sampling_rate = (float)1000000000 / interval.toRawNanoseconds();
        if(sampling_rate < 0.2) {
          sampling_rate = 0.2;
        }
      } else {
        sampling_rate = Seconds(1).toRawNanoseconds() / interval.toRawNanoseconds();
      }

      encoded_req_len = get_encoded_resampler_enable_req(&encoded_req, &stream_suid,
          sampling_rate);

      if(NULL != encoded_req) {
        sns_buffer_arg payload = (sns_buffer_arg)
                { .buf = encoded_req, .buf_len = encoded_req_len };

        bool batching_enable;
        uint64_t batching_period = latency.toRawNanoseconds()/1000;

        if(latency.toRawNanoseconds() == 0) {
          batching_enable = false;
        } else {
          batching_enable = true;
        }

        _batching = batching_enable;

        rv = send_basic_req(this->_sensor_client, &payload, suid_resampler,
            SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG, batching_enable, batching_period);
        free(encoded_req);
      }
    }
  } else {
    LOGW("Send standard request");
    void *encoded_req = nullptr;
    size_t encoded_req_len;

    float sampling_rate;

    /* Capping the lower boundary to 0.2Hz since GTS test waits for 10 seconds to
       to finish the test. May need to revisit once GTS test is updated */
    if(interval.toRawNanoseconds() > Seconds(1).toRawNanoseconds()) {
      sampling_rate = (float)1000000000 / interval.toRawNanoseconds();
      if(sampling_rate < 0.2) {
        sampling_rate = 0.2;
      }
    } else {
      sampling_rate = Seconds(1).toRawNanoseconds() / interval.toRawNanoseconds();
    }

    encoded_req_len = get_encoded_std_enable_req(&encoded_req, sampling_rate);

    if(NULL != encoded_req) {
      sns_buffer_arg payload = (sns_buffer_arg)
        { .buf = encoded_req, .buf_len = encoded_req_len };
      rv = send_basic_req(this->_sensor_client, &payload, _suid,
          SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG, false, 0);
      free(encoded_req);
    }
  }

  return rv;
}

void ssc_sensor::disable() {
  int err;

  if(nullptr != this->_sensor_client) {
    err = sns_client_deinit(this->_sensor_client);
    if(0 != err) {
      LOGE("sns_client_deinit error %i", err);
    }
  }

  this->_sensor_client = nullptr;
}

void ssc_sensor::set_available(sns_std_suid *suid) {
  _available = true;
  _suid.suid_high = suid->suid_high;
  _suid.suid_low = suid->suid_low;

  LOGW("Sensor Available for sensor %d", _sensor_id);
  LOGW("SUID %" PRIx64" %" PRIx64, _suid.suid_low, _suid.suid_high);
}

} // namespace chre
