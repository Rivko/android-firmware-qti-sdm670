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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/


#include <stdint.h>
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_sensor_instance.h"
#include "sns_time.h"
#include "sns_printf.h"

#include "sns_sensor_uid.h"
#include "sns_bmp285_config.h"

#include "sns_async_com_port.pb.h"
#if BMP_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif
#include "sns_bmp285_dae_if.h"
#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_registry_util.h"
/** Forward Declaration of Instance API */
extern sns_sensor_instance_api sns_see_bmp285_sensor_instance_api;


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
typedef struct bmp285_com_port_info
{
  sns_com_port_config        com_config;
  sns_sync_com_port_handle  *port_handle;
} bmp285_com_port_info;

/**
 * Range attribute.
 */
typedef struct range_attr
{
  float min;
  float max;
} range_attr;


typedef enum {
  BMP285_PRESSURE_IDX = 0x0,
  BMP285_TEMPERATURE_IDX = 0x1,
  BMP_MAX_SENSOR
} bmp285_sensor_type_idx;

typedef enum
{
  BMP285_PRESSURE = 0x1,
  BMP285_TEMPERATURE = 0x2,
  BMP_SENSOR_INVALID = 0xFF
} bmp285_sensor_type;

typedef enum
{
  BMP285_CONFIG_IDLE,            /** not configuring */
  BMP285_CONFIG_POWERING_DOWN,   /** cleaning up when no clients left */
  BMP285_CONFIG_STOPPING_STREAM, /** stream stop initiated, waiting for completion */
  BMP285_CONFIG_FLUSHING_HW,     /** FIFO flush initiated, waiting for completion */
  BMP285_CONFIG_UPDATING_HW      /** updating sensor HW, when done goes back to IDLE */
} bmp285_config_step;

typedef enum
{
  BMP285_TEST_NO_ERROR,
  BMP285_FAC_TEST_HIGH_BIAS,
  BMP285_FAC_TEST_DEV_NOT_STATIONARY,
  BMP285_FAC_TEST_ZERO_VARIANCE
} bmp285_test_err_code;

typedef struct bmp285_self_test_info
{
  sns_physical_sensor_test_type test_type;
  bool test_client_present;
} bmp285_self_test_info;
#if BMP_CONFIG_ENABLE_SELF_TEST_FAC
typedef struct bmp285_factory_test_info
{
  int32_t num_samples;             /** number of samples acquired */
  float variance_threshold;        /** stationary detect variance threshold */
  float variance[TRIAXIS_NUM];          /** variance */
  float sample_square_sum[TRIAXIS_NUM]; /** sum of square of sample data */
  float sample_sum[TRIAXIS_NUM];        /** sum of sample data */
  float bias_thresholds[TRIAXIS_NUM];
  bool at_rest;
} bmp285_factory_test_info;
#endif
/*!
 *  @brief This enum holds different ODR values
 *  for Gas data
 */
typedef enum bmp285_odr
{
  BMP285_ODR_0_59MS,
  BMP285_ODR_62_5MS,
  BMP285_ODR_125MS,
  BMP285_ODR_250MS,
  BMP285_ODR_500MS,
  BMP285_ODR_1000MS,
  BMP285_ODR_10MS,
  BMP285_ODR_20MS,
  BMP285_ODR_NONE
} bmp285_odr;


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
} bmp285_power_mode;



/*!
 *  @brief This enum holds filter
 *  coefficient settings
 *  for pressure and temperature but not
 *     humidity and gas data
 *     resolution 20-bits
 *     ////
 *     The T,P result registers are reset to valye 0x80000 when the
 *     T,P measurements have been skipped (osrs_x='000')
 *     The appropriate filter memory is keupt unchanged (the value from
 *      the last measurement is kept). When the appropriate OSRS regiser is set back to nonzero, then
 *      the first valye stored to the T,P result register is filtered.
 */
enum bmp285_filter
{
  BMP285_FILTER_COEFF_OFF,
  BMP285_FILTER_COEFF_1,
  BMP285_FILTER_COEFF_3,
  BMP285_FILTER_COEFF_7,
  BMP285_FILTER_COEFF_15,
  BMP285_FILTER_COEFF_31,
  BMP285_FILTER_COEFF_63,
  BMP285_FILTER_COEFF_127
};


/*!
 *  @brief This enum holds osrs setting
 *  of TPH data
 */
enum bmp285_osrs_x
{
  BMP285_OSRS_NONE,
  BMP285_OSRS_1X,
  BMP285_OSRS_2X,
  BMP285_OSRS_4X,
  BMP285_OSRS_8X,
  BMP285_OSRS_16X
};

typedef struct bmp285_sensor_deploy_info
{
  /** Determines which Sensor data to publish. Uses
   *  bmp285_sensor_type as bit mask. */
  uint8_t           publish_sensors;
  uint8_t           enable;
} bmp285_sensor_deploy_info;


typedef struct bmp285_sensor_cfg_info
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
  uint64_t                max_requested_flush_ticks;
  sns_time          expect_time;
  bmp285_self_test_info  test_info;
} bmp285_sensor_cfg_info;

/* async port for the data stream which use the COM port handle */
typedef struct bmp285_async_com_port_info {
  uint32_t port_handle;
} bmp285_async_com_port_info;

typedef struct sns_bmp285_registry_cfg
{
  bmp285_sensor_type sensor_type;
  float               fac_cal_scale[1];
  float               fac_cal_bias[1];
  uint32_t            version;
}sns_bmp285_registry_cfg;

struct bmp280_calib_param_t
{
  u16 dig_T1;/**<calibration T1 data*/
  s16 dig_T2;/**<calibration T2 data*/
  s16 dig_T3;/**<calibration T3 data*/
  u16 dig_P1;/**<calibration P1 data*/
  s16 dig_P2;/**<calibration P2 data*/
  s16 dig_P3;/**<calibration P3 data*/
  s16 dig_P4;/**<calibration P4 data*/
  s16 dig_P5;/**<calibration P5 data*/
  s16 dig_P6;/**<calibration P6 data*/
  s16 dig_P7;/**<calibration P7 data*/
  s16 dig_P8;/**<calibration P8 data*/
  s16 dig_P9;/**<calibration P9 data*/
  /*this calibration is for bmp285*/
  s8 dig_P10;/**<calibration P10 data*/
  s32 t_fine;/**<calibration t_fine data*/
};
/** Private state. */
typedef struct bmp285_instance_state
{
  /** -- sensor configuration details --*/
  /** pressure HW config details*/
  bmp285_sensor_cfg_info pressure_info;
  /** temperature HW config details */
  bmp285_sensor_cfg_info temperature_info;
  bmp285_sensor_deploy_info   deploy_info;
  /** COM port info */
  bmp285_com_port_info com_port_info;
  /**--------Async Com Port--------*/
  sns_async_com_port_config  ascp_config;
  #if BMP_CONFIG_ENABLE_DAE
  /**--------DAE interface---------*/
  bmp285_dae_if_info       dae_if;
  #endif
  bmp285_config_step       config_step;
  /** Data streams from dependentcies. */
  sns_sensor_uid                 timer_suid;
  sns_data_stream                *temperature_timer_data_stream;
  sns_data_stream                *async_com_port_data_stream;  /* data streaming channel */
  sns_data_stream                *pressure_timer_data_stream;    /* sample sensor data */
  /* request/configure stream */
  uint32_t                       client_req_id;
  sns_std_sensor_config          bmp285_req;   /* stream for the configure */
  size_t                         encoded_imu_event_len;
  /**----------Sensor specific registry configuration----------*/
  sns_bmp285_registry_cfg sensor_temp_registry_cfg;
  sns_bmp285_registry_cfg sensor_pressure_registry_cfg;
  sns_diag_service               *diag_service;  /* for diagnostic to print debug message */
  sns_sync_com_port_service      *scp_service;
  #if BMP_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service *island_service;
  #endif
  bmp285_power_mode              op_mode;
  uint32_t                       interface;
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
#if BMP_CONFIG_ENABLE_SELF_TEST_FAC
  bool fac_test_in_progress;
  bmp285_factory_test_info fac_test_info;
  bmp285_sensor_type fac_test_sensor;
#endif
  bool update_fac_cal_in_registry;
  struct bmp280_calib_param_t calib_param;/**<calibration data*/
  u8 oversamp_temperature;/**< temperature over sampling*/
  u8 oversamp_pressure;/**< pressure over sampling*/
  u8 sample_rate_no_need_change;
  size_t           log_raw_encoded_size;

  float  pressure_data;
} bmp285_instance_state;

typedef struct sns_bmp285_cfg_req {
  float               sample_rate;
  float               report_rate;
  bmp285_sensor_type  sensor_type;
  bmp285_power_mode      op_mode;
  uint8_t             sample_rate_no_not_change;
  uint64_t  desired_flush_ticks;
  sns_bmp285_registry_cfg  registry_cfg;
} sns_bmp285_cfg_req;

sns_rc bmp285_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate);
sns_rc bmp285_inst_deinit(sns_sensor_instance *const this);
sns_rc bmp285_inst_set_client_config(
     sns_sensor_instance * const this,
    sns_request const *client_request);
void bmp285_send_cal_event(sns_sensor_instance *const instance, bmp285_sensor_type sensor_type);

