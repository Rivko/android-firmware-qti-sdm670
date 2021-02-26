#pragma once
/*******************************************************************************
* Copyright (c) 2019, Bosch Sensortec GmbH
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
#include "sns_bmm150_config.h"

#include "sns_async_com_port.pb.h"
#if BMM_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif
#include "sns_bmm150_dae_if.h"
#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "bmm150_defs.h"
#include "sns_math_util.h"
#include "sns_registry_util.h"
/** Forward Declaration of Instance API */
extern sns_sensor_instance_api sns_see_bmm150_sensor_instance_api;


#ifndef s8
#define s8                                  int8_t
#define u8                                  uint8_t
#define s16                                 int16_t
#define u16                                 uint16_t
#define s32                                 int32_t
#define u32                                 uint32_t
typedef int64_t                             s64;/**< used for signed 64bit */
typedef uint64_t                            u64;/**< used for signed 64bit */
#endif

/* physical COM port structure */
typedef struct bmm150_com_port_info
{
  sns_com_port_config        com_config;
  sns_sync_com_port_handle  *port_handle;
} bmm150_com_port_info;

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
  BMM150_MAG = 0x1,
  BMM150_TEMPERATURE = 0x2,
  BMP_SENSOR_INVALID = 0xFF
} bmm150_sensor_type;

typedef enum
{
  BMM150_CONFIG_IDLE,            /** not configuring */
  BMM150_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  BMM150_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  BMM150_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  BMM150_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} bmm150_config_step;

typedef enum
{
  BMM150_TEST_NO_ERROR,
  BMM150_FAC_TEST_HIGH_BIAS,
  BMM150_FAC_TEST_DEV_NOT_STATIONARY,
  BMM150_FAC_TEST_ZERO_VARIANCE
} bmm150_test_err_code;
typedef struct bmm150_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} bmm150_self_test_info;
#if BMM_CONFIG_ENABLE_SELF_TEST_FAC
typedef struct bmm150_factory_test_info
{
  int32_t num_samples;             /** number of samples acquired */
  float variance_threshold;        /** stationary detect variance threshold */
  float variance[TRIAXIS_NUM];          /** variance */
  float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
  float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
  float bias_thresholds[TRIAXIS_NUM];
  bool at_rest;
} bmm150_factory_test_info;
#endif
/*!
 *  @brief This enum holds different ODR values
 *  for Gas data
 */
typedef enum bmm150_odr
{
  BMM150_ODR_0_59MS,
  BMM150_ODR_62_5MS,
  BMM150_ODR_125MS,
  BMM150_ODR_250MS,
  BMM150_ODR_500MS,
  BMM150_ODR_1000MS,
  BMM150_ODR_10MS,
  BMM150_ODR_20MS,
  BMM150_ODR_NONE
} bmm150_odr;


/*!
* low level operation mode
* */
typedef enum
{
  SLEEP_MODE,         /* sleep mode      : no measurements are performed*/
  FORCED_MODE,        /* forced mode     :single TPHG cycle is performed; sensor automatically returns to sleep mode afterwards*/
  SEQUENTIAL_MODE,    /* sequential mode :TPHG measurements are performed continuously until mode change; Between each cycle,
                                the sensor enters stand-by for a period of time according to the odr control register*/
  PARALLEL_MODE,      /* parallel mode   : TPHG measurements are performed continuously until mode change;
                                 no stand-by occurs between consecutive TPHG cycles*/
  MAX_NUM_OP_MODE,    /* the max number of operation mode */
  INVALID_WORK_MODE = MAX_NUM_OP_MODE, /* invalid mode */
} bmm150_power_mode;
typedef struct bmm150_sensor_deploy_info
{
  /** Determines which Sensor data to publish. Uses
   *  bmm150_sensor_type as bit mask. */
  uint8_t           publish_sensors;
  uint8_t           enable;
} bmm150_sensor_deploy_info;


typedef struct bmm150_sensor_cfg_info
{
  float             desired_odr;
  float             curr_odr;
  sns_sensor_uid    suid;
  uint64_t          trigger_num;
  sns_time          timeout_ticks; /* derived from the odr */
  sns_time          expection_timeout_ticks_derived_from_odr;
  bool              timer_is_active;
  uint32_t          report_timer_hz;
  float             report_rate_hz;
  float             sampling_rate_hz;
  sns_time          sampling_intvl;
  uint64_t          max_requested_flush_ticks;
  sns_time          expect_time;
  bmm150_self_test_info  test_info;
} bmm150_sensor_cfg_info;

/* async port for the data stream which use the COM port handle */
typedef struct bmm150_async_com_port_info {
  uint32_t port_handle;
} bmm150_async_com_port_info;

typedef struct sns_bmm150_registry_cfg
{
  bmm150_sensor_type sensor_type;
  matrix3             fac_cal_corr_mat;
  float               fac_cal_bias[3];
  uint32_t            version;
}sns_bmm150_registry_cfg;


/** Private state. */
typedef struct bmm150_instance_state
{
  /** -- sensor configuration details --*/
  /** pressure HW config details*/
  bmm150_sensor_cfg_info mag_info;
  /** temperature HW config details */
  bmm150_sensor_cfg_info temperature_info;
  bmm150_sensor_deploy_info   deploy_info;
  /** COM port info */
  bmm150_com_port_info com_port_info;
  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;
  #if BMM_CONFIG_ENABLE_DAE
  /**--------DAE interface---------*/
  bmm150_dae_if_info       dae_if;
  #endif
  bmm150_config_step       config_step;
  /** Data streams from dependentcies. */
  sns_sensor_uid                 timer_suid;
  sns_data_stream                *temperature_timer_data_stream;
  sns_data_stream                *async_com_port_data_stream;  /* data streaming channel */
  sns_data_stream                *mag_timer_data_stream;    /* sample sensor data */
  /* request/configure stream */
  uint32_t                       client_req_id;
  sns_std_sensor_config          bmm150_req;   /* stream for the configure */
  size_t                         encoded_imu_event_len;
  size_t                         encoded_temp_event_len;
  /**----------Axis Conversion----------*/
  triaxis_conversion axis_map[TRIAXIS_NUM];

  /**----------Sensor specific registry configuration----------*/
  sns_bmm150_registry_cfg mag_registry_cfg;
  sns_bmm150_registry_cfg sensor_temp_registry_cfg;
  sns_diag_service               *diag_service;  /* for diagnostic to print debug message */
  sns_sync_com_port_service      *scp_service;
  #if BMM_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service *island_service;
  #endif
  bmm150_power_mode                 op_mode;
  sns_rc (* com_read)(
     sns_sync_com_port_service *scp_service,
      sns_sync_com_port_handle *port_handle,
      uint32_t rega,
      uint8_t  *regv,
      uint32_t bytes,
      uint32_t *xfer_bytes);
  sns_rc (* com_write)(
      sns_sync_com_port_service *scp_service,
      sns_sync_com_port_handle *port_handle,
      uint32_t rega,
      uint8_t  *regv,
      uint32_t bytes,
      uint32_t *xfer_bytes,
      bool save_write_time);
  bool instance_is_ready_to_configure;
  bool new_self_test_request;
#if BMM_CONFIG_ENABLE_SELF_TEST_FAC
  bool fac_test_in_progress;
  bmm150_factory_test_info fac_test_info;
  bmm150_sensor_type fac_test_sensor;
#endif
  bool update_fac_cal_in_registry;
  bool calib_data_loaded;
  u8 sample_rate_no_need_change;
  size_t           log_raw_encoded_size;
  bmm150_intf interface;
  bmm150_delay_fptr_t delay_ms;
  /*! Trim registers */
  struct bmm150_trim_registers trim_data;
  /*! Sensor settings */
  struct bmm150_settings settings;
  /*! Interrupt status */
  uint16_t int_status;
  /*! Structure containing mag data */
  struct bmm150_mag_data data;
  /*optimize the timestamp*/
  sns_time current_timestamp;
  sns_time old_timestamp;
  uint32_t sample_cnt_to_skip_odrc;
} bmm150_instance_state;


typedef struct sns_bmm150_cfg_req {
  float               sample_rate;
  float               report_rate;
  bmm150_sensor_type  sensor_type;
  bmm150_power_mode      op_mode;
  uint8_t             sample_rate_no_not_change;
  uint64_t  desired_flush_ticks;
  sns_bmm150_registry_cfg  registry_cfg;
} sns_bmm150_cfg_req;

sns_rc bmm150_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);
sns_rc bmm150_inst_deinit(sns_sensor_instance *const this);
sns_rc bmm150_inst_set_client_config(
     sns_sensor_instance * const this,
    sns_request const *client_request);


/**
 * Processes results for factory test for accel and gyro.
 *
 * @param[i] instance   reference to instance
 */
void bmm150_process_fac_test(sns_sensor_instance *instance);
void bmm150_send_cal_event(sns_sensor_instance *const instance, bmm150_sensor_type sensor_type);

