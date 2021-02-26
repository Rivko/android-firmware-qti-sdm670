#pragma once
/*******************************************************************************
 * Copyright (c) 2018, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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
 *******************************************************************************/

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
#if BMA2X2_CONFIG_ENABLE_DAE
#include "sns_bma2x2_dae_if.h"
#endif
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"
#include "sns_bma2x2_cfg.h"

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
#include "sns_island_service.h"
#endif


/** Forward Declaration of Instance API */
sns_sensor_instance_api bma2x2_sensor_instance_api;


#define BMA2X2_ENABLE_FILTER_OUT_FILTER_DELAY                1

#if  BMA2X2_ENABLE_DEBUG

#define SENSOR_DEBUG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMA2X2_DEFAULT_LOG_LEVEL) { \
            SNS_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}


#define INST_DEBUG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMA2X2_DEFAULT_LOG_LEVEL) { \
            SNS_INST_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#else
#define SENSOR_DEBUG(LOG_LEVEL, this, arg...)
#define INST_DEBUG(LOG_LEVEL, this, arg...)

#endif

#define BMA2X2_DEBUG_REGISTERS          (36)
/** Number of entries in reg_map table. */
#define BMA2X2_REG_MAP_TABLE_SIZE       (11)


#define BMA2X2_CONFIG_TS_ITVL_EST_COEF              8
#define BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT      3
#define BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT_HIGH 2
#define BMA2X2_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY  (0.2)


#define BMA2X2_ENABLE_ALL_SENSORS  (BMA2X2_ACCEL | BMA2X2_MOTION_DETECT | BMA2X2_SENSOR_TEMP)

/**
 * Accelerometer BMA2X2_ACC Full Scales in register setting.
 */
typedef enum
{
  BMA2X2_ACCEL_RANGE_2G   = 0x03,  /*corresponding value in register setting*/
  BMA2X2_ACCEL_RANGE_4G   = 0x05,
  BMA2X2_ACCEL_RANGE_8G   = 0x08,
  BMA2X2_ACCEL_RANGE_16G  = 0x0C,
} bma2x2_accel_range;

/**
 * Accelerometer BMA2X2_ACC sensitivity for each range.
 */
typedef enum
{
  BMA2X2_ACCEL_SSTVT_2G  = 61,   /* in the unit of micro-g/digit */
  BMA2X2_ACCEL_SSTVT_4G  = 122,
  BMA2X2_ACCEL_SSTVT_8G  = 244,
  BMA2X2_ACCEL_SSTVT_16G = 488,
} bma2x2_accel_sstvt;

typedef enum {
	BMA2X2_RANGE_ACC_2G_IDX,
	BMA2X2_RANGE_ACC_4G_IDX,
	BMA2X2_RANGE_ACC_8G_IDX,
	BMA2X2_RANGE_ACC_16G_IDX,
} bma2x2_acc_range_idx;
/**
 * Accelerometer LSM6DS3 ACC filter bandwidth in register setting
 */
typedef enum
{
  BMA2X2_ACCEL_BW50      = 0x03,  /* 50 Hz bandwidth */
  BMA2X2_ACCEL_BW100     = 0x02,  /* 100 Hz bandwidth */
  BMA2X2_ACCEL_BW200     = 0x01,  /* 200 Hz bandwidth */
  BMA2X2_ACCEL_BW400     = 0x00   /* 400 Hz bandwidth */
} bma2x2_accel_bw;

/**
 * Accelerometer BMA2X2_ACC output data rate in register setting
 */
typedef enum
{
  BMA2X2_ACCEL_ODR_OFF    = 0x00,   /*!< BMA2X2_ACCEL_ODR_OFF */
  BMA2X2_REGV_BW_7_81HZ   = 0x08,   /*!< BMA2X2_REGV_BW_7_81HZ */
  BMA2X2_REGV_BW_15_63HZ  = 0x09,   /*!< BMA2X2_REGV_BW_15_63HZ */
  BMA2X2_REGV_BW_31_25HZ  = 0x0A,   /*!< BMA2X2_REGV_BW_31_25HZ */
  BMA2X2_REGV_BW_62_50HZ  = 0x0B,   /*!< BMA2X2_REGV_BW_62_50HZ */
  BMA2X2_REGV_BW_125HZ    = 0x0C,   /*!< BMA2X2_REGV_BW_125HZ */
  BMA2X2_REGV_BW_250HZ    = 0x0D,   /*!< BMA2X2_REGV_BW_250HZ */
  BMA2X2_REGV_BW_500HZ    = 0x0E,   /*!< BMA2X2_REGV_BW_500HZ */
  BMA2X2_REGV_BW_1000HZ   = 0x0F    /*!< BMA2X2_REGV_BW_1000HZ */
} bma2x2_accel_odr_regv;

typedef struct bma2x2_com_port_info
{
  sns_com_port_config     com_config;
  sns_sync_com_port_handle     *port_handle;
} bma2x2_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  BMA2X2_ACCEL         = 0x1,
  BMA2X2_MOTION_DETECT = 0x2,
  BMA2X2_SENSOR_TEMP   = 0x4
} bma2x2_sensor_type;

typedef enum
{
  BMA2X2_CONFIG_IDLE,            /** not configuring */
  BMA2X2_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  BMA2X2_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  BMA2X2_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  BMA2X2_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} bma2x2_config_step;

/*!
 * bma2x2 sensor version enumerate value
 */
typedef enum {
    BMA2X2_SENSOR_VERSION_280,   /*!< BMA2X2_SENSOR_VERSION_280 */
    BMA2X2_SENSOR_VERSION_255,   /*!< BMA2X2_SENSOR_VERSION_255 */
    BMA2X2_SENSOR_VERSION_250E,  /*!< BMA2X2_SENSOR_VERSION_250E */
    BMA2X2_SENSOR_VERSION_222E,  /*!< BMA2X2_SENSOR_VERSION_222E */
    BMA2X2_SENSOR_VERSION_UNKNOWN/*!< BMA2X2_SENSOR_VERSION_UNKNOWN */
} bma2x2_sensor_hw_version;

typedef enum
{
  BMA2X2_REGV_CONFIG_IDLE,
  BMA2X2_REGV_CONFIG_INPROGRESS,
} bma2x2_regv_config_step;

typedef struct sns_bma2x2_regv_cfg_ctrl
{
  sns_data_stream *reg_config_timer;
  bma2x2_regv_config_step cfg_state;
  uint8_t saved_regv;
} bma2x2_regv_cfg_ctrl;


typedef struct bma2x2_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} bma2x2_self_test_info;


typedef struct bma2x2_factory_test_info
{
  int32_t num_samples;             /** number of samples acquired */
  float variance_threshold;        /** stationary detect variance threshold */
  float variance[TRIAXIS_NUM];          /** variance */
  float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
  float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
  float bias_thresholds[TRIAXIS_NUM];
  bool at_rest;
} bma2x2_factory_test_info;

//fifo handler

typedef struct bma2x2_fifo_read_ctx {
  uint8_t             ctx_type                :4;
  uint8_t             sync_read               :2;
} bma2x2_fifo_read_ctx_t;

typedef enum BMA2X2_FIFO_READ_CTX_TYPE {
    BMA2X2_FIFO_READ_CTX_TYPE_WMI,
    BMA2X2_FIFO_READ_CTX_TYPE_WMI_RECHECK,
    BMA2X2_FIFO_READ_CTX_TYPE_FLUSH,
    BMA2X2_FIFO_READ_CTX_TYPE_FLUSH_SYNC_DRAIN,
#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
    BMA2X2_FIFO_READ_CTX_TYPE_HEARTBEAT,
#endif
} bma2x2_fifo_read_ctx_type_t;


typedef enum BMA2X2_FIFO_STATUS_TYPE {
    BMA2X2_FIFO_STATUS_TYPE_FIFO_WMI,
    BMA2X2_FIFO_STATUS_TYPE_FIFO_FULL,
} bma2x2_fifo_status_type_t;



typedef enum BMA2X2_FIFO_FLUSH_TRIGGER {
    BMA2X2_FIFO_FLUSH_TRIGGER_CLIENTS,
    BMA2X2_FIFO_FLUSH_TRIGGER_HW_CHANGE,
} bma2x2_fifo_flush_trigger_t;


typedef enum
{
  BMA2X2_FAC_TEST_NO_ERROR,
  BMA2X2_FAC_TEST_HIGH_BIAS,
  BMA2X2_FAC_TEST_DEV_NOT_STATIONARY,
  BMA2X2_FAC_TEST_ZERO_VARIANCE,
  BMA2X2_FAC_TEST_DEV_BUSY
} bma2x2_test_err_code;

/** HW FIFO information */
typedef struct bma2x2_fifo_info
{
  /** FIFO enabled or not. Uses bma2x2_sensor_type as bit mask
   *  to determine which FIFO Sensors are enabled */
  uint8_t fifo_enabled;

  /** Determines which Sensor data to publish. Uses
   *  bma2x2_sensor_type as bit mask. */
  uint8_t publish_sensors;

  /** fifo cur rate index */
  bma2x2_accel_odr_regv fifo_rate;

  /** FIFO watermark levels for accel */
  uint16_t cur_wml;

  /** max requested FIFO watermark levels; possibly larger than max HW FIFO */
  uint32_t max_requested_wmk;

  /** sampling interval derived from the fifo rate(hz) */
  sns_time sampling_intvl;
  /** max flush ticks*/
  uint64_t max_requested_flush_ticks;

  // fifo timestamp
  sns_time             ts_itvl_ideal;
  sns_time             ts_itvl_est;              /* frame interval */

  sns_time             ts_irq;
  sns_time             ts_irq_last;
  sns_time             ts_ff_start;
  sns_time             ts_ff_end;
  sns_time             ts_itvl_fast_fixed;       /* frame interval */

  //fifo flags
  uint8_t      ff_flush_trigger        ;
  uint8_t      ff_read_ctx_type        ;
  uint8_t      ff_int_status           ;
  uint64_t     ff_accum_cnt;
  uint64_t     ff_isr_cnt;

  uint32_t     sample_cnt_to_skip_odrc         :8;
  uint32_t     ff_itvl_gap_need_fix_too_low    :4;
  uint32_t     ff_itvl_gap_need_fix_too_high   :4;
  uint32_t     ff_ts_avail_last_sample         :1;
  uint32_t     ff_itvl_need_fast_fix           :1;
  uint32_t     ff_flush_in_proc                :1;
  uint32_t     ff_ascp_in_progress             :1;

} bma2x2_fifo_info;

typedef struct bma2x2_accel_info
{
  float                  sample_rate_req;
  float                  report_rate_req;

  bma2x2_accel_odr_regv  curr_odr_regv;
  bma2x2_accel_sstvt     sstvt;
  bma2x2_accel_range     range;
  bma2x2_accel_bw        bw;
  sns_sensor_uid         suid;
  bma2x2_self_test_info  test_info;

  bma2x2_regv_cfg_ctrl   regv_cfg_manager;

  uint8_t                acc_res_idx;
  // fifo
  uint8_t                ff_wml;
  uint8_t                num_samples_to_discard;

  // INT status
  uint8_t                int_status;
  /* HW version and corresponding resolution*/
  bma2x2_sensor_hw_version hw_ver;
  float                  current_range_resolution;
  uint32_t               ticks_filter_delay;
  // status flag
  uint32_t               lp_mode :1;
  uint32_t               gated_client_present:1;
  uint32_t               acc_cfg_in_process  :1;
  uint32_t               hw_acc_need_recfg   :4;

} bma2x2_accel_info;


typedef struct bma2x2_motion_detect_info
{
  uint16_t                desired_wmk;
  bma2x2_accel_odr_regv   desired_odr;
  bma2x2_accel_sstvt      sstvt;
  bma2x2_accel_range      range;
  bma2x2_accel_bw         bw;
  sns_sensor_uid          suid;
  bool                    enable_md_int;
  bool                    md_client_present;
  bool                    md_new_req;
  sns_motion_detect_event md_state;
  bma2x2_self_test_info   test_info;
  sns_registry_md_cfg     md_config;
#if BMA2X2_ENABLE_FORCE_WAIT
#endif
} bma2x2_motion_detect_info;

typedef struct bma2x2_sensor_temp_info
{
  sns_sensor_uid          suid;
  bool                    timer_is_active;
  float                   report_rate_hz;
  float                   sampling_rate_hz;
  sns_time                sampling_intvl;
  bma2x2_self_test_info   test_info;
  uint64_t                max_requested_flush_ticks;
} bma2x2_sensor_temp_info;

typedef struct bma2x2_irq_info
{
  sns_interrupt_req       irq_config;
  bool                    irq_ready;
  bool                    irq_registered;
} bma2x2_irq_info;

typedef struct bma2x2_async_com_port_info
{
  uint32_t                port_handle;
}bma2x2_async_com_port_info;

typedef struct sns_bma2x2_registry_cfg
{
  bma2x2_sensor_type    sensor_type;
  uint8_t               acc_res_idx;
  matrix3               fac_cal_corr_mat;
  float                 fac_cal_bias[TRIAXIS_NUM];
  uint32_t              version;
}sns_bma2x2_registry_cfg;

/** Private state. */
typedef struct bma2x2_instance_state
{
  /** fifo details*/
  bma2x2_fifo_info       fifo_info;

  /** accel HW config details*/
  bma2x2_accel_info      accel_info;

  /** motion detect info */
  bma2x2_motion_detect_info md_info;

  /** Sensor Temperature config details. */
  bma2x2_sensor_temp_info sensor_temp_info;

  /** resolution configuration*/
  /** Interrupt dependency info. */
  bma2x2_irq_info        irq_info;

  /** COM port info */
  bma2x2_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;

#if BMA2X2_CONFIG_ENABLE_DAE
  /**--------DAE interface---------*/
  bma2x2_dae_if_info       dae_if;
#endif

  bma2x2_config_step       config_step;


  /** Data streams from dependentcies. */
  sns_sensor_uid       timer_suid;
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_data_stream;
  sns_data_stream      *async_com_port_data_stream;
#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
  sns_data_stream      *timer_heart_beat_data_stream;
#endif


  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_bma2x2_registry_cfg accel_registry_cfg;
  sns_bma2x2_registry_cfg sensor_temp_registry_cfg;

  sns_diag_service *diag_service;
  sns_sync_com_port_service * scp_service;

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
  sns_island_service *island_service;
#endif

  // sensor configuration
  bool instance_is_ready_to_configure;
#if BMA2X2_ENABLE_TEMP_PATCH
  bool do_temperature_patch;
#endif

  // self test
  bool new_self_test_request;
  bool fac_test_in_progress;
  bma2x2_factory_test_info fac_test_info;
  bma2x2_sensor_type fac_test_sensor;
  bool update_fac_cal_in_registry;

  size_t log_interrupt_encoded_size;
  size_t log_raw_encoded_size;
  size_t encoded_imu_event_len;
  size_t encoded_sensor_temp_event_len;

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
  // heart timer
  sns_time heart_beat_timeout;
  sns_time heart_beat_timestamp;
#endif

  uint32_t               low_latency_req    :1;
} bma2x2_instance_state;

typedef struct bma2x2_odr_reg_map
{
  uint16_t           u_odr;
  uint8_t            discard_samples;
  bma2x2_accel_odr_regv   acc_bw_regv;
  float              odr;
} bma2x2_odr_reg_map;


typedef struct sns_bma2x2_req
{
  float                    desired_sample_rate;
  float                    desired_report_rate;
  uint64_t                 desired_flush_ticks;
  sns_bma2x2_registry_cfg  registry_cfg;
  bma2x2_sensor_hw_version hw_ver;
} sns_bma2x2_req;

/** Need to use ODR table. */
extern const bma2x2_odr_reg_map bma2x2_reg_map[BMA2X2_REG_MAP_TABLE_SIZE];
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

sns_rc bma2x2_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);


sns_rc bma2x2_inst_deinit(sns_sensor_instance *const this);


sns_rc bma2x2_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer);

// <self test>
void bma2x2_run_self_test(sns_sensor_instance *instance);


/**
 * Processes results for factory test for gyro and temperature
 *
 * @param[i] instance   reference to instance
 */

void bma2x2_inst_assess_overall_req(sns_sensor_instance *inst);

void
bma2x2_set_settle_resolution(sns_sensor_instance *const instance);
sns_rc bma2x2_accel_match_odr(float desired_sample_rate,
                                      float *chosen_sample_rate,
                                      bma2x2_accel_odr_regv *chosen_reg_value,
                                      uint8_t *num_samples_to_discard);
sns_rc bma2x2_validate_sensor_temp_odr(bma2x2_instance_state *state);

void bma2x2_process_fac_test(sns_sensor_instance *instance);
// <self test>

/**
 * get elapse time between two time point
 * @param start
 * @param end
 * @return          end - start
 */
static inline
sns_time bma2x2_get_time_elaspse_sys (
    sns_time start,
    sns_time end)
{
  sns_time elapse;

  if (end >= start)
  {
    elapse = end - start;
  }
  else
  {
    elapse = (sns_time)(-1) - end + start + 1;
  }

  return elapse;
}

static inline
sns_time bma2x2_get_time_deviation(
        sns_time a,
        sns_time b)
{
  if (a > b) {
    return a - b;
  }
  else {
    return b - a;
  }
}

static inline
bool bma2x2_itvl_within_accuracy(
    sns_time itvl_expected,
    sns_time itvl_est)
{
  sns_time err;

  if (itvl_expected > itvl_est)
  {
    err = itvl_expected - itvl_est;
  }
  else
  {
    err = itvl_est - itvl_expected;
  }

  return (100 * err) <= itvl_expected * BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT;
}

static inline
bool bma2x2_itvl_within_haccuracy(
    sns_time itvl_expected,
    sns_time itvl_est)
{
  sns_time err;

  if (itvl_expected > itvl_est)
  {
    err = itvl_expected - itvl_est;
  }
  else
  {
    err = itvl_est - itvl_expected;
  }

  return (100 * err) <= itvl_expected * BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT_HIGH;
}

static inline
bool is_bma2x2_ts_within_hifi_accuracy(sns_time itvl_expected,
    uint32_t itvl_est)
{
  sns_time itvl_comp = (sns_time) itvl_est;
  sns_time itvl_hifi_low_margin = itvl_expected * 98 / 100;
  sns_time itvl_hifi_high_margin = itvl_expected * 102 / 100;

  if ((itvl_comp > itvl_hifi_low_margin) &&
      (itvl_comp < itvl_hifi_high_margin))
  {
    return true;
  }
  else
  {
    return false;
  }
}



static inline
bool bma2x2_is_est_itvl_accuracy_over_isr(
    sns_time itvl_expected,
    sns_time itvl_est,
    sns_time itvl_isr
    )
{
  sns_time err_est, err_isr;

  if (itvl_expected > itvl_est)
  {
    err_est = itvl_expected - itvl_est;
  }
  else
  {
    err_est = itvl_est - itvl_expected;
  }

  if (itvl_expected > itvl_isr)
  {
    err_isr = itvl_expected - itvl_isr;
  }
  else
  {
    err_isr = itvl_isr - itvl_expected;
  }

  if (err_est < err_isr)
  {
    return true;
  }
  else
  {
    return false;
  }
}



static inline
bool bma2x2_time_not_before(
    sns_time a,
    sns_time b)
{
  sns_time h;
  h = (((sns_time)(-2)) >> 1) + 1;

  return ((a + h) > (b + h));
}

#if !BMA2X2_ENABLE_FORCE_WAIT
void bma2x2_hal_create_reg_cfg_timer(sns_sensor_instance * const this, sns_time timeout_ticks);
#endif
