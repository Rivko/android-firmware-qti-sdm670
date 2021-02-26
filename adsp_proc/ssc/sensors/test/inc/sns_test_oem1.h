#pragma once
/**
 * @file sns_test_oem1.h
 *
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *
 **/

#include "sns_mem_util.h"
#include "sns_oem1.pb.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_stream_service.h"
#include "sns_time.h"

/** Test Sensor State. */

typedef struct sns_oem1_test_data
{
  float               oem1_payload[3];
  sns_std_sensor_sample_status   accuracy;
  uint64_t            num_data_events;
  sns_time            ts;
  sns_time            last_ts;
  sns_time            delta_sum_ts;
  float               mean_time;
} sns_oem1_test_data;

/**
 * Create sensor request payload to be sent to sensor being
 * tested
 *
 * @param[io] payload - Pointer to payload buffer in which the
 *   request payload is passed back to the test framework.
 *   This payload is passed to the payload field in the
 *   sns_std_request as defined in sns_std.proto
 *   The length of the payload may not exceed
 *   SNS_TEST_REQ_PAYLOAD_LEN
 * @param[io] payload_fields - Protocol Buffer field descriptor
 *   that describes the payload
 * @param[io] message_id - Message ID to be sent with the
 *   request as defined in the .proto api of the sensor being
 *   tested
 * @param[io] std_req - sns_std_request for Sensor under test.
 *
 * @return None
 */
void sns_test_create_oem1_request(const sns_sensor *sensor,
                                     void *sensor_req,
                                     const pb_field_t **payload_fields,
                                     uint32_t *message_id,
                                     sns_std_request *std_req);

/**
 * Process events coming from the sensor being tested
 *
 * @param[i] event - Protocol Buffer Encoded message received
 * from the sensor under test.
 * @param[i] event_len - Length of the encoded event message.
 * @param[i] test_data - Pointer to opaque test buffer that can
 *  be used to store test diagnostic information. The test
 *  buffer is allocated on the test sensor state strucure and
 *  has a length of SNS_TEST_DATA_LEN
 * @param[i] message_id - Message ID of the input event
 * @param[i] timestamp - Timestamp of the input event
 *
 * @return None
 */
void sns_test_oem1_process_event(const sns_sensor *sensor,
                                    void *event,
                                    uint32_t event_len,
                                    void *test_data,
                                    uint32_t message_id,
                                    sns_time timestamp);
