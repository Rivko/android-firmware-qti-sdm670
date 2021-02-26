#ifndef SNS_LOW_LAT_H_
#define SNS_LOW_LAT_H_

/*============================================================================
  @file sns_low_lat.h

  @brief
  API functions, data structures, and constants used by the Low Latency Stream
  Library

  @note
  File line length should generally be limited to <= 80 columns.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <sys/types.h>

enum {
  SNS_LLCM_FLAG_INTERRUPT_EN          = 0x1,
  SNS_LLCM_FLAG_ANDROID_STYLE_OUTPUT  = 0x2,
  SNS_LLCM_FLAG_CALIBRATED_STREAM     = 0x4
};

int sns_llcm_config(void* output_buf,
                       unsigned int output_buf_size,
                       unsigned int output_buf_offset,
                       int64 timestamp_offset,
                       const sns_std_suid_t *sensor_suid,
                       unsigned int sample_period_us,
                       unsigned int flags,
                       int  sensor_handle,
                       int* stream_id);

int sns_llcm_disable(int stream_id);

int sns_llcm_poll(int stream_id);

#endif /* SNS_LOW_LAT_H_ */
