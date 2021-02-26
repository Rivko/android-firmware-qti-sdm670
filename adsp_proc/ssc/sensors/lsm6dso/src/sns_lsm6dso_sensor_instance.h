#pragma once
/**
 * @file sns_lsm6dso_sensor_instance.h
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/

#include <stdint.h>

#include "sns_async_com_port.pb.h"
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_interrupt.pb.h"
#include "sns_lsm6dso_dae_if.h"
#include "sns_math_util.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_printf.h"
#include "sns_registry_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"

/** Forward Declaration of Instance API */
sns_sensor_instance_api lsm6dso_sensor_instance_api;

/** Number of registers to read for debug */
#define LSM6DSO_DEBUG_REGISTERS          (32)

/** Number of entries in reg_map table. */
#define LSM6DSO_REG_MAP_TABLE_SIZE       (11)
#define BUILD_DB         0
#define LSM6DSO_DUMP_REG 0
#define LSM6DSO_LOG_VERBOSE 0
#define LSM6DSO_DEBUG_TS    0
#define SELF_TEST_DATA_COUNT_MAX 25
#define LSM6DSO_DEBUG    0
#define LSM6DSO_DATA_LOG_ENABLE 0

#if LSM6DSO_DEBUG_TS
#define LSM6DSO_INST_DEBUG_TS(prio, inst, ...) do { \
SNS_INST_PRINTF(prio, inst, __VA_ARGS__); \
} while (0)

#else
#define LSM6DSO_INST_DEBUG_TS(prio, sensor, ...)
#endif

//QC change to enable MD only
#define LSM6DSO_MD_SAMPLE_RATE           (13)             //Default sample rate for MD
#define LSM6DSO_MD_REPORT_PERIOD           (1000 *1000) //Default report  period for MD in secs conv to msec

#if LSM6DSO_LOG_VERBOSE

/** TODO: consider removing unsed parameters from these macros like diag, file, line. */
#define DBG_PRINT(diag, sensor, prio, file, line, ...) do { \
  (void)diag; \
  SNS_PRINTF(prio, sensor, __VA_ARGS__); \
} while (0)

#define DBG_INST_PRINT(inst, prio, file, line, ...) do { \
  SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
} while (0)


#else
#define DBG_PRINT(sensor,...) UNUSED_VAR(sensor);
#define DBG_INST_PRINT(inst,...) UNUSED_VAR(inst);
#endif
/**
 * Accelerometer LSM6DSO_ACC Full Scales in register setting.
 */
typedef enum
{
  LSM6DSO_ACCEL_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
  LSM6DSO_ACCEL_RANGE_4G   = 0x08,
  LSM6DSO_ACCEL_RANGE_8G   = 0x0C,
  LSM6DSO_ACCEL_RANGE_16G   = 0x04,
} lsm6dso_accel_range;

/**
 * Accelerometer LSM6DSO_ACC sensitivity for each range.
 */
typedef enum
{
  LSM6DSO_ACCEL_SSTVT_2G  = 61,   /* in the unit of micro-g/digit */
  LSM6DSO_ACCEL_SSTVT_4G  = 122,
  LSM6DSO_ACCEL_SSTVT_8G  = 244,
  LSM6DSO_ACCEL_SSTVT_16G = 488,
} lsm6dso_accel_sstvt;

/**
 * Accelerometer LSM6DSO ACC filter bandwidth in register setting
 */
typedef enum
{
  LSM6DSO_ACCEL_BW0       = 0x00,  /* 00 Hz bandwidth */
  LSM6DSO_ACCEL_BW13      = 0x00,  /* 13 Hz bandwidth */
  LSM6DSO_ACCEL_BW26      = 0x00,  /* 26 Hz bandwidth */
  LSM6DSO_ACCEL_BW50      = 0x00,  /* 50 Hz bandwidth */
  LSM6DSO_ACCEL_BW100     = 0x00,  /* 100 Hz bandwidth */
  LSM6DSO_ACCEL_BW200     = 0x00,  /* 200 Hz bandwidth */
  LSM6DSO_ACCEL_BW400     = 0x00,  /* 400 Hz bandwidth */
  LSM6DSO_ACCEL_BW800     = 0x00,  /* 800 Hz bandwidth */
  LSM6DSO_ACCEL_BW1600    = 0x01,  /* 1600 Hz bandwidth */
  LSM6DSO_ACCEL_BW3300    = 0x01,  /* 3300 Hz bandwidth */
  LSM6DSO_ACCEL_BW6600    = 0x01   /* 6600 Hz bandwidth */
} lsm6dso_accel_bw;
#if 0
typedef enum
{
  LSM6DSO_ODR_0      = 0 ,  /* power down output data rate */
  LSM6DSO_ODR_13     = 13,  /* 13 Hz output data rate */
  LSM6DSO_ODR_26     = 26,  /* 26 Hz output data rate */
  LSM6DSO_ODR_52     = 52,  /* 52 Hz output data rate */
  LSM6DSO_ODR_104    = 104,  /* 104 Hz output data rate */
  LSM6DSO_ODR_208    = 208,  /* 208 Hz output data rate */
  LSM6DSO_ODR_416    = 416,  /* 416 Hz output data rate */
  LSM6DSO_ODR_833    = 833,  /* 833 Hz output data rate */
  LSM6DSO_ODR_1660   = 1660,  /* 1.66 kHz output data rate */
  LSM6DSO_ODR_3330   = 3330,  /* 3.33 kHz output data rate */
  LSM6DSO_ODR_6660   = 6660,  /* 6.66 kHz output data rate */
} lsm6dso_odr;
#endif
/**
 * Accelerometer LSM6DSO_ACC output data rate in register setting
 */
typedef enum
{
  LSM6DSO_ACCEL_ODR_OFF   = 0x00,  /* power down output data rate */
  LSM6DSO_ACCEL_ODR13     = 0x10,  /* 13 Hz output data rate */
  LSM6DSO_ACCEL_ODR26     = 0x20,  /* 26 Hz output data rate */
  LSM6DSO_ACCEL_ODR52     = 0x30,  /* 52 Hz output data rate */
  LSM6DSO_ACCEL_ODR104    = 0x40,  /* 104 Hz output data rate */
  LSM6DSO_ACCEL_ODR208    = 0x50,  /* 208 Hz output data rate */
  LSM6DSO_ACCEL_ODR416    = 0x60,  /* 416 Hz output data rate */
  LSM6DSO_ACCEL_ODR833    = 0x70,  /* 833 Hz output data rate */
  LSM6DSO_ACCEL_ODR1660   = 0x80,  /* 1.66 kHz output data rate */
  LSM6DSO_ACCEL_ODR3330   = 0x90,  /* 3.33 kHz output data rate */
  LSM6DSO_ACCEL_ODR6660   = 0xA0,  /* 6.66 kHz output data rate */
} lsm6dso_accel_odr;

/**
 * LSM6DSO output data rate for gyro, Disabling LPF2,
 * so BW setting is not required
 */
typedef enum
{
  LSM6DSO_GYRO_ODR_OFF = 0x00,       /* power down output data rate */
  LSM6DSO_GYRO_ODR13   = 0x10,       /* 13 Hz output data rate */
  LSM6DSO_GYRO_ODR26   = 0x20,       /* 26 Hz output data rate */
  LSM6DSO_GYRO_ODR52   = 0x30,       /* 52 Hz output data rate */
  LSM6DSO_GYRO_ODR104  = 0x40,       /* 104 Hz output data rate */
  LSM6DSO_GYRO_ODR208  = 0x50,       /* 208 Hz output data rate */
  LSM6DSO_GYRO_ODR416  = 0x60,       /* 416 Hz output data rate */
  LSM6DSO_GYRO_ODR833  = 0x70,       /* 833 Hz output data rate */
  LSM6DSO_GYRO_ODR1660 = 0x80,       /* 1.66 kHz output data rate */
} lsm6dso_gyro_odr;

/**
 * LSM6DSO Full Scales in register setting for gyro
 */
typedef enum
{
  STM_LSM6DSO_GYRO_RANGE_125DPS   = 0x02,
  STM_LSM6DSO_GYRO_RANGE_245DPS   = 0x00,  /*corresponding value in register setting*/
  STM_LSM6DSO_GYRO_RANGE_500DPS   = 0x04,
  STM_LSM6DSO_GYRO_RANGE_1000DPS  = 0x08,
  STM_LSM6DSO_GYRO_RANGE_2000DPS  = 0x0C,
} lsm6dso_gyro_range;

typedef float lsm6dso_gyro_sstvt;

typedef struct lsm6dso_com_port_info
{
  sns_com_port_config     com_config;
  sns_sync_com_port_handle     *port_handle;

} lsm6dso_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum {
  FILTER_DEFAULT ,
  SLOPE_FILTER ,
  HP_FILTER,
}lsm6dso_md_filter_type;
typedef enum {
  DISABLED, //md disabled
  ENABLED_INT, //md enabled with interrupt
  ENABLED, //md enabled, no interrupt
}lsm6dso_md_state;
typedef enum
{
  LSM6DSO_ACCEL         = 0x1,
  LSM6DSO_GYRO          = 0x2,
  LSM6DSO_MOTION_DETECT = 0x4,
  LSM6DSO_SENSOR_TEMP   = 0x8
} lsm6dso_sensor_type;

typedef enum
{
  LSM6DSO_CONFIG_IDLE,            /** not configuring */
  LSM6DSO_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  LSM6DSO_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  LSM6DSO_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  LSM6DSO_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} lsm6dso_config_step;

/*contains fifo reading req information
 * either interrupt or flush */

typedef struct lsm6dso_fifo_req
{
  /** timestamp when fifo interrupt fired*/
  bool interrupt_fired;
  sns_time  interrupt_ts;
  bool flush_req;
  bool flush_client_present;
  sns_time  use_time;
  sns_time  sampling_intvl;
}lsm6dso_fifo_req;

/** HW FIFO information */
typedef struct lsm6dso_fifo_info
{
  /** FIFO enabled or not. Uses lsm6dso_sensor_type as bit mask
   *  to determine which FIFO Sensors are enabled */
  uint8_t fifo_enabled;

  uint8_t int1_ctrl_reg;

  /** Determines which Sensor data to publish. Uses
   *  lsm6dso_sensor_type as bit mask. */
  uint8_t publish_sensors;

  /** instance publish sensors */
  uint8_t inst_publish_sensors;

  /** fifo cur rate index */
  lsm6dso_accel_odr fifo_rate;

  /** fifo desired rate index */
  lsm6dso_accel_odr desired_fifo_rate;

  /** desired FIFO watermark levels for accel and gyro*/
  uint16_t desired_wmk;

  /** FIFO watermark levels for accel and gyro*/
  uint16_t cur_wmk;

  /** is fifo reconfiguration is req for the new config req*/
  bool reconfig_req;

  /** to recheck if int is high after handling interrupt*/
  bool recheck_int;

  /** expected sample boundary*/
  sns_time sampleTime_exp;
  /** reconfig time active */
  bool timer_active;

  /** max requested FIFO watermark levels; possibly larger than max HW FIFO */
  uint32_t max_requested_wmk;

  /** max flush ticks*/
  uint64_t max_requested_flush_ticks;
  /** timestamp of last sample sent to framework*/
  sns_time             last_timestamp;
  /** ascp event timestamp
   * ascp fills timestamp once reading completes */
  sns_time                   ascp_event_timestamp;
  /** timestamp when fifo interrupt fired*/
  sns_time                   prev_interrupt_timestamp;
  /** timestamp when fifo interrupt fired*/
  sns_time                   interrupt_timestamp;
  /** number of interrupts fired without reconfig*/
  uint16_t                   interrupt_cnt;
  /** avg interrupt interval without reconfiguring*/
  uint32_t                   avg_interrupt_intvl;
  /** avg sampling interval without reconfiguring*/
  sns_time                   avg_sampling_intvl;
  /*contains info before sending ascp req*/
  lsm6dso_fifo_req th_info;
  /*contains info to use after ascp returns */
  lsm6dso_fifo_req bh_info;

} lsm6dso_fifo_info;

typedef enum
{
  LSM6DSO_MODE_POLLING    = 0x1,
  LSM6DSO_MODE_FIFO       = 0x2,
  LSM6DSO_MODE_SELF_TEST  = 0x4
} lsm6dso_streaming_mode;

typedef enum
{
  LSM6DSO_SELF_TEST_STAGE_1    = 0x1,
  LSM6DSO_SELF_TEST_STAGE_2    = 0x2,
  LSM6DSO_SELF_TEST_STAGE_3    = 0x3,
  LSM6DSO_SELF_TEST_STAGE_4    = 0x4,
  LSM6DSO_SELF_TEST_STAGE_5    = 0x5,
} lsm6dso_self_test_stage;

typedef struct lsm6dso_common_info
{
  uint8_t desired_odr_idx;
  lsm6dso_accel_odr accel_curr_odr;
  lsm6dso_gyro_odr gyro_curr_odr;
  sns_time  accel_odr_settime;
  sns_time  gyro_odr_settime;
  uint8_t mode;
  //add few more hre
} lsm6dso_common_info;

typedef struct lsm6dso_self_test_info
{
  bool test_alive;
  bool reconfig_postpone;
  lsm6dso_self_test_stage  self_test_stage;
  uint16_t  polling_count;
  float cumulative_data_pre[3];
  float cumulative_data_post[3];
  lsm6dso_sensor_type sensor;
  lsm6dso_accel_odr curr_odr;
  uint8_t odr_idx;
  uint8_t skip_count;
  sns_physical_sensor_test_type test_type;
  bool update_registry;
  //add few more hre
} lsm6dso_self_test_info;

typedef struct lsm6dso_accel_info
{
  lsm6dso_accel_odr desired_odr;
  lsm6dso_accel_sstvt     sstvt;
  lsm6dso_accel_range     range;
  uint8_t                 range_idx;
  lsm6dso_accel_bw        bw;
  bool                    lp_mode;
  bool                    self_test_is_successful;
  sns_sensor_uid          suid;
  uint16_t                num_samples_to_discard;
  bool                    gated_client_present;
  uint8_t                 opdata_raw[6];
  sns_std_sensor_sample_status opdata_status;
} lsm6dso_accel_info;

typedef struct lsm6dso_gyro_info
{
  lsm6dso_gyro_odr desired_odr;
  lsm6dso_gyro_sstvt      sstvt;
  lsm6dso_gyro_range      range;
  uint8_t                 range_idx;
  bool                    is_in_sleep;
  bool                    self_test_is_successful;
  sns_sensor_uid          suid;
  uint16_t                num_samples_to_discard;
  uint8_t                 opdata_raw[6];
  sns_std_sensor_sample_status opdata_status;
} lsm6dso_gyro_info;

typedef struct lsm6dso_motion_detect_info
{
  uint16_t                desired_wmk;
  lsm6dso_accel_odr       desired_odr;
  lsm6dso_accel_sstvt     sstvt;
  lsm6dso_accel_range     range;
  lsm6dso_accel_bw        bw;
  sns_sensor_uid          suid;
  bool                    self_test_is_successful;
  bool                    enable_md_int;
  bool                    md_client_present;
  bool                    md_new_req;
  sns_motion_detect_event cur_md_state;
  sns_motion_detect_event prev_md_state;
  lsm6dso_md_filter_type filter;
  bool is_filter_settled;
  sns_registry_md_cfg     md_config;
} lsm6dso_motion_detect_info;

typedef struct lsm6dso_sensor_temp_info
{
  sns_sensor_uid          suid;
  bool                    timer_is_active;
  bool                    self_test_is_successful;
  float                   report_rate_hz;
  float                   desired_sampling_rate_hz;
  float                   cur_sampling_rate_hz;
  sns_time                sampling_intvl;
  uint64_t                max_requested_flush_ticks;
} lsm6dso_sensor_temp_info;

typedef struct lsm6dso_irq_info
{
  sns_interrupt_req       irq_config;
  bool                    irq_registered:1;
  bool                    irq_ready:1;
} lsm6dso_irq_info;

typedef struct lsm6dso_async_com_port_info
{
  uint32_t                port_handle;
}lsm6dso_async_com_port_info;

typedef struct sns_lsm6dso_registry_cfg
{
  lsm6dso_sensor_type sensor_type;
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];
  uint32_t            registry_persist_version;
  uint32_t            registry_instance_version;
}sns_lsm6dso_registry_cfg;

typedef struct lsm6dso_hw_config
{
  //value of a/g odr in HZ
  uint16_t odr;
  //fifo odr - if set fifo mode, polling
  uint16_t fifo_odr;
  //set wmk
  uint16_t wmk;
  //decides which sensors are enabled
  uint8_t enabled_sensors;
  //md feature enabled, set only md intr enabled
  bool md_enabled;

 } lsm6dso_hw_config;

typedef struct sns_lsm6dso_registry_reset
{
  lsm6dso_sensor_type sensor_type;
  bool            request;
}sns_lsm6dso_registry_reset;

typedef struct lsm6dso_health
{
  bool heart_attack;
  uint8_t heart_attack_cnt;
} lsm6dso_health;

/** Private state. */
typedef struct lsm6dso_instance_state
{
  uint8_t *fifo_start_address;

  /** detial about self test params*/
  lsm6dso_self_test_info self_test_info;

  /** Debug counter for sample count */
  uint32_t accel_sample_counter;

  /** Debug counter for sample count */
  uint32_t gyro_sample_counter;

/** Determines which Sensor requested for config change
   *  lsm6dso_sensor_type as bit mask. */
  uint8_t config_sensors;

  /** detial about common params*/
  lsm6dso_common_info common_info;
  /** fifo details*/
  lsm6dso_fifo_info       fifo_info;

  /** accel HW config details*/
  lsm6dso_accel_info      accel_info;

  /** gyro HW config details*/
  lsm6dso_gyro_info       gyro_info;

  /** motion detect info */
  lsm6dso_motion_detect_info md_info;

  /** Sensor Temperature config details. */
  lsm6dso_sensor_temp_info sensor_temp_info;

  /** current instance configuration */
  lsm6dso_hw_config current_conf;

  /** desired instance configuration */
  lsm6dso_hw_config desired_conf;

  /** Interrupt dependency info. */
  lsm6dso_irq_info        irq_info;

  /** COM port info */
  lsm6dso_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;
  int16_t ascp_req_count;

  /**--------DAE interface---------*/
  lsm6dso_dae_if_info       dae_if;
  lsm6dso_config_step       config_step;

  bool instance_is_ready_to_configure;

  /** Data streams from dependentcies. */
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_sensor_temp_data_stream;
  sns_data_stream      *timer_self_test_data_stream;
  sns_data_stream      *timer_md_data_stream;
  sns_data_stream      *async_com_port_data_stream;
  sns_data_stream      *timer_heart_beat_data_stream;

  sns_sensor_uid          timer_suid;

  lsm6dso_health  health;
  uint64_t  heart_beat_timeout;

  size_t               encoded_imu_event_len;
  size_t               encoded_sensor_temp_event_len;

  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_lsm6dso_registry_cfg accel_registry_cfg;
  sns_lsm6dso_registry_cfg gyro_registry_cfg;
  sns_lsm6dso_registry_cfg sensor_temp_registry_cfg;
  sns_lsm6dso_registry_reset registry_reset;

  /**----------debug----------*/
  uint32_t  num_accel_samples;
  uint32_t  num_gyro_samples;
  uint32_t  num_temp_samples;
  uint32_t  num_md_ints;
  uint32_t  num_ascp_null_events;
  uint8_t   reg_status[LSM6DSO_DEBUG_REGISTERS];

  sns_diag_service *diag_service;
  sns_sync_com_port_service * scp_service;
  size_t           log_interrupt_encoded_size;

  size_t           log_raw_encoded_size;
  size_t           log_temp_raw_encoded_size;

} lsm6dso_instance_state;

typedef struct odr_reg_map
{
  float              odr;
  lsm6dso_accel_odr  accel_odr_reg_value;
  lsm6dso_gyro_odr   gyro_odr_reg_value;
  uint16_t           accel_discard_samples;
  uint16_t           gyro_discard_samples;
} odr_reg_map;

typedef struct sns_lsm6dso_req
{
  float desired_sample_rate;
  float desired_report_rate;
  uint64_t desired_flush_ticks;
  sns_lsm6dso_registry_cfg  registry_cfg;
} sns_lsm6dso_req;

typedef struct sns_lsm6dso_self_test_req
{
  sns_physical_sensor_test_type test_type;
} sns_lsm6dso_self_test_req;

sns_rc lsm6dso_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

void lsm6dso_disable_fifo(sns_sensor_instance *this);
sns_rc lsm6dso_inst_deinit(sns_sensor_instance *const this);
void lsm6dso_set_fifo_config_timer(sns_sensor_instance *this);
void lsm6dso_set_client_test_config(sns_sensor_instance *this);
void lsm6dso_inst_hw_self_test(sns_sensor_instance *const this);
void lsm6dso_inst_factory_self_test(sns_sensor_instance *const this);
void lsm6dso_context_save(sns_sensor_instance * const this,uint8_t context_buffer [ ],uint8_t reg_map [ ],uint8_t reg_num);
void lsm6dso_context_restore(sns_sensor_instance * const this,uint8_t context_buffer [ ],uint8_t reg_map [ ],uint8_t reg_num);

