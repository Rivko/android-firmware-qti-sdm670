#pragma once
/**
 * @file sns_test_sensor.h
 *
 * The test virtual Sensor.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/test/inc/sns_test_sensor.h#4 $
 * $DateTime: 2018/05/22 10:22:33 $
 * $Change: 16226472 $
 *
 **/

#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_sensor.h"
#include "sns_sensor_util.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid_util.h"
#include "sns_test_std_sensor.h"

#define SNS_TEST_REQ_PAYLOAD_SIZE 256
#define SNS_TEST_DATA_SIZE 256

#define TEST_SUID 0x11,0xe8,0x65,0xd0,0xdd,0x70,0x4a,0x7e,\
                    0xaf,0x18,0x49,0x4e,0x3f,0x13,0x57,0x06

/* for sanity test on simulation */
#if defined(SNS_TEST_BUILD_GATING)
#define SNS_TEST_GRAVITY
#define NUM_EVENTS_TO_PROCESS 20
#define NUM_TEST_ITERATIONS 1
#else
/* for on-target test */
#define NUM_EVENTS_TO_PROCESS 10000
#define NUM_TEST_ITERATIONS 1
#endif

/** Forward Declaration of Sensor API */
sns_sensor_api sns_test_sensor_api;

sns_rc sns_test_init(sns_sensor *const this);
sns_rc sns_test_deinit(sns_sensor *const this);
sns_sensor_instance* sns_test_set_client_request(sns_sensor *const this,
                                                 struct sns_request const *exist_request,
                                                 struct sns_request const *new_request,
                                                 bool remove);

typedef void (*sns_test_create_request_func)( sns_sensor *sensor,
                                              void*, const pb_field_t**,
                                              uint32_t*, sns_std_request*);
typedef void (*sns_test_process_event_func)(const sns_sensor *sensor,
                                             void*, uint32_t, void*,
                                             uint32_t, sns_time);

typedef struct _sns_test_state
{
  sns_data_stream *sensor_stream;
  sns_data_stream *suid_stream;
  sns_diag_service *diag_service;
  int32_t remaining_events;
  int32_t remaining_iterations;
  uint32_t num_events_received;
  /* Sensor-specific */
  SNS_SUID_LOOKUP_DATA(4) suid_lookup_data;
  bool test_in_progress;
  uint64_t sns_pb_req_payload[SNS_TEST_REQ_PAYLOAD_SIZE/8];
  uint64_t test_data[SNS_TEST_DATA_SIZE/8];
  sns_test_create_request_func test_sensor_create_request;
  sns_test_process_event_func test_sensor_process_event;
} sns_test_state;

typedef struct sns_test_implementation
{
  char *datatype;
  uint32_t datatype_len;
  sns_test_create_request_func create_request_func;
  sns_test_process_event_func process_event_func;
} sns_test_implementation;
