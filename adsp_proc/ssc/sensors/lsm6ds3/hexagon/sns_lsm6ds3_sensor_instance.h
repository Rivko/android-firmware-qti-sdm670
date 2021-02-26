#pragma once
/**
 * @file sns_lsm6ds3_sensor_instance.h
 *
 * LSM6DS3 Accel virtual Sensor Instance implementation.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <stdint.h>
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"
#include "sns_printf.h"

#include "sns_sensor_uid.h"

#include "sns_async_com_port.pb.h"
#include "sns_diag_service.h"
#include "sns_interrupt.pb.h"
#include "sns_island_service.h"
#include "sns_lsm6ds3_dae_if.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"

/** Forward Declaration of Instance API */
sns_sensor_instance_api lsm6ds3_sensor_instance_api;

// Enable below macro to enable driver debug messages
//#define LSM6DS3_ENABLE_DEBUG_MSG

#ifdef LSM6DS3_ENABLE_DEBUG_MSG
/** TODO: consider removing unsed parameters from these macros like diag, file, line. */
#define LSM6DS3_PRINTF(prio, sensor, ...) do { \
  SNS_PRINTF(prio, sensor, __VA_ARGS__); \
} while (0)
#define LSM6DS3_INST_PRINTF(prio, inst, ...) do { \
  SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
} while (0)
#else  //LSM6DS3_ENABLE_DEBUG_MSG
#define LSM6DS3_PRINTF(sensor,...)
#define LSM6DS3_INST_PRINTF(inst,...)
#endif  //LSM6DS3_ENABLE_DEBUG_MSG

/** Number of registers to read for debug */
#define LSM6DS3_DEBUG_REGISTERS          (32)

/** Number of entries in reg_map table. */
#define LSM6DS3_REG_MAP_TABLE_SIZE       (11)

/**
 * Accelerometer LSM6DS3_ACC Full Scales in register setting.
 */
typedef enum
{
  LSM6DS3_ACCEL_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
  LSM6DS3_ACCEL_RANGE_4G   = 0x08,
  LSM6DS3_ACCEL_RANGE_8G   = 0x0C,
  LSM6DS3_ACCEL_RANGE_16G   = 0x04,
} lsm6ds3_accel_range;

/**
 * Accelerometer LSM6DS3_ACC sensitivity for each range.
 */
typedef enum
{
  LSM6DS3_ACCEL_SSTVT_2G  = 61,   /* in the unit of micro-g/digit */
  LSM6DS3_ACCEL_SSTVT_4G  = 122,
  LSM6DS3_ACCEL_SSTVT_8G  = 244,
  LSM6DS3_ACCEL_SSTVT_16G = 488,
} lsm6ds3_accel_sstvt;

/**
 * Accelerometer LSM6DS3 ACC filter bandwidth in register setting
 */
typedef enum
{
  LSM6DS3_ACCEL_BW50      = 0x03,  /* 50 Hz bandwidth */
  LSM6DS3_ACCEL_BW100     = 0x02,  /* 100 Hz bandwidth */
  LSM6DS3_ACCEL_BW200     = 0x01,  /* 200 Hz bandwidth */
  LSM6DS3_ACCEL_BW400     = 0x00   /* 400 Hz bandwidth */
} lsm6ds3_accel_bw;

/**
 * Accelerometer LSM6DS3_ACC output data rate in register setting
 */
typedef enum
{
  LSM6DS3_ACCEL_ODR_OFF   = 0x00,  /* power down output data rate */
  LSM6DS3_ACCEL_ODR13     = 0x10,  /* 13 Hz output data rate */
  LSM6DS3_ACCEL_ODR26     = 0x20,  /* 26 Hz output data rate */
  LSM6DS3_ACCEL_ODR52     = 0x30,  /* 52 Hz output data rate */
  LSM6DS3_ACCEL_ODR104    = 0x40,  /* 104 Hz output data rate */
  LSM6DS3_ACCEL_ODR208    = 0x50,  /* 208 Hz output data rate */
  LSM6DS3_ACCEL_ODR416    = 0x60,  /* 416 Hz output data rate */
  LSM6DS3_ACCEL_ODR833    = 0x70,  /* 833 Hz output data rate */
  LSM6DS3_ACCEL_ODR1660   = 0x80,  /* 1.66 kHz output data rate */
  LSM6DS3_ACCEL_ODR3330   = 0x90,  /* 3.33 kHz output data rate */
  LSM6DS3_ACCEL_ODR6660   = 0xA0,  /* 6.66 kHz output data rate */
} lsm6ds3_accel_odr;

/**
 * LSM6DS3 output data rate for gyro, Disabling LPF2,
 * so BW setting is not required
 */
typedef enum
{
  LSM6DS3_GYRO_ODR_OFF = 0x00,       /* power down output data rate */
  LSM6DS3_GYRO_ODR13   = 0x10,       /* 13 Hz output data rate */
  LSM6DS3_GYRO_ODR26   = 0x20,       /* 26 Hz output data rate */
  LSM6DS3_GYRO_ODR52   = 0x30,       /* 52 Hz output data rate */
  LSM6DS3_GYRO_ODR104  = 0x40,       /* 104 Hz output data rate */
  LSM6DS3_GYRO_ODR208  = 0x50,       /* 208 Hz output data rate */
  LSM6DS3_GYRO_ODR416  = 0x60,       /* 416 Hz output data rate */
  LSM6DS3_GYRO_ODR833  = 0x70,       /* 833 Hz output data rate */
  LSM6DS3_GYRO_ODR1660 = 0x80,       /* 1.66 kHz output data rate */
} lsm6ds3_gyro_odr;

/**
 * LSM6DS3 Full Scales in register setting for gyro
 */
typedef enum
{
  STM_LSM6DS3_GYRO_RANGE_245DPS   = 0x00,  /*corresponding value in register setting*/
  STM_LSM6DS3_GYRO_RANGE_500DPS   = 0x04,
  STM_LSM6DS3_GYRO_RANGE_1000DPS  = 0x08,
  STM_LSM6DS3_GYRO_RANGE_2000DPS  = 0x0C,
} lsm6ds3_gyro_range;

typedef float lsm6ds3_gyro_sstvt;

typedef struct lsm6ds3_com_port_info
{
  sns_com_port_config     com_config;
  sns_sync_com_port_handle     *port_handle;

} lsm6ds3_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  LSM6DS3_ACCEL         = 0x1,
  LSM6DS3_GYRO          = 0x2,
  LSM6DS3_MOTION_DETECT = 0x4,
  LSM6DS3_SENSOR_TEMP   = 0x8
} lsm6ds3_sensor_type;

typedef enum
{
  LSM6DS3_CONFIG_IDLE,            /** not configuring */
  LSM6DS3_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  LSM6DS3_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  LSM6DS3_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  LSM6DS3_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} lsm6ds3_config_step;

typedef enum
{
  LSM6DS3_TEST_NO_ERROR,
  LSM6DS3_FAC_TEST_HIGH_BIAS,
  LSM6DS3_FAC_TEST_DEV_NOT_STATIONARY,
  LSM6DS3_FAC_TEST_ZERO_VARIANCE
} lsm6ds3_test_err_code;

typedef struct lsm6ds3_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} lsm6ds3_self_test_info;

typedef struct lsm6ds3_factory_test_info
{
  int32_t num_samples;             /** number of samples acquired */
  float variance_threshold;        /** stationary detect variance threshold */
  float variance[TRIAXIS_NUM];          /** variance */
  float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
  float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
  float bias_thresholds[TRIAXIS_NUM];
  bool at_rest;
} lsm6ds3_factory_test_info;

/** HW FIFO information */
typedef struct lsm6ds3_fifo_info
{
  /** FIFO enabled or not. Uses lsm6ds3_sensor_type as bit mask
   *  to determine which FIFO Sensors are enabled */
  uint8_t fifo_enabled;

  /** Determines which Sensor data to publish. Uses
   *  lsm6ds3_sensor_type as bit mask. */
  uint8_t publish_sensors;

  /** fifo cur rate index */
  lsm6ds3_accel_odr fifo_rate;
  /*Actual ODR running on H/W*/
  lsm6ds3_accel_odr  configured_odr;
  // Number of samples need to be discarded
  uint8_t num_samples_to_discard;
  /** FIFO watermark levels for accel and gyro*/
  uint16_t cur_wmk;
  // True, If only WMK changed with new request
  bool wmk_change;
  /** number of interrupts fired without reconfig*/
  uint16_t  interrupt_cnt;
  /** True, If it is first interrupt */
  bool first_interrupt;
  /** max requested FIFO watermark levels; possibly larger than max HW FIFO */
  uint32_t max_requested_wmk;

  /** max flush ticks*/
  uint64_t max_requested_flush_ticks;

} lsm6ds3_fifo_info;

typedef struct lsm6ds3_accel_info
{
  lsm6ds3_accel_odr       curr_odr;
  lsm6ds3_accel_sstvt     sstvt;
  lsm6ds3_accel_range     range;
  lsm6ds3_accel_bw        bw;
  bool                    lp_mode;
  sns_sensor_uid          suid;
  uint8_t                 num_samples_to_discard;
  bool                    gated_client_present;
  lsm6ds3_self_test_info  test_info;
} lsm6ds3_accel_info;

typedef struct lsm6ds3_gyro_info
{
  lsm6ds3_gyro_odr        curr_odr;
  lsm6ds3_gyro_sstvt      sstvt;
  lsm6ds3_gyro_range      range;
  bool                    is_in_sleep;
  sns_sensor_uid          suid;
  uint8_t                 num_samples_to_discard;
  lsm6ds3_self_test_info  test_info;
} lsm6ds3_gyro_info;

typedef struct lsm6ds3_motion_detect_info
{
  uint16_t                desired_wmk;
  lsm6ds3_accel_odr       desired_odr;
  lsm6ds3_accel_sstvt     sstvt;
  lsm6ds3_accel_range     range;
  lsm6ds3_accel_bw        bw;
  sns_sensor_uid          suid;
  bool                    enable_md_int;
  bool                    md_client_present;
  bool                    md_new_req;
  sns_motion_detect_event md_state;
  lsm6ds3_self_test_info  test_info;
  sns_registry_md_cfg     md_config;
} lsm6ds3_motion_detect_info;

typedef struct lsm6ds3_sensor_temp_info
{
  sns_sensor_uid          suid;
  bool                    timer_is_active;
  float                   report_rate_hz;
  float                   sampling_rate_hz;
  sns_time                sampling_intvl;
  lsm6ds3_self_test_info  test_info;
  uint64_t                max_requested_flush_ticks;
} lsm6ds3_sensor_temp_info;

typedef struct lsm6ds3_irq_info
{
  sns_interrupt_req       irq_config;
  bool                    irq_ready;
  bool                    irq_registered;
} lsm6ds3_irq_info;

typedef struct lsm6ds3_async_com_port_info
{
  uint32_t                port_handle;
}lsm6ds3_async_com_port_info;

typedef struct sns_lsm6ds3_registry_cfg
{
  lsm6ds3_sensor_type sensor_type;
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];
  uint32_t            version;
}sns_lsm6ds3_registry_cfg;

/** Private state. */
typedef struct lsm6ds3_instance_state
{
  /** fifo details*/
  lsm6ds3_fifo_info       fifo_info;

  /** accel HW config details*/
  lsm6ds3_accel_info      accel_info;

  /** gyro HW config details*/
  lsm6ds3_gyro_info       gyro_info;

  /** motion detect info */
  lsm6ds3_motion_detect_info md_info;

  /** Sensor Temperature config details. */
  lsm6ds3_sensor_temp_info sensor_temp_info;

  /** Interrupt dependency info. */
  lsm6ds3_irq_info        irq_info;

  /** COM port info */
  lsm6ds3_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;

  /**--------DAE interface---------*/
  lsm6ds3_dae_if_info       dae_if;

  lsm6ds3_config_step       config_step;

  sns_time             interrupt_timestamp;
  /** timestamp of last sample sent to framework*/
  sns_time last_timestamp; 
  /** timestamp at which heart beat timer should start*/
  sns_time heart_beat_timestamp;
  /** avg sampling interval without reconfiguring*/
  sns_time  avg_sampling_intvl;  
  /* sample time stamp should be less than this time stamp. Otherwise, skip sample*/
  sns_time sample_timestamp_check;
  /** Data streams from dependentcies. */
  sns_sensor_uid       timer_suid;
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_data_stream;
  sns_data_stream      *timer_heart_beat_data_stream;
  sns_data_stream      *async_com_port_data_stream;
  
  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_lsm6ds3_registry_cfg accel_registry_cfg;
  sns_lsm6ds3_registry_cfg gyro_registry_cfg;
  sns_lsm6ds3_registry_cfg sensor_temp_registry_cfg;

  uint8_t   reg_status[LSM6DS3_DEBUG_REGISTERS];

  sns_diag_service *diag_service;
  sns_sync_com_port_service * scp_service;
  sns_island_service *island_service;
  bool instance_is_ready_to_configure;
  bool fifo_flush_in_progress;
  bool new_self_test_request;
  bool fac_test_in_progress;
  bool fifo_flush_client_present;
  // True, If ascp transation is ongoing
  bool ascp_in_progress;
  // True, If interrupt fired
  bool interrupt_fired;
  lsm6ds3_factory_test_info fac_test_info;
  lsm6ds3_sensor_type fac_test_sensor;
  bool update_fac_cal_in_registry;
  sns_time heart_beat_timeout;
  float fifo_running_odr;
  
  size_t log_interrupt_encoded_size;
  size_t log_raw_encoded_size;
  size_t encoded_imu_event_len;
  size_t encoded_sensor_temp_event_len;

} lsm6ds3_instance_state;

typedef struct odr_reg_map
{
  float              odr;
  lsm6ds3_accel_odr  accel_odr_reg_value;
  lsm6ds3_gyro_odr   gyro_odr_reg_value;
  uint8_t            discard_samples;
} odr_reg_map;

typedef struct sns_lsm6ds3_req
{
  float  desired_sample_rate;
  float  desired_report_rate;
  uint64_t  desired_flush_ticks;
  sns_lsm6ds3_registry_cfg  registry_cfg;
} sns_lsm6ds3_req;

sns_rc lsm6ds3_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

sns_rc lsm6ds3_inst_deinit(sns_sensor_instance *const this);

/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instace
 *
 * @return none
 */
void lsm6ds3_run_self_test(sns_sensor_instance *instance);

/**
 * Processes results for factory test for accel and gyro.
 *
 * @param[i] instance   reference to instance
 */
void lsm6ds3_process_fac_test(sns_sensor_instance *instance);

