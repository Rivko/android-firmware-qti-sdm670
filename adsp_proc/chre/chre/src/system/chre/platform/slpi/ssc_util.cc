/**
 * @file ssc_util.cc
 *
 * Utility functions for encoding / decoding pb messages
 * for communication with SEE
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "ssc_sensor.h"
#include "ssc_util.h"

#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "sns_resampler.pb.h"
#include "sns_proximity.pb.h"
#include "sns_cal.pb.h"
#include "sns_mcmd.pb.h"
#include "sns_pedometer.pb.h"
#include "sns_cmc.pb.h"

namespace chre {

/**
 * Extern variables defined in paltform_sensor_qcom.cc
 */
extern DynamicVector<ssc_sensor> sensor_list;
extern const char *SSC_DATATYPE_RESAMPLER;
extern sns_signal_s sns_ssc_sensor_init_signal;
extern sns_std_suid suid_resampler;

// PEND: pus/pop implies FILO but this is actually FIFO
/**
 * Insert the data node at the end of the sensor data list
 *
 * @param[i] list Sensor data list that the data would be stored
 * @param[i] data Data to be inserted
 */
void sensor_data_push(sensor_data_list *list, sensor_data_node *data)
{
  data->next = nullptr;
  if(list->head == nullptr) {
    list->head = data;
    list->tail = data;
  } else {
    list->tail->next = data;
    list->tail = data;
  }
  list->num_data++;
}

/**
 * Returns the pointer to first item in the sensor data list
 *
 * @param[i] list Sensor data list to search the data for
 */
sensor_data_node *sensor_data_pop(sensor_data_list *list)
{
  sensor_data_node *rv = nullptr;
  if(list->num_data == 0) {
    LOGE("No data to pop");
  } else {
    rv = list->head;
    list->head = rv->next;
    list->num_data--;
    if(list->num_data == 0) {
      list->tail = nullptr;
    }
  }

  return rv;
}

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
bool encode_payload(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_buffer_arg *info = (sns_buffer_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, (const pb_byte_t *)info->buf, info->buf_len))
    return false;

  return true;
}

/**
 * Decode a string of bytes from a field. This function is intended to be
 * used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
bool decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_buffer_arg *data = (sns_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

/**
 * Encode request message for sensor suid request
 *
 * @param[i] data_type Data type in string to specific sensor
 * @param[o] encoded_req Encoded message for suid request
 *
 * @return size of encoded request message
 */
size_t get_encoded_suid_req(char const *data_type, void **encoded_req)
{
  size_t encoded_req_size;
  sns_suid_req suid_req = sns_suid_req_init_default;

  *encoded_req = NULL;

  sns_buffer_arg buffer_arg;
  buffer_arg.buf = data_type;
  buffer_arg.buf_len = strlen(data_type);

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &buffer_arg;
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;
  suid_req.has_default_only = true;
  suid_req.default_only = true;

  if(!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req)) {
    LOGE("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)encoded_buffer,
                                                 encoded_req_size);

    if(!pb_encode(&stream, sns_suid_req_fields, &suid_req)) {
      LOGE("pb_get_encoded_size error: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Create an encoded Magnitude Change MD request message
 *
 * @param[o] encoded_req Generated encoded request
 * @param[i] mode Detection mode to config whether motion / stationary
 *
 * @return Length of encoded_req
 */
size_t get_encoded_mcmd_enable_req(void **encoded_req, sns_mcmd_type mode)
{
  size_t encoded_req_size;
  sns_mcmd_config config_req = sns_mcmd_config_init_default;
  config_req.type = mode;

  *encoded_req = nullptr;

  if(!pb_get_encoded_size(&encoded_req_size, sns_mcmd_config_fields, &config_req)) {
    LOGE("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_mcmd_config_fields, &config_req)) {
      LOGE("Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Create an encoded Resampler request message
 *
 * @param[o] encoded_req Generated encoded request
 * @param[i] suid SUID of specific sensor that resampler will stream data for
 * @param[i] sampl_rate Requested sample rate
 *
 * @return Length of encoded_req
 */
size_t get_encoded_resampler_enable_req(void **encoded_req, sns_std_suid *suid, float sample_rate)
{
  size_t encoded_req_size;
  sns_resampler_config resampler_config = sns_resampler_config_init_default;

  memcpy(&resampler_config.sensor_uid, suid, sizeof(resampler_config.sensor_uid));
  resampler_config.resampled_rate = sample_rate;
  resampler_config.rate_type = SNS_RESAMPLER_RATE_FIXED;
  resampler_config.filter = true;

  *encoded_req = nullptr;

  if(!pb_get_encoded_size(&encoded_req_size, sns_resampler_config_fields, &resampler_config)) {
    LOGE("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_resampler_config_fields, &resampler_config)) {
      LOGE("Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Create an encoded Standard request message
 *
 * @param[o] encoded_req Generated encoded request
 * @param[i] sampl_rate Requested sample rate
 *
 * @return Length of encoded_req
 */
size_t get_encoded_std_enable_req(void **encoded_req, float sample_rate)
{
  size_t encoded_req_size;
  sns_std_sensor_config config_req = sns_std_sensor_config_init_default;
  config_req.sample_rate = sample_rate;

  *encoded_req = nullptr;

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_sensor_config_fields, &config_req)) {
    LOGE("pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_sensor_config_fields, &config_req)) {
      LOGE("Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Get the message ID of the encoded event residing on stream.
 *
 * @return Decoded message id
 */
static bool get_msg_id(pb_istream_t *stream, pb_event_arg *event_arg)
{
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    LOGE("event: %s", PB_GET_ERROR(stream));
    return false;
  } else {
    //LOGW("Decoding event with message ID %i, timestamp %" PRIu64, event.msg_id, event.timestamp);
    event_arg->msg_id = event.msg_id;
    event_arg->ts = event.timestamp;
    return true;
  }
}

/**
 * Decode attribute value from stream
 *
 * @return true on success, false otherwise
 */
bool decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);

  pb_attr_arg *data = (pb_attr_arg *)*arg;

  pb_attr_arg attr_data;
  sns_buffer_arg str_data = (sns_buffer_arg){ .buf = NULL, .buf_len = 0 };
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;

  value.str.funcs.decode = &decode_payload;
  value.str.arg = &str_data;

  value.subtype.values.funcs.decode = &decode_attr_value;
  value.subtype.values.arg = &attr_data;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value)) {
    LOGE("Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(value.has_flt) {
//    LOGW("Attribute float: %f", value.flt);
    data->flt = value.flt;
  } else if(value.has_sint) {
//    LOGW("Attribute int: %" PRIi64, value.sint);
    data->sint = value.sint;
  } else if(value.has_boolean) {
//    LOGW("Attribute boolean: %i", value.boolean);
    data->boolean = value.boolean;
  } else if(NULL != str_data.buf) {
//    LOGW("Attribute string: %s", (char*)str_data.buf);
    data->buf.buf = str_data.buf;
    data->buf.buf_len = str_data.buf_len;
  } else if(value.has_subtype) {
//    LOGW("Attribute nested");
  } else {
//    LOGW("Unknown attribute type");
  }

  return true;
}

/**
 * Decode attribute from stream
 *
 * @return true on success, false otherwise
 */
bool decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *arg_ptr = (pb_event_arg *)*arg;
  ssc_sensor *sensor_ptr = arg_ptr->sensor_ptr;
  pb_istream_t stream_cpy = *stream;
  sns_std_attr attribute = sns_std_attr_init_default;
  pb_attr_arg data;
  int32_t attr_id;

  if(!pb_decode(&stream_cpy, sns_std_attr_fields, &attribute)) {
    LOGE("Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }
  attr_id = attribute.attr_id;

  if(attr_id == SNS_STD_SENSOR_ATTRID_NAME || attr_id == SNS_STD_SENSOR_ATTRID_STREAM_TYPE ||
     attr_id == SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR || attr_id == SNS_STD_SENSOR_ATTRID_TYPE) {
    attribute.value.values.funcs.decode = &decode_attr_value;
    attribute.value.values.arg = &data;
  }

  if(!pb_decode(stream, sns_std_attr_fields, &attribute)) {
    LOGE("Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(SNS_STD_SENSOR_ATTRID_NAME == attr_id) {
    LOGW("Attribute ID %i value '%s'", attribute.attr_id, data.buf.buf);
    sensor_ptr->set_sensor_name((char *)data.buf.buf, data.buf.buf_len);
  } else if (SNS_STD_SENSOR_ATTRID_STREAM_TYPE == attr_id) {
    LOGW("Attribute ID %i value %i", attribute.attr_id, data.sint);
    if(sensor_ptr->get_stream_type() == (sns_std_sensor_stream_type)data.sint) {
      sensor_ptr->set_available(&arg_ptr->suid);
      sensor_ptr->disable();
    }

  } else if (SNS_STD_SENSOR_ATTRID_TYPE == attr_id) {
    LOGW("Attribute ID %i value '%s'", attribute.attr_id, data.buf.buf);
  }

  return true;
}

/**
 * Decode attribute event from stream
 *
 * @return true on success, false otherwise
 */
bool decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *arg_ptr = (pb_event_arg *)*arg;
  pb_event_arg event_arg;

  LOGW("decode attr event");

  event_arg.sensor_ptr = arg_ptr->sensor_ptr;
  event_arg.sensor_id = arg_ptr->sensor_id;
  event_arg.msg_id = arg_ptr->msg_id;
  event_arg.ts = arg_ptr->ts;
  event_arg.suid.suid_high = arg_ptr->suid.suid_high;
  event_arg.suid.suid_low = arg_ptr->suid.suid_low;

  sns_std_attr_event attr_event = sns_std_attr_event_init_default;

  attr_event.attributes.funcs.decode = &decode_attr;
  attr_event.attributes.arg = &event_arg;

  if(!pb_decode(stream, sns_std_attr_event_fields, &attr_event)) {
    LOGE("Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

/**
 * Decode an array of float values, such as the array within the sensor
 * sample data.
 *
 * @return true on success, false otherwise
 */
bool decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_float_arr_arg const *float_arr = (pb_float_arr_arg const*)*arg;

  if(!pb_decode_fixed32(stream, &(float_arr->arr[*float_arr->arr_index])))
      return false;

  (*float_arr->arr_index)++;

  return true;
}

/**
 * Decode physical config event from stream
 *
 * @return true on success, false otherwise
 */
bool decode_physical_config_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_physical_config_event event = sns_std_sensor_physical_config_event_init_default;

  if(!pb_decode(stream, sns_std_sensor_physical_config_event_fields, &event)) {
    LOGE("decode_physical_config_event: Error decoding event: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

/**
 * Decode sensor event from stream and store decoded sensor data into
 * sensor data storage
 *
 * @return true on success, false otherwise
 */
bool decode_sensor_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == event_arg->msg_id) {
    sns_std_sensor_event event = sns_std_sensor_event_init_default;
    float sensor_data[3] = {0, 0, 0};
    uint8_t arr_index = 0;
    pb_float_arr_arg float_arg;
    SensorType sensor_type = getSensorTypeFromSensorId(event_arg->sensor_id);

    //LOGW("Received std sensor event");

    float_arg.arr = sensor_data;
    float_arg.arr_index = &arr_index;

    switch(sensor_type) {
      case SensorType::Accelerometer:
      case SensorType::Gyroscope:
      case SensorType::GeomagneticField:
      case SensorType::UncalibratedAccelerometer:
      case SensorType::UncalibratedGyroscope:
      case SensorType::UncalibratedGeomagneticField:
      {
        // Three Axis Float
        //LOGW("Recieved Three Axis Float");

        float_arg.arr_len = 3;
        event.data.funcs.decode = &decode_float_arr_cb;
        event.data.arg = &float_arg;
        break;
      }

      case SensorType::Pressure:
      case SensorType::Light:
      case SensorType::AccelerometerTemperature:
      case SensorType::GyroscopeTemperature:
      {
        // Float
        //LOGW("Recieved Float");
        float_arg.arr_len = 1;
        event.data.funcs.decode = &decode_float_arr_cb;
        event.data.arg = &float_arg;
        break;
      }

      case SensorType::InstantMotion:
      case SensorType::StationaryDetect:
      {
        // Occurrence
        //LOGW("Recieved Occurrence");
        break;
      }

      case SensorType::Proximity:
      {
        // Byte
        //LOGW("Recieved Byte");
        break;
      }

      default:
        //LOGE("Recieved Unknown SensorType %d", sensor_type);
        break;
    }

    if(!pb_decode(stream, sns_std_sensor_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding std Event: %s",
          PB_GET_ERROR(stream));
    } else {
      event_arg->sensor_ptr->store_data(sensor_data[0], sensor_data[1],
          sensor_data[2], event_arg->ts);
    }
  } else {
    LOGW("Received Unknown Message id %d", event_arg->msg_id);
    pb_read(stream, NULL, stream->bytes_left);
  }

  return true;
}

/**
 * Decode proximity event from stream and store decoded proximity data into
 * sensor data storage
 *
 * @return true on success, false otherwise
 */
bool decode_prox_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT == event_arg->msg_id) {
    LOGW("Received prox event");
    sns_proximity_event event = sns_proximity_event_init_default;

    if(!pb_decode(stream, sns_proximity_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding prox Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    event_arg->sensor_ptr->store_data(event.proximity_event_type, 0, 0, event_arg->ts);
    return true;
  } else {
    LOGW("Received unknown event");
    return false;
  }
}

/**
 * Decode calibration event from stream and store calibration parameter
 * to bias field of given sensor id from arg
 *
 * @return true on success, false otherwise
 */
bool decode_calibration_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_CAL_MSGID_SNS_CAL_EVENT == event_arg->msg_id) {
    sns_cal_event event = sns_cal_event_init_default;
    float sensor_data[3] = {0, 0, 0};
    uint8_t arr_index = 0;
    pb_float_arr_arg float_arg;

    LOGW("Received calibration event");

    float_arg.arr = sensor_data;
    float_arg.arr_index = &arr_index;
    float_arg.arr_len = 3;

    event.bias.funcs.decode = &decode_float_arr_cb;
    event.bias.arg = &float_arg;

    if(!pb_decode(stream, sns_cal_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding cal Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    event_arg->sensor_ptr->set_sensor_cal_bias(sensor_data[0], sensor_data[1], sensor_data[2]);

    return true;
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == event_arg->msg_id) {
    sns_std_sensor_event event = sns_std_sensor_event_init_default;
    float sensor_data[3] = {0, 0, 0};
    uint8_t arr_index = 0;
    pb_float_arr_arg float_arg;
    SensorType sensor_type = getSensorTypeFromSensorId(event_arg->sensor_id);

    //LOGW("Received std sensor event");

    float_arg.arr = sensor_data;
    float_arg.arr_index = &arr_index;

    // Find the type of output
    if(sensor_type == SensorType::Accelerometer || sensor_type == SensorType::Gyroscope ||
        sensor_type == SensorType::GeomagneticField) {
      // Three Axis Float
      //LOGW("Three Axis Float");

      float_arg.arr_len = 3;
      event.data.funcs.decode = &decode_float_arr_cb;
      event.data.arg = &float_arg;
    }

    if(!pb_decode(stream, sns_std_sensor_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding std Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    event_arg->sensor_ptr->store_data(sensor_data[0], sensor_data[1],
                                      sensor_data[2], event_arg->ts);
    return true;
  } else {
    LOGW("Received unknown event");
    return false;
  }
}

/**
 * Decode Magnitude Change MD event
 *
 * @return true on success, false otherwise
 */
bool decode_mcmd_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(stream);
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_MCMD_MSGID_SNS_MCMD_EVENT == event_arg->msg_id) {
    LOGW("Received mcmd event");
    event_arg->sensor_ptr->store_data(0, 0, 0, event_arg->ts);
    return true;
  } else {
    LOGW("Received unknown event");
    return false;
  }
}

/**
 * Decode pedometer event from stream
 *
 * @return true on success, false otherwise
 */
bool decode_pedo_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_PEDOMETER_MSGID_SNS_STEP_EVENT == event_arg->msg_id) {
    LOGW("Received pedometer event");
    sns_step_event event = sns_step_event_init_default;

    if(!pb_decode(stream, sns_step_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding pedometer Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    // decoding done. send data through sensor ptr here
    return true;
  } else if(SNS_PEDOMETER_MSGID_SNS_STEP_EVENT_CONFIG == event_arg->msg_id) {
    LOGW("Received pedometer config event");
    sns_step_event event = sns_step_event_config_init_default;

    if(!pb_decode(stream, sns_step_event_config_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding pedometer config Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    // decoding done. send data through sensor ptr here
    return true;
  } else {
    LOGW("Received unknown event");
    return false;
  }
}

/**
 * Decode cmc event data from stream
 *
 * @return true on success, false otherwise
 */
bool decode_cmc_event_data(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_cmc_event_data *event_arg = (sns_cmc_event_data *)*arg;
  sns_cmc_event_data event = sns_cmc_event_data_init_default;

  if(!pb_decode(stream, sns_cmc_event_data_fields, &event)) {
    LOGE("decode_sensor_event: Error decoding cmc data Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  event_arg->ms_state = event.ms_state;
  event_arg->ms_state_event = event.ms_state_event;

  return true;
}

/**
 * Decode cmc event from stream
 *
 * @return true on success, false otherwise
 */
bool decode_cmc_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_event_arg *event_arg = (pb_event_arg *)*arg;

  if(SNS_CMC_MSGID_SNS_CMC_EVENT == event_arg->msg_id) {
    LOGW("Received cmc event");
    sns_cmc_event event = sns_cmc_event_init_default;
    sns_cmc_event_data event_arg = sns_cmc_event_data_init_zero;

    event.events.funcs.decode = decode_cmc_event_data;
    event.events.arg = &event_arg;

    if(!pb_decode(stream, sns_cmc_event_fields, &event)) {
      LOGE("decode_sensor_event: Error decoding cmc Event: %s", PB_GET_ERROR(stream));
      return false;
    }

    // decoding done. send data through sensor ptr here
    return true;
  } else {
    LOGW("Received unknown event");
    return false;
  }
}

/*
 * Distinguish the type of decoding needed based on message id
 *
 * @param[i] arg pb_event_arg containing msg_id, sensor_id, and timestamp
 *
 * @return true on success, false otherwise
 */
bool decode_stream_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  pb_event_arg *event_arg = (pb_event_arg *)*arg;
  uint32_t msg_id = event_arg->msg_id;

  // handle std messages
  if(SNS_STD_MSGID_SNS_STD_ATTR_EVENT == msg_id) {
    LOGW("Received attr event");
    bool rv = decode_attr_event(stream, field, arg);
    return rv;
  } else if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == msg_id) {
    LOGW("Received Flush event");
    return event_arg->sensor_ptr->send_data();
  } else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == msg_id) {
    LOGE("Received Error event");
    return false;
  }

  // handle std sensor message
  if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == msg_id) {
    LOGW("Received std sensor config");
    pb_byte_t temp[stream->bytes_left];
    pb_read(stream, temp, stream->bytes_left);
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == msg_id) {
    LOGW("Received sensor on change config");
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == msg_id) {
    LOGW("Received sensor phy conf");
    return decode_physical_config_event(stream, field, arg);
  }
  else {
    return (event_arg->sensor_ptr->decode_event())(stream, field, arg);
  }

  return true;
}

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * @param[i] arg pb_event_arg containing msg_id, sensor_id, and timestamp
 *
 * @return true on success, false otherwise
 */
bool decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = false;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  pb_event_arg *arg_ptr = (pb_event_arg *)*arg;
  pb_event_arg event_arg;
  pb_istream_t stream_cpy = *stream;

  event_arg.sensor_ptr = arg_ptr->sensor_ptr;
  event_arg.sensor_id = arg_ptr->sensor_id;

  rv = get_msg_id(&stream_cpy, &event_arg);

  event_arg.suid.suid_high = arg_ptr->suid.suid_high;
  event_arg.suid.suid_low = arg_ptr->suid.suid_low;

  if(rv) {
    event.payload.funcs.decode = &decode_stream_event;
    event.payload.arg = &event_arg;

    if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
      LOGE("Error decoding Event: %s", PB_GET_ERROR(stream));
      rv = false;
    }
  }

  return rv;
}

/**
 * Decode suid field. This function is intended to be used
 * as a callback function during decode process
 */
bool decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_std_suid *uid = (sns_std_suid*)*arg;

  if(!pb_decode(stream, sns_std_suid_fields, uid)) {
    LOGE("Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

/**
 * Decode suid event field. This function is intended to be used
 * as a callback function during decode process
 */
bool decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);

  sns_suid_event suid_event;
  sns_buffer_arg data;
  sns_std_suid uid;
  uid.suid_high = 0;
  uid.suid_low = 0;

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.suid.arg = &uid;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event)) {
    LOGE("Error decoding SUID Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  char data_type[data.buf_len + 1];
  strlcpy(data_type, (char*)data.buf, sizeof(data_type));

  if(uid.suid_low == 0 && uid.suid_high == 0) {
    LOGW("Received SUID Event with data type '%s' NOT FOUND", data_type);
    return false;
  } else {
    LOGW("Received SUID Event with data type '%s'", data_type);
    LOGW("Received SUID Event with SUID %" PRIx64" %" PRIx64, uid.suid_low, uid.suid_high);

    if(strcmp(SSC_DATATYPE_RESAMPLER, data_type) == 0) {
      LOGW("Received resampler SUID");
      suid_resampler.suid_low = uid.suid_low;
      suid_resampler.suid_high = uid.suid_high;

      uid.suid_high = 0;
      uid.suid_low = 0;
    }
  }

  sns_std_suid *uid_to_return;
  uid_to_return = (sns_std_suid *)*arg;
  uid_to_return->suid_high = uid.suid_high;
  uid_to_return->suid_low = uid.suid_low;

  return true;
}

/**
 * Decode sns client event field. This function is intended to be used
 * as a callback function during decode process.
 *
 */
bool decode_suid_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_std_suid uid;
  sns_std_suid *uid_to_return;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  // PEND: Handle error events here
  /*
  pb_istream_t stream_cpy = *stream;
  pb_event_arg event_arg;

  get_msg_id(&stream_cpy, &event_arg);
  if(event_arg.msg_id != SNS_STD_ERROR_NO_ERROR) {
    LOGE("msg_id ERROR %i", event_arg.msg_id);
    return false;
  }
  */

  LOGW("Begin decoding suid event");

  event.payload.funcs.decode = decode_suid_event;
  event.payload.arg = (void *)&uid;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    LOGE("Error decoding Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  uid_to_return = (sns_std_suid *)*arg;
  uid_to_return->suid_high = uid.suid_high;
  uid_to_return->suid_low = uid.suid_low;
  LOGW("Suid Event decoding complete");

  return true;
}

} /* namespace chre */
