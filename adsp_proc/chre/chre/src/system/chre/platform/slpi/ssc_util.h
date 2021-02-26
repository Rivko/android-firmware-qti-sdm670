/**
 * @file ssc_util.h
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#ifndef SSC_UTIL_H_
#define SSC_UTIL_H_

extern "C" {
#include "qurt.h"
} /* extern "C" */

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_std.pb.h"
#include "sns_mcmd.pb.h"

namespace chre {

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/**
 * Utility structure used to encode a string or already encoded message
 * into another Protocol Buffer message.
 */
typedef struct sns_buffer_arg
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} sns_buffer_arg;

/**
 * Structure to be used with pb_decode_float_arr_cb
 */
typedef struct pb_float_arr_arg
{
  /* Where to place the decoded float array */
  float *arr;
  /* Length of the float array */
  uint8_t arr_len;
  /* Array index used for decoding.
     Must be initialized to 0 prior to calling pb_decode_float_arr_cb*/
  uint8_t *arr_index;
} pb_float_arr_arg;

/**
 * Structure to be used with decode_attr_value
 */
typedef struct pb_attr_arg
{
  sns_buffer_arg buf;
  bool boolean;
  int sint;
  float flt;
} pb_attr_arg;

/**
 * Structure to be used to store sensor data
 */
typedef struct sns_float_data
{
  float value[3];
  uint64_t ts;
} sns_float_data;

/**
 * Node to be used for sensor data
 */
typedef struct sensor_data_node
{
  sns_float_data data;
  sensor_data_node *next;
} sensor_data_node;

/**
 * List structure for sensor data
 */
typedef struct sensor_data_list
{
  sensor_data_node *head;
  sensor_data_node *tail;
  int num_data;
} sensor_data_list;

/**
 * Signal structure for thread
 */
typedef struct sns_signal_s {
  qurt_anysignal_t signal;
  uint32_t flags;
} sns_signal_s;

/**
 * Signal used to notify the thread waiting for incoming indication
 * from SEE.
 */
#define CHRE_SNS_DRV_SEE_MSG 0x00000001U
/**
 * Signal used to notify the thread to exit when CHRE sensor driver
 * is unloaded.
 */
#define CHRE_SNS_DRV_EXIT    0x10000000U

/**
 * Signal used to notify the thread to proceed to next sensor
 * initialization.
 */
#define CHRE_SNS_DRV_INIT_NEXT 0x00000001U

void sensor_data_push(sensor_data_list *list, sensor_data_node *data);
sensor_data_node *sensor_data_pop(sensor_data_list *list);

bool decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_physical_config_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_sensor_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_prox_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_calibration_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_mcmd_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_stream_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_suid_events(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_pedo_event(pb_istream_t *stream, const pb_field_t *field, void **arg);
bool decode_cmc_event(pb_istream_t *stream, const pb_field_t *field, void **arg);

bool encode_payload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);
bool decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg);
size_t get_encoded_suid_req(char const *data_type, void **encoded_req);
size_t get_encoded_mcmd_enable_req(void **encoded_req, sns_mcmd_type mode);
size_t get_encoded_resampler_enable_req(void **encoded_req, sns_std_suid *suid, float sample_rate);
size_t get_encoded_std_enable_req(void **encoded_req, float sample_rate);

} /* namespace chre */

#endif /* SSC_UTIL_H_ */
