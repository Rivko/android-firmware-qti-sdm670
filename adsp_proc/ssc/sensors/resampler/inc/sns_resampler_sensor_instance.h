#pragma once
/*=============================================================================
  @file resampler_sensor_instance.h

  Common return codes used by the Sensor Framework.

  Copyright (c) 2016 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_resampler_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/
typedef struct
{
  /** interpolator object can produce new sample with desired_timestamp
      when sample1's timestamp <= desired_timestamp <= sample2's timestamp */
  resampler_sample sample1; /**< 1st known sample */
  resampler_sample sample2; /**< 2nd known sample */
  sns_time             desired_timestamp;
  /**< interval computed from desired frequency */
  uint32_t             interval_ticks;
} resampler_interp_obj_s;

typedef struct
{
  float             input_frequency;
  /**< pointer to the dynamic array */
  float*            accumulator;
  /**<number of samples used for filtering */
  uint32_t          usable_count; /* Number of non-unreliable samples */
  uint32_t          count; /* Total count including unreliable samples */
  uint32_t          factor;
  uint32_t          delay_ticks;
  /**< number of bytes in the allocated array */
  uint32_t          axis_cnt_bytes;
  /**< holds the minimum accuracy of the samples used for filtering */
  sns_std_sensor_sample_status status;
} resampler_cic_obj_s;

typedef struct
{
  uint32_t                sampling_factor;
  uint32_t                sampling_count;
} resampler_cur_samp_obj;

typedef struct resampler_instance_state
{
  bool                    cic_enabled;
  bool                    interp_enabled;

  // physical cfg
  bool                    phys_cfg_received;
  uint64_t                sync_ts_anchor;
  bool                    stream_is_synchronous;
  float                   sample_rate;

  // Stream of sensor data to be resampled
  sns_data_stream         *sensor_stream;

  sns_time                ts_last_saved;
  //Axis count decoded from client request
  uint32_t				  axis_cnt;
  sns_resampler_quality   quality;
  resampler_cic_obj_s     cic_obj;
  resampler_interp_obj_s  interp_obj;
  resampler_cur_samp_obj  cur_samp_obj;
  sns_resampler_config    client_config;
} resampler_instance_state;
