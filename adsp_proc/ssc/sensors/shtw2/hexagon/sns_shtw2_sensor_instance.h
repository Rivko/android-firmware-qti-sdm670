#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <stdint.h>

#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_printf.h"

#define SHT_SENSOR_INST_PRINTF(diag, sensor, state, prio, filename, line, fmt, ...) \
  do { \
    if (diag != NULL && diag->api != NULL) { \
      SNS_INST_PRINTF(prio, sensor, fmt, ##__VA_ARGS__); \
    } \
  } while (0)

#define SHTW2_ODR_TO_INTERVAL_NS(o) (1000000000 / (o))
#define SHTW2_REQUEST_PAYLOAD_SIZE  50

/**
 * Thresholds for reporting.
 */
#define SHTW2_TEMP_CHANGE  0.5f
#define SHTW2_HUMIDITY_CHANGE 0.5f

/** Forward Declaration of Instance API */
sns_sensor_instance_api shtw2_sensor_instance_api;

typedef struct shtw2_com_port_info
{
  sns_com_port_config          com_config;
  sns_sync_com_port_handle     *port_handle;

} shtw2_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  SHTW2_TEMPERATURE     = 0x1,
  SHTW2_HUMIDITY        = 0x2
} shtw2_sensor_type;

typedef struct shtw2_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool                    test_client_present;
} shtw2_self_test_info;

typedef struct shtw2_temperature_info
{
  sns_sensor_uid          suid;
  bool                    enabled;
  bool                    report_sample;
  bool                    force_first_sample;
  float                   latest_reading;
  shtw2_self_test_info    test_info;
} shtw2_temperature_info;

typedef struct shtw2_humidity_info
{
  sns_sensor_uid          suid;
  bool                    enabled;
  bool                    report_sample;
  bool                    force_first_sample;
  float                   latest_reading;
  shtw2_self_test_info    test_info;
} shtw2_humidity_info;

typedef struct sns_shtw2_registry_cfg
{
  shtw2_sensor_type sensor_type;
  float             fac_cal_scale[1];
  float             fac_cal_bias[1];
  float             threshold;
} sns_shtw2_registry_cfg;

/**
 * Measurement timer states.
 */
typedef enum
{
  IDLE,
  MEASURING,
  WAITING
} shtw2_measurement_timer_state;

/** Private state. */
typedef struct shtw2_instance_state
{
  /* SHTW2 temperature HW config details */
  shtw2_temperature_info temperature_info;

  /* SHTW2 humidity HW config details */
  shtw2_humidity_info    humidity_info;

  /* COM port info */
  shtw2_com_port_info    com_port_info;

  /* Current output data rate */
  float                  current_odr;

  /* Data streams from dependencies. */
  sns_data_stream        *one_shot_timer_data_stream;
  sns_data_stream        *periodic_timer_data_stream;
  sns_sensor_uid         timer_suid;

  uint32_t               client_req_id;
  sns_std_sensor_config  sensor_req;

  size_t                 encoded_sensor_event_len;

  sns_diag_service       *diag_service;
  sns_sync_com_port_service *scp_service;
  bool                   instance_is_ready_to_configure;

  shtw2_measurement_timer_state timer_state;
  size_t                 log_raw_encoded_size;

  bool                   new_self_test_request;

  sns_shtw2_registry_cfg temperature_registry_cfg;
  sns_shtw2_registry_cfg humidity_registry_cfg;

} shtw2_instance_state;

typedef struct sns_shtw2_request
{
  float                  sample_rate;
  sns_shtw2_registry_cfg registry_cfg;
} sns_shtw2_request;

sns_rc shtw2_inst_init(sns_sensor_instance *const this,
                       sns_sensor_state const *sstate);

sns_rc shtw2_inst_deinit(sns_sensor_instance *const this);

sns_rc shtw2_init_measurement(sns_sensor_instance *const this);

sns_rc shtw2_inst_set_client_config(sns_sensor_instance *const this,
                                    sns_request const *client_request);

sns_rc shtw2_measurement_timer_set(sns_time timeout_ticks,
                                   sns_data_stream *timer_data_stream,
                                   bool is_periodic);

void shtw2_stop_all_timers(sns_sensor_instance *const this);
