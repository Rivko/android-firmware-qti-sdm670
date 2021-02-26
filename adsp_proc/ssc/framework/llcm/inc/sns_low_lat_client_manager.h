#pragma once
/*=============================================================================
  @file sns_low_lat_client_manager.h

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
#include <qurt_signal2.h>
#include "qmi_csi.h"
#include "sns_client.pb.h"
#include "sns_data_stream.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_profiler.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_registry.pb.h"
#include "sns_registry_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_std.pb.h"
#include "sns_std_type.pb.h"
#include "sns_suid_util.h"

/*=============================================================================
  Defines
  ===========================================================================*/

/* Enabel more debug logs include   */
#define LLCM_DEBUG_MORE   0

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/* Macros to split 64 bit value to low and hi 32 bits */
#define SNS_LLCM_FETCH_HI_32(x) (uint32_t)(x>>32)
#define SNS_LLCM_FETCH_LOW_32(y) (uint32_t)(y)

/* String of names */
#define SNS_LLCM_SENSOR_NAME "llcm"
#define SNS_LLCM_VENDOR_NAME "qualcomm"
#define SNS_LLCM_PROTO_NAME  "sns_client.proto"

#define SNS_LLCM_INVALID_STREAM_ID        (-1)
#define SNS_LLCM_MIN_EIG_P                (0.0001f)

#define SNS_LLCM_US_PER_SEC               (1000000)

#define SNS_LLCM_MAX_SAMPLE_RATE_HZ       (1000)
#define SNS_LLCM_MIN_SAMPLE_PERIOD_US     (SNS_LLCM_US_PER_SEC / SNS_LLCM_MAX_SAMPLE_RATE_HZ)
#define SNS_LLCM_MIN_SAMPLE_RATE_HZ       (1)
#define SNS_LLCM_MAX_SAMPLE_PERIOD_US     (SNS_LLCM_US_PER_SEC / SNS_LLCM_MIN_SAMPLE_RATE_HZ)

/* System wakeup latency to config, in uS */
#define SNS_LLCM_WAKEUP_LATENCY_US        (1)

/*
 * The maximum amount of open low latency stream channels
 * 32 was chosen to match the Android Direct Report specification
 */
#define SNS_LLCM_MAX_CHANNELS              (32)

/* Signal masks for poll function */
#define SNS_LLCM_SIGNAL_IND               0x00000001
#define SNS_LLCM_SIGNAL_CANCEL_POLL       0x00000002

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/
/*
 * Sensor configuration mask.
 */
enum {
  SNS_LLCM_FLAG_INTERRUPT_EN          = 0x1,
  SNS_LLCM_FLAG_ANDROID_STYLE_OUTPUT  = 0x2,
  SNS_LLCM_FLAG_CALIBRATED_STREAM     = 0x4
};

/*
 * Sensor configurations to be sent to instance.
 */
typedef struct sns_llcm_config_req
{
  /* Channel to be configured */
  uint32_t       channel_id;
  /* Sensor to be configured */
  const sns_std_suid   *sensor_suid;
  /* Sensor handle from sensor HAL, used as sensor identifier in Android */
  int            sensor_handle;
  /* Sampling period of sensor */
  uint32_t       sample_period_us;  
  /* Offset between AP and SSC */
  int64_t        timestamp_offset_ns;
  /* Request calibrated sensor stream */
  bool           calibrated_stream;
  /* Update timestamp offset only,
   * no config will be performed if this is true */
  bool           offset_update_only;
} sns_llcm_config_req;

/*
 * State of of particular low latency channel.
 * LLCM use single instance and supports up to 32 channels, channels initialize
 * when new share memory address is passed into sns_llcm_config() function,
 * de-initialize when sns_llcm_disable() is called. 
 */
typedef struct sns_llcm_channel
{
  /*
   * Pointer to the output buffer, must be a non-NULL value
   * If this value is NULL, then the entire stream state is invalid
   */
  void*          output_buf;
  unsigned int   output_buf_size;

  /*
   * These are needed for the Android style of output since there's no header in
   * the android circular buffer
   */
  unsigned int   output_buf_most_recent_idx;
  unsigned int   output_buf_max_samples;
  /*
   * Monotonically increase counter to indicate event sequence.
   * Note: 0 is an invalid value, should be initialized to 1 and roll back to 1
   */
  unsigned int   atomic_counter;

  /* Signal used to wait on indications */
  qurt_signal2_t signal;

  /* List of sensor data streams */
  sns_isafe_list sensor_client_list;
} sns_llcm_channel;

/*
 * Structure which stores all registry related configuration.
 */
typedef struct sns_llcm_reg_config
{
  /* Enable Qualcomm VR mode */
  bool qc_vr_mode;
} sns_llcm_reg_config;

/**
 * Singular state for the Low Latency Client Manager.  Represented to the Framework
 * as a Sensor.
 */
typedef struct sns_llcm_state
{
  sns_island_client_handle island_client;
  /* SCPM manager client handle */
  sns_scpm_mgr_client *scpm_client;
  /* Dep sensors: Gyro Cal, Resampler, Registry */
  SNS_SUID_LOOKUP_DATA(3) suid_lookup_data;
  /* Optional dep sensors: Accel Cal, Mag Cal */
  SNS_SUID_LOOKUP_DATA(2) opt_suid_lookup_data;
  /* The stream associated with registry sensor */
  sns_data_stream        *registry_stream;
  /* Structure which stores all registry configuration for LLCM */
  sns_llcm_reg_config     registry_config;
} sns_llcm_state;

/*=============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * This function configure sensor in particular low latency channel.
 *
 * @param[in] output_buf         Point to sensor data output buffer.
 * @param[in] output_buf_size    Size of output buffer, in bytes.
 * @param[in] output_buf_offset  Write offset for first sensor data, give
 *                               non-zero value if not start from begin of output buffer.
 * @param[in] timestamp_offset   Timestamp offset between AP and SSC.
 * @param[in] sensor_suid        Sensor to configure, an special SUID is defined
 *                               to stop all sensors in channel.
 * @param[in] sample_period_us   Sensor sampling period.
 * @param[in] flags              Special configure flags.
 * @param[in] sensor_handle      Sensor identifier used in Android.
 * @param[out] stream_id         Pointer of Low lateny channel(stream) ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_config(void* output_buf,
                       unsigned int output_buf_size,
                       unsigned int output_buf_offset,
                       int64_t timestamp_offset,
                       const sns_std_suid *sensor_suid,
                       unsigned int sample_period_us,
                       unsigned int flags,
                       int  sensor_handle,
                       int* stream_id);

/**
 * Disable all sensors in particular low latency channel.
 *
 * @param[in] stream_id            Low lateny channel ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_disable(int stream_id);

/**
 * Polling particular low latency channel for new sensor data.
 *
 * @param[in] stream_id            Low lateny channel ID.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_poll(int stream_id);

/**
 * Update the timestamp offset between AP and SSC
 *
 * @param[in] timestamp_offset       Timestamp offset between AP and SSC.
 * @param[in] slope       Slope of timestamp offset change, this is reserved for future use.
 *
 * @return sns_rc
 *
 */
sns_rc sns_llcm_offset_update(int64_t timestamp_offset,
                       int64_t slope);

