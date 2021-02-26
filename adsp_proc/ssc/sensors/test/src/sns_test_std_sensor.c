/**
 * @file sns_test_std_sensor.c
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/
#include "sns_cal.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_test_sensor.h"
#include "sns_test_std_sensor.h"
#include "sns_types.h"

static const float TEST_SAMPLE_RATE = 1.0f;
static const float TEST_BATCH_PERIOD = 1000000.0f;

/** See sns_test_std_sensor.h */
void sns_test_std_sensor_create_request(sns_sensor *sensor,
                                   void* payload,
                                   const pb_field_t** payload_fields,
                                   uint32_t* message_id,
                                   sns_std_request *std_req)
{
  UNUSED_VAR(sensor);
  sns_std_sensor_config* config = (sns_std_sensor_config*)payload;
  config->sample_rate = TEST_SAMPLE_RATE;
  *message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  *payload_fields = sns_std_sensor_config_fields;
  std_req->has_batching = true;
  std_req->batching.batch_period = TEST_BATCH_PERIOD;
}

/** See sns_test_std_sensor.h */
void sns_test_on_change_sensor_create_request(const sns_sensor *sensor,
                                   void* payload,
                                   const pb_field_t** payload_fields,
                                   uint32_t* message_id,
                                   sns_std_request *std_req)
{
  UNUSED_VAR(sensor);
  UNUSED_VAR(std_req);
  UNUSED_VAR(payload);
  UNUSED_VAR(payload_fields);
  *message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
}
