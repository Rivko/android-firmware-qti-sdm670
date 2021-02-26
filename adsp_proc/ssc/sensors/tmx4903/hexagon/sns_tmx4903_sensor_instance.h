/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __SNS_TMX4903_SENSOR_INSTANCE__H__
#define __SNS_TMX4903_SENSOR_INSTANCE__H__

/** Forward Declaration of Instance API */
sns_sensor_instance_api ams_sensor_instance_api;

typedef struct ams_com_port_info
{
  sns_com_port_config     com_config;
  sns_sync_com_port_handle     *port_handle;
} ams_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  AMS_OC_IDX    = 0,
  AMS_STRM_IDX  = 1,
} ams_mode_index;

typedef enum
{
  /* on-change types */
  AMS_ALS_OC      = 0x0001,
  AMS_PROX_OC     = 0x0002,
  AMS_RGB_OC      = 0x0004,
  /* streaming types */
  AMS_ALS_STRM    = 0x0100,
  AMS_PROX_STRM   = 0x0200,
  AMS_RGB_STRM    = 0x0400,
} ams_sensor_type;
#define AMS_STREAMERS   0xff00
#define AMS_ON_CHANGERS 0x00ff
#define NEGATIVE_INFINITY  (-(float)HUGE_VAL) /* QC - doesn't -INFINITY work?  also, do not use double, use HUGE_VALF instead */

#define AMS_ALS_SELFTEST_USEC  2000000
#define AMS_PROX_SELFTEST_USEC  200000

#define ALS_PROTO_STD_ARRAY_LEN     2
#define RGB_PROTO_STD_ARRAY_LEN     6

typedef struct ams_sensor_client_cfg
{
  float                   chosen_sample_rate;
  float                   chosen_report_rate;
  bool                    oc_client_present;
  bool                    strm_client_present;
  int32_t                 num_oc_clients_prev;
  bool                    oc_send_last_event;
} ams_sensor_client_cfg;

typedef struct ams_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} ams_self_test_info;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
typedef struct ams_als_info
{
  ams_sensor_client_cfg   client_cfg;
  float                   prev_OC_report[ALS_PROTO_STD_ARRAY_LEN];
  sns_sensor_uid          suid[2]; /* use AMS_xxx_IDX as subscripts */
  ams_self_test_info      test_info[2];
  sns_std_sensor_sample_status status;
} ams_als_info;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
/* Variant of the std. type: includes a marker for the invalid state */
typedef enum _ams_proximity_event_type {
    AMS_PROXIMITY_EVENT_TYPE_INVALID = 0,
    AMS_PROXIMITY_EVENT_TYPE_NEAR = 1,
    AMS_PROXIMITY_EVENT_TYPE_FAR = 2
} ams_proximity_event_type;

typedef struct ams_prox_info
{
  ams_sensor_client_cfg   client_cfg;
  sns_sensor_uid          suid[2]; /* use AMS_xxx_IDX as subscripts */
  ams_proximity_event_type  prev_OC_report[1];
  sns_proximity_event     prox_state;
  ams_self_test_info      test_info[2];
} ams_prox_info;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
typedef struct ams_rgb_info
{
  ams_sensor_client_cfg   client_cfg;
  float                   prev_OC_report[RGB_PROTO_STD_ARRAY_LEN];
  sns_sensor_uid          suid[2]; /* use AMS_xxx_IDX as subscripts */
  ams_self_test_info      test_info[2];
  sns_std_sensor_sample_status status;
} ams_rgb_info;

#endif

typedef enum
{
  AMS_DRI_UNKNOWN,
  AMS_USING_DRI,
  AMS_NOT_USING_DRI,
} ams_dri_use;

typedef struct ams_irq_info
{
  ams_dri_use             dri_use;
  sns_interrupt_req       irq_config;
} ams_irq_info;


/** Private state. */
typedef struct ams_instance_state
{
  uint16_t publish_sensors;
  uint16_t publish_sensors_prev;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  /** ALS config details*/
  ams_als_info    als_info;
  size_t          encoded_als_event_len;
  size_t          als_enc_raw_log_size;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  /** Proximity config details*/
  ams_prox_info   prox_info;
  size_t          encoded_prox_event_len;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  /** RGB config details*/
  ams_rgb_info    rgb_info;
  size_t          encoded_rgb_event_len;
  size_t          rgb_enc_raw_log_size;
#endif
  /** Interrupt dependency info. */
  ams_irq_info        irq_info;

  /** COM port info */
  ams_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;

  sns_time             interrupt_timestamp;

  /** Data streams from dependencies. */
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_data_stream;

  sns_sensor_uid          irq_suid;
  sns_sensor_uid          timer_suid;

  sns_diag_service *diag_service;

  bool                    timer_is_active;
  float                   report_rate_hz;
  float                   sampling_rate_hz;
  sns_time                sampling_intvl;

  sns_sync_com_port_service *   scp_service;
  size_t                        log_interrupt_encoded_size;
  bool                          instance_is_ready_to_configure;
  bool                          new_self_test_request;
  ams_sensor_type               sensor_type_under_test;

  void                          *priv; // ptr to our deviceCtx
  ams_sensor_type               sensor_type;
  float                         device_sample_rate;
  AMS_PORT_portHndl             port_handle;
  AMS_PORT_portHndl             *port_handle_ptr;
  int32_t                       client_present;

  ams_calibrationData_t   calibration_data;
  uint32_t                cal_version[CALIBRATION_DATA_INDEX_MAX];
  bool                    new_or_chng_req;
} ams_instance_state;

typedef struct sns_std_req
{
  float desired_sample_rate;
  float desired_report_rate;
} sns_std_req;

/* QC - no need to featurerize based on SCEP version.
   Driver is not expected to be backwards compatible with older SCEP version */

typedef struct log_sensor_state_raw_info
{
  /* Pointer to diag service */
  sns_diag_service *diag;
  /* Pointer to sensor instance */
  sns_sensor_instance *instance;
  /* Pointer to sensor UID*/
  struct sns_sensor_uid *sensor_uid;
  /* Size of a single encoded sample */
  size_t encoded_sample_size;
  /* Pointer to log*/
  void *log;
  /* Size of allocated space for log*/
  uint32_t log_size;
  /* Number of actual bytes written*/
  uint32_t bytes_written;
  /* Number of batch samples written*/
  /* A batch may be composed of several logs*/
  uint32_t batch_sample_cnt;
  /* Number of log samples written*/
  uint32_t log_sample_cnt;
  /* AMS sensor type*/
  ams_sensor_type sensor_type;
} log_sensor_state_raw_info;

sns_rc ams_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

sns_rc ams_inst_deinit(sns_sensor_instance *const this);

/* For now, we only support a single device-wide polling rate */
bool ams_validate_device_odr(sns_sensor_instance *const instance, float *sample_rate);
#endif //__SNS_TMX4903_SENSOR_INSTANCE__H__

