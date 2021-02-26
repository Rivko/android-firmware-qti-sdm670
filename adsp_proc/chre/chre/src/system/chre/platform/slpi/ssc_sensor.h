/**
 * @file ssc_sensor.h
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#ifndef SSC_SENSOR_H_
#define SSC_SENSOR_H_

extern "C" {
#include "sns_suid.pb.h"
} // extern "C"

#include "sns_client_api_v01.h"
#include "chre/core/sensor_request.h"
#include "chre/util/dynamic_vector.h"
#include "chre/util/time.h"
#include "sns_std_sensor.pb.h"
#include "ssc_util.h"

// PEND: find better place to put this
#define NUM_SSC_UTIL_SENSORS 1
#define NUM_SSC_SENSORS 14

namespace chre {

// Decode function pointer to be used by each sensor object
typedef bool (*decode_fn_t)(pb_istream_t *, const pb_field_t *, void **);

class ssc_sensor
{
public:
  ssc_sensor(char const *see_datatype, SensorType chre_sensor_type, decode_fn_t decode_fn, sns_std_sensor_stream_type stream_type);
  ~ssc_sensor();

  void set_suid(sns_std_suid suid);
  void set_stream_type(sns_std_sensor_stream_type stream_type);
  void set_sensor_id(int sensor_id);
  void set_sensor_name(char *sensor_name, uint8_t len);
  void set_sensor_cal_bias(float x, float y, float z); // PEND : needs to be generic..
  void set_available(sns_std_suid *suid);

  bool available() { return _available; }
  char const *get_see_datatype() { return _see_datatype; }
  SensorType get_chre_sensor_type() { return _chre_sensor_type; }
  int get_sensor_id() { return _sensor_id; }
  sns_std_suid get_suid() { return _suid; }
  sns_std_sensor_stream_type get_stream_type() { return _stream_type; }
  char const *get_sensor_name() { return _sensor_name; }
  decode_fn_t decode_event() { return _decode_fn; }
  bool is_batching() { return _batching; }

  /**
   * Store the event data
   * This function will be used to also store byte event and single axis event
   * by using only valid X value (Y and Z being 0)
   *
   * @param[i] x X axis value of event data
   * @param[i] y Y axis value of event data
   * @param[i] z Z axis value of event data
   * @param[i] ts Timestamp of event data
   */
  void store_data(float x, float y, float z, uint64_t ts); // PEND : needs to be generic..

  /**
   * Send all the event data to client
   */
  bool send_data();

  /**
   * Send SUID request for this sensor type
   */
  void query_suid();

  /**
   * Process pb encoded message from SEE
   *
   * @param[i] msg_payload Pointer to pb encoded message
   * @param[i] msg_payload_len Length of msg_payload
   *
   */
  bool handle_msg(void *msg_payload, uint32_t msg_payload_len);
  /**
   * Create and send sensor request
   *
   * @param[i] latency Latency requirement for this request in nanoseconds
   * @param[i] interval Interval requirement for this request in nanoseconds
   *
   * @return true on success, false otherwise
   */
  bool enable(Nanoseconds latency, Nanoseconds interval);
  /**
   * Disable sensor and close qmi connection
   */
  void disable();

private:
  /* Opens the QMI/QSocket Client connection */
  void open_connection();

  /* QSocket connection handle for the sensor */
  struct sns_client *_sensor_client;

  /* If this sensor is available for use */
  bool _available;

  /* String representation of the SEE sensor type */
  char const *_see_datatype;

  /* Enum representatoin of CHRE sensor type */
  SensorType _chre_sensor_type;

  /* Sensor Id of CHRE sensor */
  int _sensor_id;

  /* Unique ID representing the sensor */
  sns_std_suid _suid;

  /* Stream type of the sensor
   * can be one of
   * SNS_STD_SENSOR_STREAM_TYPE_STREAMING
   * SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE
   * SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT
   */
  sns_std_sensor_stream_type _stream_type;

  /* Name of the sensor e.g. lsm6dsm */
  char *_sensor_name;

  /* Bias fields of the sensor. This is initialized to 0 and will only
   * have meaningful value when calibration bias is requested */
  float _cal_bias[3];

  /* Indicates if current request is batching */
  bool _batching;

  /* Pointer to sensor data vector to store and flush data from SEE */
  sensor_data_list _sensor_data;

  /* Function pointer to decode indication event */
  decode_fn_t _decode_fn;
};

/**
 * Structure to be used to hold indication data
 */
typedef struct sns_msg_node_s {
  /* next */
  sns_msg_node_s *next;
  /* Pointer to ssc_sensor this message belongs to*/
  ssc_sensor *sensor_ptr;
  /* Encoded payload length */
  uint32_t payload_len;
  /* Encoded payload */
  uint8_t payload[1];
} sns_msg_node_s;

/**
 * List structure for sensor data
 */
typedef struct sns_msg_list_s
{
  sns_msg_node_s *head;
  sns_msg_node_s *tail;
  int num_data;
} sns_msg_list_s;

/**
 * Structure to be used to carry sensor_id, msg_id, and timestamp
 * while decoding process to identify origin of the event
 */
typedef struct pb_event_arg
{
  ssc_sensor *sensor_ptr;
  int sensor_id;
  uint32_t msg_id;
  uint64_t ts;
  sns_std_suid suid;
} pb_event_arg;

// shared functions
SensorType getSensorTypeFromSensorId(uint8_t sensorId);

void sns_msg_list_push(sns_msg_list_s *list, sns_msg_node_s *node);
sns_msg_node_s *sns_msg_list_pop_first(sns_msg_list_s *list);
int sns_msg_list_size(sns_msg_list_s *list);

} //namespace chre

#endif
