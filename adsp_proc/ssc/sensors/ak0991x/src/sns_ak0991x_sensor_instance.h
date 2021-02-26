#pragma once
/**
 * @file sns_ak0991x_sensor_instance.h
 *
 * AK0991X Mag virtual Sensor Instance implementation.
 *
 * Copyright (c) 2016-2017 Asahi Kasei Microdevices
 * All Rights Reserved.
 * Confidential and Proprietary - Asahi Kasei Microdevices
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include <stdint.h>
#include "sns_printf.h"

#include "sns_diag_service.h"
#include "sns_interrupt.pb.h"
#include "sns_timer.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_ak0991x_lite.h"

#include "sns_ak0991x_dae_if.h"

#include "sns_async_com_port.pb.h"
#include "sns_physical_sensor_test.pb.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
#include "sns_device_mode.pb.h"
#endif //AK0991X_ENABLE_DEVICE_MODE_SENSOR


/** Forward Declaration of Instance API */
extern sns_sensor_instance_api ak0991x_sensor_instance_api;

/** Number of entries in reg_map table. */
#define AK0991X_REG_MAP_TABLE_SIZE  (7)

/** AK0991X max number of settings */
#define AK0991X_MAX_NUM_REP_MODE    3
#define AK0991X_MAX_NUM_OPE_MODE    3
#define AK0991X_MAX_NUM_ODR         6

#ifdef AK0991X_ENABLE_DEBUG_MSG
#define AK0991X_PRINT(prio, sensor, ...) do { \
  SNS_PRINTF(prio, sensor, __VA_ARGS__); \
} while (0)

#define AK0991X_INST_PRINT(prio, inst, ...) do { \
  SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
} while (0)
#else // AK0991X_ENABLE_DEBUG_MSG
#define AK0991X_PRINT(prio,...)
#define AK0991X_INST_PRINT(prio,...)
#endif // AK0991X_ENABLE_DEBUG_MSG

/** Supported AKM Devices */
typedef enum
{
  AK09911,
  AK09912,
  AK09913,
  AK09915C,
  AK09915D,
  AK09916C,
  AK09916D,
  AK09917,
  AK09918,
  SUPPORTED_DEVICES
} akm_device_type;

/**
 * AK0991X output data rate for mag
 */
typedef enum
{
  AK0991X_MAG_ODR_OFF = 0x00,      /* power down output data rate */
  AK0991X_MAG_ODR_SNG_MEAS = 0x01, /* single measurement mode */
  AK0991X_MAG_ODR10 = 0x02,        /* 10 Hz output data rate */
  AK0991X_MAG_ODR20 = 0x04,        /* 20 Hz output data rate */
  AK0991X_MAG_ODR50 = 0x06,        /* 50 Hz output data rate */
  AK0991X_MAG_ODR100 = 0x08,       /* 100 Hz output data rate */
  AK0991X_MAG_ODR200 = 0x0A,       /* 200 Hz output data rate */
  AK0991X_MAG_ODR1 = 0x0C,         /* 1 Hz output data rate */
  AK0991X_MAG_SELFTEST = 0x10,     /* selftest */
  AK0991X_MAG_FUSEROM = 0x1F,      /* FUSE ROM access mode */
} ak0991x_mag_odr;


typedef float ak0991x_mag_sstvt;

typedef struct ak0991x_com_port_info
{
  sns_com_port_config      com_config;
  sns_sync_com_port_handle *port_handle;
} ak0991x_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr
{
  float min;
  float max;
} range_attr;

typedef enum
{
  AK0991X_CONFIG_IDLE,              /** not configuring */
  AK0991X_CONFIG_POWERING_DOWN,     /** cleaning up when no clients left */
  AK0991X_CONFIG_STOPPING_STREAM,   /** stream stop initiated, waiting for completion */
  AK0991X_CONFIG_FLUSHING_HW,       /** FIFO flush initiated, waiting for completion */
  AK0991X_CONFIG_UPDATING_HW        /** updating sensor HW, when done goes back to IDLE */
} ak0991x_config_step;

#ifdef AK0991X_ENABLE_S4S
typedef enum
{
  AK0991X_S4S_NOT_SYNCED,
  AK0991X_S4S_SYNCING,
  AK0991X_S4S_1ST_SYNCED,
  AK0991X_S4S_SYNCED
} ak0991x_s4s_state;
#endif // AK0991X_ENABLE_S4S

typedef struct ak0991x_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} ak0991x_self_test_info;

typedef struct ak0991x_mag_info
{
  ak0991x_mag_odr   desired_odr;
  ak0991x_mag_odr   curr_odr;
  uint32_t          flush_period;
  ak0991x_mag_sstvt sstvt_adj[3];
  ak0991x_mag_sstvt resolution;
  akm_device_type   device_select;
  uint32_t       req_wmk;
  uint16_t       cur_wmk;
  uint16_t       max_fifo_size;
  bool           use_dri;
  bool           use_fifo;
  bool           flush_only;
  uint8_t        nsf;
  uint8_t        sdr;
  sns_sensor_uid suid;
  ak0991x_self_test_info test_info;
  bool                   use_sync_stream;
  uint32_t      data_count;
  uint32_t      clock_error_meas_count;

#ifdef AK0991X_ENABLE_S4S
  ak0991x_s4s_state      s4s_sync_state;
  uint8_t                s4s_rr;
  bool                   s4s_dt_abort;
#endif // AK0991X_ENABLE_S4S
} ak0991x_mag_info;

typedef struct ak0991x_irq_info
{
  sns_interrupt_req irq_config;
  bool is_registered;
  bool is_ready;
  bool detect_irq_event;
} ak0991x_irq_info;


typedef struct ak0991x_cal_param
{
  bool registry_fac_cal_received;
  matrix3 fac_cal_corr_mat;
  float fac_cal_bias[TRIAXIS_NUM];
  float fac_cal_scale[TRIAXIS_NUM];
}ak0991x_cal_param;


typedef struct ak0991x_async_com_port_info
{
  uint32_t port_handle;
} ak0991x_async_com_port_info;

typedef struct sns_ak0991x_registry_cfg
{
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];

  uint32_t            version;
}sns_ak0991x_registry_cfg;

/** Private state. */
typedef struct ak0991x_instance_state
{
  /** mag HW config details*/
  ak0991x_mag_info mag_info;
  uint32_t total_samples; /* throughout the life of this instance */

  /** sampling info. */
  uint8_t num_samples;
  uint8_t ascp_xfer_in_progress;
  uint8_t heart_beat_sample_count;
  uint8_t heart_beat_attempt_count;
  uint8_t flush_sample_count;
  bool this_is_first_data;
  bool data_over_run;
  bool data_is_ready;
  bool re_read_data_after_ascp;
  bool fifo_flush_in_progress;
  bool new_self_test_request;
  bool config_mag_after_ascp_xfer;
  bool this_is_the_last_flush;
  bool reg_event_done;
#ifdef AK0991X_ENABLE_S4S
  bool s4s_reg_event_done;
#endif
  bool is_temp_average;
  bool in_clock_error_procedure;
  bool previous_meas_is_irq;
  bool previous_meas_is_correct_wm;
  sns_time interrupt_timestamp;
  sns_time irq_event_time;
  sns_time pre_timestamp;
  sns_time first_data_ts_of_batch;
  sns_time averaged_interval;
  sns_time temp_averaged_interval;
  sns_time system_time;
  sns_time previous_irq_time;
  sns_time heart_beat_timestamp;
  sns_time heart_beat_timeout_period;
  sns_time nominal_intvl;
  sns_time half_measurement_time;
  sns_time hb_timer_fire_time;
  sns_timer_sensor_config req_payload;
  int64_t internal_clock_error;

  /** Timer info */
  sns_sensor_uid timer_suid;

  /** Interrupt dependency info. */
  ak0991x_irq_info irq_info;

  /** COM port info */
  ak0991x_com_port_info com_port_info;

#if defined(AK0991X_ENABLE_DRI) || defined(AK0991X_ENABLE_FIFO)
  /**--------Async Com Port--------*/
  ak0991x_async_com_port_info async_com_port_info;
  sns_async_com_port_config ascp_config;
#endif // AK0991X_ENABLE_DRI || AK0991X_ENABLE_FIFO

  /**--------DAE interface---------*/
  ak0991x_dae_if_info       dae_if;
  ak0991x_config_step       config_step;

  /** Data streams from dependencies. */
  sns_data_stream       *timer_data_stream;
#ifdef AK0991X_ENABLE_DRI
  sns_data_stream       *interrupt_data_stream;
  sns_data_stream       *async_com_port_data_stream;
#endif // AK0991X_ENABLE_DRI
#ifdef AK0991X_ENABLE_S4S
  sns_data_stream       *s4s_timer_data_stream;
#endif // AK0991X_ENABLE_S4S

  uint32_t              client_req_id;
  sns_std_sensor_config mag_req;
  int16_t               pre_lsbdata[TRIAXIS_NUM];

  size_t encoded_mag_event_len;

  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_ak0991x_registry_cfg mag_registry_cfg;

#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
  sns_data_stream       *device_mode_stream;
  ak0991x_cal_param     cal_parameter[MAX_DEVICE_MODE_SUPPORTED];
  uint32_t              cal_id;
  sns_device_mode_event_mode_spec  device_mode[MAX_DEVICE_MODE_SUPPORTED];
  uint32_t device_mode_cnt;
#endif //AK0991X_ENABLE_DEVICE_MODE_SENSOR

  sns_diag_service *diag_service;
  sns_sync_com_port_service *scp_service;
  size_t           log_raw_encoded_size;

} ak0991x_instance_state;

typedef struct odr_reg_map
{
  float           odr;
  ak0991x_mag_odr mag_odr_reg_value;
  uint8_t         discard_samples;
} odr_reg_map;

typedef struct sns_ak0991x_mag_req
{
  float sample_rate;
  float report_rate;
  uint32_t flush_period;
  bool is_flush_only;
  sns_ak0991x_registry_cfg registry_cfg;
} sns_ak0991x_mag_req;


sns_rc ak0991x_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

sns_rc ak0991x_inst_deinit(sns_sensor_instance *const this);

sns_rc ak0991x_inst_set_client_config(sns_sensor_instance *const this,
                                      sns_request const *client_request);
