#pragma once
/**
 * @file sns_test_std_sensor.h
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights 
 * Reserved. Confidential and Proprietary - Qualcomm 
 * Technologies, Inc. 
 *
 **/

#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_time.h"

/**
 * Sensor specific test data structure
 *
 * This is used for collecting any test diagnostic data while
 * test runs.
 *
 * Note: size of this struct shall not exceed SNS_TEST_DATA_SIZE
 */
typedef struct _sns_test_std_sensor_data
{
  /**
   * number of Senser events received
   */
  uint32_t num_events;
  /**
   * timestamp of last event received (in ticks)
   */
  sns_time last_ts;
  /**
   * running average of the delta between two consecutive
   *        samples (in ticks)
   */
  float avg_delta;
} sns_test_std_sensor_data;

/**
 * Create a request for Sensor
 *
 * @param[o] payload  request payload. size of the payload shall 
 *  not exceed SNS_TEST_REQ_PAYLOAD_SIZE
 * @param[o] payload_fields  pb fields for request message.
 * @param[o] message_id  request message ID
 * @param[o] std_req  sns_std_request for Sensor under test.
 *
 * return none
 */
void sns_test_std_sensor_create_request( sns_sensor *sensor,
                                   void *payload,
                                   const pb_field_t **payload_fields,
                                   uint32_t *message_id,
                                   sns_std_request *std_req);

/**
 * Processes events from the Sensor
 *
 * @param[i] event       input event
 * @param[i] event_len   input event length
 * @param[i] test_data   data specific to test
 * @param[i] message_id  event message ID
 * @param[i] timestamp   event timestamp
 *
 * return none
 */
void sns_test_std_sensor_process_event(const sns_sensor *sensor,
                                  void *event,
                                  uint32_t event_len,
                                  void *test_data,
                                  uint32_t message_id,
                                  sns_time timestamp);

/**
 * Create a request for on-change Sensor
 *
 * @param[o] payload  request payload. size of the payload shall 
 *  not exceed SNS_TEST_REQ_PAYLOAD_SIZE
 * @param[o] payload_fields  pb fields for request message.
 * @param[o] message_id  request message ID
 * @param[o] std_req  sns_std_request for Sensor under test.
 *
 * return none
 */
void sns_test_on_change_sensor_create_request(const sns_sensor *sensor,
                                   void* payload,
                                   const pb_field_t** payload_fields,
                                   uint32_t* message_id,
                                   sns_std_request *std_req);

