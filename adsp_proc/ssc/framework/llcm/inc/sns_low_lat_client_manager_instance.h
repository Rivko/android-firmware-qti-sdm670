#pragma once
/*=============================================================================
  @file sns_low_lat_lient_manager_instance.h

  This module provides low latency sensor data channel by works with few
  other modules.
  This module receive sensor request via fastRPC call and write sensor
  data into shared memory.  Some APIs of this module are exported and
  called by fastRPC remote functions, these exported APIs receive sensor
  requests and this module will generate sensor client requests accordingly
  to sensor framework, then receive sensor events from sensor framework
  and write sensor data into shared memory which accessible to sensor data
  consumer.

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include "DALDeviceId.h"
#include "DDIIPCInt.h"
#include "sns_low_lat_client_manager.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_resampler.pb.h"
#include "sns_sensor_uid.h"
#include "sns_std_type.pb.h"

/*=============================================================================
  Defines
  ===========================================================================*/

#define SNS_LLCM_IMU_SENSOR_AXES_NUM      (3)
/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * State associated with a particular sensor instance.
 */
typedef struct sns_llcm_inst_state
{
  /* Accel cal is available */
  bool accel_cal_available;
  /* Mag cal is available */
  bool mag_cal_available;

  /* Lock for 'sensor_client_list' in sns_llcm_channel structure 
   * Clients list of all LLCM channles share one global lock here.
   */
  sns_osa_lock *client_list_lock;

  /* Unique identifier for this client */
  uint32_t client_id;

  /* sensor SUIDs */
  sns_sensor_uid accel_cal_suid;
  sns_sensor_uid gyro_cal_suid;
  sns_sensor_uid mag_cal_suid;
  sns_sensor_uid resampler_suid;

  /* Data stream of calibration sensors */
  sns_data_stream *accel_cal_stream;
  sns_data_stream *gyro_cal_stream;
  sns_data_stream *mag_cal_stream;

  /* Calibration data and calibration status */
  float accel_cal_bias[SNS_LLCM_IMU_SENSOR_AXES_NUM];
  sns_std_sensor_sample_status accel_cal_status;
  float mag_cal_bias[SNS_LLCM_IMU_SENSOR_AXES_NUM];
  sns_std_sensor_sample_status mag_cal_status;
  float gyro_cal_bias[SNS_LLCM_IMU_SENSOR_AXES_NUM];
  sns_std_sensor_sample_status gyro_cal_status;
  
  /* Client count of calibration streams */
  int accel_cal_use_cnt;
  int gyro_cal_use_cnt;
  int mag_cal_use_cnt;

  /* Offset between AP and SSC */
  int64_t timestamp_offset_ns;

  /* Handle of interrupt device */
  DalDeviceHandle *interrupt_handle;
  /* Structure which stores all registry configuration for LLCM */
  sns_llcm_reg_config         registry_config; 
} sns_llcm_inst_state;

/**
 * This structure store information of particular sensor client.
 */
typedef struct sns_llcm_sensor_client
{
  /* Sensor type */
  int              sensor_type;
  /* Sensor identifier used in Android */
  int              sensor_handle;
  /* Sensor suid */
  sns_std_suid     suid;
  /* Data stream to physical sensor . */
  sns_data_stream  *raw_data_stream;
  /* Data stream though resampler sensor. */
  sns_data_stream  *resampler_stream;
  sns_isafe_list_item    item;
} sns_llcm_sensor_client;

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * This function flush the cache, defined in dsp_cache.S
 *
 * @param[in] addr    Memory address be flushed.
 * @param[in] size    Size of memory need to flush, in bytes.
 *
 */
void sns_llcm_cache_clean(void* addr, int size);

