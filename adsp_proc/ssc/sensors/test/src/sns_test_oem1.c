/**
 * @file sns_test_oem1.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_test_oem1.h"
#include "sns_time.h"
#include "sns_types.h"

/**
 * See sns_test_oem1.h
 */
void sns_test_create_oem1_request(const sns_sensor *sensor,
                                     void *payload,
                                     const pb_field_t **payload_fields,
                                     uint32_t *message_id,
                                     sns_std_request *std_req)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(std_req);
  sns_std_sensor_config *config = payload;
  config->sample_rate = 13.0;

  *message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  *payload_fields = sns_std_sensor_config_fields;
}

/**
 * See sns_test_oem1.h
 */
void sns_test_oem1_process_event(const sns_sensor *sensor,
                                    void *event,
                                    uint32_t event_len,
                                    void *test_data,
                                    uint32_t message_id,
                                    sns_time timestamp)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(timestamp);
  sns_oem1_test_data *oem1_test_data = test_data;
  if(message_id == SNS_OEM1_MSGID_SNS_OEM1_DATA)
  {
    float oem1_payload[3] = { 0 };
    pb_istream_t stream =
      pb_istream_from_buffer((pb_byte_t*)event, event_len);
    sns_std_sensor_event stream_event = sns_std_sensor_event_init_default;
    uint8_t arr_index = 0;
    pb_float_arr_arg arg = {.arr = oem1_payload, .arr_len = 3, .arr_index = &arr_index};

    stream_event.data = (struct pb_callback_s)
      {.funcs.decode = &pb_decode_float_arr_cb, .arg = &arg};

    if(!pb_decode(&stream, sns_std_sensor_event_fields, &stream_event))
    {
      SNS_PRINTF(ERROR, sensor, "Error in decoding oem1 data event");
    }
    else
    {
      sns_memscpy(oem1_test_data->oem1_payload,
                  sizeof(oem1_test_data->oem1_payload),
                  oem1_payload,
                  sizeof(oem1_payload));
    }

    SNS_PRINTF(LOW, sensor, "OEM1 output: 1-%f, 2-%f, 3-%f",
        oem1_test_data->oem1_payload[0],
        oem1_test_data->oem1_payload[1],
        oem1_test_data->oem1_payload[2]);
  }
  else
  {
    SNS_PRINTF(ERROR, sensor, "Unexpected event");
  }
}
