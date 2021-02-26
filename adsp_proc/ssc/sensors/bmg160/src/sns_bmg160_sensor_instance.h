#pragma once
/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
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
#include "sns_stream_service.h"
#include "sns_motion_detect.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"

#include "sns_bmg160_cfg.h"

#if BMG160_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif

/** Forward Declaration of Instance API */
extern sns_sensor_instance_api bmg160_sensor_instance_api;

#define BMG160_ENABLE_FILTER_OUT_FILTER_DELAY                1

// Enable below macro to enable driver debug messages

#if BMG160_CONFIG_ENABLE_DEBUG
/** TODO: consider removing unsed parameters from these macros like diag, file, line. */
#define BMG160_PRINTF(prio, sensor, ...) do { \
  if (sensor != NULL) { \
    if (SNS_##prio >= BMG160_DEFAULT_LOG_LEVEL) { \
      SNS_PRINTF(prio, sensor, __VA_ARGS__); \
    }\
  } \
} while (0)

#define BMG160_INST_PRINTF(prio, inst, ...) do { \
  if (inst != NULL) { \
    if (SNS_##prio >= BMG160_DEFAULT_LOG_LEVEL) { \
      SNS_INST_PRINTF(prio, inst , __VA_ARGS__); \
    } \
  } \
} while (0)

#else  //BMG160_ENABLE_DEBUG_MSG
#define BMG160_PRINTF(sensor,...)
#define BMG160_INST_PRINTF(inst,...)
#endif  //BMG160_ENABLE_DEBUG_MSG

#define BMG160_INST_RV_ASSERT_LOG(rc, expect_rv, inst) { \
  if ((rc) != (expect_rv)) \
  { \
    if (inst != NULL)\
    { \
      SNS_INST_PRINTF(ERROR, inst, "ERROR! rv:%d", rc); \
    } \
  } \
}


#define BMG160_CONFIG_TS_ERR_TOLERANCE_PERCENT   2  /*HIFI*/
/** Number of registers to read for debug */
#define BMG160_DEBUG_REGISTERS          (32)

/** Number of entries in reg_map table. */
#define BMG160_REG_MAP_TABLE_SIZE       (11)


#define BMG160_CHIP_ID          0x0F

typedef struct bmg160_com_port_info
{
  sns_com_port_config     com_config;
  sns_sync_com_port_handle     *port_handle;
} bmg160_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr {
  float min;
  float max;
} range_attr;

typedef enum
{
  BMG160_GYRO= 0x1,
  BMG160_SENSOR_TEMP   = 0x2
} bmg160_sensor_type;

typedef enum
{
  BMG160_CONFIG_IDLE,            /** not configuring */
  BMG160_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  BMG160_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  BMG160_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  BMG160_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} bmg160_config_step;

// <test>
typedef enum
{
  BMG160_TEST_NO_ERROR,
  BMG160_FAC_TEST_HIGH_BIAS,
  BMG160_FAC_TEST_DEV_NOT_STATIONARY,
  BMG160_FAC_TEST_ZERO_VARIANCE
} bmg160_test_err_code;

typedef enum
{
  BMG160_REGV_CONFIG_IDLE,
  BMG160_REGV_CONFIG_INPROGRESS,
} bmg160_regv_config_step;

typedef enum
{
  BMG160_GYRO_RANGE_125DPS   = 0x04,  /*corresponding value in register setting*/
  BMG160_GYRO_RANGE_250DPS   = 0x03,  /*corresponding value in register setting*/
  BMG160_GYRO_RANGE_500DPS   = 0x02,
  BMG160_GYRO_RANGE_1000DPS  = 0x01,
  BMG160_GYRO_RANGE_2000DPS  = 0x00,
} bmg160_gyro_range;


typedef enum
{
  BMG160_GYRO_ODR_OFF         = 0xFF,       /* power down output data rate */
  BMG160_GYRO_ODR2000         = 0x00,       /* 2000 Hz output data rate */
  BMG160_GYRO_ODR2000_FB230   = 0x01,       /* 2000 Hz output data rate */
  BMG160_GYRO_ODR1000         = 0x02,       /* 1000 Hz output data rate */
  BMG160_GYRO_ODR400          = 0x03,       /* 400 Hz output data rate */
  BMG160_GYRO_ODR200_FB23     = 0x04,       /* 200 Hz output data rate */
  BMG160_GYRO_ODR100_FB12     = 0x05,       /* 100 Hz output data rate */
  BMG160_GYRO_ODR200_FB64     = 0x06,       /* 200 Hz output data rate */
  BMG160_GYRO_ODR100_FB32     = 0x07,       /* 100 Hz output data rate */
} bmg160_gyro_odr_regv;

typedef struct bmg160_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} bmg160_self_test_info;

typedef struct bmg160_factory_test_info
{
  int32_t num_samples;             /** number of samples acquired */
  float variance_threshold;        /** stationary detect variance threshold */
  float variance[TRIAXIS_NUM];          /** variance */
  float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
  float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
  float bias_thresholds[TRIAXIS_NUM];
  uint32_t  fac_test_sensor       :3;
  uint32_t  at_rest               :1;
} bmg160_factory_test_info;

typedef float bmg160_gyro_resolution;


typedef enum BMG160_FIFO_READ_CTX_TYPE {
    BMG160_FIFO_READ_CTX_TYPE_WMI,
    BMG160_FIFO_READ_CTX_TYPE_WMI_RECHECK,
    BMG160_FIFO_READ_CTX_TYPE_FLUSH,
    BMG160_FIFO_READ_CTX_TYPE_HEARTBEAT,
} bmg160_fifo_read_ctx_type_t;


typedef enum BMG160_FIFO_FLUSH_TRIGGER {
    BMG160_FIFO_FLUSH_TRIGGER_CLIENTS,
    BMG160_FIFO_FLUSH_TRIGGER_HW_CHANGE,
} bmg160_fifo_flush_trigger_t;

typedef struct bmg160_fifo_read_ctx {
    uint8_t             ctx_type                :4;
    uint8_t             sync_read               :2;
} bmg160_fifo_read_ctx_t;

typedef enum
{
  BMG160_TS_FIX_NONE,
  BMG160_TS_FIX_LIGHT_OVERLAP,
  BMG160_TS_FIX_OVERLAP,
  BMG160_TS_FIX_LIGHT_SYNC,
  BMG160_TS_FIX_SYNC,
  BMG160_TS_FIX_REBASE,
} bmg160_ts_fx_flag;

/** HW FIFO information */
typedef struct bmg160_fifo_info
{
  /** FIFO enabled or not. Uses bmg160_sensor_type as bit mask
   *  to determine which FIFO Sensors are enabled */
  uint8_t fifo_enabled;

  uint8_t ff_read_ctx_type ;
  uint8_t ff_flush_trigger ;

  /** Determines which Sensor data to publish. Uses
   *  bmg160_sensor_type as bit mask. */
  uint8_t publish_sensors;

  /** fifo cur rate index */
  bmg160_gyro_odr_regv fifo_rate;

  /** FIFO watermark levels for gyro */
  uint16_t cur_wml;

  /** max requested FIFO watermark levels; possibly larger than max HW FIFO */
  uint32_t max_requested_wmk;

  /** max flush ticks*/
  uint64_t max_requested_flush_ticks;
  // fifo
  sns_time isr_ts;
  sns_time isr_last_ts;
  sns_time sampling_intvl;
  sns_time ts_itvl_fast_fixed;
  sns_time ts_fff_intvl_est;       /* frame interval */
  sns_time ts_fff_intvl_ideal;     /* frame interval */
  sns_time ts_1st_sample_est;      /* the first frame ts estimation */
  sns_time timestamp;              /* 1st frame ts of this batching */
  sns_time end_timestamp;          /* last frame ts of this batching */

  uint64_t ff_accum_cnt;
  uint32_t  ff_isr_cnt;

  uint32_t ff_itvl_gap_need_fix_too_low  :8;
  uint32_t ff_itvl_gap_need_fix_too_high :8;
  uint32_t avail_accurate_ts_last_sample :1;
  uint32_t ff_flush_in_proc              :1;
  uint32_t ascp_in_progress              :1;

  uint32_t ts_fix_ignore                 :1;
  bmg160_ts_fx_flag        ts_fix_flag;


} bmg160_fifo_info;

typedef struct bmg160_gyro_info
{
  struct bmg160_state     *sstate;
  bmg160_gyro_odr_regv  curr_odr_regv;
  bmg160_gyro_resolution resolution;
  bmg160_gyro_range     range_regv;
  sns_sensor_uid        suid;
  uint8_t               num_samples_to_discard;
  bmg160_self_test_info test_info;
  /* HW version and corresponding resolution*/
  float                   current_range_resolution;
  float                   sample_rate_req;
  float                   report_rate_req;
  float                   sample_rate_matched;
  uint32_t              ticks_filter_delay;

  uint32_t lp_mode                       :1;
  uint32_t hw_gyro_need_refcg            :4;
} bmg160_gyro_info;


typedef struct bmg160_sensor_temp_info
{
  struct bmg160_state     *sstate;
  uint32_t hw_temp_need_recfg            :1;
  bool                    timer_is_active;
  float                   sample_rate_req;
  float                   report_rate_req;
  float                   report_rate_hz;
  float                   sampling_rate_hz;
  sns_time                sampling_intvl;
  uint64_t                max_requested_flush_ticks;
  sns_sensor_uid          suid;
  bmg160_self_test_info   test_info;
} bmg160_sensor_temp_info;

typedef struct bmg160_irq_info
{
  sns_interrupt_req       irq_config;
  bool                    irq_ready;
  bool                    irq_registered;
} bmg160_irq_info;

typedef struct bmg160_async_com_port_info
{
  uint32_t                port_handle;
}bmg160_async_com_port_info;

typedef struct sns_bmg160_registry_cfg
{
  bmg160_sensor_type  sensor_type;
  uint8_t             range_idx;
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];
  uint32_t            version;
}sns_bmg160_registry_cfg;

typedef struct sns_bmg160_regv_cfg_ctrl
{
  sns_data_stream *reg_config_timer;
  bmg160_regv_config_step cfg_state;
  uint8_t saved_regv;
} sns_bmg160_regv_cfg_ctrl;

/** Private state. */
typedef struct bmg160_instance_state
{
  /** fifo details*/
  bmg160_fifo_info       fifo_info;

  /** gyro HW config details*/
  bmg160_gyro_info       gyro_info;

  /** Sensor Temperature config details. */
  bmg160_sensor_temp_info sensor_temp_info;

  /** Interrupt dependency info. */
  bmg160_irq_info        irq_info;

  /** COM port info */
  bmg160_com_port_info   com_port_info;

  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;

  bmg160_config_step   config_step;

  /** Data streams from dependentcies. */
  sns_sensor_uid       timer_suid;
  sns_data_stream      *interrupt_data_stream;
  sns_data_stream      *timer_data_stream;
  sns_data_stream      *async_com_port_data_stream;

  /**----------Sensor specific registry configuration----------*/
  sns_bmg160_registry_cfg gyro_registry_cfg;
  sns_bmg160_registry_cfg sensor_temp_registry_cfg;
  sns_bmg160_regv_cfg_ctrl regv_cfg_ctrl;

  sns_diag_service *diag_service;
  sns_sync_com_port_service * scp_service;
#if BMG160_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service *island_service;
#endif

  bmg160_factory_test_info fac_test_info;

  size_t log_interrupt_encoded_size;
  size_t           log_raw_encoded_size;
  size_t encoded_imu_event_len;
  size_t encoded_sensor_temp_event_len;

  uint32_t sample_cnt_to_skip_odrc        :8;
  uint32_t continuous_bad_sample_cnt      :8;   /*continuous bad data sample counter */
  uint32_t sensors_col                    :8;
  uint32_t instance_is_ready_to_configure :1;
  uint32_t new_self_test_request          :1;
  uint32_t fac_test_in_progress           :1;
  uint32_t update_fac_cal_in_registry     :1;
  uint32_t low_latency_req                :1;
  uint32_t in_low_power_mode              :1;

  sns_sensor_instance *owner;
} bmg160_instance_state;

typedef struct odr_reg_map
{
  uint16_t           u_odr;
  float              odr;
  bmg160_gyro_odr_regv   acc_bw_regv;
  uint8_t            discard_samples;
} odr_reg_map;


typedef struct sns_bmg160_req
{
  float                    desired_sample_rate;
  float                    desired_report_rate;
  uint64_t  desired_flush_ticks;
  sns_bmg160_registry_cfg  registry_cfg;
} sns_bmg160_req;

sns_rc bmg160_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);

sns_rc bmg160_inst_deinit(sns_sensor_instance *const this);

/**
 * Executes requested self-tests.
 *
 * @param instance   reference to the instance
 *
 * @return none
 */
void bmg160_run_self_test(sns_sensor_instance *instance);

/**
 *
 * @param this
 * @param client_request
 * @return
 */
sns_rc bmg160_inst_set_client_config(sns_sensor_instance *const this,
                                             sns_request const *client_request);

/**
 * assess overall request at instance level
 * @param inst   instance handler
 */
void bmg160_inst_assess_overall_req(
    sns_sensor_instance *const inst);
/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] state         Instance state
 * @param[o] buffer        who am I value read from HW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc bmg160_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer);

void bmg160_process_fac_test(sns_sensor_instance *instance);

void bmg160_send_fac_cal_event(
        sns_sensor_instance *const  instance,
        struct bmg160_state *sstate);

void bmg160_reset_fac_cal_data(
        sns_sensor_instance *const  instance,
        struct bmg160_state         *sstate,
        bool                        update_version);

/**
 * get elapse time between two time point
 * @param start
 * @param end
 * @return          end - start
 */
static inline
sns_time bmg160_get_time_elaspse_sys (
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
sns_time bmg160_get_time_deviation(
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
bool bmg160_itvl_within_hifi_accuracy(
    sns_time itvl_expected, /*ideal*/
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

  return (100 * err) <= itvl_expected * BMG160_CONFIG_TS_ERR_TOLERANCE_PERCENT;
}

static inline
bool bmg160_time_not_before(
    sns_time a,
    sns_time b)
{
  sns_time h;
  h = (((sns_time)(-2)) >> 1) + 1;

  return ((a + h) > (b + h));
}
